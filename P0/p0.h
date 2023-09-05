
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

#define DATA 32
#define MAXLINEA 4096
#define MAXTROZOS 1028
#define MAXPATHLEN PATH_MAX
#define MAXFyH 128

void funAutores();
void funFecha();
void funAyuda();
void funPid();
void funInfosis();
void funCarpeta();
void funFin(tList *L);
void funHist(tList *L);

struct cmd {
  char *nombre;
  void (*pfun)();
}

struct ax {
  char *nombre;
  char *info;
}

struct cmd comandos[] = {
        {"ayuda",funAyuda}, 
        {"fecha",funFecha},
        {"autores",funAutores},
        {"pid",funPid},
        {"infosis",funInfosis},
        {"carpeta",funCarpeta},
        {NULL,NULL},
}

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
  {NULL,NULL},
}


