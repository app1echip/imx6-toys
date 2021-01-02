#include "art.h"

uint8_t art(char c) {
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
    case 'A':
      return ART_UPPER_A;
    case 'b':
      return ART_LOWER_B;
    case 'H':
      return ART_UPPER_H;
    case 'h':
      return ART_LOWER_H;
    case 'O':
      return ART_UPPER_O;
    case 'L':
      return ART_UPPER_L;
    case '_':
      return ART_UNDERLINE;
    default:
      return ART_BLANK;
  }
}