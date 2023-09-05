/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include <stdbool.h>

#define MAX_ENTRADAMEM 500

typedef char datoMem[MAX_ENTRADAMEM];

typedef struct elementMem{
	void *memAddress;
	int sizeBlock;
	datoMem timeAlloc;
	datoMem typeAlloc;
	datoMem fichName;	//Mmap != NULL
	int cl;	//Shared != NULL & Mmap == fd
}elementMem;

typedef struct nodoMem* posicionMem;

struct nodoMem{
	elementMem elemento;
	posicionMem next;
};

typedef posicionMem tListaMem;

void createListMem(tListaMem*);
bool crearNodoMem(posicionMem*);
void printListaMem(tListaMem, char met[]);
bool insertElementMem(elementMem, tListaMem*);
void deleteElementMem(tListaMem*, posicionMem);
void deleteListMem(tListaMem*);
elementMem getElementMem(tListaMem, posicionMem);

posicionMem findElementShd(tListaMem*, int);
posicionMem findElementMllc(tListaMem*, int);
posicionMem findElementMmap(tListaMem*, char*);
posicionMem findElement(tListaMem*, void*);

posicionMem lastMem(tListaMem);
posicionMem firstMem(tListaMem);
posicionMem tNextMem(tListaMem, posicionMem);
