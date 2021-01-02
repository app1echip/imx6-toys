#include <stdint.h> /* uintN_t */

/* nfc controller device */
extern int nfc;
/* wake up controller */
void nfc_wakeup();
/* send command frame to controller */
void write_cmd(const void* pd, uint8_t n);
/* digest ACK response */
void digest_ack();
/* receive response frame from controller */
void read_ans(void* buf, uint8_t* n);
/* wait for card and return uid */
uint32_t fetch_uid();