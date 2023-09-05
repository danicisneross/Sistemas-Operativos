
#include <stdio.h>

#define NUM 200
#define TROZOS 30

// ------------------------------------------------------------------



// -----------------------------------------------------------------



typedef struct{
    pid_t pid;
    int prio;
    char user [15];
    char data[30];
    char state [20];
    char commandline [80];
    int signal;
}tDataJ;

typedef struct NodoJ {
    tDataJ data;
    struct NodoJ *seguinte;
} NodoJ;


typedef struct ListaJ {
    NodoJ *cabeza;
    int lonxitude;
} ListaJ;


NodoJ *crearNodoJ(tDataJ *dataJ) {
    NodoJ *nodo = malloc(sizeof(NodoJ));
    nodo->data.pid = dataJ->pid;   // pid
    nodo->data.pid = dataJ->pid;   // prio
    strcpy(nodo->data.user, dataJ->user);    // user
    strcpy(nodo->data.data, dataJ->data);    // data
    strcpy(nodo->data.state, dataJ->state);    // state
    strcpy(nodo->data.commandline, dataJ->commandline);    // commandlines
    nodo->data.signal = dataJ->signal;   // signal
    nodo->seguinte = NULL;
    return nodo;
}

void borrarNodoJ(NodoJ *nodo) {
    free(nodo);
}

tDataJ *obterJ (pid_t pid, ListaJ *lista){
    if (lista->cabeza==NULL){
        return NULL;
    } else{
        struct NodoJ *punteiro = lista->cabeza;
        int i = 0;
        while (pid != punteiro->data.pid && punteiro->seguinte){

            if ((punteiro->data.prio = getpriority(PRIO_PROCESS,punteiro->data.pid)) == -1)
                strcpy(punteiro->data.state, "TERMINADO");
            else
                strcpy(punteiro->data.state, "ACTIVO");

            punteiro=punteiro->seguinte;
            i++;
        }
        if (pid != punteiro->data.pid){
            return NULL;
        } else{
            return &punteiro->data;
        }
    }
}

int borrarJ (pid_t pid, ListaJ *lista){
    if (lista->cabeza==NULL){
        return 0;
    } else {
        struct NodoJ *punteiro = lista->cabeza;
        struct NodoJ *punteiroB;
        int i = 0;

        if (punteiro->data.pid == pid) {
            lista->cabeza = punteiro->seguinte;
            borrarNodoJ(punteiro);
            lista->lonxitude--;
            return 0;
        } else {

            while (pid != punteiro->seguinte->data.pid && punteiro->seguinte) {
                punteiro = punteiro->seguinte;
                i++;
            }
            if (pid != punteiro->seguinte->data.pid) {
                return -1;
            } else {
                punteiroB = punteiro->seguinte;
                punteiro->seguinte = punteiro->seguinte->seguinte;
                borrarNodoJ(punteiroB);
                lista->lonxitude--;
                return 0;
            }
        }
    }
}

void borrarAll(ListaJ *lista){                  // (( X ))
    NodoJ *p;
    p = lista->cabeza;
    lista->cabeza = p->seguinte;

    while (p->seguinte != NULL) {
        p->data.prio = getpriority(PRIO_PROCESS,p->data.pid);
        if (p->data.prio == -1)
            borrarNodoJ(p);
        p=p->seguinte;
    }


}


void insertarNodoJ(ListaJ *lista, tDataJ *data) {

    NodoJ *nodo = crearNodoJ(data);
    if (lista->cabeza == NULL) {
        lista->cabeza = nodo;
    }
    else {
        NodoJ *punteiro = lista->cabeza;
        while (punteiro->seguinte) {
            punteiro = punteiro->seguinte;
        }
        lista->lonxitude++;
        punteiro->seguinte = nodo;
    }

}

void imprimirListaJ(ListaJ *lista) {
    NodoJ *p, *q;
    p = lista->cabeza;
    int i = 0;
    int status;
    while (p != NULL) {
        q = p;

        if ((q->data.prio = getpriority(PRIO_PROCESS,q->data.pid)) == -1)
            strcpy(q->data.state, "TERMINADO");
        else
            strcpy(q->data.state, "ACTIVO");

        if (waitpid (q->data.pid, &status, WNOHANG | WUNTRACED | WCONTINUED)  == q->data.pid){
            if (WIFEXITED(status)) {
                q->data.signal = WEXITSTATUS(status);             // exited
            }

            else if (WIFSIGNALED(status)) {
                q->data.signal = WTERMSIG(status);                // by signal
            }

            else if (WIFCONTINUED(status)) {                    // continued
                q->data.signal = 000;
            }
        }

        printf("%d || %s || p=%d || %s || (%d) || %s|| %s", (int) q->data.pid, q->data.user, q->data.prio, q->data.state, q->data.signal, q->data.commandline, q->data.data);
        q = p->seguinte;
        p = q;
        i++;
    }
}

void vaciarListaJ(ListaJ *lista) {
    NodoJ *p;

    while (lista->cabeza != NULL) {
        p = lista->cabeza;
        lista->cabeza = p->seguinte;
        borrarNodoJ(p);
        lista->lonxitude--;
    }
}

