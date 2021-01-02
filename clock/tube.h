
#include <pthread.h>
#include <string.h>

#include "art.h"

struct tube_t {
  uint8_t buf[8];
  uint8_t len;
};

uint8_t* tube_ind;
uint8_t* tube_val;

void tube_init(struct tube_t* tube) {
  memset(tube->buf, ART_BLANK, sizeof(tube->buf));
  tube->len = 0;
}

void tube_render(struct tube_t* tube) {
  for (;;) {
    for (int i = 0; i < sizeof(tube->buf); ++i) {
      *tube_ind = 1 << (sizeof(tube->buf) - i - 1);
      *tube_val = tube->buf[i];
      usleep(1000);
    }
  }
}

void tube_print(struct tube_t* tube, const char* str) {
  tube->len = strlen(str) > sizeof(tube->buf) ? sizeof(tube->buf) : strlen(str);
  for (int i = 0; i < sizeof(tube->buf); ++i)
    tube->buf[i] = i <= tube->len ? tube_art(str[i]) : ART_BLANK;
}

void tube_push(struct tube_t* tube, char c) {
  if (tube->len < sizeof(tube->buf)) {
    tube->buf[tube->len++] = tube_art(c);
  }
}

void tube_pop(struct tube_t* tube) {
  if (tube->len > 0) {
    tube->buf[--tube->len] = ART_BLANK;
  }
}

void tube_dump(char* buf, struct tube_t tube) {
  for (int i = 0; i < tube.len; ++i) {
    buf[i] = reverse_art(tube.buf[i]);
  }
  tube.buf[tube.len] = '\0';
}

