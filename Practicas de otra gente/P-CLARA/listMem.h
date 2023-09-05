#include <stdio.h>

#define NUM 200
#define TROZOS 30

typedef struct{
    void *memdir;
    char data[30];
    char tipo [20];
    char nome [20];
    int tam;
    int fd;
}tDataM;

typedef struct NodoM {
    tDataM data;
    struct NodoM *seguinte;
} NodoM;


typedef struct ListaM {
    NodoM *cabeza;
    int lonxitude;
} ListaM;



NodoM *crearNodoM(tDataM *dataM) {
    NodoM *nodo = malloc(sizeof(NodoM));
    nodo->data.memdir = dataM->memdir;    // *memdir
    strcpy(nodo->data.data, dataM->data);    // data
    strcpy(nodo->data.tipo, dataM->tipo);    // tipo
    strcpy(nodo->data.nome, dataM->nome);    // tipo
    nodo->data.tam = dataM->tam;   // tam
    nodo->data.fd=dataM->fd;    // fd
    nodo->seguinte = NULL;

    return nodo;
}

tDataM *obterM (char *fich, ListaM *lista, tDataM *dataM) {         // obter tData a partir do nome

    if (lista->cabeza == NULL) {
    } else {
        NodoM *punteiro = lista->cabeza;
        while (strcmp(punteiro->data.nome,fich) != 0 && punteiro->seguinte) {
            punteiro = punteiro->seguinte;
        }
        if (strcmp(punteiro->data.nome,fich) != 0) {

        } else {
            dataM->memdir = punteiro->data.memdir;    // *memdir
            strcpy(dataM->data, punteiro->data.data);    // data
            strcpy(dataM->tipo, punteiro->data.tipo);    // tipo
            strcpy(dataM->nome, punteiro->data.nome);    // tipo
            dataM->tam = punteiro->data.tam;   // tam
            dataM->fd = punteiro->data.fd;    // fd
        }
    }
    return dataM;
}

tDataM *obterM2 (void *dir, ListaM *lista, tDataM *dataM) {         // obter tData a partir da dirección

    if (lista->cabeza == NULL) {
    } else {

        NodoM *punteiro = lista->cabeza;
        while (punteiro->data.memdir != dir && punteiro->seguinte) {
            punteiro = punteiro->seguinte;
        }
        if (punteiro->data.memdir != dir) {

        } else {
            strcpy(dataM->nome,punteiro->data.nome);    // nome
            strcpy(dataM->data, punteiro->data.data);    // data
            strcpy(dataM->tipo, punteiro->data.tipo);    // tipo
            dataM->memdir = punteiro->data.memdir;    // tipo
            dataM->tam = punteiro->data.tam;   // tam
            dataM->fd = punteiro->data.fd;    // fd
        }
    }
    return dataM;
}

void insertarNodoM(ListaM *lista, tDataM *data) {

    NodoM *nodo = crearNodoM(data);
    if (lista->cabeza == NULL) {
        lista->cabeza = nodo;
    }
    else {
        NodoM *punteiro = lista->cabeza;
        while (punteiro->seguinte) {
            punteiro = punteiro->seguinte;
        }
        punteiro->seguinte = nodo;
    }
    lista->lonxitude++;
}


int borrarNodoM1(ListaM *lista, int tam) {
    NodoM *nodo = lista->cabeza;
    NodoM *anterior = NULL;

    if (nodo->data.tam == tam){

        if (lista->lonxitude==1) {      // Se é o 1º e único nodo
            free(nodo);
            lista->lonxitude = 0;
            lista->cabeza = NULL;
            return 0;
        } else{                         // Se é o 1º nodo
            nodo = lista -> cabeza;
            lista -> cabeza = nodo->seguinte;
            free(nodo);
            lista->lonxitude--;
            return 0;
        }
    }
    while (nodo->data.tam != tam && nodo->seguinte) {       // Se é un nodo do medio ou final
        anterior = nodo;
        nodo = nodo->seguinte;
    }
    if (nodo->data.tam != tam){
        return -1;
    }
    anterior->seguinte = nodo->seguinte;
    free(nodo);
    lista->lonxitude--;
    return 0;
}


int borrarNodoM2(ListaM *lista, char *nome) {
    NodoM *nodo = lista->cabeza;
    NodoM *anterior = NULL;

    if (strcmp(nodo->data.nome,nome) == 0){

        if (lista->lonxitude==1) {      // Se é o 1º e único nodo
            free(nodo);
            lista->lonxitude = 0;
            lista->cabeza = NULL;
            return 0;
        } else{                         // Se é o 1º nodo
            nodo = lista -> cabeza;
            lista -> cabeza = nodo->seguinte;
            free(nodo);
            lista->lonxitude--;
            return 0;
        }
    }
    while ((strcmp(nodo->data.nome,nome) != 0) && nodo->seguinte) {       // Se é un nodo do medio ou final
        anterior = nodo;
        nodo = nodo->seguinte;
    }
    if (strcmp(nodo->data.nome,nome) != 0){
        return -1;
    }
    anterior->seguinte = nodo->seguinte;
    free(nodo);
    lista->lonxitude--;
    return 0;
}


void imprimirListaM(ListaM *lista) {
    NodoM *p, *q;
    p = lista->cabeza;
    int i = 0;
    while (p != NULL) {
        q = p;
        printf("%p\t\t%d\t%s\t\t%s", q->data.memdir, q->data.tam, q->data.tipo, q->data.data);

        q = p->seguinte;
        p = q;
        i++;
    }
}

void imprimirListaShared(ListaM *lista) {
    NodoM *p, *q;
    p = lista->cabeza;
    int i = 0;
    while (p != NULL) {
        q = p;
        printf("%p\t\t%d %s (key %s)\t\t%s", q->data.memdir, q->data.tam, q->data.tipo, q->data.nome, q->data.data);

        q = p->seguinte;
        p = q;
        i++;
    }
}

void imprimirListaMmap(ListaM *lista) {
    NodoM *p, *q;
    p = lista->cabeza;
    int i = 0;
    while (p != NULL) {
        q = p;
        printf("%p\t\t%d %s %s\t(descriptor: %d)\t\t%s", q->data.memdir, q->data.tam, q->data.tipo, q->data.nome, q->data.fd, q->data.data);

        q = p->seguinte;
        p = q;
        i++;
    }
}

void vaciarListaM(ListaM *lista) {
    NodoM *p;

    while (lista->cabeza != NULL) {
        p = lista->cabeza;
        lista->cabeza = p->seguinte;
        free(p);
        lista->lonxitude--;

    }


}



