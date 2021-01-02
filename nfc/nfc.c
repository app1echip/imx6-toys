#include "nfc.h"

#include <unistd.h> /* read(), write() */

#define PDMAX 0xfe

/* command and response frame structure */
static const uint8_t PREAMBLE = 0x00;
static const uint8_t STARTCODE[] = {0x00, 0xff};
static uint8_t LEN;
static uint8_t LCS;
static const uint8_t TFI_WRITE = 0xd4;
static const uint8_t TFI_READ = 0xd5;
static uint8_t PD[PDMAX]; /* dirty trick: used as blackhole */
static uint8_t DCS;
static const uint8_t POSTAMBLE = 0x00;
/* ACK */
static const uint8_t ACK[] = {0x00, 0x00, 0xff, 0x00, 0xff, 0x00};
/* wake up signal */
static const uint8_t WAKEUP[] = {0x55, 0x55, 0x00, 0x00, 0x00};
/* command: change to normal mode */
static const uint8_t NORMALMODE[] = {0x14, 0x01};
/* command: scan one nfc target */
static const uint8_t SCANTARGET[] = {0x4a, 0x01, 0x00};

void nfc_wakeup() {
  write(nfc, WAKEUP, sizeof(WAKEUP));        /* send wake up signal */
  write_cmd(NORMALMODE, sizeof(NORMALMODE)); /* change to normal mode */
  digest_ack();                              /* read ACK */
  read_ans(PD, &(uint8_t){0});               /* read response */
}

void write_cmd(const void* pd, uint8_t n) {
  LEN = n + 1;
  LCS = 0x100 - LEN;
  int sum = TFI_WRITE;
  for (int i = 0; i < n; ++i) sum += ((uint8_t*)pd)[i];
  DCS = 0x100 - sum % 0x100;
  write(nfc, &PREAMBLE, sizeof(PREAMBLE));
  write(nfc, STARTCODE, sizeof(STARTCODE));
  write(nfc, &LEN, sizeof(LEN));
  write(nfc, &LCS, sizeof(LCS));
  write(nfc, &TFI_WRITE, sizeof(TFI_WRITE));
  write(nfc, pd, n);
  write(nfc, &DCS, sizeof(DCS));
  write(nfc, &POSTAMBLE, sizeof(POSTAMBLE));
}

void digest_ack() { read(nfc, PD, sizeof(ACK)); }

void read_ans(void* buf, uint8_t* n) {
  read(nfc, PD, sizeof(PREAMBLE));
  read(nfc, PD, sizeof(STARTCODE));
  read(nfc, &LEN, sizeof(LEN));
  *n = LEN - 1;
  read(nfc, &LCS, sizeof(LCS));
  read(nfc, PD, sizeof(TFI_READ));
  for (uint8_t got = 0; got < *n;) got += read(nfc, buf + got, *n - got);
  read(nfc, &DCS, sizeof(DCS));
  read(nfc, PD, sizeof(POSTAMBLE));
}

uint32_t fetch_uid() {
  write_cmd(SCANTARGET, sizeof(SCANTARGET)); /* scan target */
  digest_ack();                              /* digest ACK */
  uint8_t buf[0x20];
  read_ans(buf, &(uint8_t){0}); /* fetch response */
  return *(uint32_t*)(buf + 7); /* parse uid */
}