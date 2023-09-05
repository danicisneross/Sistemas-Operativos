/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#ifndef PRUEBA_SO_HIST_LIST_H
#define PRUEBA_SO_HIST_LIST_H

#include <stdlib.h>
#include <stdbool.h>

#define LNULL NULL

typedef char *tCommandName;
typedef struct tItemC {
    tCommandName CommandName;
} tItemC;
typedef struct tNodeC *tPosC;
typedef struct tNodeC {
    tItemC item;
    tPosC next;
} tNodeC;
typedef tNodeC *tListC;

void createEmptyListC(tListC *L);

bool isEmptyListC(tListC L);

tPosC firstC(tListC L);

tPosC lastC(tListC L);

tPosC previousC(tPosC p, tListC L);

tPosC nextC(tPosC p, tListC L);

tItemC getItemC(tPosC p, tListC L);

tPosC findItemC(int n, tListC L);

void deleteAtPositionC(tPosC p, tListC *L);

void deleteListC(tListC *L);

bool insertItemC(tItemC i, tListC *L);


#endif //PRUEBA_SO_HIST_LIST_H
