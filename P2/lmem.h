
#include <stdio.h>
#include<stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXI 1024

typedef char mem[MAXI];



typedef struct tNodem* tPosMem;
typedef struct {
  void *direc;
  int tam;
  mem tempo;
  mem tipo;
  // E para ficheiros mapeados :
  int chave;
  mem nomefich;
  int df; 
}tItemMem;

struct tNodem{
tItemMem data;
tPosMem next;
};

typedef tPosMem tListMem;

void createListm(tListMem* L);

bool isEmptyListm(tListMem L);

tPosMem firstm(tListMem L);

tPosMem nextm(tPosMem , tListMem L);

tItemMem getItemm(tPosMem p, tListMem L);

bool insertElementm(tItemMem d, tListMem* L);

void removeElementm(tListMem* L, tPosMem p);

