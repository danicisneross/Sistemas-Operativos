#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include <time.h>
#include <sys/utsname.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <grp.h>
#include <fcntl.h>
#include "historial.h"
#include "lmem.h"
#include "lproc.h"
#include "lenv.h"
#include<sys/shm.h>
#include<sys/mman.h>
#include<sys/ipc.h>
#include <sys/wait.h>
#include <ctype.h>

#define DATA 32
#define MAXLINEA 4096
#define MAXTROZOS 1028
#define MAXFyH 128
#define NAMEMAX 1024
#define TAMANO 2048
#define MAXVAR 256

void funAutores();
void funFecha();
void funAyuda();
void funPid();
void funInfosis();
void funCarpeta();
void funFin(tList *L, tListMem *Lm, tListP *Lp, tListE *Le);
void funHist(tList *L);
void funCreate();
void funList();
void funStat();
void funDelete();
void funDeltree();
void funAlloc(tListMem *L);
void funDealloc(tListMem *L, int flag);
void funIo();
void funMemDump();
void funMemFill();
void funMemory(tListMem *L);
void funRecursiva();
void funPriority();
void funShowVar(char *env[]);
void funChangeVar(char *env[], tListE *Le);
void funShowenv(char *env[]);
void funFork(tListP *Lp);
void funExecute();
void funJob(tListP *Lp);
void funListJobs(tListP *Lp);
void funDelJobs(tListP *Lp);


struct cmd {
  char *nombre;
  void (*pfun)();
};

struct ax {
  char *nombre;
  char *info;
};

struct cmd comandos[] = {
        {"ayuda",funAyuda}, 
        {"fecha",funFecha},
        {"autores",funAutores},
        {"pid",funPid},
        {"infosis",funInfosis},
        {"carpeta",funCarpeta},
        {"create",funCreate},
        {"list", funList},
        {"stat",funStat},
        {"delete",funDelete},
        {"deltree", funDeltree},
        {"i-o",funIo},
        {"e-s",funIo},
        {"i/o",funIo},
        {"e/s",funIo},
        {"memdump", funMemDump},
        {"memfill", funMemFill},
        {"recurse",funRecursiva},
        {"priority", funPriority},
        {"execute", funExecute},
        {NULL,NULL},
};

struct ax tabla[] = {
  {"fecha"," [-d|.h	Muestra la fecha y o la hora actual"},
  {"autores"," [-n|-l]	Muestra los nombres y logins de las autoras"},
  {"pid"," [-p]	Muestra el pid del shell o de su proceso padre"},
  {"hist", " [-c|-N]	Muestra el historico de comandos, con -c lo borra"},
  {"carpeta", " [dir] Cambia (o muestra) el directorio actual del shell"},
  {"infosis", " Muestra informacion de la maquina donde corre el shell"},
  {"ayuda", " [cmd]	Muestra ayuda sobre los comandos"},
  {"fin"," Termina la ejecucion del shell"},
  {"bye"," Termina la ejecucion del shell"},
  {"salir"," Termina la ejecucion del shell"},
  {"create"," [-f] [name]    Crea un directorio o un fichero (-f)"},
  {"stat"," [-long] [-acc] [-link] name1 name2 ..  	lista ficheros;\n\t-long: listado largo\n\t-acc: acesstime\n\t-link: si es enlace simbolico, el path contenido"},
  {"list", " [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 ..	lista contenidos de directorios\n-hid: incluye los ficheros ocultos\n-reca: recursivo (antes)\n-recb: recursivo (despues)\nresto parametros como stat"},
  {"delete", " [name1 name2 ..] Borra ficheros o directorios vacios"},
  {"deltree", " [name1 name2 ..] Borra ficheros o directorios no vacios recursivamente"},
  {"i-o","[read|write] [-o] fiche addr cont \nread fich addr cont: Lee cont bytes desde fich a addr\n write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n	addr es una direccion de memoria"},
  {"e-s","[read|write] [-o] fiche addr cont \nread fich addr cont: Lee cont bytes desde fich a addr\n write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n	addr es una direccion de memoria"},
  {"i/o","[read|write] [-o] fiche addr cont \nread fich addr cont: Lee cont bytes desde fich a addr\n write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n	addr es una direccion de memoria"},
  {"e/s","[read|write] [-o] fiche addr cont \nread fich addr cont: Lee cont bytes desde fich a addr\n write [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n	addr es una direccion de memoria"},
  {"recurse","[n]	Invoca a la funcion recursiva n veces.\n"},
  {"allocate","[-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n\t-malloc tam: asigna un bloque malloc de tamano tam.\n\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n"},
  {"deallocate","[-malloc|-shared|-delkey|-mmap|addr]..	Desasigna un bloque de memoria\n\t-malloc tam: desasigna el bloque malloc de tamano tam.\n\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n\t-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n\t-mmap fich: desmapea el fichero mapeado fich\n\taddr: desasigna el bloque de memoria en la direccion addr\n"},
  {"memory","memory [-blocks|-funcs|-vars|-all|-pmap] ..	Muestra muestra detalles de la memoria del proceso.\n-blocks: los bloques de memoria asignados\n-funcs: las direcciones de las funciones\n-vars: las direcciones de las variables\n-all: todo\n-pmap: muestra la salida del comando pmap(o similar)\n"},
  {"memfill","addr cont byte 	Llena la memoria a partir de addr con byte."},
  {"memdump", "addr cont 	Vuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr."},
  {"priority", "[pid] [valor] 	Muestra o cambia la prioridad del proceso pid a valor."},
  {"showvar", "var1  Muestra el valor y las direcciones de la variable de entorno var."},
  {"changevar", "[-a|-e|-p] var valor  Cambia el valor de una variable de entorno.\n\t-a: accede por el tercer arg de main\n\t-e: accede mediante environ\n\t-p: accede mediante putenv"},
  {"showenv", " [-environ|-addr] 	 Muestra el entorno del proceso."},
  {"fork", "  Hace una llamada fork para crear un proceso."},
  {"execute", "prog args.... 	Ejecuta, sin crear un proceso, prog con argumentos."},
  {"listjobs", "Lista los procesos en segundo plano."},
  {"deljobs", "[-term][-sig]	Elimina los procesos de la lista procesos en sp.\n\t-term: los terminados\n\t-sig: los terminados por senal"},
  {"job", "[-fg] pid  Muestra informacion del proceso pid.\n\t-fg: lo pasa a primer plano"},
  {NULL,NULL},
};
