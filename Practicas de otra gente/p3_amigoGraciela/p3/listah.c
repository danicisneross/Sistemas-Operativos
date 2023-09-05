/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "listah.h"


void createList(tLista *L){

	*L = NULL;

}

bool crearNodo(posicion *p){

	*p = malloc(sizeof(**p));

	return *p != NULL;

}

bool insertElement(element e, tLista *L){

	posicion p, q;

	if(!crearNodo(&p))
		return false;
	else{
		p->elemento = e;
		p->next = NULL;

		if(*L==NULL)
			*L = p;
		else{
			for(q = *L ; q->next != NULL ; q = q->next);
			q->next = p;
		}
		return true;
	}

}

void deleteElement(tLista *L, posicion p){

	posicion q;

	if(p==*L){
		*L = (*L)->next;
	}
	else
		if(p->next == NULL){

		for(q=*L ; q->next != p ; q = q->next);
		q->next = NULL;
		}
		else{

		q = p->next;
		p->elemento = q->elemento;
		p->next = q->next;
		p = q;

		}

	free(p);

}

element getElement(tLista L, posicion p){
	return p->elemento;
}


posicion last(tLista L){
	posicion p;

	for(p=L ; p->next != NULL ; p=p->next);
	return p;
}

posicion first(tLista L){
	return L;
}

posicion tNext(tLista L, posicion p){

	p = p->next;
	return p;
}
