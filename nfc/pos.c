#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

#include "bell.h"
#include "database.h"
#include "key.h"
#include "nfc.h"
#include "tube.h"

int nfc;       /* fd: NFC */
int buz;       /* fd: beeper */
int kbd;       /* fd: keyboard */
int mem;       /* fd: digital tube */
uint8_t* addr; /* tube memory map */

typedef void (*key_handler)(int);
typedef void (*uid_handler)(uint32_t);
typedef void (*initializer)(void);

/* define running mode */
typedef struct state_t {
  key_handler ans_key; /* handle keyboard input */
  uid_handler ans_uid; /* handle NFC uid */
  initializer refresh; /* 'constructor' */
} state_t;

/* running state
 *   idle - show screensaver
 *   bank - handle transactions
 *   info - fetch history
 */
state_t idle, bank, info, *current;

void change_to_state(state_t* state) {
  current = state;
  state->refresh();
}

/* data shared across states and events */
unsigned bank_digit;       /* number currently enterd */
uint8_t bank_giveaway;     /* direction of transfer */
struct cinfo* info_client; /* currently showing user */
int info_bal;              /* whether to show balance or history */
int info_log;              /* currently showing history */

/* util: print number to tube in style */
void printnum(int n, char p) {
  char buf[10];
  int a = n > 0 ? n : -n;
  sprintf(buf, "%c%5d%02d", p, n / 100, a % 100);
  if (n < 0 && n > -100) buf[4] = '-';
  tube_print(buf);
#if defined(DEBUG)
  printf("DEBUG: [output] %s\n", buf);
#endif
}

/* print HELLO */
void idle_init() {
  tube_print("HELLO");
  tube_dot(0x07);
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [mode] now idle\n");
#endif
}
/* jump to 'bank' on number pressed */
void idle_key(int key) {
  switch (key) {
    case KEYCODE_ZERO:
    case KEYCODE_ONE:
    case KEYCODE_TWO:
    case KEYCODE_THREE:
    case KEYCODE_FOUR:
    case KEYCODE_FIVE:
    case KEYCODE_SIX:
    case KEYCODE_SEVEN:
    case KEYCODE_EIGHT:
    case KEYCODE_NINE:
      bank_digit = keyval(key);
      change_to_state(&bank);
      break;
  }
}
/* jump to 'info' on incoming NFC uid */
void idle_uid(uint32_t uid) {
  info_client = db_fetch(uid);
  if (info_client == NULL) {
#if defined(DEBUG) || defined(INFO)
    printf("INFO: [database] card not found\n");
#endif
    tube_print("Err 404");
    tube_dot(0);
    buz_alert(buz);
    sleep(2);
    idle_init();
  } else {
    change_to_state(&info);
  }
}
/* start in deduction mode */
void bank_init() {
  bank_giveaway = 0x00;
  printnum(bank_digit, bank_giveaway ? 'A' : '-');
  tube_dot(1 << 2);
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [mode] enter bank\n");
#endif
}
/*
 * show number if 0-9 pressed
 * backspace if right bottom pressed, back to 'idle' if empty
 * switch transfer direction if left bottom pressed
 */
void bank_key(int key) {
  switch (key) {
    case KEYCODE_ZERO:
    case KEYCODE_ONE:
    case KEYCODE_TWO:
    case KEYCODE_THREE:
    case KEYCODE_FOUR:
    case KEYCODE_FIVE:
    case KEYCODE_SIX:
    case KEYCODE_SEVEN:
    case KEYCODE_EIGHT:
    case KEYCODE_NINE:
      if (bank_digit / 1000000 == 0) {
        bank_digit = bank_digit * 10 + keyval(key);
        printnum(bank_digit, bank_giveaway ? 'A' : '-');
      }
      break;
    case KEYCODE_LB:
      bank_giveaway = ~bank_giveaway;
      printnum(bank_digit, bank_giveaway ? 'A' : '-');
      break;
    case KEYCODE_RB:
      if (bank_digit == 0) {
        change_to_state(&idle);
      } else {
        bank_digit /= 10;
        printnum(bank_digit, bank_giveaway ? 'A' : '-');
      }
      break;
  }
}
/* update database and go back to 'idle' if transaction ok, else alert */
void bank_uid(uint32_t uid) {
  int record = bank_giveaway ? bank_digit : -bank_digit;
  if (db_update(uid, record) != 0) {
    tube_print("Error");
    buz_alert();
    sleep(3);
    printnum(bank_digit, bank_giveaway ? 'A' : '-');
  } else {
    db_dump();
    buz_beep();
    change_to_state(&idle);
  }
}
/* show balance at start, set timeout on entering */
void info_init() {
  alarm(5);
  info_bal = 1;
  info_log = info_client->log.len - 1;
  printnum(info_client->bal, 'b');
  tube_dot(1 << 2);
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [mode] enter info\n");
#endif
}
/*
 * show further or nearer history if left or right bottom pressed
 * else back to 'idle'
 * set timeout on entering
 */
void info_key(int key) {
  alarm(5);
  switch (key) {
    case KEYCODE_LB:
      if (info_log > 0) {
        if (!info_bal) info_log--;
        printnum(*(int*)vat(info_client->log, info_log), 'h');
        info_bal = 0;
      }
      break;
    case KEYCODE_RB:
      if (info_log < info_client->log.len - 1) {
        info_log++;
        printnum(*(int*)vat(info_client->log, info_log), 'h');
      } else {
        printnum(info_client->bal, 'b');
        info_bal = 1;
      }
      break;
    default:
      alarm(0);
      change_to_state(&idle);
      break;
  }
}
/* just ignore */
void info_uid(uint32_t uid) {}
/* go back to 'idle' on timeout */
void info_timeout(int dum) {
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [display] time out\n");
#endif
  change_to_state(&idle);
}

/* extra thread
 *   kbd_th - listen for keyboard input and dispatch to current handler
 *   nfc_th - listen for nfc input and dispatch to current hanlder
 *   render_th - renders digital tube
 */
pthread_t kbd_th, nfc_th, render_th;
void listen_kbd(void*);
void listen_nfc(void*);
/* handle SIGINT (e.g. Ctrl+C), cancel threads and reach clean-up code */
void terminate(int);

int main(int argc, char* argv[]) {
  /* open keyboard */
  kbd = open("/dev/input/event4", O_RDONLY);
  /* open beeper */
  buz = open("/dev/input/event1", O_RDWR);
  /* open nfc */
  nfc = open("/dev/ttyS2", O_RDWR);
  /* make it raw */
  struct termios save, raw;
  tcgetattr(nfc, &save);
  raw = save;
  cfmakeraw(&raw);
  /* set baud rate */
  cfsetispeed(&raw, B115200);
  cfsetospeed(&raw, B115200);
  tcsetattr(nfc, TCSANOW, &raw);
  /* wake it up */
  nfc_wakeup();
  /* open digital tube */
  mem = open("/dev/mem", O_RDWR);
  /* map it memory */
  addr = mmap(NULL, 0x1cc + 1, PROT_WRITE, MAP_SHARED, mem, 0x8000000);
  /* initialize display */
  tube_init();

  /* load database */
  char* dbfile = NULL;
  int opt;
  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
      case 'd':
        dbfile = strdup(optarg);
        break;
    }
  }
  if (dbfile == NULL) dbfile = strdup("db.bin");
  db_load(dbfile);
  free(dbfile);

  /* bind event hanlders */
  idle = (state_t){idle_key, idle_uid, idle_init};
  bank = (state_t){bank_key, bank_uid, bank_init};
  info = (state_t){info_key, info_uid, info_init};
  signal(SIGINT, terminate);
  signal(SIGALRM, info_timeout);

  /* start in idle */
  change_to_state(&idle);

  pthread_create(&kbd_th, NULL, (void* (*)(void*))listen_kbd, NULL);
  pthread_create(&nfc_th, NULL, (void* (*)(void*))listen_nfc, NULL);
  pthread_create(&render_th, NULL, (void* (*)(void*))tube_render, NULL);
  pthread_join(render_th, &(void*){0});
  pthread_join(kbd_th, &(void*){0});
  pthread_join(nfc_th, &(void*){0});

  /* clean up */
  db_dump();
  db_free();

  munmap(addr, 0x1cc + sizeof(uint8_t));
  close(mem);
  tcsetattr(nfc, TCSANOW, &save);
  close(nfc);
  close(buz);
  close(kbd);

#if defined(DEBUG)
  printf("DEBUG: [main] normal exist\n");
#endif
  return 0;
}

void listen_kbd(void* arg) {
  for (;;) {
    int key = getkey(kbd);
#if defined(DEBUG)
    printf("DEBUG: [key] code %d\n", key);
#endif
    current->ans_key(key);
  }
}

void listen_nfc(void* arg) {
  for (;;) {
    sleep(1);
    uint32_t uid = fetch_uid(nfc);
#if defined(DEBUG) || defined(INFO)
    printf("INFO: [uid] ");
    for (int i = 0; i < sizeof(uid); ++i)
      printf("%02hhx ", ((uint8_t*)&uid)[i]);
    printf("\n");
#endif
    current->ans_uid(uid);
  }
}

void terminate(int arg) {
  pthread_cancel(render_th);
  pthread_cancel(nfc_th);
  pthread_cancel(kbd_th);
#if defined(DEBUG)
  printf("\nDEBUG: [signal] SIGINT\n");
#endif
}
