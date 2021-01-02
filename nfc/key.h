#define KEYCODE_ONE 2    /* 1 */
#define KEYCODE_TWO 3    /* 2 */
#define KEYCODE_THREE 4  /* 3 */
#define KEYCODE_FOUR 5   /* 4 */
#define KEYCODE_FIVE 6   /* 5 */
#define KEYCODE_SIX 7    /* 6 */
#define KEYCODE_SEVEN 8  /* 7 */
#define KEYCODE_EIGHT 9  /* 8 */
#define KEYCODE_NINE 10  /* 9 */
#define KEYCODE_ZERO 115 /* 0, middle bottom */
#define KEYCODE_LB 1     /* left bottom */
#define KEYCODE_RB 114   /* right bottom */
#define KEYCODE_ASTERISK KEYCODE_LB
#define KEYCODE_HASHTAG KEYCODE_RB
#define KEYCODE_BACKSPACE KEYCODE_LB
#define KEYCODE_ENTER KEYCODE_RB
#define KEYVAL_UNDEFINED -1

/* keyboard device */
extern int kbd;

/* blocking wait on one key */
unsigned short getkey();

/* key code -> value (0-9,undef) */
int keyval(unsigned short code);
