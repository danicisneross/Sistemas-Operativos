/*
  Autores: RUBEN DIZ MARTINEZ       ruben.diz.@udc.es
           CAROLINA GRILLE SIEIRA   carolina.grille@udc.es
           GRUPO: 2.3                 P2 SO
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/mman.h> //mmap
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "listah.h"
#include "listamem.h"
#include "listaproc.h"

#define MAX_TROZOS 30
#define TAMANO 2048
#define MAXVAR 500

int gvar1=1;
int gvar2=2;
int gvar3=3;

extern char ** environ;

char fileRederr[MAXVAR] = "original";

//Prototipos de las funciones

//P0
void autores(char cmd[30]);

void pid(char cmd[]);

void carpeta(char *cmd);

void fecha(char cmd[]);

void hist(char cmd[], tLista *L);

void infosis();

void comando(char cmd[], tLista *L, tListaMem *Lm, char *envp[], tListaProc *Lp);

void ayuda(char cmd[]);

//P1
void crear(char cmd1[], char cmd2[]);

void borrar(int ntrozos, char *trozos[]);

void borrarrec(int ntrozos, char *trozos[]);

void listfich(int ntrozos, char *trozos[]);

void listdir(int ntrozos, char *trozos[]);

//P2

void malloc_alloc(char *trozos[], tListaMem *Lm);

void malloc_dealloc(char *trozos[], tListaMem *Lm);

void mallocf(int ntrozos, char *trozos[], tListaMem *Lm);

void mmap_alloc(char *trozos[], tListaMem *Lm);

void * MmapFichero(char * fichero, int protection, tListaMem *Lm);

void mmap_dealloc(char *trozos[], tListaMem *Lm);

void Mmap(int ntrozos, char *trozos[], tListaMem *Lm);

void SharedCreate(char *arg[], tListaMem *Lm);

void * ObtenerMemoriaShmget(key_t clave, size_t tam, tListaMem *Lm);

void Shared(int ntrozos, char *trozos[], tListaMem *Lm);

void shared_alloc(char *trozos[], tListaMem *Lm);

void SharedDelkey(char *args[]);

void shared_dealloc(char *trozos[], tListaMem *Lm);

void memoria_funcs();

void memoria_vars();

void memoria_dopmap(char *trozos[]);

void memoria(int ntrozos, char *trozos[], tListaMem *Lm);

void esOption(int ntrozos, char *trozos[]);

void deallocOption(char *trozos[], tListaMem *Lm);

void deallocGeneral(char *trozos[], tListaMem *Lm);

void volcarmem(char *trozos[]);

void llenarmem(char *trozos[]);

void doRecursive(int n);

int recursiva(int ntrozos, char *trozos[]);

void esread(int ntrozos, char *trozos[]);

void eswrite(int ntrozos, char *trozos[]);

//P3

void priority(char *trozos[]);

void setprio(char *prio, int pid);

void getPrio(int pid);

void rederr(char *trozos[]);

int buscarVariable (char * var, char *e[]);

int cambiarVariable(char * var, char * valor, char *e[]);

void entorno(char *trozos[], char *envp[]);

void mostrarVar(char *trozos[], char *envp[]);

void cambiarVar(char *trozos[], char *envp[]);

void mostrarEntorno (char **entorn, char * nombre_entorno);

int setLogin(char *user);

void hgetuid (char * trozos[]);

void hsetuid (char * trozos[]);

void uid(char *trozos[]);

void hfork(int ntrozos, char *trozos[]);

void ejec(int ntrozos, char *trozos[]);

void ejecpri(int ntrozos, char *trozos[]);

char *concatTrozos(char *trozos[]);

void insertProcess(tListaProc *Lp, pid_t pid, int priority, uid_t uid, char *commandLine);

void executePrograms(char *trozos[], bool sec, tListaProc *Lp, char* commandLin);

void fgpri(char *trozos[]);

void fg(char *trozos[], tListaProc *Lp);

void back(char *trozos[], tListaProc *Lp);

void backpri(char *trozos[], tListaProc *Lp);

void ejecas(char *trozos[]);

void fgas(char *trozos[]);

void bgas(char *trozos[], tListaProc *Lp);

void listjobs(tListaProc *Lp);

void job(char *trozos[], tListaProc *Lp);

void borrarjobs(char *trozos[], tListaProc *Lp);

void prog(int ntrozos, char *trozos[], tListaProc *Lp);

//Salida
int salirShell(tLista *L, tListaMem *Lm, tListaProc *Lp);

//Función que ya nos dan
int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL) {
        return 0;
    }
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

//Función que printea el principio de cada línea
void prishell() {
    printf("\n -> ");
}

//Función que lee el comando introducido por pantalla
void readshell(char *comando, tLista *L) {

    fgets(comando, MAX_ENTRADA, stdin);
}

void insertList(char *comando, char *trozos[], tLista *L) {
    element command;
    strcpy(command.datos, comando);
    if (strcmp(trozos[0], "comando") != 0) {
        if (!insertElement(command, L)) puts("ERROR");
    }
}

void delete(char *dir){
  if(remove(dir) != 0 )perror("Error");
}

char LetraTF (mode_t m){
  switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
    case S_IFSOCK: return 's'; /*socket */
    case S_IFLNK: return 'l'; /*symbolic link*/
    case S_IFREG: return '-'; /* fichero normal*/
    case S_IFBLK: return 'b'; /*block device*/
    case S_IFDIR: return 'd'; /*directorio */
    case S_IFCHR: return 'c'; /*char device*/
    case S_IFIFO: return 'p'; /*pipe*/
    default: return '?'; /*desconocido, no deberia aparecer*/
  }
}

char * ConvierteModo (mode_t m, char *permisos){
  strcpy (permisos,"---------- ");
  permisos[0]=LetraTF(m);
  if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
  if (m&S_IWUSR) permisos[2]='w';
  if (m&S_IXUSR) permisos[3]='x';
  if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
  if (m&S_IWGRP) permisos[5]='w';
  if (m&S_IXGRP) permisos[6]='x';
  if (m&S_IROTH) permisos[7]='r'; /*resto*/
  if (m&S_IWOTH) permisos[8]='w';
  if (m&S_IXOTH) permisos[9]='x';
  if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
  if (m&S_ISGID) permisos[6]='s';
  if (m&S_ISVTX) permisos[9]='t';
  return permisos;
}

int processCommand(char *trozos[], int ntrozos, tLista *L, tListaMem *Lm, char *envp[], tListaProc *Lp) {
    int i = 0;
    if (trozos[0] != NULL) {

                //Comandos p0
        if (strcmp("autores", trozos[0]) == 0) {
            autores(trozos[1]);
        } else if (strcmp("pid", trozos[0]) == 0) {
            pid(trozos[1]);
        } else if (strcmp("carpeta", trozos[0]) == 0) {
            carpeta(trozos[1]);
        } else if (strcmp("fecha", trozos[0]) == 0) {
            fecha(trozos[1]);
        } else if (strcmp("hist", trozos[0]) == 0) {
            hist(trozos[1], L);
        } else if (strcmp("comando", trozos[0]) == 0) {
            comando(trozos[1], L, Lm, envp, Lp);
        } else if (strcmp("infosis", trozos[0]) == 0) {
            infosis();

                  //Comandos p1
        } else if (strcmp("borrarrec", trozos[0]) == 0) {
            borrarrec(ntrozos, trozos);
        } else if (strcmp("listfich", trozos[0]) == 0) {
            listfich(ntrozos, trozos);
        } else if (strcmp("listdir", trozos[0]) == 0) {
            listdir(ntrozos, trozos);
        } else if (strcmp("borrar", trozos[0]) == 0) {
            borrar(ntrozos, trozos);
        } else if (strcmp("crear", trozos[0]) == 0) {
            crear(trozos[1], trozos[2]);
        } else if (strcmp("ayuda", trozos[0]) == 0) {
            ayuda(trozos[1]);

                  //Comandos p2
        } else if (strcmp("recursiva", trozos[0]) == 0) {
	          recursiva(ntrozos, trozos);
        } else if (strcmp("e-s", trozos[0])==0) {
	          esOption(ntrozos,trozos);
        } else if (strcmp("volcarmem", trozos[0]) == 0) {
	         volcarmem(trozos);
        } else if (strcmp("llenarmem", trozos[0]) == 0) {
	         llenarmem(trozos);
        } else if (strcmp("dealloc", trozos[0])==0) {
           deallocOption(trozos, Lm);
        } else if (strcmp("malloc", trozos[0]) == 0) {
	         mallocf(ntrozos,trozos, Lm);
        } else if (strcmp("mmap", trozos[0]) == 0) {
	         Mmap(ntrozos,trozos, Lm);
        } else if (strcmp("shared", trozos[0]) == 0) {
           Shared(ntrozos, trozos, Lm);
        } else if (strcmp("memoria", trozos[0]) == 0) {
           memoria(ntrozos, trozos, Lm);

                  //Comandos p3
        } else if (strcmp("priority", trozos[0]) == 0) {
        	priority(trozos);
        } else if (strcmp("rederr", trozos[0])==0) {
          rederr(trozos);
        } else if (strcmp("entorno", trozos[0]) == 0) {
        	entorno(trozos, envp);
        } else if (strcmp("mostrarvar", trozos[0]) == 0) {
        	mostrarVar(trozos, envp);
        } else if (strcmp("cambiarvar", trozos[0])==0) {
          cambiarVar(trozos, envp);
        } else if (strcmp("uid", trozos[0]) == 0) {
        	uid(trozos);
        } else if (strcmp("fork", trozos[0]) == 0) {
        	hfork(ntrozos, trozos);
        } else if (strcmp("ejec", trozos[0]) == 0) {
          ejec(ntrozos, trozos);
        } else if (strcmp("ejecpri", trozos[0]) == 0) {
          ejecpri(ntrozos,trozos);
        } else if (strcmp("fg", trozos[0]) == 0) {
          fg(trozos, Lp);
        } else if (strcmp("fgpri", trozos[0]) == 0) {
          fgpri(trozos);
        } else if (strcmp("back", trozos[0]) == 0) {
          back(trozos, Lp);
        } else if (strcmp("backpri", trozos[0]) == 0) {
          backpri(trozos, Lp);
        } else if (strcmp("ejecas", trozos[0]) == 0) {
          ejecas(trozos);
        } else if (strcmp("fgas", trozos[0])==0) {
          fgas(trozos);
        } else if (strcmp("bgas", trozos[0]) == 0) {
          bgas(trozos, Lp);
        } else if (strcmp("listjobs", trozos[0]) == 0) {
          listjobs(Lp);
        } else if (strcmp("job", trozos[0])==0) {
          job(trozos, Lp);
        } else if (strcmp("borrarjobs", trozos[0]) == 0) {
          borrarjobs(trozos, Lp);


                  //Comandos salida
        } else if (strcmp("fin", trozos[0]) == 0) {
            i = salirShell(L, Lm, Lp);
        } else if (strcmp("salir", trozos[0]) == 0) {
            i = salirShell(L, Lm, Lp);
        } else if (strcmp("bye", trozos[0]) == 0) {
            i = salirShell(L, Lm, Lp);
        } else {
            prog(ntrozos, trozos, Lp);
        }
    }
    return i;
}

////////////////////////////////////////////////P0:

//Función que devuelve autores y/o logins
void autores(char cmd[30]) {
    if (cmd == NULL) {
        printf("Ruben Diz Martinez: ruben.diz@udc.es\n");
        printf("Carolina Grille Sieira: carolina.grille@udc.es\n");
    } else if (strcmp("-l", cmd) == 0) {
        printf("ruben.diz@udc.es\n");
        printf("carolina.grille@udc.es\n");
    } else if (strcmp("-n", cmd) == 0) {
        printf("Ruben Diz Martinez\n");
        printf("Carolina Grille Sieira\n");
    }
}

//Función que devuelve el pid hijo o padre
void pid(char cmd[]) {
    if (cmd == NULL) {
        pid_t pid;
        pid = getpid();
        printf("Pid del shell: %d\n", pid);
    } else if (strcmp("-p", cmd) == 0) {
        pid_t ppid;
        ppid = getppid();
        printf("Pid del padre del shell: %d\n", ppid);
    }
}

//Función que cambia el directorio
void carpeta(char *cmd) {
    if (cmd == NULL) {
        char dir[MAX_ENTRADA];
        if (getcwd(dir, sizeof(dir)) != NULL)
            printf("%s\n", dir);
        else
            perror("Error:");
    } else {
        if (chdir(cmd) != 0)
            perror("Imposible cambiar de directorio");
    }
}

//Funcion que da la fecha y/o la hora
void fecha(char cmd[]) {
    if ((cmd == NULL) || (strcmp("-d", cmd) == 0)) {
        time_t date_now;
        time(&date_now);
        struct tm *date;
        if ((date = localtime(&date_now)) == NULL) {
            perror("Error");
        } else {
            printf("%d/%d/%d\n", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);
        }
    }
    if ((cmd == NULL) || (strcmp("-h", cmd) == 0)) {
        time_t tiempo = time(0);
        struct tm *tlocal;
        if ((tlocal = localtime(&tiempo)) == NULL) {
            perror("Error");
        } else {
            char output[128];
            strftime(output, 128, "%H:%M:%S", tlocal);
            printf("%s\n", output);
        }
    }
}

//Funcion que muestra el historial de comandos
void hist(char cmd[], tLista *L) {

    int i = 0;
    int valor;
    posicion p;
    element item;

    if (cmd == NULL) {
        for (p = first(*L); p != NULL; p = tNext(*L, p)) {
            item = getElement(*L, p);
            i++;
            printf("%d-> %s\n", i, item.datos);
        }
    } else if (strcmp(cmd, "-c") == 0) {

        for (p = first(*L); p != NULL; p = first(*L)) {
            deleteElement(L, p);
        }
    } else {
        valor = abs(atoi(cmd));
        if (valor > 0) {
            p = first(*L);
            for (int j = 0; j < valor && p != NULL; j++) {
                i++;
                item = getElement(*L, p);
                printf("%d-> %s\n", i, item.datos);
                p = tNext(*L, p);
            }
        }
    }
}

//Funcion que nos muetra N comando
void comando(char cmd[], tLista *L, tListaMem *Lm, char *envp[], tListaProc *Lp) {
    char *trozos[MAX_TROZOS];
    int numtrozos;
    if (cmd != NULL) {
        int valor = abs(atoi(cmd));
        if (valor != 0) {
            element item;
            posicion p = first(*L);
            for (int i = 1; i != valor && p != NULL; i++) {
                p = tNext(*L, p);
            }
            if (p != NULL) {
                item = getElement(*L, p);
                numtrozos = TrocearCadena(item.datos, trozos);
                printf("Ejecutando hist (%d): %s\n", valor, item.datos);
                processCommand(trozos, numtrozos, L, Lm, envp, Lp);
            }
        }
    }
}

//Printea la informacion de la maquina que esta ejecutando el shell
void infosis() {
    struct utsname unameData;
    if (uname(&unameData) < 0) {
        perror("Uname");
    } else {
        printf("%s (%s), OS: %s-%s-%s\n", unameData.nodename, unameData.machine, unameData.sysname,
                                          unameData.release, unameData.version);
    }
}


////////////////////////////////////////////////P1:

//Funcion que crea ficheros o directorios
void crear(char cmd1[],char cmd2[]){

  int fd;

  if(cmd1 != NULL){
    if(cmd2 != NULL){

        if( strcmp(cmd1, "-f") == 0 ){
            fd = open(cmd2 , O_RDWR|O_CREAT, 0664);
            if(fd<0){
              perror("Couldnt create file:");
            }
        }

    }
    else{
      if (mkdir(cmd1, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
    printf("Error: %s\n", strerror(errno));
}

    }
  }
  else{
    carpeta(NULL);
  }
}

//Funcion para borrar directorios
void borrar(int ntrozos, char *trozos[]){

  for(int i=1; i<ntrozos; i++){
    delete(trozos[i]);
  }
}

//Funcion auxiliar para borrar recursivamente

void rec_borrar(char * dir){
	DIR * dirp = opendir(dir); //Abrir un directorio para recorrerlo, devuelve NULL si falla
    struct dirent *di; //Las dos primeras entradas de cualquier directorio corresponden los directorios . y ..
    char direction[900];

    if (dirp!=NULL){ //si podemos abrir el dir
        while((di = readdir(dirp)) != NULL){ //almacena resultado en di
            strcpy(direction,dir);
            strcat(direction,"/");
            strcat(direction,di->d_name); //copiamos nombre directorio.
            if ((strcmp(di->d_name,".") != 0 && (strcmp(di->d_name,"..") != 0))){ //seguimos borrando si no es el dir actual o el padre
                if (di->d_type == DT_DIR){  //si es directorio borramos recursivo outra vez
                    rec_borrar(direction);
                }else{
                    delete(direction);
                }
            }
        }delete(dir);  //borramos actual
    }else perror("Error");
    closedir(dirp);
}

//Funcion para borrar directorios recursivamente
void borrarrec(int ntrozos, char *trozos[]){

  if(trozos[1] != NULL){
    for(int i=1; i<ntrozos; i++){
        rec_borrar(trozos[i]);
      }
    }
  else{
    carpeta(NULL);
  }
}

//Funcion para printar la informacion de los ficheros
void printInfo(char *dir, bool tLong, bool tLink, bool tAcc, char *fich){

  struct tm *time;
  struct stat statbuf;
  char buffer[300];
  struct passwd *pwd;
  struct group *gr;
  char *permisos= malloc( MAX_ENTRADA * sizeof(char) );
  char link[PATH_MAX] = {};
  char destino[PATH_MAX] = {};

  if( lstat(dir, &statbuf) == -1) {
      printf("Error al acceder a %s: %s\n",dir, strerror(errno));
  }
  else{
    if(tAcc==true){ //SELECCION DE QUE FECHA MUESTRA
      time = localtime(&statbuf.st_atime);
      strftime(buffer, sizeof(buffer), "%Y/%m/%d-%H:%M", time);}
    else{
      time = localtime(&statbuf.st_mtime);
      strftime(buffer, sizeof(buffer), "%Y/%m/%d-%H:%M", time);}

      pwd = getpwuid(statbuf.st_uid);
      if(pwd != NULL) {
          gr = getgrgid(statbuf.st_gid);
            if(gr != NULL) {

              if(tLong==true){  //LONG
                ConvierteModo(statbuf.st_mode, permisos);
                printf("%s", buffer);
                printf("%4ld ( %ld) %8s %8s %s", statbuf.st_nlink, statbuf.st_ino,
                              pwd->pw_name, gr->gr_name, permisos);
                  }

                printf(" %7ld  %s", statbuf.st_size, fich);  //PRINT GENERAL

                if(tLong==true && tLink==true){ //LINK (SOLO SI LONG ESTÁ PRESENTE)

                if( readlink(dir, link, sizeof(link)-1) != -1 ){
                  if(realpath(link, destino)!=NULL){
                    printf(" -> %s",destino);
                  }else{ perror("Error"); }
                }
              }

            }else {perror("Error:"); }
        }else{ perror("Error:"); }
        printf("\n");
  }
      free(permisos);

}

//Funcion que lista ficheros
void listfich(int ntrozos, char *trozos[]){

  bool tLong=false, tLink=false, tAcc=false, noName=true;

  int cond=ntrozos;

  for(int i=1; i<ntrozos && noName!=false ; i++){
    if(strcmp(trozos[i], "-long") != 0 && strcmp(trozos[i], "-link") != 0 && strcmp(trozos[i], "-acc") != 0){
      noName=false;
      cond=i;
    }
  }

  for(int i=1; i<4 && i<cond; i++){

      if(strcmp(trozos[i], "-long") == 0){
        tLong = true;
      }else if(strcmp(trozos[i], "-link") == 0){
        tLink=true;
      }else if(strcmp(trozos[i], "-acc") == 0){
        tAcc=true;
      }
  }

  if(noName==false){

    for(int i=cond; i<ntrozos; i++){

        printInfo(trozos[i], tLong, tLink, tAcc, trozos[i]);

    }

  }
  else{
    carpeta(NULL);  //Sin opciones ni nombres
  }

}

//Funcion que printa el directorio completo
void printDirectorio(char * dir, bool tLong, bool tLink, bool tAcc, bool tHid){

  DIR * dirp = opendir(dir);
  struct dirent *di;
  char direction[900];

  if (dirp!=NULL){

      printf("************%s\n",dir);

      while((di = readdir(dirp)) != NULL){
          strcpy(direction,dir);
          strcat(direction,"/");
          strcat(direction,di->d_name);

          if ( (di->d_name[0] != '.')){
                printInfo(direction, tLong, tLink, tAcc, di->d_name);
            }
            else if(tHid){
                printInfo(direction, tLong, tLink, tAcc, di->d_name);
              }
        }
      }
      else{
        printInfo(dir, tLong, tLink, tAcc, dir);
      }
      closedir(dirp);
}

//Funcion auxiliar recursividad B
void recB(char *dir, bool tLong, bool tLink, bool tAcc, bool tHid){

    DIR * dirp = opendir(dir);
    struct dirent *di;
    char direction[900];

    if (dirp!=NULL){

          while((di = readdir(dirp)) != NULL){

              strcpy(direction,dir);
              strcat(direction,"/");
              strcat(direction,di->d_name);

              if ((strcmp(di->d_name,".") != 0 && (strcmp(di->d_name,"..") != 0))){
                  if (di->d_type == DT_DIR){
                    recB(direction,tLong, tLink, tAcc, tHid);
                  }
              }
            }

    printDirectorio(dir, tLong, tLink, tAcc, tHid);


  }else{ printInfo(dir, tLong, tLink, tAcc, dir); }  //ES UN FICHERO

    closedir(dirp);
}

//Funcion auxiliar recursividad A

void recA(char * dir, bool tLong, bool tLink, bool tAcc, bool tHid){

    DIR * dirp = opendir(dir);
    struct dirent *di;
    char direction[900];

    if (dirp!=NULL){

        printDirectorio(dir,tLong, tLink, tAcc, tHid);

            while((di = readdir(dirp)) != NULL){
                strcpy(direction,dir);
                strcat(direction,"/");
                strcat(direction,di->d_name);

                if ((strcmp(di->d_name,".") != 0 && (strcmp(di->d_name,"..") != 0))){
                    if (di->d_type == DT_DIR){
                      recA(direction,tLong, tLink, tAcc, tHid);
                    }
                }
              }

    }else{printInfo(dir, tLong, tLink, tAcc,dir);}  //ES UN FICHERO

    closedir(dirp);

}

//Funcion que lista directorios
void listdir(int ntrozos, char *trozos[]){

  int cond=ntrozos;

  bool tLong=false, tLink=false, tAcc=false, noName=true;
  bool tReca=false, tRecb=false, tHid=false;

  for(int i=1; i<ntrozos && noName!=false; i++){
    if(strcmp(trozos[i], "-long") != 0 &&
       strcmp(trozos[i], "-link") != 0 &&
       strcmp(trozos[i], "-acc") != 0 &&
       strcmp(trozos[i], "-hid") != 0 &&
       strcmp(trozos[i], "-reca") != 0 &&
       strcmp(trozos[i], "-recb") != 0){
      noName=false;
      cond=i;
    }
  }

  for(int i=1; i<ntrozos && i<cond; i++){

      if(strcmp(trozos[i], "-long") == 0){
        tLong = true;
      }else if(strcmp(trozos[i], "-link") == 0){
        tLink=true;
      }else if(strcmp(trozos[i], "-acc") == 0){
        tAcc=true;
      }else if(strcmp(trozos[i], "-hid") == 0){
        tHid=true;
      }

      //Si todavía no apareció reca o recb lo cambia, pero no se pueden hacer ambos
      if( (strcmp(trozos[i], "-reca") == 0) && (!tReca && !tRecb) ){
        tReca=true;
      }else if( (strcmp(trozos[i], "-recb") == 0 ) && (!tReca && !tRecb) ){
        tRecb=true;
      }
  }

  if(noName==false){

    for(int i=cond; i<ntrozos; i++){

        if(tReca){
          recA(trozos[i],tLong, tLink, tAcc, tHid);
        }else if(tRecb){
          recB(trozos[i],tLong, tLink, tAcc, tHid);
        }else{
          printDirectorio(trozos[i],tLong, tLink, tAcc, tHid);
        }

    }

  }
  else{
    carpeta(NULL);  //Sin opciones ni nombres
  }

}

////////////////////////////////////////////////Funciones p2

void insertElemMem(char *address, int sizeB, char *type, char *fichName, int cl, tListaMem *Lm){

  elementMem prItem;
  time_t date_n;
  time(&date_n);
  struct tm *date;


  if ((date = localtime(&date_n)) == NULL) {
      perror("Error");
  }
   else {

      strcpy(prItem.timeAlloc, asctime(date) );
      prItem.memAddress = address;
      prItem.sizeBlock=sizeB;
      strcpy(prItem.typeAlloc,type);

      if( strcmp(type,"mmap")==0 ){
        strcpy(prItem.fichName,fichName);
      }
      else{
        strcpy(prItem.fichName, "");
      }

      if( strcmp(type,"shared")==0 || strcmp(type,"mmap")==0){
        prItem.cl=cl;
      }
      else{
        prItem.cl=0;
      }

      insertElementMem(prItem, Lm);

  }
}


void malloc_alloc(char *trozos[], tListaMem *Lm){

  void *p;
  int sB;

	if (trozos[1]!=NULL){

    sB = atoi(trozos[1]);
		if(sB>0){
      p=malloc(sB);
  		printf("Allocated %s at: %p\n",trozos[1],p);
  		insertElemMem( (char *) p, sB, "malloc", "", 0, Lm);
    }
    else{
      puts("No se asignan bloques de 0 bytes");
    }


	}else{
		printListaMem(*Lm, "malloc");
  }
}

void malloc_dealloc(char *trozos[], tListaMem *Lm){
  void *p;
  int sB;
  posicionMem pos;
  elementMem item;

  if(trozos[2]!=NULL){

    sB=atoi(trozos[2]);
    pos=findElementMllc(Lm, sB);

    if(pos!=NULL){
      item=getElementMem(*Lm, pos);
      p=item.memAddress;
      free(p);
      deleteElementMem(Lm, pos);
      printf("Deallocated %d at: %p\n",item.sizeBlock, item.memAddress);
    }
    else{
      printListaMem(*Lm, "malloc");
    }

  }else{
    printListaMem(*Lm, "malloc");
  }

}

void mallocf(int ntrozos, char *trozos[], tListaMem *Lm){
	if (trozos[1]!=NULL){
		if (strcmp(trozos[1],"-free")==0){
			malloc_dealloc(trozos, Lm);
    }
		else{
			malloc_alloc(trozos, Lm);
    }
	}
  else{
    printListaMem(*Lm, "malloc");
  }
}


void mmap_alloc(char *arg[], tListaMem *Lm){
  char *perm;
  void *p;
  int protection=0;

  if (arg[1]!=NULL){
    if ((perm=arg[2])!=NULL && strlen(perm)<4) {
      if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
      if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
      if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if((p=MmapFichero(arg[1], protection, Lm))==NULL)
      perror ("Imposible mapear fichero");
    else
      printf ("Fichero %s mapeado en: %p\n", arg[0], p);
  }
  else{
    printListaMem(*Lm, "mmap");
    return;
  }
}

void * MmapFichero (char * fichero, int protection, tListaMem *Lm){
  int df, map=MAP_PRIVATE,modo=O_RDONLY;
  struct stat s;
  void *p;

  if (protection&PROT_WRITE) modo=O_RDWR;
  if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1){
    return NULL;
  }
  if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED){
    return NULL;
  }
  insertElemMem( (char *) p, s.st_size, "mmap", fichero , df, Lm);
  return p;
}

void mmap_dealloc(char *trozos[], tListaMem *Lm){

  void *p;
  char *file;
  posicionMem pos;
  elementMem item;

  if(trozos[2]!=NULL){
    file=trozos[2];

    pos=findElementMmap(Lm, file);

    if(pos!=NULL){
      item=getElementMem(*Lm, pos);
      p=item.memAddress;
      munmap(p, item.sizeBlock);
      deleteElementMem(Lm, pos);
      printf("Deallocated %s at: %p\n",file ,p);
    }
    else{
      printListaMem(*Lm, "mmap");
    }

  }else{
    printListaMem(*Lm, "mmap");
  }


}

void Mmap(int ntrozos, char *trozos[], tListaMem *Lm){
	if (trozos[1]!=NULL){
		if (strcmp(trozos[1],"-free")==0)
			mmap_dealloc(trozos, Lm);
		else
			mmap_alloc(trozos, Lm);
	}else
		printListaMem(*Lm, "mmap");
}


void * ObtenerMemoriaShmget(key_t clave, size_t tam, tListaMem *Lm){
  void * p;                                            /*si tam >0 intenta crearla y si tam==0 asume que existe*/
  int aux,id,flags=0777;
  struct shmid_ds s;
  if (tam) /*si tam no es 0 la crea en modo exclusivo esta funcion vale para shared y shared -create*/
    flags=flags | IPC_CREAT | IPC_EXCL; /*si tam es 0 intenta acceder a una ya creada*/


  if (clave==IPC_PRIVATE){  //no nos vale
    errno=EINVAL;
    return NULL;
  }
  if ( (id=shmget(clave, tam, flags))==-1 ){
    return (NULL);
  }

  if ((p=shmat(id,NULL,0))==(void*) -1){
    aux=errno; /*si se ha creado y no se puede mapear*/
    if (tam) /*se borra */
      shmctl(id,IPC_RMID,NULL);
    errno=aux;
    return (NULL);
  }
  shmctl (id,IPC_STAT,&s);
  insertElemMem((char *) p, tam, "shared", "", (int) clave, Lm);
  return (p);
}

void SharedCreate(char *arg[], tListaMem *Lm){
  key_t k;
  size_t tam=0;
  void *p;
  if(arg[2]==NULL || arg[3]==NULL){
      printListaMem(*Lm, "shared");
      return;
   }
  k = (key_t) atoi(arg[2]);
  if(arg[3]!=NULL)
    tam=(size_t) atoll(arg[3]);
  if( (p=ObtenerMemoriaShmget(k,tam,Lm))==NULL )
    perror ("Imposible obtener memoria shmget");
  else
    printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}

void Shared(int ntrozos, char *trozos[], tListaMem *Lm){
	if (trozos[1]!=NULL){
		if(strcmp(trozos[1],"-free")==0){
			   shared_dealloc(trozos, Lm);
		}else if(strcmp(trozos[1],"-create")==0){
			   SharedCreate(trozos, Lm);
    }else if(strcmp(trozos[1],"-delkey")==0){
         SharedDelkey(trozos);
      }else
        shared_alloc(trozos, Lm);

	}else
		printListaMem(*Lm, "shared");
}

void shared_alloc(char *trozos[], tListaMem *Lm){

  void * p;
  key_t cl;
  int id, aux;
  char *key=trozos[1];
  struct shmid_ds s;

    if ( (cl=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
      printf(" shared clave_valida\n");
      return;
    }
    if ((id=shmget(cl,0,0666))==-1){
      perror("shmget: imposible obtener memoria compartida");
      return;
    }

    if ((p=shmat(id,NULL,0))!=(void*) -1){
      shmctl (id,IPC_STAT,&s);
      printf ("Memoria de shmget de clave %d asignada en %p\n",cl,p);
      insertElemMem((char *) p, s.shm_segsz, "shared", "", (int) cl, Lm);
    }
    else{
      aux=errno; /*si se ha creado y no se puede mapear*/
      if (s.shm_segsz) /*se borra */
        shmctl(id,IPC_RMID,NULL);
      errno=aux;
      perror("Imposible obtener memoria shmget");
    }

}

void SharedDelkey(char *args[]){

  key_t clave;
  int id;
  char *key=args[2];
  if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
    printf (" shared -delkey clave_valida\n");
    return;
  }
  if ((id=shmget(clave,0,0666))==-1){
    perror ("shmget: imposible obtener memoria compartida");
    return;
  }
  if (shmctl(id,IPC_RMID,NULL)==-1)
    perror ("shmctl: imposible eliminar memoria compartida\n");
  printf("Key %s removed from the system", key);
}

void shared_dealloc(char *trozos[], tListaMem *Lm){

  void *p;
  int cl,suc;
  posicionMem pos;
  elementMem item;

  if(trozos[2]!=NULL){
    cl=atoi(trozos[2]);

    pos=findElementShd(Lm, cl);

    if(pos!=NULL){
      item=getElementMem(*Lm, pos);
      p=item.memAddress;
      suc=shmdt(p);
      if(suc!=-1){
        deleteElementMem(Lm, pos);
        printf("Deallocated %d at: %p\n",cl ,p);
      }
      else{
        perror("shmdt: imposible eliminar memoria compartida\n");
      }
    }
    else{
      printf("No hay ninguna direccion asociada a la clave %s\n",trozos[2]);
    }

  }else{
    printListaMem(*Lm, "shared");
  }

}

//Funcion memoria
void memoria_funcs(){
	printf("\nShell functions: \n%p\n%p\n%p\n", &carpeta, &pid, &fecha);
	printf("Library functions: \n%p\n%p\n%p\n", &scanf, &fgets, &strcat);
}

void memoria_vars(){
	int a= 4;
	int b= 5;
	int c= 6;
	static int d= 7;
	static int e= 8;
	static int f= 9;
	printf("\nGlobal variables: \n%p\n%p\n%p\n", &gvar1, &gvar2, &gvar3);
	printf("Static variables:\n%p\n%p\n%p\n",&d, &e, &f);
	printf("Local variables:\n%p\n%p\n%p\n", &a, &b, &c);
}

void memoria_dopmap(char *trozos[]){
	pid_t pid;
	char elpid[32];
	char *argv[3]={"pmap",elpid,NULL};

	sprintf (elpid,"%d", (int) getpid());
	if ((pid=fork())==-1){
		perror ("Imposible crear proceso");
		return;
	}
	if (pid==0){
		if (execvp(argv[0],argv)==-1)
			perror("cannot execute pmap");
		exit(1);
	}
	waitpid (pid,NULL,0);
}

void memoria(int ntrozos, char *trozos[], tListaMem *Lm){
	if (trozos[1] != NULL){
		if(strcmp(trozos[1], "-all")==0){
			memoria_vars();
			memoria_funcs();
			printListaMem(*Lm, "all");
		} else if(strcmp(trozos[1], "-vars")==0){
		        memoria_vars();
		}else if(strcmp(trozos[1], "-funcs")==0){
		        memoria_funcs();
		}else if(strcmp(trozos[1], "-pmap")==0){
		        memoria_dopmap(trozos);
		}else if(strcmp(trozos[1], "-blocks")==0){
      printListaMem(*Lm, "all");
    }else{
      printf("Opcion %s no contemplada\n", trozos[1]);
    }
	}else{
    memoria_vars();
    memoria_funcs();
    printListaMem(*Lm, "all");
  }
}

void volcarmem(char *trozos[]){

	int bytes=25;
	char *address;
  unsigned char c;

	if(trozos[1]==NULL){
		printf("Se necesita una dirección\n");
	}
  else{

  	if(sscanf(trozos[1], "0x%p", &address)==0 || address == NULL){
  		fprintf(stderr, "%s no es una direccion valida\n",trozos[1]);
  	    return;
  		}
      else{

  	if(trozos[2]!=NULL){ bytes=atoi(trozos[2]); }
      	printf("Volcando %d bytes desde la direccion %p\n", bytes, address);

      	for(int bytes2 = 0; bytes2<bytes;bytes2 += 25){
      		for(int offset = bytes2; offset < bytes2 + 25 && offset < bytes; offset++){

      			c = *(address + offset);
      			if(c>=32 && c<=127){
      				printf(" %c ",c);
      				}
      				else{
      					printf("  ");
      					}
      			}
      			printf("\n");
      			for(int offset = bytes2; (offset < bytes2 + 25) && (offset < bytes) ; offset++){
      				c = *(address + offset);
      				printf("%02X ", c);
      				}
      			printf("\n");
          }
        }
    }
}



void llenarmem(char *trozos[]){

  char *direc;
  unsigned int  byte=128;
	unsigned char c='A';

	if(trozos[1]==NULL){
		printf("address not valid\n");
		return;
	}

	if(sscanf(trozos[1],"0x%p",&direc)==0 || direc==NULL){
		fprintf(stderr,"%s no es una direccion valida\n",trozos[1]);
		return;
	}
	if(trozos[2]!=NULL){
		if(sscanf(trozos[2],"%ud",&byte)==0){
			fprintf(stderr,"%s no es un numero valido",trozos[2]);
			return;
		}
		if(trozos[3]!=NULL){
			if(sscanf(trozos[3],"0x%02hhX",&c)==0){
				fprintf(stderr,"%s no es un valor hexadecimal valido\n",trozos[3]);
			}
		}
	}

	printf("Llenando %u bytes de memoria con el byte %c(%x) a partir de la direccion %p\n",byte,(char) c,c,direc);

	for(int offset=0;offset<byte;offset++){
		direc[offset]=c;
	}
}


void doRecursive(int n){
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf ("\n Parametro n:%4d en %14p\n",n,&n);
    printf (" Array estatico en %12p \n",&estatico);
    printf (" Array automatico en %14p\n",&automatico);
    n--;

    if (n>=0){
        doRecursive(n);
    }else{ printf("\n"); }
}

int recursiva(int ntrozos, char *trozos[]){
    int n;

    if(trozos[1]==NULL){
        printf("Uso recursiva n\n");
    }
    else{
      n = atoi(trozos[1]);
      doRecursive(n);
    }
    return 0;
}

void esOption(int ntrozos, char *trozos[]){
  if(trozos[1]!=NULL){
    if( strcmp("read", trozos[1]) == 0 ){
      esread(ntrozos, trozos);
    }
    else if(strcmp("write", trozos[1]) == 0){
      eswrite(ntrozos,trozos);
    }else{ puts("e-s [read|write] [-o] fiche addr cont 	Invoca a la funcion recursiva n veces"); }
  }
  else{ puts("e-s [read|write] [-o] fiche addr cont 	Invoca a la funcion recursiva n veces"); }
}

void esread(int ntrozos, char *trozos[]){

  if(trozos[2]!=NULL && trozos[3]!=NULL){

    char *fichero=trozos[2];
  	void * p= (void *) strtoul(trozos[3],NULL,16);
  	ssize_t tam;
  	int df,totalcar;
  	struct stat s;
  	if (stat (fichero,&s)==-1 || (df=open(fichero,O_RDWR))==-1){
  		printf("Error abriendo fichero\n");
  		close (df);
  		return;
  	}

  	if (trozos[4]!=NULL)
  		tam=atoi(trozos[4]);
  	else
  		tam=s.st_size-1;

  	if ((totalcar=read(df,p,tam))==-1){
  		printf("Mala lectura\n");
  		close (df);
  		return;
  	}else
  		printf("Numero de caracteres leidos: %d\n",totalcar);
  	close (df);
  	return;

  }
  else{ puts("e-s read fich addr cont"); }
}

void eswrite(int ntrozos, char *trozos[]){
	int con, per;

  if(trozos[2]!=NULL){

  	if((strcmp(trozos[2],"-o")==0)){
      if( (trozos[3]!=NULL) && (trozos[4]!=NULL) && (trozos[5]!=NULL)){
    		per = O_WRONLY | O_CREAT | O_TRUNC;
    		char *dir = (char*)strtoul(trozos[4],NULL,16);
    		if((con=open(trozos[3], per, S_IRWXU | S_IRWXG | S_IRWXO))==-1){
    			perror("No es posible abrir el fichero para escribir");
    			return;
    		}else{
    			printf("Se ha modificado el archivo de texto\n");
    			write(con,dir,atoi(trozos[5]));
          close(con);
        }
      } else{ puts("e-s write [-o] fich addr cont"); }
  	}
    else{
      if( (trozos[3]!=NULL) && (trozos[4]!=NULL) ){
    		per = O_WRONLY | O_CREAT | O_EXCL;
    		char *dir = (char*)strtoul(trozos[3],NULL,16);
    		if((con=open(trozos[2], per, S_IRWXU | S_IRWXG | S_IRWXO))==-1){
    			perror("No es posible abrir el fichero para escribir");
    			return;
    		}else{
    			printf("Se ha creado el archivo de texto en la memoria especificada\n");
    			write(con,dir,atoi(trozos[4]));
          close(con);
    		}
      }
      else{ puts("e-s write [-o] fich addr cont"); }
  	}
  } else{ puts("e-s write [-o] fich addr cont"); }
}

void deallocOption(char *trozos[], tListaMem *Lm){

  if(trozos[1]!=NULL){
    if( strcmp("-malloc", trozos[1]) == 0 ){
        malloc_dealloc(trozos, Lm);
    }else if(strcmp("-mmap", trozos[1]) == 0){
    	  mmap_dealloc(trozos, Lm);
    }else if(strcmp("-shared", trozos[1]) == 0){
    	  shared_dealloc(trozos, Lm);
    }else{
        deallocGeneral(trozos, Lm);
    }

  } else{ printListaMem(*Lm, "all"); }

}

void deallocGeneral(char *trozos[], tListaMem *Lm){

  char *direc;
  posicionMem pos;
  elementMem item;
  char *type;

  if(sscanf(trozos[1],"0x%p",&direc)==0 || direc==NULL){
		fprintf(stderr,"%s no es una direccion valida\n",trozos[1]);
		return;
	}

  pos = findElement(Lm, direc);

  if(pos!=NULL){
    item = getElementMem(*Lm, pos);
    type = item.typeAlloc;

    if( (strcmp("malloc", type) == 0) ){
        free(direc);
    }else if(strcmp("mmap", type) == 0){
        munmap(direc, item.sizeBlock);
    }else if( (strcmp("shared", type) == 0) ){
        shmdt(direc);
    }else{
      puts("dealloc addr");
    }

    printf("Block at address %p deallocated (%s)\n",direc, type);
    deleteElementMem(Lm,pos);

  }
  else{
    printListaMem(*Lm, "all");
  }
}

////////////////////////////////////////////////Funciones p3

void getPrio(int pid){
  errno=0;
  int prio = getpriority(PRIO_PROCESS, pid);
  if(errno==0){
    printf("Prioridad del proceso %d es %d\n", pid, prio);
  }
  else{
    perror("Imposible obtener la prioridad del proceso");
  }

}

void setprio(char *prio, int pid){
  int pri = atoi(prio);
  if( setpriority(PRIO_PROCESS, pid, pri)!=0 ){
    perror("Imposible cambiar la prioridad del proceso");
  }
}

void priority(char *trozos[]){
  pid_t pid;
  int pri;

  if(trozos[1]!=NULL){

    pid = atoi(trozos[1]);
    if(trozos[2]!=NULL){
      setprio(trozos[2], pid);
    }
    else{ getPrio(pid); }
  }
  else{
    pid = getpid();
    getPrio(pid);
  }
}

void rederr(char *trozos[]){
  int desc;
  int file;

  if(trozos[1]!=NULL){

    if( strcmp(trozos[1],"-reset")==0 ){

      dup2(STDOUT_FILENO, STDERR_FILENO);
      strcpy(fileRederr,"original");

    }
    else{
      file = open(trozos[1], O_WRONLY | O_APPEND | O_CREAT | O_EXCL); //Crea el fichero
      if(file == -1){
        perror("Error");
        return;
      }
        dup2(file, STDERR_FILENO);
        strcpy(fileRederr,trozos[1]);
        close(file);
    }
  }
  else{
      if( strcmp(fileRederr, "original")==0 ){
        printf("La salida se realiza por la configuracion original\n");
      }
      else{
        printf("La salida se realiza al fichero %s", fileRederr);
      }
  }
}

void entorno(char *trozos[], char *envp[]){

  if(trozos[1]!=NULL){

    if( strcmp(trozos[1],"-environ")==0 ){
      mostrarEntorno(environ, "environ");
      return;
    }
    else if( strcmp(trozos[1],"-addr")==0 ){
      if(environ[0]!=NULL && envp[0]!=NULL){
      printf("%10s: %12p (almacenado en %12p)\n", "environ", &environ[0], &environ );
      printf("%10s: %12p (almacenado en %12p)\n", "main arg3", &envp[0], &envp );
      }
      else{ puts("Error"); }
      return;
    }
    else{
      puts("entorno [-environ|-addr] 	 Muestra el entorno del proceso");
    }

  }
  else{
    mostrarEntorno(envp, "main arg3");
  }
}

void mostrarVar(char *trozos[], char *envp[]){

  int posMain, posEnv;
  char *getEnvVar;

  if(trozos[1]!=NULL){
    posMain = buscarVariable(trozos[1], envp);
    posEnv = buscarVariable(trozos[1], environ);
    getEnvVar = getenv(trozos[1]);

    if(posMain != -1){
      printf("Con arg3 main %s(%p) @%p\n", envp[posMain], envp[posMain], &envp[posMain]);
    }
    if(posEnv != -1){
      printf("Con environ %s(%p) @%p\n", environ[posEnv], environ[posEnv], &environ[posEnv]);
    }
    if(getEnvVar != NULL){
      printf("Con getenv %s(%p)\n", getEnvVar, getEnvVar);
    }
  }
  else{
    entorno(trozos, envp);
  }

}

void mostrarEntorno (char **entorn, char * nombre_entorno){
  int i=0;
  while (entorn[i]!=NULL) {
    printf ("%p->%s[%d]=(%p) %s\n", &entorn[i],
    nombre_entorno, i,entorn[i],entorn[i]);
    i++;
  }
}

void cambiarVar(char *trozos[], char *envp[]){
  char *stringEnv = malloc(MAX_ENTRADA * sizeof(char));

  if(trozos[1]!=NULL){
      if(trozos[2]!=NULL && trozos[3]!=NULL){
        if( strcmp(trozos[1],"-a")==0 ){
          cambiarVariable(trozos[2], trozos[3], envp);
        }
        else if( strcmp(trozos[1],"-e")==0 ){
          cambiarVariable(trozos[2], trozos[3], environ);
        }
        else if( strcmp(trozos[1],"-p")==0 ){
          strcpy(stringEnv,trozos[2]);
          strcat(stringEnv,"=");
          strcat(stringEnv,trozos[3]);
          putenv(stringEnv);

        }
        else{ ayuda(trozos[0]); }
      }
      else{ ayuda(trozos[0]); }
  }
  else{ ayuda(trozos[0]); }

}

int buscarVariable (char * var, char *e[]){
  int pos=0;
  char aux[MAXVAR];
  strcpy (aux,var);
  strcat (aux,"=");

  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else
      pos++;

  errno=ENOENT;
  /*no hay tal variable*/
  return(-1);
}

int cambiarVariable(char * var, char * valor, char *e[]){
  int pos;

  char *aux;if ((pos=buscarVariable(var,e))==-1)
    return(-1);
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
    return -1;

  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

void hfork(int ntrozos, char *trozos[]){
  pid_t pid = fork();
		if (pid==-1)
			perror("Error");

		if (pid==0){
			printf("Ejecutando proceso %i\n", getpid());
		}
    waitpid(pid,NULL,0);
}

char *NombreUsuario(uid_t uid){
	struct passwd *p;

	if((p=getpwuid(uid))==NULL){
		return(" ??????");
	}
	return p->pw_name;
}

uid_t UidUsuario(char *nombre){
	struct passwd *p;
	if((p=getpwnam(nombre))==NULL){
		return (uid_t) -1;
	}
	return p->pw_uid;
}

void hgetuid (char * trozos[]){
	uid_t real=getuid(), efec=geteuid();
	printf ("Credencial real: %d, (%s)\n", real, NombreUsuario(real));
	printf ("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario(efec));
}

int setLogin(char *user){
  id_t uid;
  if((uid=UidUsuario(user))==(uid_t) -1){
    printf("Usuario no existente %s\n", user);
    return (-1);
  }
  if(setuid(uid)==-1){
    printf("Imposible cambiar credencial: %s\n", strerror(errno));
    return (-1);
  }
  return 0;
}

void hsetuid (char * trozos[]){
  uid_t uid;
  int aux;

  if(trozos[2]!=NULL){
    if( strcmp(trozos[2], "-l")==0){
      if(trozos[3]!=NULL){
        if((uid=UidUsuario(trozos[3]))==(uid_t) -1){
          printf("Usuario no existente %s\n", trozos[3]);
          return;
        }
      }
      else{
        hgetuid(trozos);
        return;
      }
    }
    else{
      if((uid=(uid_t) ((aux=atoi(trozos[2]))<0)? -1: aux) == (uid_t)-1){
        printf("Valor no valido de la credencial %s\n", trozos[2]);
        return;
      }
    }
    if(setuid(uid)==-1){ printf("Imposible cambiar credencial: %s\n", strerror(errno)); }
  }
  else{
    hgetuid(trozos);
    return;
  }
}


void uid(char *trozos[]){

  if(trozos[1]!=NULL){
      if( strcmp(trozos[1],"-get")==0 ){
        hgetuid(trozos);
      }
      else if( strcmp(trozos[1],"-set")==0 ){
        hsetuid(trozos);
      }
      else{ hsetuid(trozos); }
  }
  else{ hgetuid(trozos); }

}

void insertProcess(tListaProc *Lp, pid_t pid, int priority, uid_t uid, char *commandLine){

  elementProc item;
  time_t date_now;
  struct tm *date;
  time(&date_now);

  if ((date = localtime(&date_now)) != NULL) {

    item.pid = pid;
    item.priority = priority;
    strcpy( item.user, NombreUsuario(uid) );
    strcpy( item.commandLine, commandLine );
    strcpy(item.timeD, asctime(date));
    strcpy(item.stat, "ACTIVO");
    item.senal = 0;

    if( !insertElementProc(item, Lp) ){
      puts("Error al insertar en la lista de procesos");
    }
  }
  else { perror("Error"); }

}

char *concatTrozos(char *trozos[]){
  static char commandLin[MAX_ENTRADA];

    for(int i=0; trozos[i]!=NULL; i++){
      if(i==0){
        strcpy(commandLin, trozos[0]);
      }else{
        strcat(commandLin, " ");
        strcat(commandLin, trozos[i]);
      }
    }
  return commandLin;
}


void executePrograms(char *trozos[], bool sec, tListaProc *Lp, char* commandLin){
  uid_t uid = getuid();
  pid_t pid = fork();
  int priority = getpriority(PRIO_PROCESS, pid);

  if (pid==-1)
    perror("Error");

  if (pid==0){
    if( execvp(trozos[0], &trozos[0]) == -1 ){
      perror("Error");
    }
    exit(255);
  }
  else{
    if(!sec){ waitpid(pid,NULL,0); }
    else{
      insertProcess(Lp, pid, priority, uid, commandLin);
    }
  }
}

void ejec(int ntrozos, char *trozos[]){

  if(trozos[1]!=NULL){
    if( execvp(trozos[1], &trozos[1]) == -1 ){
      perror("Error");
    }
  }
  else{ ayuda(trozos[0]); }

}

void ejecpri(int ntrozos, char *trozos[]){
  pid_t pid;

  if( trozos[1]!=NULL){
     if(trozos[2]!=NULL){
        pid=getpid();
          setprio(trozos[1], (int) pid);
          if(execvp(trozos[2], &trozos[2]) == -1){
            perror("Error");
          }
    }
    else{ ayuda(trozos[0]); }
  }
  else{ ayuda(trozos[0]); }
}


void fg(char *trozos[], tListaProc *Lp){
  if(trozos[1]!=NULL){

    executePrograms(&trozos[1], false, Lp, concatTrozos(&trozos[0]));
  }
  else{ ayuda(trozos[0]); }
  }


void fgpri(char *trozos[]){
  pid_t pid;

  if(trozos[1]!=NULL){
    pid = fork();
    if (pid==-1)
      perror("Error");

    if (pid==0){
      setprio(trozos[1], (int) pid);
      if( execvp(trozos[2], &trozos[2]) == -1 ){
        perror("Error");
      }
      exit(255);
    }
    else{ waitpid(pid,NULL,0); }
  }
  else{ ayuda(trozos[0]); }
}


void back(char *trozos[], tListaProc *Lp){

  if(trozos[1]!=NULL){
    executePrograms(&trozos[1], true, Lp, concatTrozos(&trozos[1]));
  }
  else{ ayuda(trozos[0]); }
  }

void backpri(char *trozos[], tListaProc *Lp){
  pid_t pid;

  if(trozos[1]!=NULL){
    pid = fork();
    if (pid==-1)
      perror("Error");

    if (pid==0){
      setprio(trozos[1], (int) pid);
      if( execvp(trozos[2], &trozos[2]) == -1 ){
        perror("Error");
      }
      exit(255);
    }
    insertProcess(Lp, pid, atoi(trozos[1]), getuid(), concatTrozos(&trozos[2]));
  }
  else{ ayuda(trozos[0]); }
}

void ejecas(char *trozos[]){
  pid_t pid;
  int test;

  if( trozos[1]!=NULL){
     if(trozos[2]!=NULL){
        pid=getpid();
        test = setLogin(trozos[1]);
        if(test != -1){
          if(execvp(trozos[2], &trozos[2]) == -1){ perror("Error"); }
        }
    }
    else{ ayuda(trozos[0]); }
  }
  else{ ayuda(trozos[0]); }
}

void fgas(char *trozos[]){
  pid_t pid;

  if(trozos[1]!=NULL && trozos[2]!=NULL){
    pid = fork();
    if (pid==-1)
      perror("Error");

    if (pid==0){
      if( setLogin(trozos[1])==0 ){
        if( execvp(trozos[2], &trozos[2]) == -1 ){
          perror("Error");
        }
      }
      exit(255);
    }
    else{ waitpid(pid,NULL,0); }
  }
  else{ ayuda(trozos[0]); }
}

void bgas(char *trozos[], tListaProc *Lp){
  pid_t pid;

  if(trozos[1]!=NULL && trozos[2]!=NULL){
    pid = fork();
    if (pid==-1)
      perror("Error");

    if (pid==0){
      if(setLogin(trozos[1])==0){
        if( execvp(trozos[2], &trozos[2]) == -1 ){
          perror("Error");
        }
      }
      exit(255);
    }
    insertProcess(Lp, pid, getpriority(PRIO_PROCESS, pid), UidUsuario(trozos[1]), concatTrozos(&trozos[2]));
  }
  else{ ayuda(trozos[0]); }
}


void listjobs(tListaProc *Lp){ printListaProc(Lp); }


void job(char *trozos[], tListaProc *Lp){
  int pid;
  posicionProc p;
  int est;

  if(trozos[1]!=NULL){

    if( strcmp(trozos[1],"-fg") == 0 ){
      if(trozos[2]!=NULL){
        pid = atoi(trozos[2]);
        p = findElementProc(Lp, pid);
        if(p!=NULL){
          waitpid(pid, &est, 0);
          printf("El proceso %d fue terminado ", pid);
          deleteElementProc(Lp, p);

          if(WIFEXITED(est)){
      			printf("correctamente\n");
      		}else	if(WIFSIGNALED(est)){
      			printf("por la senal %s\n", NombreSenal(WTERMSIG(est)) );
          }
        }
      }
      else{listjobs(Lp); }
    }
    else{
      pid = atoi(trozos[1]);
      p = findElementProc(Lp, pid);
      if(p!=NULL){ printProc(p, Lp); }
      else{ listjobs(Lp); }

    }

  }
  else{ listjobs(Lp); }
}


void borrarjobs(char *trozos[], tListaProc *Lp){
  if(trozos[1]!=NULL){
    if( strcmp(trozos[1],"-term") == 0 ){ borrarterm(Lp); }
    else if( strcmp(trozos[1],"-sig") == 0 ){ borrarsig(Lp); }
    else if( strcmp(trozos[1],"-all") == 0 ){ borrarterm(Lp); borrarsig(Lp); }
    else if( strcmp(trozos[1],"-clear") == 0 ){ deleteListProc(Lp); }
    else{ listjobs(Lp); }
  }
  else{ listjobs(Lp); }
}

void prog(int ntrozos, char *trozos[], tListaProc *Lp){
    pid_t pid;

    if( strcmp(trozos[ntrozos-1], "&") ==0 ){
      trozos[ntrozos-1] = NULL;
      executePrograms(trozos, true, Lp, concatTrozos(trozos));
    }
    else{ executePrograms(trozos, false, Lp, concatTrozos(trozos)); }
}


//Funcion para abandonar el shell
int salirShell(tLista *L, tListaMem *Lm, tListaProc *Lp){
    for (posicion p = first(*L); p != NULL; p = first(*L)) {
        deleteElement(L, p);
    }
    deleteListMem(Lm);
    deleteListProc(Lp);
    return 1;
}

//Funcion que printea la lista de comandos
void ayuda(char cmd[]) {
    if (cmd != NULL) {
                //Comandos P0
        if (strcmp("autores", cmd) == 0) {
            puts("autores [-n|-l]	Muestra los nombres y logins de los autores");
        } else if (strcmp("pid", cmd) == 0) {
            puts("pid [-p]	Muestra el pid del shell o de su proceso padre");
        } else if (strcmp("carpeta", cmd) == 0) {
            puts("carpeta [dir]	Cambia (o muestra) el directorio actual del shell");
        } else if (strcmp("fecha", cmd) == 0) {
            puts("fecha [-d|-h]	Muestra la fecha y o la hora actual");
        } else if (strcmp("hist", cmd) == 0) {
            puts("hist [-c|-N]	Muestra el historico de comandos, con -c lo borra");
        } else if (strcmp("comando", cmd) == 0) {
            puts("comando [-N]	Repite el comando N (del historico)");
        } else if (strcmp("infosis", cmd) == 0) {
            puts("infosis 	Muestra informacion de la maquina donde corre el shell");

                      //Comandos P1
        } else if (strcmp("borrarrec", cmd) == 0) {
            puts("borrarrec [name1 name2 ..]	 Borra ficheros o directorios no vacios");
        } else if (strcmp("listfich", cmd) == 0) {
            puts("listfich [-long][-link][-acc] n1 n2 ..	lista ficheros");
        } else if (strcmp("listdir", cmd) == 0) {
            puts("listdir [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 ..	lista ficheros contenidos de directorios");
        } else if (strcmp("borrar", cmd) == 0) {
            puts("borrar [name1 name2 ..]	 Borra ficheros o directorios vacios");
        } else if (strcmp("crear", cmd) == 0) {
            puts("crear [-f] [name]	 Crea un fichero o directorio");
        } else if (strcmp("ayuda", cmd) == 0) {
            puts("ayuda [cmd]	Muestra ayuda sobre los comandos");

                    //Comandos P2
        } else if (strcmp("recursiva", cmd) == 0) {
              puts("recursiva [n] 	Invoca a la funcion recursiva n veces");
        } else if (strcmp("e-s", cmd) == 0) {
              puts("e-s [read|write] [-o] fiche addr cont 	Invoca a la funcion recursiva n veces");
        } else if (strcmp("volcarmem", cmd) == 0) {
              puts("volcarmem addr cont 	Vuelca en pantallas los"
                    "contenidos (cont bytes) de la posicion de memoria addr");
        } else if (strcmp("llenarmem", cmd) == 0) {
              puts("llenarmem addr cont byte 	 Llena la memoria a partir de addr con byte");
        } else if (strcmp("dealloc", cmd) == 0) {
              puts("dealloc -malloc|-shared|-mmap....	Desasigna un bloque "
                    "de memoria asignado con malloc, shared o mmap");
        } else if (strcmp("malloc", cmd) == 0) {
              puts("malloc [-free] tam	 asigna(o desasigna) memoria en el programa");
        } else if (strcmp("mmap", cmd) == 0) {
              puts("mmap [-free] fich prms	 mapea(o desmapea) ficheros "
                    " en el espacio de direcciones del proceso");
        } else if (strcmp("shared", cmd) == 0) {
              puts("shared [-free|-create|-delkey] cl tam	 asigna(o desasigna) "
                    "memoria compartida en el programa");
        } else if (strcmp("memoria", cmd) == 0) {
              puts("memoria [-blocks|-funcs|-vars|-all|-pmap] ..	"
                    "Muestra muestra detalles de la memoria del proceso");

                    //Comandos P3
        } else if (strcmp("priority", cmd) == 0) {
              puts("priority [pid] [valor] 	Muestra o cambia la prioridad del proceso pid a valor");
        } else if (strcmp("rederr", cmd)==0) {
              puts("rederr [-reset] fich 	Redirecciona el error estándar del shell");
        } else if (strcmp("entorno", cmd) == 0) {
              puts("entorno [-environ|-addr] 	 Muestra el entorno del proceso");
        } else if (strcmp("mostrarvar", cmd) == 0) {
              puts("mostrarvar 	 Muestra el valor y las direcciones de una variable de entorno");
        } else if (strcmp("cambiarvar", cmd)==0) {
              puts("cambiarvar [-a|-e|-p] var valor	Cambia el valor de una variable de entorno");
        } else if (strcmp("uid", cmd) == 0) {
              puts("uid [-get|-set] [-l] [id] 	 Muestra o cambia (si puede) la credencial del proceso que ejecuta el shell");
        } else if (strcmp("fork", cmd) == 0) {
              puts("fork 	Hace una llamada fork para crear un proceso");
        } else if (strcmp("ejec", cmd) == 0) {
              puts("ejec prog args....	Ejecuta, sin crear proceso,prog con argumentos");
        } else if (strcmp("ejecpri", cmd) == 0) {
              puts("ejecpri prio prog args....	Ejecuta, sin crear proceso, prog con argumentos con la prioridad cambiada a prio");
        } else if (strcmp("fg", cmd) == 0) {
              puts("fg prog args...	Crea un proceso que ejecuta en primer plano prog con argumentos");
        } else if (strcmp("fgpri", cmd) == 0) {
              puts("fgpri prio prog args...	Crea un proceso que ejecuta en primer plano prog "
                   "con argumentos con la prioridad cambiada a prio");
        } else if (strcmp("back", cmd) == 0) {
              puts("back prog args...	Crea un proceso que ejecuta en segundo plano prog con argumentos");
        } else if (strcmp("backpri", cmd) == 0) {
              puts("backpri prio prog args...	Crea un proceso que ejecuta en segundo "
                   "plano prog con argumentos con la prioridad cambiada a prio");
        } else if (strcmp("ejecas", cmd) == 0) {
              puts("ejecas user prog args..	Ejecuta, sin crear proceso y como el usuario user, prog con argumentos");
        } else if (strcmp("fgas", cmd)==0) {
              puts("fgas us prog args...	Crea un proceso que ejecuta en primer plano, y como el usuario us, prog con argumentos");
        } else if (strcmp("bgas", cmd) == 0) {
              puts("bgas us prog args...	Crea un proceso que ejecuta en segundo plano, y como el usuario us, prog con argumentos");
        } else if (strcmp("listjobs", cmd) == 0) {
              puts("listjobs 	Lista los procesos en segundo plano");
        } else if (strcmp("job", cmd)==0) {
              puts("job [-fg] pid	Muestra informacion del proceso pid. -fg lo pasa a primer plano");
        } else if (strcmp("borrarjobs", cmd) == 0) {
              puts("borrarjobs [-term][-sig][-all][-clear]	Elimina los propcesos terminados o terminados por senal de la lista de procesos en s.p.");

                    //Comandos de salida
        } else if (strcmp("fin", cmd) == 0) {
              puts("fin 	Termina la ejecucion del shell");
        } else if (strcmp("salir", cmd) == 0) {
          puts("salir 	Termina la ejecucion del shell");
        } else if (strcmp("bye", cmd) == 0) {
              puts("bye 	Termina la ejecucion del shell");
        } else {
            puts("Comando no disponible");
        }
    } else {
        puts("'ayuda cmd' donde cmd es uno de los siguientes comandos:");
        printf("fin / salir / bye / fecha / pid / autores / hist / comando / carpeta / infosis / ayuda"); //Comandos p1
        printf(" / crear / borrar / borrarrec / listfich / listdir / ");  //Comandos p1

        printf("recursiva / e-s / volcarmem / llenarmem / dealloc / malloc / mmap / shared / memoria"); //Comandos p2

        printf("priority / rederr / uid / entorno / mostrarvar / cambiarvar / fork / ejec / ejecpri "); //Comandos p3
        printf("/ ejecas / fg / fgpri / back / backpri / fgas / bgas / listjobs / borrarjobs / job\n"); //Comandos p3
    }
}


int main(int argc, char *argv[], char *envp[]) {
    char *comando = malloc(MAX_ENTRADA * sizeof(char));
    char copiaComando[MAX_ENTRADA];
    char *trozos[MAX_TROZOS];
    int ntrozos;
    int end = 0;
    int i = 0;
    int arrayLen = 0;

    //Creamos la lista del historico
    tLista L;
    createList(&L);

    //Creamos la lista para la memoria
      tListaMem Lm;
      createListMem(&Lm);

    //Creamos la lista para procesos
      tListaProc Lp;
      createListProc(&Lp);

    while (!end) {
        prishell();
        readshell(comando, &L);
        strcpy(copiaComando, comando);
        arrayLen = strlen(copiaComando);
        for (i = 0; i <= arrayLen; i++) {
            if (copiaComando[i] == '\n') {
                copiaComando[i] = ' ';
            }
        }
        ntrozos = TrocearCadena(comando, trozos);
        if (trozos[0] != NULL) {
            insertList(copiaComando, trozos, &L);
        }
        end = processCommand(trozos, ntrozos, &L, &Lm, envp, &Lp);
    }
    free(comando);
    return 0;
}
