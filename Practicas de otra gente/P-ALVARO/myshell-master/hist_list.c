/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#include "hist_list.h"
#include <string.h>

void createEmptyListC(tListC *L) {
    *L = LNULL;
}

bool isEmptyListC(tListC L) {
    return L == LNULL;
}

tPosC firstC(tListC L) {
    return L;
}

tPosC lastC(tListC L) {
    tPosC pos;

    if (isEmptyListC(L))
        return LNULL;

    for (pos = L; pos->next != LNULL; pos = pos->next);
    return pos;
}

tPosC previousC(tPosC p, tListC L) {
    tPosC pos;

    if (p == L)
        pos = LNULL;
    else
        for (pos = L; pos->next != p; pos = pos->next);
    return pos;
}

tPosC nextC(tPosC p, tListC L) {
    return p->next;
}

tItemC getItemC(tPosC p, tListC L) {
    return p->item;
}

tPosC findItemC(int n, tListC L) {
    int cnt = 1;
    tPosC pos;

    for (pos = L; (pos != LNULL); pos = pos->next) {
        if (cnt == n)
            return pos;
        else
            cnt++;
    }
    return pos;
}

void deleteAtPositionC(tPosC p, tListC *L) {
    if (p == *L) {
        *L = p->next;
    } else
        previousC(p, *L)->next = p->next;
    free(p);
}

void deleteListC(tListC *L) {
    tPosC lastpos, aux;

    if (isEmptyListC(*L))
        free(*L);
    else {
        lastpos = lastC(*L);
        aux = previousC(lastpos, *L);
        if (aux == LNULL) {
            free(lastpos);
            *L = LNULL;
            deleteListC(L);
        } else {
            aux->next = LNULL;
            free(lastpos);
            deleteListC(L);
        }
    }
}

bool insertItemC(tItemC i, tListC *L) {
    tPosC node;

    node = malloc(sizeof(struct tNodeC));

    if (node == LNULL)
        return false;
    else {
        node->item = i;
        node->next = LNULL;

        if (*L == LNULL)
            *L = node;
        else
            lastC(*L)->next = node;
    }
    return true;
}
