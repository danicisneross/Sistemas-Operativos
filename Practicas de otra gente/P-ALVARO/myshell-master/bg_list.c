/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include "bg_list.h"

void createEmptyListP(tListP *L) {
    *L = LNULL;
}

bool isEmptyListP(tListP L) {
    return L == LNULL;
}

tPosP firstP(tListP L) {
    return L;
}

tPosP lastP(tListP L) {
    tPosP pos;

    if (isEmptyListP(L)) {
        return LNULL;
    }

    for (pos = L; pos->next != LNULL; pos = pos->next);
    return pos;
}

tPosP previousP(tPosP p, tListP L) {
    tPosP pos;

    if (p == L) {
        pos = LNULL;
    } else {
        for (pos = L; pos->next != p; pos = pos->next);
    }
    return pos;
}

tPosP nextP(tPosP p, tListP L) {
    return p->next;
}

tItemP getItemP(tPosP p, tListP L) {
    return p->item;
}

tPosP findPosP(pid_t pid, tListP L) {
    tPosP pos;

    for (pos = L; (pos != LNULL); pos = pos->next) {
        if (getItemP(pos, L).pid == pid) {
            return pos;
        }
    }
    return pos;
}

void updateItem(tItemP item, tPosP pos, tListP *L) {
    pos->item = item;
}

void deleteItemP(tPosP p, tListP *L) {
    if (p == *L) {
        *L = p->next;
    } else {
        previousP(p, *L)->next = p->next;
    }
    free(p);
}

void deleteListP(tListP *L) {
    tPosP lastpos, aux;

    if (isEmptyListP(*L)) {
        free(*L);
    } else {
        lastpos = lastP(*L);
        aux = previousP(lastpos, *L);
        if (aux == LNULL) {
            free(lastpos);
            *L = LNULL;
            deleteListP(L);
        } else {
            aux->next = LNULL;
            free(lastpos);
            deleteListP(L);
        }
    }
}

void clearListP(tListP *L) {
    tPosP pos;

    if (!isEmptyListP(*L)) {
        for (pos = *L; (pos != LNULL); pos = pos->next) {
            deleteItemP(pos, L);
        }
    }

}

bool insertItemP(tItemP i, tListP *L) {
    tPosP node;

    node = malloc(sizeof(struct tNodeP));

    if (node == LNULL) {
        return false;
    } else {
        node->item = i;
        node->next = LNULL;

        if (*L == LNULL) {
            *L = node;
        } else {
            lastP(*L)->next = node;
        }
    }
    return true;
}