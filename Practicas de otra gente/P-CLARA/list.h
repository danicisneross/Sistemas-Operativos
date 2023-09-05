#include <stdio.h>

#define NUM 200
#define TROZOS 30
typedef char tData[NUM];

typedef struct Nodo {
    tData data;
    struct Nodo *seguinte;
} Nodo;


typedef struct Lista {
    Nodo *cabeza;
    int lonxitude;
} Lista;


Nodo *crearNodo(tData *data) {
    Nodo *nodo = malloc(sizeof(Nodo));
    strcpy(nodo->data, *data);
    nodo->seguinte = NULL;
    return nodo;
}

void borrarNodo(Nodo *nodo) {
    free(nodo);
}

tData *obter (int n, Lista *lista){
    if (lista->cabeza==NULL){
        return NULL;
    } else{
        struct Nodo *punteiro = lista->cabeza;
        int i = 0;
        while (i<n && punteiro->seguinte){
            punteiro=punteiro->seguinte;
            i++;
        }
        if (i!=n){
            return NULL;
        } else{
            return &punteiro->data;
        }
    }
}


void insertarNodo(Lista *lista, tData *data) {

    Nodo *nodo = crearNodo(data);
    if (lista->cabeza == NULL) {
        lista->cabeza = nodo;
    }
    else {
        Nodo *punteiro = lista->cabeza;
        while (punteiro->seguinte) {
            punteiro = punteiro->seguinte;
        }
        lista->lonxitude++;
        punteiro->seguinte = nodo;
    }

}

void imprimirLista(Lista *lista) {
    Nodo *p, *q;
    p = lista->cabeza;
    int i = 0;
    while (p != NULL) {
        q = p;
        printf("%d->%s", i, q->data);
        q = p->seguinte;
        p = q;
        i++;
    }
}

void vaciarLista(Lista *lista) {
    Nodo *p;

    while (lista->cabeza != NULL) {
        p = lista->cabeza;
        lista->cabeza = p->seguinte;
        borrarNodo(p);
        lista->lonxitude--;
    }


}

void imprimirListaN(Lista *lista, int n) {
    Nodo *p, *q;
    p = lista->cabeza;
    int i = 0;
    if (n < lista->lonxitude) {
        while (i < n) {
            q = p->seguinte;
            printf("%d->%s", i, q->data);
            p = q;
            i++;
        }
    }else
        printf("No hay elemento %d en el histórico\n", n);
}

bool imprimirComando(Lista *lista, int n) {
    Nodo *p, *q;
    p = lista->cabeza;
    int i = 0;
    if (n < lista->lonxitude) {
        while (i != n) {
            q = p->seguinte;
            p = q;
            i++;
        }
        printf("Ejecutando hist (%d): %s", i, q->data);
        return true;
    } else {
        printf("No hay elemento %d en el histórico\n", n);
        return false;
    }

}
