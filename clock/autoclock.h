#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "sclock.h"

#define CLOCK_PAUSED 0
#define CLOCK_RUNNING 1
#define CLOCK_STOPPED 2

#define SUBSIZ 5

struct autoclock_t;
typedef void(autoclock_callback)(struct autoclock_t*, void*);

struct autoclock_t {
  struct sclock_t sclock;
  uint8_t state;
  autoclock_callback* calls[BUFSIZ];
  void* args[SUBSIZ];
};

void autoclock_init(struct autoclock_t* autoclock) {
  autoclock->sclock = (struct sclock_t){0, 0, 0};
  autoclock->state = CLOCK_PAUSED;
  for (int i = 0; i < SUBSIZ; ++i) {
    autoclock->args[i] = NULL;
    autoclock->calls[i] = NULL;
  }
}

void autoclock_loop(struct autoclock_t* autoclock) {
  clock_t prev = clock();
  while (autoclock->state != CLOCK_STOPPED) {
    if (autoclock->state != CLOCK_PAUSED) {
      sclock_tick(&autoclock->sclock);
      for (int i = 0; i < SUBSIZ; ++i) {
        if (autoclock->calls[i] != NULL) {
          autoclock->calls[i](autoclock, autoclock->args[i]);
        }
      }
    }
    clock_t now = clock();
    int elapse = 1000000 - (now - prev);
    if (elapse > 0) usleep(elapse);
    prev = now;
  }
}

void autoclock_dump(char* buf, struct autoclock_t autoclock) {
  sprintf(buf, "%02d-%02d-%02d", autoclock.sclock.hour, autoclock.sclock.minute,
          autoclock.sclock.second);
}

int autoclock_subscribe(struct autoclock_t* autoclock,
                        autoclock_callback* callback, void* arg) {
  for (int i = 0; i < SUBSIZ; ++i) {
    if (autoclock->calls[i] == NULL) {
      autoclock->calls[i] = callback;
      autoclock->args[i] = arg;
      return i;
    }
  }
  return -1;
}
void autoclock_unsubscribe(struct autoclock_t* autoclock,
                           autoclock_callback* callback) {
  for (int i = 0; i < SUBSIZ; ++i) {
    if (autoclock->calls[i] == callback) {
      autoclock->calls[i] = NULL;
      autoclock->args[i] = NULL;
      return;
    }
  }
}