/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#ifndef P3_SO_BG_LIST_H
#define P3_SO_BG_LIST_H

#include <stdbool.h>
#include <sys/types.h>

#define LNULL NULL

typedef struct tItemP {
    pid_t pid;
    char *user;
    char *command;
    char *time;
    int state;
    int end;
} tItemP;

typedef struct tNodeP *tPosP;
typedef struct tNodeP {
    tItemP item;
    tPosP next;
} tNodeP;
typedef tNodeP *tListP;


void createEmptyListP(tListP *L);

bool isEmptyListP(tListP L);

tPosP firstP(tListP L);

tPosP lastP(tListP L);

tPosP previousP(tPosP p, tListP L);

tPosP nextP(tPosP p, tListP L);

tItemP getItemP(tPosP p, tListP L);

tPosP findPosP(pid_t pid, tListP L);

void updateItem(tItemP i, tPosP p, tListP *L);

void deleteItemP(tPosP p, tListP *L);

void deleteListP(tListP *L);

void clearListP(tListP *L);

bool insertItemP(tItemP i, tListP *L);

#endif //P3_SO_BG_LIST_H