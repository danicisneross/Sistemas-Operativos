/*
 * TÍTULO: P3
 * MATERIA: Sistemas Operativos
 * AUTORA 1: Clara Cidrás Fernández LOGIN 1: clara.cidras
 * AUTOR 2: Pablo Torrado García LOGIN 2: p.torrado
 * GRUPO: 1.3
 * CURSO: 2021 / 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>


#include "list.h"
#include "listMem.h"
#include "listJobs.h"



#define NUM 200
#define TROZOS 30
#define LEERCOMPLETO ((ssize_t)-1)
#define MAXVAR 80

// Variables globais
int a = 6;
char b = 'b';
double c = 1.;



typedef char tData[NUM];

bool error = false;
char *trozos[TROZOS];

struct SEN{
    char *nombre;
    int senal;
};


// ---------------------------------------------------------------------
char LetraTF (mode_t m){
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's';  //socket
        case S_IFLNK: return 'l';   // symbolic link
        case S_IFREG: return '-';   // ficheiro normal
        case S_IFBLK: return 'b';   // block device
        case S_IFDIR: return 'd';   // directorio
        case S_IFCHR: return 'c';   // char device
        case S_IFIFO: return 'p';   // pipe
        default: return '?';   // desoñecido, non debería aparecer
    } }


int borrarRecursivo(char* dirName ){
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir (dirName);

    if (dir != NULL){
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (LetraTF(s.st_mode) == 'd') {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                    borrarRecursivo(abs_filename);
                }
            } else{
                remove(abs_filename);
            }
        }
        closedir (dir);
    }

    remove(dirName);

    return 0;

}


// ---------------------------------------------------------------------

void imprimirPrompt() {
    printf("-> ");
}

bool terminado() {

    if (strcmp(trozos[0], "fin") == 0)
        return true;
    else if (strcmp(trozos[0], "bye") == 0)
        return true;
    else if (strcmp(trozos[0], "salir") == 0)
        return true;
    else
        return false;
}

int trocearCadena(tData *data, char *trozo[]) {
    int i = 1;
    if ((trozo[0] = strtok(*data, " \n\t")) == NULL)
        return 0;
    while ((trozo[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void leerEntrada(tData *data) {
    fgets(*data, NUM, stdin);
}

char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r'; // propietario
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r'; // grupo
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r'; // resto
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s'; // setuid, setgid y stickybit
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    return (permisos);
}

int listFilesAcc(char *dirName, int cont, int ind) {      // FUNCIÓN NOVA
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir(dirName);


    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (cont == 0) {
                printf("*** %s ***\n", dirName);
                cont++;
            }

            if (ind == 2) {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 &&
                    strcmp(dp->d_name, ".DS_Store") != 0)
                    printf("%lld  %s\n", s.st_size, dp->d_name);

            } else {


                // owner and group
                struct group *grp;
                struct passwd *pwd;
                grp = getgrgid(s.st_gid);
                pwd = getpwuid(s.st_uid);

                // mode
                static char mode[12];
                strcpy(mode, ConvierteModo2(s.st_mode));
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 &&
                    strcmp(dp->d_name, ".DS_Store") != 0) {
                    char tempo[50];
                    strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_atime));

                    printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                           grp->gr_name, mode, s.st_size, dp->d_name);
                }
            }
        }
    }
    closedir(dir);
    return 0;
}


int listFilesLong(char *dirName, int cont){
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir(dirName);


    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (cont == 0) {
                printf("*** %s ***\n", dirName);
                cont++;
            }


            // owner and group
            struct group *grp;
            struct passwd *pwd;
            grp = getgrgid(s.st_gid);
            pwd = getpwuid(s.st_uid);

            // mode
            static char mode[12];
            strcpy(mode, ConvierteModo2(s.st_mode));
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".DS_Store") != 0) {
                char tempo[50];
                strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                       grp->gr_name, mode, s.st_size, dp->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}


int listFilesHid(char *dirName, int cont, int ind){
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir(dirName);


    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (cont == 0) {
                printf("*** %s ***\n", dirName);
                cont++;
            }

            if (ind == 2) {
                printf("%lld  %s\n", s.st_size, dp->d_name);

            } else {

                // owner and group
                struct group *grp;
                struct passwd *pwd;
                grp = getgrgid(s.st_gid);
                pwd = getpwuid(s.st_uid);

                // mode
                static char mode[12];
                strcpy(mode, ConvierteModo2(s.st_mode));

                char tempo[50];
                strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                       grp->gr_name, mode, s.st_size, dp->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}


int listdirRecursivo(char* dirName){
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir (dirName);
    if (dir != NULL){
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (LetraTF(s.st_mode) == 'd') {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".DS_Store") != 0) {
                    printf("%lld\t%s\n", s.st_size, dp->d_name);
                    listdirRecursivo(abs_filename);
                    printf("************%s\n",abs_filename);
                }
            } else{
                printf("%lld\t%s\n", s.st_size, dp->d_name);
            }
        }
    }
    return 0;
}


int listdirRecursivoLong(char* dirName){
    DIR *dir;
    struct dirent *dp;
    char abs_filename[FILENAME_MAX];
    dir = opendir (dirName);
    if (dir != NULL){
        while ((dp = readdir(dir)) != NULL) {
            struct stat s;
            snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirName, dp->d_name);
            if (lstat(abs_filename, &s) < 0)
                error = true;

            if (LetraTF(s.st_mode) == 'd') {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".DS_Store") != 0) {
                    struct group *grp;
                    struct passwd *pwd;
                    grp = getgrgid(s.st_gid);
                    pwd = getpwuid(s.st_uid);

                    // mode
                    static char mode[12];
                    strcpy(mode, ConvierteModo2(s.st_mode));
                    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".DS_Store") != 0) {
                        char tempo[50];
                        strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                        printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                               grp->gr_name, mode, s.st_size, dp->d_name);
                        listdirRecursivoLong(abs_filename);
                        printf("************%s\n",abs_filename);
                    }
                } else{

                    // owner and group
                    struct group *grp;
                    struct passwd *pwd;
                    grp = getgrgid(s.st_gid);
                    pwd = getpwuid(s.st_uid);

                    // mode
                    static char mode[12];
                    strcpy(mode, ConvierteModo2(s.st_mode));

                    char tempo[50];
                    strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                    printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                           grp->gr_name, mode, s.st_size, dp->d_name);
                }
            }
        }
    }
    return 0;
}

//************************************************************************
// FUNCIÓNS MMAP
//************************************************************************


void * MmapFichero (char * fichero, int protection){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    if (protection&PROT_WRITE)  modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    /*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    return p;
}

void * Mmap (char *arg[]) { // arg[0] is the file name and arg[1] is the permissions
    char *perm;
    void *p;
    int protection = 0;

    if ((perm=arg[1]) != NULL && strlen(perm) < 4) {
        if (strchr(perm,'r')!=NULL) protection |= PROT_READ;
        if (strchr(perm,'w')!=NULL) protection |= PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection |= PROT_EXEC;
    }
    if ((p = MmapFichero(arg[0], protection)) == NULL)
        error = true;
    else
        printf("Fichero %s mapeado en %p\n", arg[0], p);
    return p;
}

//************************************************************************
// FUNCIÓNS SHARED
//************************************************************************

void * ObtenerMemoriaShmget (key_t clave, size_t tam) {                /*Obtienen un puntero a una zaona de memoria compartida*/
    /*si tam >0 intenta crearla y si tam==0 asume que existe*/
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam)  /*si tam no es 0 la crea en modo exclusivo esta funcion vale para shared y shared -create*/
        flags = flags | IPC_CREAT | IPC_EXCL;
    /*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE)   /*no nos vale*/
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;   /*si se ha creado y no se puede mapear*/
        if (tam)
            /*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    return (p);
}

void *SharedCreate (char *arg[]) { /*arg[0] is the key and arg[1] is the size*/
    key_t k;
    size_t tam=0;
    void *p;

    k=(key_t) atoi(arg[0]);
    if (arg[1]!=NULL)
        tam=(size_t) atoll(arg[1]);
    if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
        error = true;
    else
        printf ("Memoria de compartida de clave %d asignada en %p\n",k,p);
    return p;

}

void SharedDelkey (char *args) { /*arg[0] points to a str containing the key*/
    key_t clave;
    int id;
    char *key = args;
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("Error: Clave no válida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        printf("Error: No hay memoria reservada para esta clave\n");
        return; }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        printf ("Error: No hay memoria reservada para esta clave\n");
}

//************************************************************************
// FUNCIÓNS MEMORIA
//************************************************************************

void dopmap (void) {    /*no arguments necessary*/
    pid_t pid;         /*ejecuta el comando externo pmap para */
    char elpid[32];    /*pasandole el pid del proceso actual */
    char *argv[3]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1) {
            printf("No se puede ejecutar pmap\n");
            error = true;
        }
        exit(1);
    }
    waitpid (pid,NULL,0);
}

//************************************************************************
// FUNCIÓNS RECURSIVA
//************************************************************************

void doRecursiva (int n) {
    char automatico[4096];
    static char estatico[4096];
    printf ("parámetro n = %d:\n",n);
    printf ("parámetro en ->        %p\n",&n);
    printf ("array estático en ->   %p \n",estatico);
    printf ("array automático en -> %p\n",automatico);
    puts("");
    n--;
    if (n > 0)
        doRecursiva(n);
}
//************************************************************************
// LER FICHEIRO
//************************************************************************

ssize_t LeerFichero (char *fich, void *p, ssize_t n){
    ssize_t nleidos,tam=n;
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return ((ssize_t)-1);
    if (n==LEERCOMPLETO)
        tam=(ssize_t) s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

//************************************************************************
// FUNCIÓN ENTORNO
//************************************************************************

void MostrarEntorno (char **entorno, char * nombre_entorno) {
    int i=0;
    while (entorno[i]!=NULL) {
        printf ("%p->%s[%d]=(%p) %s\n", &entorno[i],
                nombre_entorno, i,entorno[i],entorno[i]);
        i++;
    }
}

//************************************************************************
// FUNCIÓN MOSTRARVAR
//************************************************************************

int BuscarVariable (char * var, char *e[]) {
    int pos=0;
    char aux[MAXVAR];
    strcpy (aux,var);
    strcat (aux,"=");
    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

//************************************************************************
// FUNCIÓN CAMBIARVAR
//************************************************************************

int CambiarVariable(char * var, char * valor, char *e[]) {
    int pos;
    char *aux;
    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);
    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}


//************************************************************************
// FUNCIÓNS UID
//************************************************************************

char * NombreUsuario (uid_t uid) {
    struct passwd *p;
    if ((p=getpwuid(uid))==NULL)
        return ("??????");
    return p->pw_name;
}

uid_t UidUsuario (char * nombre) {
    struct passwd *p;
    if ((p=getpwnam (nombre))==NULL)
        return  (uid_t) -1;
    return p->pw_uid;
}

void MostrarUidsProceso (void) {
    uid_t real=getuid(), efec=geteuid();
    printf ("Credencial real: %d, (%s)\n", real, NombreUsuario (real));
    printf ("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario (efec));
}

int CambiarUidLogin (char * login) {            // se se cambia devolve 0, senon -1
    uid_t uid;
    if ((uid=UidUsuario(login)) == (uid_t) -1){
        printf("login no válido: %s\n", login);
        return -1;
    }
    if (setuid(uid) != 0) {
        printf("Imposible cambiar credencial: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

//************************************************************************
// FUNCIÓNS SINAL
//************************************************************************

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
};    /*fin array sigstrnum */

int Senal(char * sen) {

    /*devuel el numero de senial a partir del nombre*/

    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal;
    return -1;
}

char *NombreSenal(int sen) {

    /*devuelve el nombre senal a partir de la senal*/
    /* para sitios donde no hay sig2str*/

    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}

//************************************************************************
//************************************************************************


void procesarEntrada(tData *data, Lista *L, ListaM *LM, ListaM *LMmap, ListaM *LMshared, ListaJ *ListJobs, int argc, char *argv[], char * env[]) {

    insertarNodo(L, data);

    int i = trocearCadena(data, trozos);

    if (strcmp(trozos[0], "autores") == 0) { // Se o comando é autores
        if (i == 1)
            printf("Nombre del autor1: login del autor1\n"
                   "Nombre del autor2: login del autor2\n");
        else if (strcmp(trozos[1], "-l") == 0)
            printf("login del autor1\n"
                   "login del autor2\n");
        else if (strcmp(trozos[1], "-n") == 0)
            printf("Nombre del autor1\n"
                   "Nombre del autor2\n");


    } else if (strcmp(trozos[0], "pid") == 0) {    // Se o comando é pid
        if (i == 1)
            printf("Pid del shell: %d\n", getpid());
        else if (strcmp(trozos[1], "-p") == 0) {
            printf("Pid del padre del shell: %d\n", getppid());
        }


    } else if (strcmp(trozos[0], "carpeta") == 0) {  // Se o comando é carpeta
        if (i == 1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error=true;
        } else {
            int j = chdir(trozos[1]);
            if (j < 0) {
                error=true;
            }
        }


    } else if (strcmp(trozos[0], "fecha") == 0) {  // Se o comando é fecha
        time_t T = time(NULL);
        struct tm tempo = *localtime(&T);
        if (i == 1) {
            printf("%02d/%02d/%04d\n", tempo.tm_mday, tempo.tm_mon + 1, tempo.tm_year + 1900);
            printf("%02d:%02d:%02d\n", tempo.tm_hour, tempo.tm_min, tempo.tm_sec);
        } else if (strcmp(trozos[1], "-d") == 0) {
            printf("%02d/%02d/%04d\n", tempo.tm_mday, tempo.tm_mon + 1, tempo.tm_year + 1900);

        } else if (strcmp(trozos[1], "-h") == 0)
            printf("%02d:%02d:%02d\n", tempo.tm_hour, tempo.tm_min, tempo.tm_sec);


    } else if (strcmp(trozos[0], "hist") == 0) {  // Se o comando é hist
        if (i == 1) {
            imprimirLista(L);

        } else if (strcmp(trozos[1], "-c") == 0) {
            vaciarLista(L);

        } else {
            char num[10];
            char orixe[10];
            int n;
            strcpy(orixe, trozos[1]);

            memmove(num, orixe + 1, strlen(orixe));
            n = atoi(num);
            imprimirListaN(L, n);
        }

    } else if (strcmp(trozos[0], "comando") == 0) {
        if (i == 2) {
            int n;
            n = atoi(trozos[1]);
            if (imprimirComando(L, n)==true) {
                tData cmd;
                strcpy(cmd, *obter(n, L));
                procesarEntrada(&cmd, L, LM, LMmap, LMshared, ListJobs, argc, argv, env);
            }
        } else
            printf("Error: Seleccione el número del comando que quiere consultar\n");


    } else if (strcmp(trozos[0], "infosis") == 0) {  // Se o comando é infosis
        struct utsname unameData;
        uname(&unameData);
        printf("%s\n", unameData.sysname);


    } else if (strcmp(trozos[0], "ayuda") == 0) {  // Se o comando é ayuda
        if (i == 1)
            printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n"
                   "fin salir bye fecha pid autores hist comando carpeta infosis ayuda \n");
        else if (strcmp(trozos[1], "fin") == 0) {
            printf("fin \tTermina la ejecucion del shell\n");

        } else if (strcmp(trozos[1], "salir") == 0) {
            printf("salir \tTermina la ejecucion del shell\n");

        } else if (strcmp(trozos[1], "bye") == 0) {
            printf("bye \tTermina la ejecucion del shell\n");

        } else if (strcmp(trozos[1], "fecha") == 0) {
            printf("fecha [-d|-h]\tMuestra la fecha y o la hora actual\n");

        } else if (strcmp(trozos[1], "pid") == 0) {
            printf("pid [-p]\tMuestra el pid del shell o de su proceso padre\n");

        } else if (strcmp(trozos[1], "autores") == 0) {
            printf("autores [-n|-l]\tMuestra los nombres y logins de los autores\n");

        } else if (strcmp(trozos[1], "hist") == 0) {
            printf("hist [-c|-N]\tMuestra el historico de comandos, con -c lo borra\n");

        } else if (strcmp(trozos[1], "comando") == 0) {
            printf("comando [-N]\tRepite el comando N (del historico)\n");

        } else if (strcmp(trozos[1], "carpeta") == 0) {
            printf("carpeta [dir]\tCambia (o muestra) el directorio actual del shell\n");

        } else if (strcmp(trozos[1], "infosis") == 0) {
            printf("infosis \tMuestra informacion de la maquina donde corre el shell\n");

        } else if (strcmp(trozos[1], "ayuda") == 0) {
            printf("ayuda [cmd]\tMuestra ayuda sobre los comandos\n");
        } else
            printf("%s no encontrado\n", trozos[1]);


        //-------------------------------------------------------------------
        // CREAR:


    } else if (strcmp(trozos[0], "crear") == 0) {
        if (i==1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error=true;

        } else if (strcmp(trozos[1],"-f") == 0) {

            if (open(trozos[2], O_RDONLY) != -1)
                printf("Imposible crear: File exists\n");
            else
                open(trozos[2], O_CREAT, 0700);

        } else if (mkdir(trozos[1], 0777) == -1) {
            error=true;
        }


        //-------------------------------------------------------------------
        // BORRAR:


    } else if (strcmp(trozos[0], "borrar") == 0) {
        if (i==1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error=true;
        }
        else{
            for(int j = 1; j < i; j++){
                if (remove(trozos[j]) != 0) {
                    error=true;
                }
            }
        }

        //-------------------------------------------------------------------
        // BORRAR RECURSIVO:

    } else if (strcmp(trozos[0], "borrarrec") == 0) {
        if (i==1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error=true;
        }else{
            for(int j = 1; j < i; j++){
                if(borrarRecursivo(trozos[j]) != 0)
                    error=true;
            }
        }


        //-------------------------------------------------------------------
        //LISTFICH:


    } else if (strcmp(trozos[0], "listfich") == 0) {
        if (i == 1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error = true;

        } else if (strcmp(trozos[1], "-long") == 0) {
            int a = 2;
            if (strcmp(trozos[2], "-acc") == 0 || strcmp(trozos[2], "-link") == 0)
                a = 3;

            for (int j = a; j < i; j++) {
                struct stat s;
                if (lstat(trozos[j], &s) == -1)
                    error = true;
                else {

                    // owner and group
                    struct group *grp;
                    struct passwd *pwd;
                    grp = getgrgid(s.st_gid);
                    pwd = getpwuid(s.st_uid);

                    // mode
                    static char mode[12];
                    strcpy(mode, ConvierteModo2(s.st_mode));


                    if (strcmp(trozos[2], "-acc") == 0) {       // Se queremos -ACC
                        // last access
                        char tempo[50];
                        strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_atime));

                        printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                               grp->gr_name, mode, s.st_size, trozos[j]);


                    } else if (strcmp(trozos[2], "-link") == 0 && LetraTF(s.st_mode) == 'l') {  // Se queremos -LINK
                        // date
                        char tempo[50];
                        strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));
                        char path[50];
                        realpath(trozos[j], path);

                        printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s -> %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name, grp->gr_name, mode, s.st_size, trozos[j], path);


                    } else if (strcmp(trozos[1], "-long") == 0){                                       // Se queremos -LONG
                        // date
                        char tempo[50];
                        strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                        printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                               grp->gr_name, mode, s.st_size, trozos[j]);
                    }
                }

            }

        } else {
            for (int j = 1; j < i; j++) {
                struct stat s;
                if (lstat(trozos[j], &s) == -1)
                    error = true;
                else
                    printf("%lld\t%s\n", s.st_size, trozos[j]);
            }
        }


        //-------------------------------------------------------------------
        //LISTDIR:


    } else if(strcmp(trozos[0], "listdir") == 0) {
        if (i == 1) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("%s\n", cwd);
            else
                error = true;

        } else if (strcmp(trozos[1], "-long") == 0) {
            if (i == 2){
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    printf("%s\n", cwd);
                else
                    error = true;
            }else {

                if (strcmp(trozos[2], "-acc") == 0) {    // -ACC

                    if (i!=3) {
                        for (int j = 3; j < i; j++) {
                            struct stat s;
                            if (lstat(trozos[j], &s) == -1)
                                error = true;

                            if (LetraTF(s.st_mode) == 'd') {      // Si é un directorio
                                int cont = 0;
                                if (listFilesAcc(trozos[j], cont, 2) != 0)
                                    error = true;

                            } else {     // Si é un ficheiro


                                // owner and group
                                struct group *grp;
                                struct passwd *pwd;
                                grp = getgrgid(s.st_gid);
                                pwd = getpwuid(s.st_uid);

                                // mode
                                static char mode[12];
                                strcpy(mode, ConvierteModo2(s.st_mode));
                                //time
                                char tempo[50];
                                strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_atime));

                                printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino,
                                       pwd->pw_name,
                                       grp->gr_name, mode, s.st_size, trozos[j]);

                            }
                        }
                    } else{
                        char cwd[PATH_MAX];
                        if (getcwd(cwd, sizeof(cwd)) != NULL)
                            printf("%s\n", cwd);
                        else
                            error = true;
                    }

                } else if (strcmp(trozos[2], "-link") == 0) {  // -LINK

                    if(i!=3) {
                        for (int j = 3; j < i; j++) {
                            struct stat s;
                            if (lstat(trozos[j], &s) == -1)
                                error = true;

                            // owner and group
                            struct group *grp;
                            struct passwd *pwd;
                            grp = getgrgid(s.st_gid);
                            pwd = getpwuid(s.st_uid);

                            // mode
                            static char mode[12];
                            strcpy(mode, ConvierteModo2(s.st_mode));
                            //time
                            char tempo[50];
                            strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));
                            char path[50];
                            realpath(trozos[j], path);

                            printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s -> %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name, grp->gr_name, mode, s.st_size, trozos[j], path);
                        }

                    } else{
                        char cwd[PATH_MAX];
                        if (getcwd(cwd, sizeof(cwd)) != NULL)
                            printf("%s\n", cwd);
                        else
                            error = true;
                    }

                } else if (strcmp(trozos[2], "-hid") == 0) {   // -HID

                    if (i!=3){
                        for (int j = 3; j < i; j++) {
                            struct stat s;
                            if (lstat(trozos[j], &s) == -1)
                                error = true;

                            if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                                int cont = 0;
                                if (listFilesHid(trozos[j], cont, 2) != 0)
                                    error = true;

                            } else {     // Si é un ficheiro

                                // owner and group
                                struct group *grp;
                                struct passwd *pwd;
                                grp = getgrgid(s.st_gid);
                                pwd = getpwuid(s.st_uid);

                                // mode
                                static char mode[12];
                                strcpy(mode, ConvierteModo2(s.st_mode));
                                //time
                                char tempo[50];
                                strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                                printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino, pwd->pw_name,
                                       grp->gr_name, mode, s.st_size, trozos[j]);

                            }
                        }
                    } else{
                        char cwd[PATH_MAX];
                        if (getcwd(cwd, sizeof(cwd)) != NULL)
                            printf("%s\n", cwd);
                        else
                            error = true;
                    }

                }
                else if (strcmp(trozos[2], "-reca") == 0) {     // -RECA

                    if (i != 3) {
                        for (int j = 3; j < i; j++) {
                            struct stat s;
                            if (lstat(trozos[j], &s) == -1)
                                error = true;
                            if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                                printf("************%s\n", trozos[j]);
                                listdirRecursivoLong(trozos[j]);
                            } else {     // Si é un ficheiro

                                // owner and group
                                struct group *grp;
                                struct passwd *pwd;
                                grp = getgrgid(s.st_gid);
                                pwd = getpwuid(s.st_uid);

                                // mode
                                static char mode[12];
                                strcpy(mode, ConvierteModo2(s.st_mode));
                                //time
                                char tempo[50];
                                strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                                printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino,
                                       pwd->pw_name,
                                       grp->gr_name, mode, s.st_size, trozos[j]);
                            }
                        }
                    } else{
                        char cwd[PATH_MAX];
                        if (getcwd(cwd, sizeof(cwd)) != NULL)
                            printf("%s\n", cwd);
                        else
                            error = true;
                    }
                }
                else if (strcmp(trozos[1], "-long") == 0) {   // -LONG
                    for (int j = 2; j < i; j++) {
                        struct stat s;
                        if (lstat(trozos[j], &s) == -1)
                            error = true;

                        if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                            int cont = 0;
                            if (listFilesLong(trozos[j], cont) != 0)
                                error = true;

                        } else {     // Si é un ficheiro

                            // owner and group
                            struct group *grp;
                            struct passwd *pwd;
                            grp = getgrgid(s.st_gid);
                            pwd = getpwuid(s.st_uid);

                            // mode
                            static char mode[12];
                            strcpy(mode, ConvierteModo2(s.st_mode));
                            //time
                            char tempo[50];
                            strftime(tempo, 50, "%Y/%m/%d-%H:%M", localtime(&s.st_mtime));

                            printf("%s\t%d (%llu)\t %s\t%s\t %s\t%lld %s\n", tempo, s.st_nlink, s.st_ino,
                                   pwd->pw_name,
                                   grp->gr_name, mode, s.st_size, trozos[j]);
                        }
                    }
                }
            }



        } else if (strcmp(trozos[1], "-hid") == 0) {
            if (i!=2) {
                for (int j = 2; j < i; j++) {
                    struct stat s;
                    if (lstat(trozos[j], &s) == -1)
                        error = true;
                    if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                        int cont = 0;
                        if (listFilesHid(trozos[j], cont, 2) != 0)
                            error = true;
                    } else                                 // Si é un ficheiro
                        printf("%lld\t%s\n", s.st_size, trozos[j]);
                }
            } else{
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    printf("%s\n", cwd);
                else
                    error = true;
            }

        } else if (strcmp(trozos[1], "-acc") == 0) {
            if (i!=2) {
                for (int j = 2; j < i; j++) {
                    struct stat s;
                    if (lstat(trozos[j], &s) == -1)
                        error = true;
                    if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                        int cont = 0;
                        if (listFilesAcc(trozos[j], cont, 2) != 0)
                            error = true;
                    } else                                 // Si é un ficheiro
                        printf("%lld\t%s\n", s.st_size, trozos[j]);
                }
            }
            else{
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    printf("%s\n", cwd);
                else
                    error = true;
            }
        }

        else if (strcmp(trozos[1], "-reca") == 0) {
            if (i!=2) {
                for (int j = 2; j < i; j++) {
                    struct stat s;
                    if (lstat(trozos[j], &s) == -1)
                        error = true;
                    if (LetraTF(s.st_mode) == 'd') {     // Si é un directorio
                        printf("************%s\n", trozos[j]);
                        listdirRecursivo(trozos[j]);
                    } else {                              // Si é un ficheiro
                        for (j = 1; j < i; j++) {
                            if (lstat(trozos[j], &s) == -1)
                                error = true;
                            else
                                printf("%lld\t%s\n", s.st_size, trozos[j]);
                        }
                    }
                }
            } else {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                    printf("%s\n", cwd);
                else
                    error = true;
            }
        }

        else {
            for (int j = 1; j < i; j++) {
                struct stat s;
                if (lstat(trozos[j], &s) == -1)
                    error = true;
                else
                    printf("%lld\t%s\n", s.st_size, trozos[j]);
            }
        }



        //-------------------------------------------------------------------
        // MALLOC:
        //-------------------------------------------------------------------

    } else if(strcmp(trozos[0], "malloc") == 0) {
        tDataM dataM;
        if (i == 1) {          // malloc
            printf("*** Lista bloques asignados malloc:\n");
            imprimirListaM(LM);

        } else if (i == 2) {
            if (strcmp(trozos[1], "-free") == 0) {         // malloc -free
                printf("*** Lista bloques asignados malloc:\n");
                imprimirListaM(LM);

            } else {         // malloc SIZE
                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);

                dataM.tam = atoi(trozos[1]);
                if (dataM.tam == 0) { // Se o SIZE é cero
                    printf("No se asignan bloques de 0 bytes\n");
                    return;
                } else if (dataM.tam < 0) // Se o SIZE é negativo
                    error = true;

                dataM.memdir = malloc(dataM.tam);
                strcpy(dataM.data, asctime(&tempo));
                strcpy(dataM.tipo, "malloc");
                insertarNodoM(LM, &dataM);
                printf("Asignados %d bytes en %p\n", dataM.tam, dataM.memdir);
            }
        } else { // malloc -free SIZE
            int tam = atoi(trozos[2]);
            if (tam == 0) { // Se o SIZE é cero
                printf("No se asignan bloques de 0 bytes\n");
                return;
            }
            if(borrarNodoM1(LM, tam) == -1)
                printf("No hay bloque de ese tamano asignado con malloc\n");
        }

        //-------------------------------------------------------------------
        // MMAP:
        //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "mmap") == 0) {
        tDataM dataM;

        if (i == 1) {                                    // mmap
            printf("*** Lista bloques asignados mmap:\n");
            imprimirListaMmap(LMmap);

        } else if (i == 2) {
            if (strcmp(trozos[1], "-free") == 0) {      // mmap -free
                printf("*** Lista bloques asignados mmap:\n");
                imprimirListaMmap(LMmap);

            } else {                                    // mmap FICH
                struct stat s;
                char *arg[2];
                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);

                if (lstat(trozos[1], &s) == -1)
                    error = true;

                strcpy(dataM.data, asctime(&tempo));
                strcpy(dataM.nome, trozos[1]);
                strcpy(dataM.tipo, "mmap");
                dataM.tam = s.st_size;
                dataM.fd = open(trozos[1], 0);

                arg[0] = trozos[1];
                arg[1] = NULL;

                dataM.memdir = Mmap(arg);

                if (error == false)
                    insertarNodoM(LMmap, &dataM);
            }

        } else if (i == 3) {
            if (strcmp(trozos[1], "-free") == 0) {       // mmap -free FICH
                tDataM *dataM1;
                dataM1 = malloc(sizeof(tDataM));

                struct stat s;
                if (lstat(trozos[2], &s) == -1) {
                    error = true;
                    return;
                }

                dataM1 = obterM(trozos[2], LMmap, dataM1);

                if (dataM1->memdir == NULL) {
                    printf("Fichero %s no mapeado\n", trozos[2]);
                    return;
                }

                munmap(dataM1->memdir, dataM1->tam);

                borrarNodoM2(LMmap, dataM1->nome);

                free (dataM1);

            } else {                                     // mmap FICH PERM
                struct stat s;
                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);

                if (lstat(trozos[1], &s) == -1) {
                    error = true;
                    return;
                }

                int protection = 0;

                // identificamos que permiso se meteu por teclado

                if (strchr(trozos[2], 'r') != NULL) protection |= PROT_READ;
                if (strchr(trozos[2], 'w') != NULL) protection |= PROT_WRITE;
                if (strchr(trozos[2], 'x') != NULL) protection |= PROT_EXEC;

                strcpy(dataM.data, asctime(&tempo));
                strcpy(dataM.nome, trozos[1]);
                strcpy(dataM.tipo, "mmap");
                dataM.tam = s.st_size;
                dataM.fd = open(trozos[1], O_RDONLY);
                char *arg[2];
                arg[0] = trozos[1];
                arg[1] = trozos[2];

                dataM.memdir = Mmap(arg);

                if (error == false)
                    insertarNodoM(LMmap, &dataM);
            }
        } else
            printf("Error: Demasiados parámetros\n");


        //-------------------------------------------------------------------
        // SHARED:
        //-------------------------------------------------------------------
    } else if (strcmp(trozos[0], "shared") == 0){
        tDataM dataM;


        if (i == 1) {                                    // shared
            printf("*** Lista bloques asignados shared:\n");
            imprimirListaShared(LMshared);

        } else if (i == 2){
            if (strcmp(trozos[1], "-create") == 0) {        // shared -create
                printf("*** Lista bloques asignados shared:\n");
                imprimirListaShared(LMshared);
            }
            else if (strcmp(trozos[1], "-free") == 0) {        // shared -free
                printf("*** Lista bloques asignados shared:\n");
                imprimirListaShared(LMshared);
            }
            else if (strcmp(trozos[1], "-delkey") == 0) {        // shared -delkey
                printf("*** Lista bloques asignados shared:\n");
                imprimirListaShared(LMshared);
            }
            else{       // shared KEY

                tDataM *dataM1;
                dataM1 = malloc(sizeof(tDataM));

                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);
                strcpy(dataM.data, asctime(&tempo));
                strcpy(dataM.tipo, "shared");

                dataM1 = obterM(trozos[1],LMshared, dataM1);

                if (dataM1->tam == 0)
                    error = true;

                dataM.tam = dataM1->tam;
                dataM.memdir = ObtenerMemoriaShmget(atoi(trozos[1]),0);

                if (dataM.memdir==NULL)
                    error =true;

                if (error == false)
                    insertarNodoM(LMshared, &dataM);

                free(dataM1);

            }

        } else if (i == 3){
            if (strcmp(trozos[1], "-create") == 0){
                printf("*** Lista bloques asignados shared:\n");
                imprimirListaShared(LMshared);
            }
            else if (strcmp(trozos[1], "-free") == 0){      // shared -free KEY

                tDataM *dataM1;
                dataM1 = malloc(sizeof(tDataM));

                dataM1 = obterM(trozos[2], LMshared, dataM1);

                if (dataM1->memdir == NULL) {
                    printf("No hay bloque de esa clave mapeado en el proceso\n");
                    return;
                }

                shmdt(dataM1->memdir);
                borrarNodoM2(LMshared, dataM1->nome);
                free (dataM1);

            }
            else if (strcmp(trozos[1], "-delkey") == 0){

                SharedDelkey(trozos[2]);

            } else
                printf("Error: Demasiados parámetros\n");

        } else if (i == 4){
            if (strcmp(trozos[1], "-create") == 0) {        // shared -create KEY TAM

                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);
                strcpy(dataM.data, asctime(&tempo));
                strcpy(dataM.nome, trozos[2]);
                strcpy(dataM.tipo, "shared");
                dataM.tam = atoi(trozos[3]);

                char *arg[2];
                arg[0] = trozos[2];
                arg[1] = trozos[3];

                dataM.memdir = SharedCreate(arg);

                if (error == false)
                    insertarNodoM(LMshared, &dataM);


            } else
                printf("Error: Demasiados parámetros\n");

        } else
            printf("Error: Demasiados parámetros\n");

        //-------------------------------------------------------------------
        // DEALLOC:
        //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "dealloc") == 0){
        if (i == 1){                                     // dealloc
            printf("*** Lista bloques asignados:\n");
            imprimirListaM(LM);
            imprimirListaMmap(LMmap);
            imprimirListaShared(LMshared);
        } else if (i == 3){
            if (strcmp(trozos[1], "-malloc") == 0){     // dealloc -malloc SIZE    [malloc -free FICH]

                int tam = atoi(trozos[2]);
                if (tam == 0) { // Se o SIZE é cero
                    printf("No se asignan bloques de 0 bytes\n");
                    return;
                }
                if(borrarNodoM1(LM, tam) == -1)
                    printf("No hay bloque de ese tamano asignado con malloc\n");

            } else if (strcmp(trozos[1], "-mmap") == 0){    // dealloc -mmap FILE   [mmap -free FICH]

                tDataM *dataM1;
                dataM1 = malloc(sizeof(tDataM));

                struct stat s;
                if (lstat(trozos[2], &s) == -1) {
                    error = true;
                    return;
                }

                dataM1 = obterM(trozos[2], LMmap, dataM1);

                if (dataM1->memdir == NULL) {
                    printf("Fichero %s no mapeado\n", trozos[2]);
                    return;
                }

                munmap(dataM1->memdir, dataM1->tam);
                borrarNodoM2(LMmap, dataM1->nome);
                free (dataM1);

            } else if (strcmp(trozos[1], "-shared") == 0){  // dealloc -shared KEY      [shared -free KEY]

                tDataM *dataM1;
                dataM1 = malloc(sizeof(tDataM));

                dataM1 = obterM(trozos[2], LMshared, dataM1);

                if (dataM1->memdir == NULL) {
                    printf("No hay bloque de esa clave mapeado en el proceso\n");
                    return;
                }

                shmdt(dataM1->memdir);
                borrarNodoM2(LMshared, dataM1->nome);
                free (dataM1);

            }
        } else if (i == 2){     // dealloc DIR

            tDataM *dataM1;
            dataM1 = malloc(sizeof(tDataM));

            dataM1 = obterM2((void*)strtoul(trozos[1],NULL,16), LM, dataM1); // primeiro miramos se está na lista malloc
            if (dataM1->memdir == NULL) {
                dataM1 = obterM2((void*)strtoul(trozos[1],NULL,16), LMmap, dataM1); // se non, miramos se está na lista mmap
                if (dataM1->memdir == NULL){
                    dataM1 = obterM2((void*)strtoul(trozos[1],NULL,16), LMshared, dataM1); // por último, miramos se está na lista shared

                    if (dataM1->memdir == NULL){ // se esa dirección non está en ningunha lista printeamos todas as listas
                        printf("*** Lista bloques asignados:\n");
                        imprimirListaM(LM);
                        imprimirListaMmap(LMmap);
                        imprimirListaShared(LMshared);
                    } else{     // se está na lista shared
                        shmdt(dataM1->memdir);
                        borrarNodoM2(LMshared, dataM1->nome);
                    }


                } else{     // se está na lista mmap
                    munmap(dataM1->memdir, dataM1->tam);
                    borrarNodoM2(LMmap, dataM1->nome);
                }


            } else{     // se está na lista malloc
                int tam = dataM1->tam;
                if (tam == 0) { // Se o SIZE é cero
                    printf("No se asignan bloques de 0 bytes\n");
                    return;
                }
                if(borrarNodoM1(LM, tam) == -1)
                    printf("No hay bloque de ese tamano asignado con malloc\n");
            }

            free(dataM1);

        } else{
            printf("*** Lista bloques asignados:\n");
            imprimirListaM(LM);
            imprimirListaMmap(LMmap);
            imprimirListaShared(LMshared);
        }

        //-------------------------------------------------------------------
        // MEMORIA:
        //-------------------------------------------------------------------
    } else if (strcmp(trozos[0], "memoria") == 0){

        // Variables locais
        int d = 6;
        char e = 'b';
        double f = 1.;

        // Variables estáticas
        static int g = 6;
        static char h = 'b';
        static double j = 1.;

        if (i == 1){        //                      // memoria [memoria -all]
            printf("*** Lista bloques asignados:\n");
            imprimirListaM(LM);
            imprimirListaMmap(LMmap);
            imprimirListaShared(LMshared);
            puts("");

            printf("Variables locales:      %p   %p   %p\n", &d, &e, &f);
            printf("Variables globales:     %p      %p      %p\n", &a, &b, &c);
            printf("Variables estáticas:    %p      %p      %p\n", &g, &h, &j);
            puts("");

            printf("Funciones programa:     %p      %p      %p\n", imprimirPrompt, terminado, trocearCadena);
            printf("Funciones librería:     %p   %p   %p\n",open,close,read);

        } else if (i == 2){
            if (strcmp(trozos[1], "-blocks") == 0){         // memoria -blocks [imprime como dealloc]
                printf("*** Lista bloques asignados:\n");
                imprimirListaM(LM);
                imprimirListaMmap(LMmap);
                imprimirListaShared(LMshared);

            } else if (strcmp(trozos[1], "-vars") == 0){                            // memoria -vars
                printf("Variables locales:      %p   %p   %p\n", &d, &e, &f);
                printf("Variables globales:     %p      %p      %p\n", &a, &b, &c);
                printf("Variables estáticas:    %p      %p      %p\n", &g, &h, &j);

            } else if (strcmp(trozos[1], "-funcs") == 0){       // memoria -funcs
                printf("Funciones programa:     %p      %p      %p\n", imprimirPrompt, terminado, trocearCadena);
                printf("Funciones librería:     %p   %p   %p\n",open,close,read);


            } else if (strcmp(trozos[1], "-all") == 0){       // memoria -all
                printf("*** Lista bloques asignados:\n");
                imprimirListaM(LM);
                imprimirListaMmap(LMmap);
                imprimirListaShared(LMshared);
                puts("");

                printf("Variables locales:      %p   %p   %p\n", &d, &e, &f);
                printf("Variables globales:     %p      %p      %p\n", &a, &b, &c);
                printf("Variables estáticas:    %p      %p      %p\n", &g, &h, &j);
                puts("");

                printf("Funciones programa:     %p      %p      %p\n", imprimirPrompt, terminado, trocearCadena);
                printf("Funciones librería:     %p   %p   %p\n",open,close,read);

            } else if (strcmp(trozos[1], "-pmap") == 0){       // memoria -pmap

                dopmap();

            } else
                printf("Opción %s no contemplada\n",trozos[1]);
        } else
            printf("Opción %s no contemplada\n",trozos[1]);



        //-------------------------------------------------------------------
        // VOLCARMEM:
        //-------------------------------------------------------------------
    } else if (strcmp(trozos[0], "volcarmem") == 0){
        int tam;
        char *direccion;
        char *caracter;

        if (i == 1){        // volcarmem
            printf("Volcarmem debe ir acompañada de una dirección\n");
        }
        else if (i == 2){   // volcarmem DIR

            direccion = (char*) strtoul(trozos[1],NULL,16);

            tam = 25;

            printf("Volcando 25 bytes desde la direccion %s\n", trozos[1]);

            for(int j = 0; j < tam; j++) {
                if (isprint(direccion[j]))
                    printf("%3c", direccion[j]);
                else
                    printf("  ");
            }
            puts("");

            for( int j = 0; j < tam; j++) {
                if (isprint(direccion[j]))
                   printf("%3X", direccion[j]);
                else
                    printf(" 0");
            }
            puts("");
        }

        else if (i == 3){       // volcarmem DIR TAM

            direccion = (char*) strtoul(trozos[1],NULL,16);

            tam = atoi(trozos[2]);
            printf("Volcando %d bytes desde la direccion %s\n", tam, trozos[1]);

            // Se TAM < 25
            if(tam < 25){
                for(int j = 0; j < tam; j++){
                    if(isprint(direccion[j]))
                        printf("%3c",(direccion[j]));
                    else
                        printf("  ");
                }

                puts("");

                for( int j = 0; j < tam; j++){
                    if(isprint(direccion[j]))
                        printf("%3X",(direccion[j]));
                    else
                        printf(" 0");
                }
                puts("");
            }

                // Se TAM >= 25
            else{
                int resto = 25;
                int cnt = 0;
                for (int z = 0; z < tam ; z = z + 25) {
                    for(int j = 0; j < resto; j++){
                        if(isprint(direccion[j+z])){
                            printf("%3c",(direccion[j+z]));
                            printf("hola1");
                            }
                       else
                            printf("  ");
                    }

                   puts("");

                    for( int j = 0; j < resto; j++){
                        if(isprint(direccion[j+z])){
                            printf("%3X",(direccion[j+z]));
                            printf("hola");
                        }
                        else
                            printf("  ");
                    }
                    puts("");
                    ++cnt;
                    if (cnt >= tam / 25)
                        resto = tam % 25;
                }
            }
        }


    //-------------------------------------------------------------------
    // LLENARMEM:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "llenarmem") == 0){

        int tam;
        char *direccion;

        if (i == 1){        // llenarmem
            printf("Llenarmem debe ir acompañada de una dirección\n");
        }
        else if (i == 2){       // llenarmem DIR
            direccion = (char*) strtoul(trozos[1],NULL,16);

            tam = 128;
            for (int j = 0; j < tam; j++) {
                direccion[j] = 'A';
            }
        }
        else if (i == 3){       // llenarmem DIR TAM
            direccion = (char*) strtoul(trozos[1],NULL,16);

            tam = atoi(trozos[2]);
            for (int j = 0; j < tam; j++){
                direccion[j] = 'A';
            }
        }
        else if (i == 4){       // llenarmem DIR TAM BYTE
            direccion = (char*) strtoul(trozos[1],NULL,16);

            tam = atoi(trozos[2]);
            for (int j = 0; j < tam; j++){
                direccion[j] = (char) strtoul(trozos[3],NULL,16);
            }
        }

    //-------------------------------------------------------------------
    // READ-WRITE:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "e-s") == 0){
        int df, conts, size, oflag;
        char *file;
        void *direccion;
        ssize_t lf;
        if (i == 1){
            printf("Uso: e-s [read|write] ......\n");
        }
        else if(i == 2 || i == 3){
            printf("Faltan parametros1\n");
        }
        else if(i == 4){
            direccion = (void*)strtoul(trozos[3],NULL,16);
            file = trozos[2];
            conts = -1;
            //read with no conts
            if(strcmp(trozos[1], "read") == 0){
                lf = LeerFichero(trozos[2], direccion, conts);
                if (lf ==-1) {
                    printf("No se ha podido abrir el archivo\n");
                    return;
                }
                else{
                    printf("Leidos %ld bytes de %s en %s\n", lf, trozos[2], trozos[3]);
                }
            }
            else if(strcmp(trozos[1], "write") == 0){
                printf("Faltan parametros2\n");
            }
            else{
                printf("Uso: e-s [read|write] ......\n");
            }
        }
        else if(i == 5){
            direccion = (void*)strtoul(trozos[3],NULL,16);
            file = trozos[2];
            conts = atoi(trozos[4]);

            //read with conts
            if(strcmp(trozos[1], "read") == 0){
                lf = LeerFichero(trozos[2], direccion, conts);
                if (lf ==-1) {
                    printf("No se ha podido abrir el archivo\n");
                    return;
                }
                else{
                    printf("Leidos %ld bytes de %s en %s\n", lf, trozos[2], trozos[3]);
                }
            }


                //write without -o
            else if(strcmp(trozos[1], "write") == 0){
                if(strcmp(trozos[2], "-o") == 0){
                    printf("Faltan parametros3\n");
                }
                else{
                    oflag = O_WRONLY ;
                    df=open(file, oflag);
                    if (df == -1){
                        printf("No se ha podido abrir el archivo\n");
                        return;
                    }
                    else{
                        write(df, direccion, conts);
                        close(df);
                        printf("Escritos %d bytes en %s desde %s\n", conts, trozos[2], trozos[3]);
                    }
                }
            }
        }
        else if(i == 6){
            direccion = (void*)strtoul(trozos[4],NULL,16);
            file = trozos[3];
            conts = atoi(trozos[5]);
            //write with -o
            if(strcmp(trozos[1], "write") == 0){
                oflag = O_WRONLY;
                df=open(file, oflag);
                if (df==-1){
                    printf("No se ha podido abrir el archivo\n");
                    return;
                }
                else{
                    write(df, direccion, conts);
                    close(df);
                    printf("Escritos %d bytes en %s desde %s\n", conts, trozos[3], trozos[4]);
                }
            }
        }
        else{
            printf("Sobran parametros\n");
        }

    //-------------------------------------------------------------------
    // RECURSIVA:
    //-------------------------------------------------------------------
    } else if (strcmp(trozos[0], "recursiva") == 0){
        if (i == 2) {
            int n = atoi(trozos[1]);
            doRecursiva(n);
        }


    // *****************************   P3   *****************************


    //-------------------------------------------------------------------
    // PRIORITY:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "priority") == 0){
        int pri;
        int pid;
        int val;
        if (i == 1){        // priority
            pid = getpid();
            pri = getpriority(PRIO_PROCESS,pid);
            if (pri == -1){
                error = true;
                return;
            }
            printf("La prioridad del proceso %d es %d\n", pid, pri);

        }if (i == 2){       // priority PID
            pid = atoi(trozos[1]);
            pri = getpriority(PRIO_PROCESS,pid);
            if (pri == -1) {
                error = true;
                return;
            }
            printf("La prioridad del proceso %d es %d\n", pid, pri);

        }if (i == 3){       // priority PID VALUE
            pid = atoi(trozos[1]);
            val = atoi(trozos[2]);
            if (setpriority(PRIO_PROCESS,pid,val) == -1) {
                error = true;
                return;
            }
            pri = getpriority(PRIO_PROCESS,pid);
            if (pri == -1) {
                error = true;
                return;
            }
            printf("La prioridad del proceso %d es %d\n", pid, pri);
        }else
            return;

    //-------------------------------------------------------------------
    // REDERR:                                                              ( X )
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "rederr") == 0){
        int fd, filedir[2];
        int copia = dup(STDERR_FILENO);

        if (i == 1){
            if (pipe(filedir) == -1)
                printf("Error estandar en fichero configuración original\n");
            else{
                printf("Error estandar en fichero actual\n");
            }
        } else if (i == 2){
            if (strcmp(trozos[1], "-reset") == 0){
                dup2(copia, STDERR_FILENO);
                close(copia);
            }
            else{
                if((fd = open(trozos[1],O_RDWR)) == -1){
                    perror("Error: ");
                }
                if(dup2(fd, STDERR_FILENO) == -1){
                    perror("Error: ");
                }
                if(close(fd) == -1){
                    perror("Error: ");
                }
            }

        } else {
            printf("Sobran parametros\n");
        }

    //-------------------------------------------------------------------
    // ENTORNO:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "entorno") == 0){
        if (i == 1){        // entorno
            char nome[10] = "main arg3";
            MostrarEntorno(env,nome);

        } else if (i == 2){
            extern char ** environ;
            if (strcmp(trozos[1], "-environ") == 0){        // entorno -environ
                char nome[10] = "environ";
                MostrarEntorno(environ,nome);

            } else if (strcmp(trozos[1], "-addr") == 0){    // entorno -addr
                printf("environ:    %p (almacenado en %p)\n", environ, &environ);
                printf("main arg3:  %p (almacenado en %p)\n", env, &env);
            } else
                printf("Uso: entorno [-environ|-addr]\n");
        }else
            return;

    //-------------------------------------------------------------------
    // MOSTRARVAR:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "mostrarvar") == 0){
        if (i == 1){            // mostrarvar
            char nome[10] = "main arg3";
            MostrarEntorno(env,nome);

        } else if (i == 2){     // mostrarvar VAR
            extern char ** environ;
            int pos = BuscarVariable(trozos[1],env);
            if (pos == -1)
                printf("Error: No hay tal variable\n");
            else{
                printf("Con main arg3 ->  %s  (%p)  @%p\n", env[pos], env[pos],&env[pos]);
                printf("Con environ   ->  %s  (%p)  @%p\n", environ[pos], environ[pos],environ[pos]);
                printf("Con getenv    ->  %s  (%p)\n", getenv(trozos[1]), getenv(trozos[1]));
            }
        } else
            return;

    //-------------------------------------------------------------------
    // CAMBIARVAR:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "cambiarvar") == 0){

        if (i == 4){
            int pos;
            if (strcmp(trozos[1],"-a") == 0){
                pos = CambiarVariable(trozos[2],trozos[3],env);
                if (pos == -1)
                    error = true;
                return;

            } else if (strcmp(trozos[1],"-e") == 0){
                extern char ** environ;
                pos = CambiarVariable(trozos[2],trozos[3],environ);
                if (pos == -1)
                    error = true;
                return;

            } else if (strcmp(trozos[1],"-p") == 0) {
                char string[50] = "";
                strcat(string,trozos[2]);
                strcat(string,"=");
                strcat(string,trozos[3]);
                if (putenv(string) != 0)
                    error = true;

            } else
                printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
        }else
            printf("Uso: cambiarvar [-a|-e|-p] var valor\n");

    //-------------------------------------------------------------------
    // UID:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "uid") == 0){
        if (i == 1 || (i == 2 && (((strcmp(trozos[1],"-get")) == 0) || (strcmp(trozos[1],"-set")) == 0)))  // uid   /   uid -get
            MostrarUidsProceso();

        else if (i == 3 && (strcmp(trozos[1],"-set")) == 0){       // uid -set ID
            uid_t uid;
            uid = atoi(trozos[2]);
            if (setuid(uid) == (uid_t) -1)
                error = true;

        } else if (i == 4 && (strcmp(trozos[2],"-l")) == 0){        // uid -set -l ID

            CambiarUidLogin(trozos[3]);

        } else
            return;

    //-------------------------------------------------------------------
    // FORK:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "fork") == 0){
        int pid;
        if ((pid = fork()) == 0)
            printf("Ejectuando proceso %d\n",getpid());

        waitpid(pid,NULL,0);

    //-------------------------------------------------------------------
    // EJEC:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "ejec") == 0){

        if (i==1)
            printf("Faltan parámetros\n");
        else{
            char* argument[i];
            for (int j = 0; j < i-1; ++j) {
                argument[j] = trozos[j+1];
            }
            argument[i-1] = NULL;

            if (execvp(trozos[1],argument) == -1)
                error = true;
        }

    //-------------------------------------------------------------------
    // EJECPRI:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "ejecpri") == 0){
        if ( i==1 || i == 2)
            printf("Faltan parámetros\n");

        else{
            int val = atoi(trozos[2]);
            if (setpriority(PRIO_PROCESS,getpid(),val) == -1) {
                error = true;
                return;
            }

            char* argument[i-1];
            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
            }
            argument[i-2] = NULL;

            if (execvp(trozos[2],argument) == -1)
                error = true;
        }

    //-------------------------------------------------------------------
    // FG:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "fg") == 0){

        if (i==1)
            printf("Faltan parámetros\n");
        else{
            char* argument[i];
            for (int j = 0; j < i-1; ++j) {
                argument[j] = trozos[j+1];
            }
            argument[i-1] = NULL;

            int pid;
            if ((pid = fork()) == 0){
                if (execvp(trozos[1],argument) == -1)
                    perror("execvp");
                exit(255);
            }
            waitpid(pid,NULL,0);

        }

    //-------------------------------------------------------------------
    // FGPRI:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "fgpri") == 0){
        if ( i==1 || i == 2)
            printf("Faltan parámetros\n");

        else{
            int val = atoi(trozos[2]);
            if (setpriority(PRIO_PROCESS,getpid(),val) == -1) {
                error = true;
                return;
            }

            char* argument[i-1];
            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
            }
            argument[i-2] = NULL;

            int pid;
            if ((pid = fork()) == 0){
                if (execvp(trozos[2],argument) == -1)
                    perror("execvp");
                exit(255);
            }
            waitpid(pid,NULL,0);
        }

    //-------------------------------------------------------------------
    // BACK:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "back") == 0){

        if (i==1)
            printf("Faltan parámetros\n");
        else{
            int status;
            char* argument[i];
            tDataJ dataJ;
            strcpy(dataJ.commandline,"");

            for (int j = 0; j < i-1; j++) {
                argument[j] = trozos[j+1];
                strcat(dataJ.commandline, argument[j]);
                strcat(dataJ.commandline, " ");
            }
            argument[i-1] = NULL;

            pid_t pid;
            if ((pid = fork()) == 0){
                if (execvp(trozos[1],argument) == -1)
                    perror("execvp");
                exit(255);
            } else
                dataJ.pid = pid;

            time_t T = time(NULL);
            struct tm tempo = *localtime(&T);

            if ((dataJ.prio = getpriority(PRIO_PROCESS,dataJ.pid)) == -1)      // pri
                strcpy(dataJ.state, "TERMINADO");                               // state
            else
                strcpy(dataJ.state, "ACTIVO");

            strcpy(dataJ.user,NombreUsuario(dataJ.prio));                       // user
            strcpy(dataJ.data, asctime(&tempo));                                // data


            if (waitpid (pid, &status, WNOHANG | WUNTRACED | WCONTINUED)  == pid){
                if (WIFEXITED(status)) {
                    dataJ.signal = WEXITSTATUS(status);             // exited

                }

                else if (WIFSIGNALED(status)) {
                    dataJ.signal = WTERMSIG(status);                // by signal
                }

                else if (WIFCONTINUED(status)) {                    // continued
                    dataJ.signal = 000;
                }
            }

            insertarNodoJ(ListJobs,&dataJ);

        }

    //-------------------------------------------------------------------
    // BACKPRI:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "backpri") == 0){

        if ( i == 1 || i == 2)
            printf("Faltan parámetros\n");
        else{
            int val = atoi(trozos[2]);
            if (setpriority(PRIO_PROCESS,getpid(),val) == -1) {
                error = true;
                return;
            }

            char* argument[i-1];
            tDataJ dataJ;
            strcpy(dataJ.commandline,"");

            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
                strcat(dataJ.commandline, argument[j]);
                strcat(dataJ.commandline, " ");
            }
            argument[i-2] = NULL;

            pid_t pid;
            int status;
            if ((pid = fork()) == 0){
                if (execvp(trozos[2],argument) == -1)
                    perror("execvp");
                exit(255);
            } else
                dataJ.pid = pid;

            time_t T = time(NULL);
            struct tm tempo = *localtime(&T);


            if ((dataJ.prio = getpriority(PRIO_PROCESS,dataJ.pid)) == -1)      // pri
                strcpy(dataJ.state, "TERMINADO");                               // state
            else
                strcpy(dataJ.state, "ACTIVO");

            strcpy(dataJ.user,NombreUsuario(dataJ.prio));                       // user
            strcpy(dataJ.data, asctime(&tempo));                                // data

            if (waitpid (pid, &status, WNOHANG | WUNTRACED | WCONTINUED)  == pid){
                if (WIFEXITED(status)) {
                    dataJ.signal = WEXITSTATUS(status);             // exited

                }

                else if (WIFSIGNALED(status)) {
                    dataJ.signal = WTERMSIG(status);                // by signal
                }

                else if (WIFCONTINUED(status)) {                    // continued
                    dataJ.signal = 000;
                }
            }

            insertarNodoJ(ListJobs,&dataJ);
        }

    //-------------------------------------------------------------------
    // EJECAS:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "ejecas") == 0){

        if (i==1 || i==2)
            printf("Faltan parámetros\n");
        else{

            if (CambiarUidLogin(trozos[1]) == -1)
                return;

            char* argument[i-1];

            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
            }
            argument[i-2] = NULL;

            if (execvp(trozos[2],argument) == -1)
                perror("execvp");
        }

    //-------------------------------------------------------------------
    // FGAS:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "fgas") == 0){

        if (i==1 || i==2)
            printf("Faltan parámetros\n");
        else{

            if (CambiarUidLogin(trozos[1]) == -1)
                return;

            char* argument[i-1];
            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
            }
            argument[i-2] = NULL;

            int pid;
            if ((pid = fork()) == 0){
                if (execvp(trozos[2],argument) == -1)
                    perror("execvp");
                exit(255);
            }
            waitpid(pid,NULL,0);
        }

    //-------------------------------------------------------------------
    // BACKAS:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "backas") == 0){

        if (i==1 || i==2)
            printf("Faltan parámetros\n");
        else{

            if (CambiarUidLogin(trozos[1]) == -1)
                return;

            char* argument[i-1];
            tDataJ dataJ;
            strcpy(dataJ.commandline,"");

            for (int j = 0; j < i-2; ++j) {
                argument[j] = trozos[j+2];
                strcat(dataJ.commandline, argument[j]);
                strcat(dataJ.commandline, " ");
            }
            argument[i-2] = NULL;

            pid_t pid;
            int status;
            if ((pid = fork()) == 0){
                if (execvp(trozos[2],argument) == -1)
                    perror("execvp");
                exit(255);
            } else
                dataJ.pid = pid;

            time_t T = time(NULL);
            struct tm tempo = *localtime(&T);


            if ((dataJ.prio = getpriority(PRIO_PROCESS,dataJ.pid)) == -1)      // pri
                strcpy(dataJ.state, "TERMINADO");                               // state
            else
                strcpy(dataJ.state, "ACTIVO");

            strcpy(dataJ.user,NombreUsuario(dataJ.prio));                       // user
            strcpy(dataJ.data, asctime(&tempo));                                // data

            if (waitpid (pid, &status, WNOHANG | WUNTRACED | WCONTINUED)  == pid){
                if (WIFEXITED(status)) {
                    dataJ.signal = WEXITSTATUS(status);             // exited

                }

                else if (WIFSIGNALED(status)) {
                    dataJ.signal = WTERMSIG(status);                // by signal
                }

                else if (WIFCONTINUED(status)) {                    // continued
                    dataJ.signal = 000;
                }
            }

            insertarNodoJ(ListJobs,&dataJ);
        }

    //-------------------------------------------------------------------
    // LISTJOBS:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "listjobs") == 0){
        if (i != 1)
            printf("Demasiados atributos.\n");
        else{
            imprimirListaJ(ListJobs);
        }

    //-------------------------------------------------------------------
    // JOB:
    //-------------------------------------------------------------------

    } else if (strcmp(trozos[0], "job") == 0){
        if (i == 2){
            tDataJ dataJ;
            pid_t pid = atoi(trozos[1]);
            if (pid == 0){
                imprimirListaJ(ListJobs);
                return;
            }
            dataJ = *obterJ(pid,ListJobs);

            if ((dataJ.prio = getpriority(PRIO_PROCESS,dataJ.pid)) == -1)
                strcpy(dataJ.state, "TERMINADO");
            else
                strcpy(dataJ.state, "ACTIVO");

            printf("%d || %s || p=%d || %s || (%d) || %s || %s", (int) pid, dataJ.user, dataJ.prio, dataJ.state, dataJ.signal, dataJ.commandline, dataJ.data);

        } else if (i == 3 && strcmp(trozos[1],"-fg") == 0){

            char estado[15];
            int status;
            tDataJ dataJ;
            pid_t pid = atoi(trozos[2]);
            dataJ = *obterJ(pid,ListJobs);
            if (dataJ.prio == -1){
                printf("Proceso %d pid ya esta finalizado\n", (int) pid);
                return;
            } else {
                waitpid(pid, NULL, 0);
                if (borrarJ(pid,ListJobs)==0) {

                    if (waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED) == pid) {
                        if (WIFEXITED(status)) {
                            dataJ.signal = WEXITSTATUS(status);             // exited
                            strcpy(estado,"normalmente");

                        } else if (WIFSIGNALED(status)) {
                            dataJ.signal = WTERMSIG(status);                // by signal
                            strcpy(estado,"por señal");
                        } else if (WIFCONTINUED(status)) {                    // continued
                            printf("Error: El proceso no ha podido terminar");
                            return;
                        }
                    }
                    printf("Proceso %d terminado normalmente. Valor devuelto %d\n", pid, dataJ.signal);           // (( X ))

                }else
                    error = true;
            }

        } else
            imprimirListaJ(ListJobs);

    //-------------------------------------------------------------------
    // BORRARJOBS:
    //------------------------------------------------------------------

    } else if (strcmp(trozos[0], "borrarjobs") == 0){

        if (i == 2) {

            if (strcmp(trozos[1], "-term") == 0) {              // (( X ))

            } else if (strcmp(trozos[1], "-sig") == 0) {        // (( X ))

            } else if (strcmp(trozos[1], "-all") == 0) {        // (( Non borra ben ))
                borrarAll(ListJobs);

            } else if (strcmp(trozos[1], "-clear") == 0) {      // feito
                vaciarListaJ(ListJobs);
            } else
                imprimirListaJ(ListJobs);
        } else
            imprimirListaJ(ListJobs);



   //-------------------------------------------------------------------

    } else if (terminado() == false) {
        int pid;

            if (strcmp(trozos[i - 1], "&") == 0) {
                char* argument[i];
                tDataJ dataJ;
                int valor;
                strcpy(dataJ.commandline,"");

                for (int j = 0; j < i-1; j++) {
                    argument[j] = trozos[j];
                    strcat(dataJ.commandline, argument[j]);
                    strcat(dataJ.commandline, " ");
                }
                argument[i-1] = NULL;

                if ((pid = fork()) == 0) {
                    if (execvp(trozos[0], argument) == -1)
                        printf("%s: no es un comando del shell\n", trozos[0]);
                    exit(255);
                }  else
                    dataJ.pid = pid;

                time_t T = time(NULL);
                struct tm tempo = *localtime(&T);


                if ((dataJ.prio = getpriority(PRIO_PROCESS,dataJ.pid)) == -1)      // pri
                    strcpy(dataJ.state, "TERMINADO");                               // state
                else
                    strcpy(dataJ.state, "ACTIVO");

                strcpy(dataJ.user,NombreUsuario(dataJ.prio));                       // user
                strcpy(dataJ.data, asctime(&tempo));                                // data

                insertarNodoJ(ListJobs,&dataJ);

            } else {
                if ((pid = fork()) == 0) {
                    if (execvp(trozos[0], trozos) == -1)
                        printf("%s: no es un comando del shell\n", trozos[0]);
                    exit(255);
                }
                waitpid(pid, NULL, 0);
            }
    }
}



//---------------------------------------------------------------------

int main(int argc, char *argv[], char * env[]) {

    char entrada[NUM];
    Lista *L;
    ListaM *LM;     // malloc
    ListaM *LMmap;      // map
    ListaM *LMshared;       // shared
    ListaJ *ListJobs;     // listjobs
    L = malloc(sizeof(Lista));
    LM = malloc(sizeof(ListaM));
    LMmap = malloc(sizeof(ListaM));
    LMshared = malloc(sizeof(ListaM));
    ListJobs = malloc(sizeof(ListaJ));

    do {
        if (error==true) {
            error=false;
            printf("Error: %s\n",strerror(errno));
        }
        imprimirPrompt();
        leerEntrada(&entrada);
        procesarEntrada(&entrada, L, LM, LMmap, LMshared, ListJobs, argc, argv, env);
    } while (terminado() == false);

    free(L);
    free(LM);
    free(LMmap);
    free(LMshared);
    free(ListJobs);

    return 0;
}
