#include <stdint.h>

#define ART_ZERO 0xc0
#define ART_ONE 0xf9
#define ART_TWO 0xa4
#define ART_THREE 0xb0
#define ART_FOUR 0x99
#define ART_FIVE 0x92
#define ART_SIX 0x82
#define ART_SEVEN 0xf8
#define ART_EIGHT 0x80
#define ART_NINE 0x90

#define ART_HYPHEN 0xbf
#define ART_SPACE 0xff
#define ART_BLANK ART_SPACE

#define ART_UPPER_E 0x86
#define ART_LOWER_R 0xaf
#define ART_LOWER_O 0xa3

uint8_t tube_art(char c) {
  switch (c) {
    case '0':
      return ART_ZERO;
    case '1':
      return ART_ONE;
    case '2':
      return ART_TWO;
    case '3':
      return ART_THREE;
    case '4':
      return ART_FOUR;
    case '5':
      return ART_FIVE;
    case '6':
      return ART_SIX;
    case '7':
      return ART_SEVEN;
    case '8':
      return ART_EIGHT;
    case '9':
      return ART_NINE;
    case '-':
      return ART_HYPHEN;
    case ' ':
      return ART_SPACE;
    case 'E':
      return ART_UPPER_E;
    case 'r':
      return ART_LOWER_R;
    case 'o':
      return ART_LOWER_O;
    default:
      return ART_BLANK;
  }
}

char reverse_art(uint8_t b) {
  switch (b) {
    case ART_ZERO:
      return '0';
    case ART_ONE:
      return '1';
    case ART_TWO:
      return '2';
    case ART_THREE:
      return '3';
    case ART_FOUR:
      return '4';
    case ART_FIVE:
      return '5';
    case ART_SIX:
      return '6';
    case ART_SEVEN:
      return '7';
    case ART_EIGHT:
      return '8';
    case ART_NINE:
      return '9';
    case ART_HYPHEN:
      return '-';
    case ART_SPACE:
      return ' ';
    case ART_UPPER_E:
      return 'E';
    case ART_LOWER_R:
      return 'r';
    case ART_LOWER_O:
      return 'o';
    default:
      return ' ';
  }
}