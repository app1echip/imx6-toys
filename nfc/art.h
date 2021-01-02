#include <stdint.h> /* uint8_t */

#define ART_ZERO 0xc0      /* '0' */
#define ART_ONE 0xf9       /* '1' */
#define ART_TWO 0xa4       /* '2' */
#define ART_THREE 0xb0     /* '3' */
#define ART_FOUR 0x99      /* '4' */
#define ART_FIVE 0x92      /* '5' */
#define ART_SIX 0x82       /* '6' */
#define ART_SEVEN 0xf8     /* '7' */
#define ART_EIGHT 0x80     /* '8' */
#define ART_NINE 0x90      /* '9' */
#define ART_DOT 0x7f       /* '.' */
#define ART_HYPHEN 0xbf    /* '-' */
#define ART_SPACE 0xff     /* ' ' */
#define ART_UPPER_E 0x86   /* 'E' */
#define ART_LOWER_R 0xaf   /* 'r' */
#define ART_LOWER_O 0xa3   /* 'o' */
#define ART_UPPER_A 0x88   /* 'A' */
#define ART_LOWER_B 0x83   /* 'b' */
#define ART_UPPER_H 0x89   /* 'H' */
#define ART_LOWER_H 0x8b   /* 'h' */
#define ART_UPPER_O 0xc0   /* 'O' */
#define ART_UPPER_L 0xc7   /* 'O' */
#define ART_UNDERLINE 0xf7 /* ‘_’ */
#define ART_BLANK ART_SPACE

uint8_t art(char ch);
