#include "bell.h"

#include <linux/input.h> /* input_event */
#include <unistd.h>      /* write(), usleep() */

void buz_beep() {
  struct input_event ev = {.type = EV_SND, .code = SND_BELL};
  ev.value = 1;
  write(buz, &ev, sizeof(ev));
  usleep(150000);
  ev.value = 0;
  write(buz, &ev, sizeof(ev));
}

void buz_alert() {
  struct input_event ev = {.type = EV_SND, .code = SND_BELL};
  for (int i = 0; i < 3; ++i) {
    ev.value = 1;
    write(buz, &ev, sizeof(ev));
    usleep(100000);
    ev.value = 0;
    write(buz, &ev, sizeof(ev));
    usleep(50000);
  }
}