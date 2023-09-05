/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include "listaproc.h"

//Creacion e insercion en la lista

void createListProc(tListaProc *L){

	*L = NULL;

}

bool crearNodoProc(posicionProc *p){

	*p = malloc(sizeof(**p));

	return *p != NULL;

}

bool insertElementProc(elementProc e, tListaProc *L){

	posicionProc p, q;

	if(!crearNodoProc(&p))
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

posicionProc findElementProc(tListaProc *L, int pid){
	posicionProc p;
	posicionProc q = NULL;
	bool found = false;

	for(p = firstProc(*L); p != NULL && !found  ; p=tNextProc(*L,p) ){
		if( p->elemento.pid == pid ){
					found=true;
					q = p;
			}
		}

		return q;
}

void printProc(posicionProc p, tListaProc *L){

	elementProc item = p->elemento;

	char *time = item.timeD;	//Eliminar el /n que se inserta al final del asctime
	time[strlen(time) - 1] = 0;

	printf("%4d %8s %2s%d %s", item.pid, item.user,"p=",item.priority, item.timeD);
	printf(" %s ", item.stat);

	if( (strcmp(item.stat, "SENALADO")==0) || (strcmp(item.stat, "PARADO")==0) ){
				printf("(%.4s) ", NombreSenal(item.senal));
	} else{ printf("(%.3d) ", item.senal); }

	printf("%s\n",item.commandLine);

}

void printListaProc(tListaProc *L){

	posicionProc p;
	for(p = firstProc(*L); p != NULL; p=p->next ){
				estadojob(p, L);
				printProc(p, L);
	}

}


int Senal(char * sen){
	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (!strcmp(sen, sigstrnum[i].nombre))
			return sigstrnum[i].senal;
	return -1;
}

char *NombreSenal(int sen){ /* para sitios donde no hay sig2str*/
	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (sen==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	return ("SIGUNKNOWN");
}


//Eliminaciones en la lista

void deleteElementProc(tListaProc *L, posicionProc p){

	posicionProc q;

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

void updateItem(elementProc d, posicionProc p, tListaProc *L){
	p->elemento = d;
}

void deleteListProc(tListaProc *L){
	posicionProc p;
	elementProc item;

	for(p = firstProc(*L); p != NULL ; p=firstProc(*L) ){
		deleteElementProc(L,p);
	}
}

void borrarterm(tListaProc* L){
	posicionProc p = firstProc(*L);

	while(p != NULL){
		estadojob(p, L);
		if (strcmp(p->elemento.stat,"TERMINADO")==0){
			deleteElementProc(L,p);
			p = firstProc(*L);
		}
		else{ p=tNextProc(*L, p); }
	}
}

void borrarsig(tListaProc* L){
	posicionProc p = firstProc(*L);

	while(p != NULL){
		estadojob(p, L);
		if (strcmp(p->elemento.stat,"SENALADO")==0){
			deleteElementProc(L,p);
			p=firstProc(*L);
		}
		else{ p=tNextProc(*L, p); }
	}
}

//para job
void estadojob(posicionProc p, tListaProc* L){
	int est;
	elementProc item = p->elemento;

	strcpy(item.stat,"TERMINADO");
	item.senal=0;

        int which = PRIO_PROCESS;
        int priority;

	if( waitpid(item.pid, &est, WNOHANG | WUNTRACED | WCONTINUED) == item.pid ){  //mira si hay algun cambio

		if(WIFEXITED(est)){
			strcpy(item.stat,"TERMINADO");
			item.senal=0;
		}else	if(WIFSIGNALED(est)){
			strcpy(item.stat,"SENALADO");
			item.senal=WTERMSIG(est);
		}else if(WIFSTOPPED(est)){
			strcpy(item.stat,"PARADO");
			item.senal=WSTOPSIG(est);
		}else if(WIFCONTINUED(est)){
			strcpy(item.stat,"ACTIVO");
			item.senal=SIGCONT;
		}
		else{
			strcpy(item.stat,"ACTIVO");
			item.senal=-1;
		}
      priority = getpriority(which, item.pid);
      item.priority=priority;
      updateItem(item, p, L);
	}
}

//Movimiento por la lista

elementProc getElementProc(tListaProc L, posicionProc p){
	return p->elemento;
}

posicionProc lastProc(tListaProc L){
	posicionProc p;

	for(p=L ; p->next != NULL ; p=p->next);
	return p;
}

posicionProc firstProc(tListaProc L){
	return L;
}

posicionProc tNextProc(tListaProc L, posicionProc p){

	p = p->next;
	return p;
}
