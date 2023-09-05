//
// Created by mateo on 29/11/2021.
//

#include "env_list.h"
#include <stdlib.h>
#include <string.h>

void createEmptyListE(tListE *L) {
    *L = LNULL;
}

bool isEmptyListE(tListE L) {
    return L == LNULL;
}

tPosE firstE(tListE L) {
    return L;
}

tPosE lastE(tListE L) {
    tPosE pos;

    if (isEmptyListE(L)) {
        return LNULL;
    }

    for (pos = L; pos->next != LNULL; pos = pos->next);
    return pos;
}

tPosE previousE(tPosE p, tListE L) {
    tPosE pos;

    if (p == L) {
        pos = LNULL;
    } else {
        for (pos = L; pos->next != p; pos = pos->next);
    }
    return pos;
}

tPosE nextE(tPosE p, tListE L) {
    return p->next;
}

tItemE getItemE(tPosE p, tListE L) {
    return p->item;
}

tPosE findPosE(char *name, tListE L) {
    char *var, *aux;
    tPosE pos;

    aux = malloc(strlen(name) + 1);
    strcpy(aux, name);

    var = strtok(aux, "=");

    for (pos = L; (pos != LNULL); pos = pos->next) {
        if (strcmp(strtok(getItemE(pos, L).name, "="), var) == 0) {
            free(aux);
            return pos;
        }
    }
    free(aux);
    return pos;
}

void deleteItemE(tPosE p, tListE *L) {
    if (p == *L) {
        *L = p->next;
    } else {
        previousE(p, *L)->next = p->next;
    }
    free(p);
}

void deleteListE(tListE *L) {
    tPosE lastpos, aux;

    if (isEmptyListE(*L)) {
        free(*L);
    } else {
        lastpos = lastE(*L);
        aux = previousE(lastpos, *L);
        if (aux == LNULL) {
            free(lastpos);
            *L = LNULL;
            deleteListE(L);
        } else {
            aux->next = LNULL;
            free(lastpos);
            deleteListE(L);
        }
    }
}

bool insertItemE(tItemE i, tListE *L) {
    tPosE node;

    node = malloc(sizeof(struct tNodeE));

    if (node == LNULL) {
        return false;
    } else {
        node->item = i;
        node->next = LNULL;

        if (*L == LNULL) {
            *L = node;
        } else {
            lastE(*L)->next = node;
        }
    }
    return true;
}