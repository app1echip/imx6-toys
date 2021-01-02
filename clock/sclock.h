#include <stdint.h>

struct sclock_t {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

void sclock_tick(struct sclock_t* sclock) {
  sclock->second++;
  if (sclock->second == 60) {
    sclock->second = 0;
    sclock->minute++;
    if (sclock->minute == 60) {
      sclock->minute = 0;
      sclock->hour++;
      if (sclock->hour == 24) {
        sclock->hour = 0;
      }
    }
  }
}

int sclock_valid(struct sclock_t sclock) {
  return sclock.hour < 24 && sclock.minute < 60 && sclock.second < 60;
}

