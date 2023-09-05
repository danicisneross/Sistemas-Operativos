/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include "memory.h"
#include "prints.h"
#include "aux_mem.h"

/* Global variables */
int global_int = 0;
char global_char = 'B';
float global_float = 0;

void cmdDealloc(char *trozos[], int n, tListM *MemoryList) {
    char *new_trozos[3];
    void *p;
    tItemM item;
    tPosM pos;

    if (n == 1) {
        print_memoryList(*MemoryList);
    } else if (n == 2) {
        p = (void *) strtol(trozos[1], NULL, 16);
        for (pos = firstM(*MemoryList); pos != LNULL; pos = nextM(pos, *MemoryList)) {
            item = getItemM(pos, *MemoryList);
            if (item.dir == p) {
                dealloc_mem(pos, MemoryList, false);
                return;
            }
        }
        invalid_arg();
    } else if (n == 3) {
        new_trozos[1] = malloc(strlen(trozos[1]) + 1);
        new_trozos[2] = malloc(strlen(trozos[2]) + 1);
        strcpy(new_trozos[1], "-free");
        strcpy(new_trozos[2], trozos[2]);
        if (strcmp(trozos[1], "-malloc") == 0) {
            cmdMalloc(new_trozos, n, MemoryList);
        } else if (strcmp(trozos[1], "-shared") == 0) {
            cmdShared(new_trozos, n, MemoryList);
        } else if (strcmp(trozos[1], "-mmap") == 0) {
            cmdMmap(new_trozos, n, MemoryList);
        } else {
            cmd_not_found();
        }
        free(new_trozos[1]);
        free(new_trozos[2]);
    } else {
        cmd_not_found();
    }
}

void cmdE_s_read(char *trozos[], int n) {
    ssize_t cont = -1;
    char *file;
    void *addr;

    if (n < 4 || n > 5) {
        invalid_nargs();
        return;
    }

    file = trozos[2];
    addr = (void *) strtol(trozos[3], NULL, 16);

    /* number of bytes is specified: */
    if (n == 5) {
        cont = atoi(trozos[4]);
    }

    /* cont bytes are read from file onto addr: */
    if ((cont = LeerFichero(file, addr, cont)) == -1) {
        print_error();
        return;
    }

    printf("%zd bytes were read from %s into %p\n", cont, file, addr);

}

void cmdE_s_write(char *trozos[], int n) {
    int fd, nbytes;
    char *file;
    void *addr;
    struct stat filestat;

    if (n < 5 || n > 6) {
        invalid_nargs();
        return;
    }

    if (n == 6 && strcmp(trozos[2], "-o") == 0) {
        file = trozos[3];
        if ((fd = open(file, O_WRONLY)) == -1) {
            print_error();
            return;
        }
        addr = (void *) strtol(trozos[4], NULL, 16);
        nbytes = atoi(trozos[5]);
        if (write(fd, addr, nbytes) == -1) {
            print_error();
            close(fd);
        } else {
            printf("Wrote %d bytes from %p into %s\n", nbytes, addr, file);
            close(fd);
        }
    } else if (n == 5) {
        file = trozos[2];
        if (stat(file, &filestat) == 0) {
            printf("File already exists, use overwrite mode (-o)\n");
            return;
        }
        if ((fd = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
            print_error();
            return;
        }
        addr = (void *) strtol(trozos[3], NULL, 16);
        nbytes = (int) atoi(trozos[4]);
        if (write(fd, addr, nbytes) == -1) {
            print_error();
            close(fd);
            return;
        } else {
            printf("Wrote %d bytes from %p into %s\n", nbytes, addr, file);
            close(fd);
        }
    } else {
        invalid_arg();
    }
}

void cmdLlenarmem(char *trozos[], int n) {
    char *p;
    int character; /* variable for the position of the param with the char */
    char c = 'A';       /* default */
    int nbytes = 128;   /* values  */

    if (n == 1 || n > 4) {
        invalid_nargs();
        return;
    }

    p = (char *) strtol(trozos[1], NULL, 16);

    if (n == 2) {
        llenar_mem(p, nbytes, c);
        return;
    }

    /* checks if the third param is "nbytes" or the char: */
    if (n == 3) {
        if ((nbytes = atoi(trozos[2])) != 0) {
            llenar_mem(p, nbytes, c);
            return;
        }
        character = 2;
        nbytes = 128;
    }

    if (n == 4) {
        nbytes = atoi(trozos[2]);
        character = 3;
    }

    /* checks if the param with the char is
     * just a char or its given in hexadecimal: */
    if (trozos[character][1] == 'x') {
        c = (char) strtol(trozos[character], NULL, 16);
    } else {
        c = (char) trozos[character][0];
    }

    /* fills the memory adress: */
    llenar_mem(p, nbytes, c);
}

void cmdMalloc(char *trozos[], int n, tListM *MemoryList) {
    int i;
    tPosM pos;
    tItemM new_malloc;
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);

    if (n == 1) {
        print_malloc(*MemoryList);
    } else if (n == 2) {
        i = atoi(trozos[1]);

        /* checks if the parameter is a number */
        if (i == 0) {
            printf("Error: Invalid size\n");
            return;
        } else {
            new_malloc.cmdType = MALLOC;
            new_malloc.size = i;
            new_malloc.dir = calloc(1, new_malloc.size);
            strftime(new_malloc.date, 128, "%a %b %d %H:%M:%S %Y", tlocal);

            if (new_malloc.dir == NULL) {
                print_error();
                return;
            }

            if (insertItemM(new_malloc, MemoryList)) {
                printf("allocated %zu bytes at %p\n", new_malloc.size, new_malloc.dir);
            } else {
                print_error();
                free(new_malloc.dir);
            }
        }
    } else if (n == 3 && strcmp(trozos[1], "-free") == 0) {
        i = atoi(trozos[2]);

        /* checks if the parameter is a number */
        if (i == 0) {
            printf("Error: Invalid size\n");
            return;
        } else {
            /* frees memory asked for */
            pos = findPosM(i, *MemoryList);
            if (pos == LNULL) {
                printf("Error: Element not found\n");
            } else {
                dealloc_mem(pos, MemoryList, false);
            }
        }
    } else {
        cmd_not_found();
    }

}

void cmdMemoria(char *trozos[], int n, tListM MemoryList) {
    bool opBlocks = false, opVars = false, opFuncs = false, opAll = false, opPmap = false;
    int nOptions = 0;

    /* variables locales */
    int local_int = 0;
    char local_char = 'c';
    float local_float = 0;

    /* Variables estáticas */
    static int static_int = 0;
    static char static_char = 'c';
    static float static_float = 0;

    checkoptions_mem(trozos, &opBlocks, &opVars, &opFuncs, &opAll, &opPmap, n);
    nOptions += opBlocks + opVars + opFuncs + opAll + opPmap;

    /* if no arguments: */
    if (nOptions + 1 != n) {
        invalid_arg();
        return;
    }

    if (opAll || nOptions == 0) {
        opBlocks = true;
        opVars = true;
        opFuncs = true;
    }

    if (opBlocks) {
        print_memoryList(MemoryList);
    }

    if (opVars) {
        printf("===========================\n");

        /* Variables locales o automáticas */
        printf("* Automatic variables (local):\n");
        printf(" - Integer: %p\n - Character: %p\n - Float: %p\n", &local_int, &local_char, &local_float);

        /* Variables globales o externas */
        printf("* External variables (global):\n");
        printf(" - Integer: %p\n - Character: %p\n - Float: %p\n", &global_int, &global_char, &global_float);

        /* Variables estáticas */
        printf("* Static variables:\n");
        printf(" - Integer: %p\n - Character: %p\n - Float: %p\n", &static_int, &static_char, &static_float);

        printf("===========================\n");
    }

    if (opFuncs) {
        printf("===========================\n");

        /* Funciones de la librería de C */
        printf("* Library functions in C:\n");
        printf("- Library function <stdio.h>: %p\n- Library function <string.h>: %p\n"
               "- Library function <stdlib.h>: %p\n", &printf, &strlen, &malloc);

        /* Program  */
        printf("* Program functions:\n");
        printf("- Library function <file.h>: %p\n- Library function <memory.h>: %p\n"
               "- Library function <auxiliars.h>: %p\n", &cmdPid, &cmdDealloc, &doRecursiva);

        printf("===========================\n");
    }

    if (opPmap) {
        printf("===========================\n");
        dopmap();
        printf("===========================\n");
    }
}

void cmdMmap(char *trozos[], int n, tListM *MemoryList) {
    int fd;
    tItemM aux_mmap;
    tPosM pos;
    void *addr;
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    struct stat filestat;

    if (n == 1 || (n == 2 && strcmp(trozos[1], "-free") == 0)) {
        /* se imprimime una lista de la
         * memoria reservada con mmap */
        print_mmap(*MemoryList);
    } else if (n == 2 || n == 3) {
        if (strcmp(trozos[1], "-free") == 0) {

            /* frees if possible: */
            for (pos = firstM(*MemoryList); pos != LNULL; pos = nextM(pos, *MemoryList)) {
                aux_mmap = getItemM(pos, *MemoryList);
                if (strcmp(aux_mmap.Union.fich.filename, trozos[2]) == 0) {
                    dealloc_mem(pos, MemoryList, false);
                    return;
                }
            }

            /* else, prints mmap list: */
            print_mmap(*MemoryList);

        } else {
            /* maps the file: */

            if ((addr = Mmap(&trozos[1], &fd)) == NULL) {
                print_error();
                return;
            }

            stat(trozos[1], &filestat);

            printf("mapped file %s at %p\n", trozos[1], addr);

            aux_mmap.Union.fich.filename = malloc(strlen(trozos[1]) + 1);

            aux_mmap.cmdType = MMAP;
            aux_mmap.dir = addr;
            aux_mmap.size = filestat.st_size;
            strftime(aux_mmap.date, 128, "%a %b %d %H:%M:%S %Y", tlocal);
            strcpy(aux_mmap.Union.fich.filename, trozos[1]);
            aux_mmap.Union.fich.fd = fd;

            insertItemM(aux_mmap, MemoryList);
        }

    } else {
        invalid_nargs();
    }
}

void cmdRecursiva(char *trozos[], int n) {
    if (n == 2) {
        doRecursiva(atoi(trozos[1]));
    } else {
        invalid_nargs();
    }
}

void cmdShared(char *trozos[], int n, tListM *MemoryList) {
    tItemM aux_item;
    tPosM pos;
    key_t key;
    time_t tiempo = time(0);
    int shmid;
    void *addr;
    struct tm *tlocal = localtime(&tiempo);
    struct shmid_ds s;

    if (n == 1) {
        print_shared(*MemoryList);
    } else if (n == 2) {
        key = (key_t) atoi(trozos[1]);
        if ((shmid = shmget(key, 0, 0)) == -1) {
            print_error();
        } else {
            if ((addr = shmat(shmid, NULL, 0)) == (void *) -1) {
                print_error();
            } else {
                if (shmctl(shmid, IPC_STAT, &s) == -1) {
                    print_error();
                } else {
                    aux_item.dir = addr;
                    strftime(aux_item.date, 128, "%a %b %d %H:%M:%S %Y", tlocal);
                    aux_item.cmdType = SHARED;
                    aux_item.size = s.shm_segsz;
                    aux_item.Union.key = key;
                    insertItemM(aux_item, MemoryList);
                    printf("Shared memory of key %d at %p\n", key, addr);
                }
            }
        }
    } else if (n == 3) {
        key = atoi(trozos[2]);

        if (strcmp(trozos[1], "-free") == 0) {
            /* frees the first element with the key received: */
            for (pos = firstM(*MemoryList); pos != LNULL; pos = nextM(pos, *MemoryList)) {
                aux_item = getItemM(pos, *MemoryList);
                if (aux_item.cmdType == SHARED) {
                    if (aux_item.Union.key == key) {
                        dealloc_mem(pos, MemoryList, false);
                        return;
                    }
                }
            }
            printf("Key not found\n");

        } else if (strcmp(trozos[1], "-delkey") == 0) {
            /* deletes the key from the system and
             * deataches the addresses from the list: */
            if (SharedDelkey(&trozos[2]) != -1) {
                key = (key_t) atoi(trozos[2]);
                printf("Key %d removed from the system\n", key);
                for (pos = firstM(*MemoryList); pos != LNULL; pos = nextM(pos, *MemoryList)) {
                    aux_item = getItemM(pos, *MemoryList);
                    if (aux_item.cmdType == SHARED && aux_item.Union.key == key) {
                        shmdt(aux_item.dir);
                    }
                }
            }
        } else {
            cmd_not_found();
        }
    } else if (n == 4 && strcmp(trozos[1], "-create") == 0) {
        /* creates a new key: */
        if ((addr = SharedCreate(&trozos[2], *MemoryList)) == NULL) {
            print_error();
        } else {
            aux_item.dir = addr;
            strftime(aux_item.date, 128, "%a %b %d %H:%M:%S %Y", tlocal);
            aux_item.size = atoi(trozos[3]);
            aux_item.cmdType = SHARED;
            aux_item.Union.key = atoi(trozos[2]);
            insertItemM(aux_item, MemoryList);
        }
    } else {
        cmd_not_found();
    }
}

void cmdVolcarmem(char *trozos[], int n) {
    char *p;
    int bytes = 25;

    if (n == 1 || n > 3) {
        invalid_nargs();
        return;
    }

    p = (char *) strtol(trozos[1], NULL, 16);

    if (n == 3) {
        bytes = (int) strtol(trozos[2], NULL, 10);
    }

    print_mem(p, bytes);
}