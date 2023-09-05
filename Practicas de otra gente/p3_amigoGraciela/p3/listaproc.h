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
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define MAX_ENTRADAPROC 500

typedef char datoProc[MAX_ENTRADAPROC];

typedef struct SEN{
	char *nombre;
	int senal;
}SEN;

typedef struct elementProc{
		pid_t pid;
		int priority;
		datoProc user;
		datoProc commandLine;
		datoProc stat;
		datoProc timeD;
		int senal;
}elementProc;

typedef struct nodoProc* posicionProc;

struct nodoProc{
	elementProc elemento;
	posicionProc next;
};

typedef posicionProc tListaProc;

static struct SEN sigstrnum[]={
	"HUP", SIGHUP,
	"INT", SIGINT,
	"QUIT", SIGQUIT,
	"ILL", SIGILL,
	"TRAP", SIGTRAP,
	"ABRT", SIGABRT,
	"IOT", SIGIOT,
	"BUS", SIGBUS,
	"FPE", SIGFPE,
	"KILL", SIGKILL,
	"USR1", SIGUSR1,
	"SEGV", SIGSEGV,
	"USR2", SIGUSR2,
	"PIPE", SIGPIPE,
	"ALRM", SIGALRM,
	"TERM", SIGTERM,
	"CHLD", SIGCHLD,
	"CONT", SIGCONT,
	"STOP", SIGSTOP,
	"TSTP", SIGTSTP,
	"TTIN", SIGTTIN,
	"TTOU", SIGTTOU,
	"URG", SIGURG,
	"XCPU", SIGXCPU,
	"XFSZ", SIGXFSZ,
	"VTALRM", SIGVTALRM,
	"PROF", SIGPROF,
	"WINCH", SIGWINCH,
	"IO", SIGIO,
	"SYS", SIGSYS,

	/*senales que no hay en todas partes*/
	#ifdef SIGPOLL
	"POLL", SIGPOLL,
	#endif

	#ifdef SIGPWR
	"PWR", SIGPWR,
	#endif

	#ifdef SIGEMT
	"EMT", SIGEMT,
	#endif

	#ifdef SIGINFO
	"INFO", SIGINFO,
	#endif

	#ifdef SIGSTKFLT
	"STKFLT", SIGSTKFLT,
	#endif

	#ifdef SIGCLD
	"CLD", SIGCLD,
	#endif

	#ifdef SIGLOST
	"LOST", SIGLOST,
	#endif

	#ifdef SIGCANCEL
	"CANCEL", SIGCANCEL,
	#endif

	#ifdef SIGTHAW
	"THAW", SIGTHAW,
	#endif

	#ifdef SIGFREEZE
	"FREEZE", SIGFREEZE,
	#endif

	#ifdef SIGLWP
	"LWP", SIGLWP,
	#endif

	#ifdef SIGWAITING
	"WAITING", SIGWAITING,
	#endif

	NULL,-1,
};

void createListProc(tListaProc*);
bool crearNodoProc(posicionProc*);
void printProc(posicionProc, tListaProc*);
void printListaProc(tListaProc*);
bool insertElementProc(elementProc, tListaProc*);
void deleteElementProc(tListaProc*, posicionProc);
void deleteListProc(tListaProc*);
elementProc getElementProc(tListaProc, posicionProc);

//añadidas por carol
void estadojob(posicionProc, tListaProc*);
void borrarterm(tListaProc*);
void borrarsig(tListaProc*);

int Senal(char*);
char *NombreSenal(int);
posicionProc findElementProc(tListaProc*, int);

posicionProc lastProc(tListaProc);
posicionProc firstProc(tListaProc);
posicionProc tNextProc(tListaProc, posicionProc);
