#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "autoclock.h"
#include "nkey.h"
#include "tube.h"

#define APP_TICKING 0
#define APP_EDITING 1

void sync_tube_clock(struct autoclock_t* autoclock, struct tube_t* tube) {
  char buf[10];
  autoclock_dump(buf, *autoclock);
  tube_print(tube, buf);
}

int main(int argc, char const* argv[]) {
  int kbd = open("/dev/input/event4", O_RDONLY);
  int mem = open("/dev/mem", O_RDWR);
  uint8_t* addr = mmap(NULL, 0x1cc + sizeof(uint8_t), PROT_WRITE, MAP_SHARED,
                       mem, 0x8000000);

  tube_ind = addr + 0x1cc;
  tube_val = addr + 0x1c8;
  struct tube_t tube;
  tube_init(&tube);

  struct autoclock_t autoclock;
  autoclock_init(&autoclock);

  pthread_t display;
  pthread_create(&display, NULL, (void* (*)(void*))tube_render, &tube);
  pthread_t clocker;
  pthread_create(&clocker, NULL, (void* (*)(void*))autoclock_loop, &autoclock);

  autoclock_subscribe(&autoclock, (autoclock_callback*)sync_tube_clock, &tube);

  int state = APP_EDITING;
  for (;;) {
    int op = getnkey(kbd);
    switch (state) {
      case APP_TICKING:
        switch (op) {
          case NKEY_ENTER:
            autoclock.state = CLOCK_PAUSED;
            state = APP_EDITING;
            break;
        }
        break;
      case APP_EDITING:
        switch (op) {
          case NKEY_ZERO:
          case NKEY_ONE:
          case NKEY_TWO:
          case NKEY_THREE:
          case NKEY_FOUR:
          case NKEY_FIVE:
          case NKEY_SIX:
          case NKEY_SEVEN:
          case NKEY_EIGHT:
          case NKEY_NINE:
            if (tube.len % 3 == 2) tube_push(&tube, '-');
            tube_push(&tube, '0' + nkey_val(op));
            break;
          case NKEY_BACKSPACE:
            tube_pop(&tube);
            if (tube.len % 3 == 0) tube_pop(&tube);
            break;
          case NKEY_ENTER:
            if (tube.len == sizeof(tube.buf)) {
              int h, m, s;
              char buf[10];
              tube_dump(buf, tube);
              if (sscanf(buf, "%d-%d-%d", &h, &m, &s) == 3 &&
                  sclock_valid((struct sclock_t){h, m, s})) {
                autoclock.sclock = (struct sclock_t){h, m, s};
                autoclock.state = CLOCK_RUNNING;
                state = APP_TICKING;
              } else {
                tube_print(&tube, "Error");
                sleep(2);
                // getnkey(kbd);
                tube_print(&tube, buf);
              }
            }
            break;
        }
        break;
    }
  }

  pthread_join(display, &(void*){0});
  pthread_join(clocker, &(void*){0});

  munmap(addr, 0x1cc + sizeof(uint8_t));
  close(mem);
  close(kbd);

  return 0;
}
