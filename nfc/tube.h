#include <stdint.h> /* uint8_t */

#define TUBE_LEN 8 /* number of display digits */

struct tube_t {
  uint8_t _buf[TUBE_LEN];
  uint8_t len;
  uint8_t dot;
};

extern int mem;
extern uint8_t* addr;

void tube_init();
void tube_render(void* arg);
void tube_print(const char* str);
void tube_dot(uint8_t bits);