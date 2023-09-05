
#include <stdio.h>
#include<stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_car 100

typedef char cmd[MAX_car];



typedef struct tNode* tPosL;
typedef struct {
  cmd comando[MAX_car];
}tItemL;

struct tNode{
tItemL data;
tPosL next;
};

typedef tPosL tList;

void createList(tList* L);

bool isEmptyList(tList L);

tPosL first(tList L);

tPosL next (tPosL , tList L);

tItemL getItem(tPosL p, tList* L);

bool insertElement(tItemL d, tList* L);

void removeElement(tList* L);

