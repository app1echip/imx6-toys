#include "key.h"

#include <linux/input.h> /* input_event */
#include <unistd.h>      /* read() */

unsigned short getkey() {
  int code;
  for (;;) {
    struct input_event ev;
    int nbytes = read(kbd, &ev, sizeof(ev));
    if (nbytes != sizeof(ev)) continue;
    if (ev.type == EV_KEY && ev.value == 1) {
      code = ev.code;
      break;
    }
  }
  return code;
}

int keyval(unsigned short code) {
  switch (code) {
    case KEYCODE_ZERO:
      return 0;
    case KEYCODE_ONE:
      return 1;
    case KEYCODE_TWO:
      return 2;
    case KEYCODE_THREE:
      return 3;
    case KEYCODE_FOUR:
      return 4;
    case KEYCODE_FIVE:
      return 5;
    case KEYCODE_SIX:
      return 6;
    case KEYCODE_SEVEN:
      return 7;
    case KEYCODE_EIGHT:
      return 8;
    case KEYCODE_NINE:
      return 9;
    default:
      return KEYVAL_UNDEFINED;
  }
}
