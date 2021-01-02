#include <linux/input.h>
#include <unistd.h>

#define NKEY_ONE 2
#define NKEY_TWO 3
#define NKEY_THREE 4
#define NKEY_FOUR 5
#define NKEY_FIVE 6
#define NKEY_SIX 7
#define NKEY_SEVEN 8
#define NKEY_EIGHT 9
#define NKEY_NINE 10
#define NKEY_ASTERISK 1
#define NKEY_ZERO 115
#define NKEY_HASHTAG 114
#define NKEY_BACKSPACE 1
#define NKEY_ENTER 114

int getnkey(int keyfd) {
  struct input_event buf;
  int pressed = 0;
  int code;
  while (!pressed) {
    int ret = read(keyfd, &buf, sizeof(buf));
    if (ret != sizeof(buf)) continue;
    if (buf.type == EV_KEY && buf.value == 1) {
      code = buf.code;
      pressed = 1;
    }
  }
  return code;
}

int nkey_val(int key) {
  switch (key) {
    case NKEY_ZERO:
      return 0;
    case NKEY_ONE:
      return 1;
    case NKEY_TWO:
      return 2;
    case NKEY_THREE:
      return 3;
    case NKEY_FOUR:
      return 4;
    case NKEY_FIVE:
      return 5;
    case NKEY_SIX:
      return 6;
    case NKEY_SEVEN:
      return 7;
    case NKEY_EIGHT:
      return 8;
    case NKEY_NINE:
      return 9;
    default:
      return -1;
  }
}