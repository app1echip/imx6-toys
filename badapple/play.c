#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  int mem = open("/dev/mem", O_RDWR);
  uint8_t* addr = mmap(NULL, 0x300 + sizeof(uint32_t) * 16, PROT_WRITE,
                       MAP_SHARED, mem, 0x8000000);
  uint32_t* data = (uint32_t*)(addr + 0x300);
  FILE* fp = fopen("data", "rb");
  for (int i = 0; i < 6572; i++) {
    for (int j = 0; j < 16; j++) fread(data + j, sizeof(uint16_t), 1, fp);
    usleep(33333);
  }
  fclose(fp);
  munmap(addr, 0x300 + sizeof(uint32_t) * 16);
  close(mem);
  return 0;
}