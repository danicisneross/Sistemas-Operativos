#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lproc.h"


void createListp(tListP* L){
*L = NULL;
}

bool isEmptyListp(tListP L){
return L == NULL;
}

tPosP firstp(tListP L){
return L;
}

tPosP nextp(tPosP p, tListP L){
        return p->next;
}

tItemP getItemp(tPosP p, tListP L){
return p->data ; // a función de tipo tItemL tenque devolver tItemL
}

bool createNodep(tPosP* p){
*p = malloc(sizeof(**p));
return *p != NULL;
}

bool insertElementp(tItemP d, tListP* L){
tPosP q, r;
bool i;
// agora comprobamos se hai espazo
if (!createNodep(&q))
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

void removeElementp(tListP *L, tPosP p){
tPosP q;
if (p==*L)  
*L = (*L)->next;
else{
if (p->next == NULL){
  for (q =*L; q->next != p; q=q->next);
  q->next = NULL;
}
 else {
    q = p->next;
    p->data = q->data;
    p->next = q->next;
    p = q;
  } 
}
free(p);
}

void updateListp(tItemP d, tPosP p, tListP *L){
  if(!isEmptyListp(*L))
    p->data = d;

}