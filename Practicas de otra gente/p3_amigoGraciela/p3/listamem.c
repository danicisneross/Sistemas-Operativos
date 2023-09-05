/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "listamem.h"

//Creacion e insercion en la lista

void createListMem(tListaMem *L){

	*L = NULL;

}

bool crearNodoMem(posicionMem *p){

	*p = malloc(sizeof(**p));

	return *p != NULL;

}

bool insertElementMem(elementMem e, tListaMem *L){

	posicionMem p, q;

	if(!crearNodoMem(&p))
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

//Busquedas en la lista

posicionMem findElementShd(tListaMem *L, int cl){
	posicionMem q = NULL;
	bool found = false;

	for(posicionMem p = firstMem(*L); p != NULL && !found  ; p=tNextMem(*L,p) ){

		if( strcmp(p->elemento.typeAlloc, "shared")==0 ){
				if(p->elemento.cl == cl){
					found=true;
					q = p;
				}
			}
		}

	return q;

}

posicionMem findElementMllc(tListaMem *L, int size){
	posicionMem p;
	posicionMem q = NULL;
	bool found = false;

	for(p = firstMem(*L); p != NULL && !found  ; p=tNextMem(*L,p) ){
		if( strcmp(p->elemento.typeAlloc, "malloc")==0 ){
				if(p->elemento.sizeBlock == size){
					found=true;
					q = p;
				}
			}
		}

		return q;
}

posicionMem findElement(tListaMem *L, void *punt){
	posicionMem p;
	posicionMem q = NULL;
	bool found = false;

	for(p = firstMem(*L); p != NULL && !found  ; p=tNextMem(*L,p) ){
		if( p->elemento.memAddress == punt ){
					found=true;
					q = p;
			}
		}

		return q;
}


posicionMem findElementMmap(tListaMem *L, char *fichN){
	posicionMem q = NULL;
	bool found = false;

	for(posicionMem p = firstMem(*L); p != NULL && !found  ; p=tNextMem(*L,p) ){

		if( strcmp(p->elemento.typeAlloc, "mmap")==0 ){
				if( strcmp(p->elemento.fichName,fichN)==0 ){
					found=true;
					q = p;
				}
			}
		}

	return q;

}

void printListaMem(tListaMem L, char met[]){

	posicionMem p;
	pid_t pid= getpid();

	printf("\nLista de bloques asignados");

	if(strcmp( "all" , met)!=0){
		printf(" %s", met);
	}

	printf(" para el proceso %d\n\n",pid);

	for(p = L; p != NULL; p=p->next ){

		elementMem item;


		if( strcmp(p->elemento.typeAlloc, met)==0 || strcmp( "all" , met)==0 ){

				item= p->elemento;
				printf("%12p: %4s%10d. %6s ", item.memAddress, "size:", item.sizeBlock, item.typeAlloc);

				if( strcmp(item.typeAlloc,"mmap")==0 ){
					printf(" %10s %6s%d) ",item.fichName,"(fd:", item.cl);
				}

				if( strcmp(item.typeAlloc,"shared")==0 ){
					printf(" %16s%d) ", "(key:", item.cl);
				}

				if( strcmp(item.typeAlloc,"malloc")==0 ){
					printf("%21s","");
				}


				printf("%20s", item.timeAlloc);

		}
	}
}

//Eliminaciones en la lista

void deleteElementMem(tListaMem *L, posicionMem p){

	posicionMem q;

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

void deleteListMem(tListaMem *L){
	void *q;
	posicionMem p;
	elementMem item;

	for(p = firstMem(*L); p != NULL ; p=firstMem(*L) ){

		item=getElementMem(*L, p);
		q=item.memAddress;
		if(strcmp(item.typeAlloc,"mmap")==0){
			munmap(q,item.sizeBlock);
		}
		else if(strcmp(item.typeAlloc,"shared")!=0){ free(q); }
		deleteElementMem(L,p);
	}
}

//Movimiento por la lista

elementMem getElementMem(tListaMem L, posicionMem p){
	return p->elemento;
}

posicionMem lastMem(tListaMem L){
	posicionMem p;

	for(p=L ; p->next != NULL ; p=p->next);
	return p;
}

posicionMem firstMem(tListaMem L){
	return L;
}

posicionMem tNextMem(tListaMem L, posicionMem p){

	p = p->next;
	return p;
}
