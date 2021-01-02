#include "database.h"

#include <stdio.h>  /* FILE, ... */
#include <stdlib.h> /* malloc(), realloc() */
#include <string.h> /* strdup() */
#include <unistd.h> /* access() */
static struct database db;

void db_load(const char* name) {
  db._fn = strdup(name);
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [database] use %s\n", db._fn);
#endif
  if (access(db._fn, F_OK) != -1) {
    FILE* f = fopen(db._fn, "rb");
    int n = fread(&db.len, sizeof(db.len), 1, f);
    if (n > 0) {
      db.c = (struct cinfo*)malloc(sizeof(struct cinfo) * db.len);
      for (int i = 0; i < db.len; ++i) {
        fread(&db.c[i].uid, sizeof(db.c[i].uid), 1, f);
        fread(&db.c[i].bal, sizeof(db.c[i].bal), 1, f);
        db.c[i].log = vload(f);
      }
    }
    fclose(f);
#if defined(DEBUG)
    printf("DEBUG: [database] verbose\n");
    for (int i = 0; i < db.len; ++i) {
      printf("       %-4d uid: %08x (uint32_t)\n", i, db.c[i].uid);
      printf("            bal: %d\n", db.c[i].bal);
      printf("            log: %d lines\n", db.c[i].log.len);
      for (int j = 0; j < db.c[i].log.len; ++j) {
        printf("                 %d\n", *(int*)vat(db.c[i].log, j));
      }
    }
#endif
  } else {
    db.len = 0;
    db.c = (struct cinfo*)malloc(0);
#if defined(DEBUG) || defined(INFO)
    printf("INFO: [database] empty\n");
#endif
  }
}

void db_free() {
  for (int i = 0; i < db.len; ++i) vfree(&db.c[i].log);
  free(db.c);
  free(db._fn);
}

void db_dump() {
  FILE* f = fopen(db._fn, "wb");
  fwrite(&db.len, sizeof(db.len), 1, f);
  for (int i = 0; i < db.len; ++i) {
    fwrite(&db.c[i].uid, sizeof(db.c[i].uid), 1, f);
    fwrite(&db.c[i].bal, sizeof(db.c[i].bal), 1, f);
    vdump(f, db.c[i].log);
  }
  fclose(f);
#if defined(DEBUG)
  printf("DEBUG: [database] changes saved\n");
#endif
}

struct cinfo* db_fetch(uint32_t uid) {
  for (int i = 0; i < db.len; ++i)
    if (db.c[i].uid == uid) return db.c + i;
  return NULL;
}

int db_find(uint32_t uid) {
  for (int i = 0; i < db.len; ++i)
    if (db.c[i].uid == uid) return i;
  return -1;
}

int db_update(uint32_t uid, int rec) {
  int ind = db_find(uid);
  if (ind == -1) {
#if defined(DEBUG) || defined(INFO)
    printf("INFO: [database] new card\n");
#endif
    ind = db.len++;
    db.c = (struct cinfo*)realloc(db.c, sizeof(struct cinfo) * db.len);
    db.c[ind].uid = uid;
    db.c[ind].bal = 0;
    db.c[ind].log = vnew(sizeof(int));
  }
  int after = db.c[ind].bal + rec;
#if defined(DEBUG) || defined(INFO)
  printf("INFO: [transaction] %08x: %7d %+7d -> %7d, ", uid, db.c[ind].bal, rec,
         after);
#endif
  if (after < CBALMIN || after > CBALMAX) {
#if defined(DEBUG) || defined(INFO)
    printf("bad\n");
#endif
    return -1;
  }
#if defined(DEBUG) || defined(INFO)
  printf("ok\n");
#endif
  db.c[ind].bal = after;
  vpush(&db.c[ind].log, &rec);
  return 0;
}
