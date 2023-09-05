/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#ifndef P2_SO_MEM_LIST_H
#define P2_SO_MEM_LIST_H

#include <stdbool.h>

#define LNULL NULL

typedef struct fich {
    int fd;
    char *filename;
} tFich;

typedef enum cmd {
    MALLOC, SHARED, MMAP
} cmd;

typedef struct tItemM {
    cmd cmdType;
    void *dir;
    size_t size;
    char date[128];
    union {
        int key;
        tFich fich;
    } Union;
} tItemM;

typedef struct tNodeM *tPosM;
typedef struct tNodeM {
    tItemM item;
    tPosM next;
} tNodeM;
typedef tNodeM *tListM;


void createEmptyListM(tListM *L);

bool isEmptyListM(tListM L);

tPosM firstM(tListM L);

tPosM lastM(tListM L);

tPosM previousM(tPosM p, tListM L);

tPosM nextM(tPosM p, tListM L);

tItemM getItemM(tPosM p, tListM L);

tPosM findPosM(size_t size, tListM L);

void deleteItemM(tPosM p, tListM *L);

void deleteListM(tListM *L);

bool insertItemM(tItemM i, tListM *L);

#endif //P2_SO_MEM_LIST_H
