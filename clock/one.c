#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "autoclock.h"
#include "tube.h"

int main(int argc, char const* argv[]) {
  int mem = open("/dev/mem", O_RDWR);
  uint8_t* addr = mmap(NULL, 0x1cc + sizeof(uint8_t), PROT_WRITE, MAP_SHARED,
                       mem, 0x8000000);
  tube_ind = addr + 0x1cc;
  tube_val = addr + 0x1c8;
  struct tube_t tube;
  tube_init(&tube);

  struct autoclock_t autoclock;
  autoclock_init(&autoclock);

  if (argc > 1) {
    int h, m, s;
    if (sscanf(argv[1], "%d:%d:%d", &h, &m, &s) == 3 &&
        sclock_valid((struct sclock_t){h, m, s})) {
      autoclock.sclock = (struct sclock_t){h, m, s};
    } else {
      printf("invalid time format\n");
    }
  }

  pthread_t display;
  pthread_create(&display, NULL, (void* (*)(void*))tube_render, &tube);
  pthread_t clocker;
  pthread_create(&clocker, NULL, (void* (*)(void*))autoclock_loop, &autoclock);

  autoclock.state = CLOCK_RUNNING;

  for (;;) {
    char buf[10];
    autoclock_dump(buf, autoclock);
    tube_print(&tube, buf);
    usleep(300000);
  }

  pthread_join(display, &(void*){0});
  pthread_join(clocker, &(void*){0});

  munmap(addr, 0x1cc + sizeof(uint8_t));
  close(mem);

  return 0;
}
