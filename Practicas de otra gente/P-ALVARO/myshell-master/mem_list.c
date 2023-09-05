/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include "mem_list.h"

void createEmptyListM(tListM *L) {
    *L = LNULL;
}

bool isEmptyListM(tListM L) {
    return L == LNULL;
}

tPosM firstM(tListM L) {
    return L;
}

tPosM lastM(tListM L) {
    tPosM pos;

    if (isEmptyListM(L)) {
        return LNULL;
    }

    for (pos = L; pos->next != LNULL; pos = pos->next);
    return pos;
}

tPosM previousM(tPosM p, tListM L) {
    tPosM pos;

    if (p == L) {
        pos = LNULL;
    } else {
        for (pos = L; pos->next != p; pos = pos->next);
    }
    return pos;
}

tPosM nextM(tPosM p, tListM L) {
    return p->next;
}

tItemM getItemM(tPosM p, tListM L) {
    return p->item;
}

tPosM findPosM(size_t size, tListM L) {
    tPosM pos;

    for (pos = L; (pos != LNULL); pos = pos->next) {
        if (getItemM(pos, L).size == size) {
            return pos;
        }
    }
    return pos;
}

void deleteItemM(tPosM p, tListM *L) {
    if (p == *L) {
        *L = p->next;
    } else {
        previousM(p, *L)->next = p->next;
    }
    free(p);
}

void deleteListM(tListM *L) {
    tPosM lastpos, aux;

    if (isEmptyListM(*L)) {
        free(*L);
    } else {
        lastpos = lastM(*L);
        aux = previousM(lastpos, *L);
        if (aux == LNULL) {
            free(lastpos);
            *L = LNULL;
            deleteListM(L);
        } else {
            aux->next = LNULL;
            free(lastpos);
            deleteListM(L);
        }
    }
}

bool insertItemM(tItemM i, tListM *L) {
    tPosM node;

    node = malloc(sizeof(struct tNodeM));

    if (node == LNULL) {
        return false;
    } else {
        node->item = i;
        node->next = LNULL;

        if (*L == LNULL) {
            *L = node;
        } else {
            lastM(*L)->next = node;
        }
    }
    return true;
}
