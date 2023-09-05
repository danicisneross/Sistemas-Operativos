

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "historial.h"

void createList(tList* L){
*L = NULL;
}

bool isEmptyList(tList L){
return L == NULL;
}

tPosL first(tList L){
return L;
}

tPosL next (tPosL p, tList L){
        return p->next;
}

tItemL getItem(tPosL p, tList* L){
return p->data ; // a función de tipo tItemL tenque devolver tItemL
}

bool createNode(tPosL* p){
*p = malloc(sizeof(**p));
return *p != NULL;
}

bool insertElement(tItemL d, tList* L){
tPosL q, r;
bool i;
// agora comprobamos se hai espazo
if (!createNode(&q))
i = false;
else{
q->data = d;
q->next=NULL;
if(*L==NULL){
*L = q;
} else{
for(r=*L; r->next != NULL; r = r->next);
r->next = q;
  }
i = true; }
return i;
}

void removeElement(tList *L){
tPosL q;
while(*L != NULL){
q = *L;
*L = (*L)->next;
free(q);}
}
