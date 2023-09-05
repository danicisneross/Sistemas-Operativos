#ifndef P3_SO_ENV_LIST_H
#define P3_SO_ENV_LIST_H

#include <stdbool.h>

#define LNULL NULL

typedef struct tItemE {
    char *name;
} tItemE;

typedef struct tNodeE *tPosE;
typedef struct tNodeE {
    tItemE item;
    tPosE next;
} tNodeE;
typedef tNodeE *tListE;


void createEmptyListE(tListE *L);

bool isEmptyListE(tListE L);

tPosE firstE(tListE L);

tPosE lastE(tListE L);

tPosE previousE(tPosE p, tListE L);

tPosE nextE(tPosE p, tListE L);

tItemE getItemE(tPosE p, tListE L);

tPosE findPosE(char *name, tListE L);

void deleteItemE(tPosE p, tListE *L);

void deleteListE(tListE *L);

bool insertItemE(tItemE i, tListE *L);

#endif //P3_SO_ENV_LIST_H