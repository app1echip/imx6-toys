#include "tube.h"

#include <string.h> /* memset(), strlen() */
#include <unistd.h> /* usleep() */

#include "art.h"

static struct tube_t tube;

void tube_init() {
  memset(tube._buf, ART_BLANK, sizeof(tube._buf));
  tube.len = 0;
  tube.dot = 0;
}

void tube_render(void* arg) {
  for (;;) {
    for (int i = 0; i < sizeof(tube._buf); ++i) {
      *(addr + 0x1cc) = 1 << (sizeof(tube._buf) - i - 1);
      *(addr + 0x1c8) = tube._buf[i] & ~((tube.dot >> (7 - i)) * ~ART_DOT);
      usleep(1000);
    }
  }
}

void tube_print(const char* str) {
  tube.len = strlen(str) > sizeof(tube._buf) ? sizeof(tube._buf) : strlen(str);
  for (int i = 0; i < sizeof(tube._buf); ++i)
    tube._buf[i] = i <= tube.len ? art(str[i]) : ART_BLANK;
}

void tube_dot(uint8_t bits) { tube.dot = bits; }