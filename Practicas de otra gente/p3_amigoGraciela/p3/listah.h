/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include <stdbool.h>

#define MAX_ENTRADA 500

typedef char dato[MAX_ENTRADA];

typedef struct element{
	dato datos;
}element;

typedef struct nodo* posicion;

struct nodo{
	element elemento;
	posicion next;
};

typedef posicion tLista;

void createList(tLista*);
bool crearNodo(posicion*);
bool insertElement(element, tLista*);
void deleteElement(tLista*, posicion);
element getElement(tLista, posicion);

posicion last(tLista);
posicion first(tLista);
posicion tNext(tLista, posicion);
