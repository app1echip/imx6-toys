#include <stdint.h> /* uint8_t */
#include <stdio.h>  /* FILE */

#define VLABLEN 0x10 /* number of elements per block */

/* define generic variable-length array */
typedef struct {
  uint8_t* _buf; /* malloc'd buffer pointer */
  uint8_t _wid;  /* sizeof(<Type>) */
  unsigned len;  /* number of elements stored */
  unsigned _blo; /* number of blocks available */
} vla_t;

/* create vla of 0 length */
vla_t vnew(uint8_t width);
/* free one vla created by 'vnew' or 'vload' */
void vfree(vla_t* vla);
/* return element of given index */
void* vat(vla_t vla, int index);
/* insert element into given index */
void* vins(vla_t* vla, int index, void* data);
/* remove element of given index */
void vrm(vla_t* vla, int index);
/* vla's version of push_back() */
void* vpush(vla_t* vla, void* data);
/* dump vla into binary file */
void vdump(FILE* fp, vla_t vla);
/* read vla from binary file */
vla_t vload(FILE* fp);