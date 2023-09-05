
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

void createListm(tListMem*);

bool isEmptyListm(tListMem);

tPosMem firstm(tListMem);

tPosMem nextm(tPosMem , tListMem );

tItemMem getItemm(tPosMem , tListMem );

bool insertElementm(tItemMem , tListMem* );

void removeElementm(tListMem* , tPosMem );

