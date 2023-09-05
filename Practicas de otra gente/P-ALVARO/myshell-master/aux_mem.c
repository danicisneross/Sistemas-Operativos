/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "prints.h"
#include "aux_mem.h"

#define LEERCOMPLETO ((ssize_t)-1)
#define TAM 4096


/**
 * Function: checkoptions_mem
 * ----------------------
 * Checks the options for memory and changes
 * its value to the corresponding one.
 *
 * @param trozos array of strings
 * @param opBlocks, opLink, opVars, opFuncs, opAll, opPmap booleans of the options
 * @param n number of arguments in trozos
 * @return void.
 */
void checkoptions_mem(char *trozos[], bool *opBlocks, bool *opVars, bool *opFuncs,
                      bool *opAll, bool *opPmap, int n) {

    for (int i = 1; i < n; i++) {
        if (trozos[i] == NULL) {
            break;
        } else if (strcmp(trozos[i], "-blocks") == 0) {
            *opBlocks = true;
        } else if (strcmp(trozos[i], "-vars") == 0) {
            *opVars = true;
        } else if (strcmp(trozos[i], "-funcs") == 0) {
            *opFuncs = true;
        } else if (strcmp(trozos[i], "-all") == 0) {
            *opAll = true;
        } else if (strcmp(trozos[i], "-pmap") == 0) {
            *opPmap = true;
        }
    }
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam) { /* Obtienen un puntero a una zona de memoria compartida */
    /* si tam > 0 intenta crearla y si tam == 0 asume que existe */
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    /* si tam no es 0 la crea en modo exclusivo esta
     * funcion vale para shared y shared -create */
    if (tam) {
        flags = flags | IPC_CREAT | IPC_EXCL;
    }

    /* si tam es 0 intenta acceder a una ya creada */
    if (clave == IPC_PRIVATE) { /* no nos vale */
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1) {
        return (NULL);
    }
    if ((p = shmat(id, NULL, 0)) == (void *) -1) {
        aux = errno; /* si se ha creado y no se puede mapear */
        if (tam) {
            /* se borra */
            shmctl(id, IPC_RMID, NULL);
        }
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);

    /* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....); */
    return (p);
}

void *SharedCreate(char *arg[], tListM L) { /* arg[0] is the key and arg[1] is the size */
    key_t k;
    size_t tam = 0;
    void *p;
    if (arg[0] == NULL || arg[1] == NULL) {
        print_shared(L);
        return 0;
    }
    k = (key_t) atoi(arg[0]);
    if (arg[1] != NULL) {
        tam = (size_t) atoll(arg[1]);
    }
    if ((p = ObtenerMemoriaShmget(k, tam)) == NULL) {
        return NULL;
    } else {
        printf("Shared memory of key %d at %p\n", k, p);
    }
    return p;
}

void *MmapFichero(char *fichero, int protection, int *fd) {
    int map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat filestat;
    void *p;
    if (protection & PROT_WRITE) modo = O_RDWR;

    if (stat(fichero, &filestat) == -1 || (*fd = open(fichero, modo)) == -1) {
        return NULL;
    }
    if ((p = mmap(NULL, filestat.st_size, protection, map, *fd, 0)) == MAP_FAILED) {
        return NULL;
    }

    /* Guardar Direccion de Mmap (p, filestat.st_size,fichero,fd......) */
    return p;
}

void *Mmap(char *arg[], int *fd) { /* arg[0] is the file name and arg[1] is the permissions */
    char *perm;
    void *p;
    int protection = 0;

    if (arg[0] == NULL) {
        return false; /* Listar Direcciones de Memoria mmap; */
    }
    if ((perm = arg[1]) != NULL && strlen(perm) < 4) {
        if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
        if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
        if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
    }

    p = MmapFichero(arg[0], protection, fd);

    return p;
}

ssize_t LeerFichero(char *fich, void *p, ssize_t n) { /* lee n bytes del fichero fich en p */
    ssize_t nleidos, tam = n; /* si n==-1 lee el fichero completo */
    int df, aux;
    struct stat s;

    if (stat(fich, &s) == -1 || (df = open(fich, O_RDONLY)) == -1) {
        return ((ssize_t) -1);
    }
    if (n == LEERCOMPLETO) {
        tam = (ssize_t) s.st_size;
    }
    if ((nleidos = read(df, p, tam)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return ((ssize_t) -1);
    }
    close(df);

    return (nleidos);
}

int SharedDelkey(char *args[]) { /* arg[0] points to a str containing the key */
    key_t clave;
    int id;
    char *key = args[0];

    if (key == NULL || (clave = (key_t) strtoul(key, NULL, 10)) == IPC_PRIVATE) {
        printf("shared -delkey valid_key\n");
        return -1;
    }
    if ((id = shmget(clave, 0, 0666)) == -1) {
        perror("Could not get shared memory");
        return -1;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1) {
        perror("Could not delete shared memory\n");
        return -1;
    }
    return 0;
}

void dopmap(void) { /* no arguments necessary */
    pid_t pid;      /* ejecuta el comando externo pmap para */
    char elpid[32]; /* pasandole el pid del proceso actual */
    char *argv[3] = {"pmap", elpid, NULL};
    sprintf(elpid, "%d", (int) getpid());

    if ((pid = fork()) == -1) {
        perror("Could not create proccess");
        return;
    }
    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            perror("cannot execute pmap");
        }
        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void doRecursiva(int n) {
    char automatico[TAM];
    static char estatico[TAM];

    printf("parameter: %d (%p) ", n, &n);
    printf("array %p, ", estatico);
    printf("static array %p\n", automatico);

    if (n > 1) doRecursiva(--n);
}

void llenar_mem(char *p, int nbytes, char c) {
    int i;

    printf("Filling %d bytes of memory with the char '%c' from the address %p\n", nbytes, c, p);

    for (i = 0; i < nbytes; i++) {
        p[i] = c;
    }
}

void printable_char(char c) {
    bool printable = (c > 32 && c < 126);


    if (printable) {
        printf("%2c ", c);
    } else {
        printf("%2c ", ' ');
    }
}

void printable_hex(char c) {
    bool printable = (c > 32 && c < 126);


    if (printable) {
        printf("%2x ", c);
    } else {
        printf("%2c ", '0');
    }
}

bool dealloc_mem(tPosM pos, tListM *L, bool exit) {
    tItemM item = getItemM(pos, *L);

    switch (item.cmdType) {
        case MALLOC:
            if (!exit) {
                printf("deallocated %zu at %p\n", item.size, item.dir);
            }
            free(item.dir);
            deleteItemM(pos, L);
            break;

        case MMAP:
            if (munmap(item.dir, item.size) == -1) {
                print_error();
                return false;
            } else {
                close(item.Union.fich.fd);
                if (!exit) {
                    printf("unmapped file %s\n", item.Union.fich.filename);
                    free(item.Union.fich.filename);
                }
                deleteItemM(pos, L);
            }
            break;

        case SHARED:
            /* checks if the adress was already deatached */
            if ((shmget(item.Union.key, 0, 0)) != -1) {
                /* tries to deatach the adress: */
                if (shmdt(item.dir) == -1) {
                    print_error();
                    return false;
                }
            }
            if (!exit) {
                printf("Shared memory block at %p (key %d) has been dealocated\n", item.dir, item.Union.key);
            }
            deleteItemM(pos, L);

            break;
    }
    return true;
}

void liberarMemoria(tListM *L) {
    tPosM pos, next;

    for (pos = firstM(*L); pos != LNULL; pos = next) {
        next = nextM(pos, *L);
        dealloc_mem(pos, L, true);
    }
}

void borrarFilenameM(tListM *L) {
    tItemM item;

    for (tPosM pos = firstM(*L); pos != NULL; pos = nextM(pos, *L)) {
        item = getItemM(pos, *L);
        if (item.cmdType == MMAP) {
            free(item.Union.fich.filename);
        }
    }
}