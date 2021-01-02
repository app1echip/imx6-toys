#include <stdint.h>

#include "vla.h"

#define CBALMAX 9999999
#define CBALMIN 0

/* define card info */
struct cinfo {
  uint32_t uid; /* nfc uid */
  unsigned bal; /* balance */
  vla_t log;    /* transaction history */
};

/* define database storage */
struct database {
  struct cinfo* c; /* array of cards */
  int len;         /* array length */
  char* _fn;       /* local file path */
};

/* load data from file */
void db_load(const char* fname);
/* free database from memory */
void db_free();
/* save data into file */
void db_dump();
/* insert one record into database */
int db_update(uint32_t uid, int record);
/* find card info by uid */
struct cinfo* db_fetch(uint32_t uid);