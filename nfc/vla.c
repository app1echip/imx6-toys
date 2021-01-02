
#include "vla.h"

#include <stdlib.h> /* malloc(), realloc() */
#include <string.h> /* memmove(), memcpy() */

vla_t vnew(uint8_t w) {
  vla_t v;
  v._wid = w;
  v._blo = 1;
  v.len = 0;
  v._buf = (uint8_t*)malloc(v._wid * VLABLEN * v._blo);
  return v;
}

void* vat(vla_t v, int i) {
  return i >= 0 && i < v.len ? v._buf + i * v._wid : NULL;
}

void* vins(vla_t* v, int id, void* el) {
  if (id < 0 || id > v->len) {
    return NULL;
  }
  if (id >= v->_blo * VLABLEN) {
    v->_blo++;
    v->_buf = (uint8_t*)realloc(v->_buf, v->_wid * VLABLEN * v->_blo);
    if (v->_buf == NULL) return NULL;
  }
  v->len++;
  memmove(vat(*v, id + 1), vat(*v, id), (v->len - 1 - id) * v->_wid);
  memcpy(vat(*v, id), el, v->_wid);
  return vat(*v, id);
}

void vrm(vla_t* v, int id) {
  if (id < 0 || id >= v->len) return;
  memmove(vat(*v, id), vat(*v, id + 1), (v->len - id) * v->_wid);
  v->len--;
}

void* vpush(vla_t* v, void* el) { return vins(v, v->len, el); }

void vdump(FILE* f, vla_t v) {
  fwrite(&v._wid, sizeof(v._wid), 1, f);
  fwrite(&v.len, sizeof(v.len), 1, f);
  fwrite(v._buf, v._wid, v.len, f);
}

vla_t vload(FILE* f) {
  vla_t v;
  fread(&v._wid, sizeof(v._wid), 1, f);
  fread(&v.len, sizeof(v.len), 1, f);
  v._blo = v.len / VLABLEN + v.len % VLABLEN != 0;
  v._buf = (uint8_t*)malloc(v._wid * VLABLEN * v._blo);
  fread(v._buf, v._wid, v.len, f);
  return v;
}

void vfree(vla_t* vla) { free(vla->_buf); }