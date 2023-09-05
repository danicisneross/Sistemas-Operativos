/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include "memory.h"
#include "listmem.h"
#include <sys/shm.h>
#include <unistd.h>


int mem_alloc_malloc(char **tokens, int ntokens, mem_list ml);

int mem_alloc_mmap(char **tokens, int ntokens, mem_list ml);

int mem_alloc_createshared(char **tokens, int ntokens, mem_list ml);

int mem_alloc_shared(char **tokens, int ntokens, mem_list ml);

int mem_dealloc_addr(char *address, mem_list ml);

int mem_dealloc_malloc(char **tokens, int ntokens, mem_list ml);

int mem_dealloc_mmap(char **tokens, int ntokens, mem_list ml);

int mem_dealloc_shared(char **tokens, int ntokens, mem_list ml);

int mem_deletekey(char *key);

int mem_show(char **tokens, int ntokens, mem_list ml);

int mem_show_vars();

int mem_show_funcs();

int mem_dopmap();

int memory_cmd(char **tokens, int ntokens, mem_list ml) {
    if (ntokens == 0) {
        print_memlist(ml, NULL);
    } else {
        if (strcmp(tokens[0], "-allocate") == 0) {
            if (ntokens == 1) {
                print_memlist(ml, NULL);
            } else {
                if (strcmp(tokens[1], "-malloc") == 0) {
                    mem_alloc_malloc(tokens + 2, ntokens - 2, ml);
                } else if (strcmp(tokens[1], "-mmap") == 0) {
                    mem_alloc_mmap(tokens + 2, ntokens - 2, ml);
                } else if (strcmp(tokens[1], "-createshared") == 0) {
                    mem_alloc_createshared(tokens + 2, ntokens - 2, ml);
                } else if (strcmp(tokens[1], "-shared") == 0) {
                    mem_alloc_shared(tokens + 2, ntokens - 2, ml);
                } else {
                    printf("Error, check the arguments.\n");
                }
            }
        } else if (strcmp(tokens[0], "-dealloc") == 0) {
            if (ntokens == 1) {
                print_memlist(ml, NULL);
            } else {
                if (strcmp(tokens[1], "-malloc") == 0) {
                    mem_dealloc_malloc(tokens + 2, ntokens - 2, ml);
                } else if (strcmp(tokens[1], "-mmap") == 0) {
                    mem_dealloc_mmap(tokens + 2, ntokens - 2, ml);
                } else if (strcmp(tokens[1], "-shared") == 0) {
                    mem_dealloc_shared(tokens + 2, ntokens - 2, ml);
                } else {
                    mem_dealloc_addr(tokens[1], ml);
                }
            }
        } else if (strcmp(tokens[0], "-deletekey") == 0) {
            if (ntokens != 2) {
                printf("Error, check the arguments.\n");
            } else mem_deletekey(tokens[1]);
        } else if (strcmp(tokens[0], "-show") == 0) {
            mem_show(tokens + 1, ntokens - 1, ml);
        } else if (strcmp(tokens[0], "-show-vars") == 0) {
            mem_show_vars();
        } else if (strcmp(tokens[0], "-show-funcs") == 0) {
            mem_show_funcs();
        } else if (strcmp(tokens[0], "-dopmap") == 0) {
            if (ntokens > 1) {
                printf("Error, check the arguments.\n");
            } else mem_dopmap();
        } else {
            printf("Error, check the arguments\n");
        }
    }
    return 1;
}

int memdump_cmd(char **tokens, int ntokens) {
    long tam = 25;
    if (ntokens == 0) {
        printf("Should receive a valid address.\n");
        return -1;
    }
    if (ntokens > 2) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    if (ntokens == 2) tam = strtol(tokens[1], NULL, 10);
    if (tam <= 0) tam = 25;
    char *ptr = (char *) strtol(tokens[0], NULL, 16);
    int pos = 0, mod_flag = 0;
    for (int i = 0; i < tam * 2; i++) {
        if (i != 0 && i % 25 == 0 && !mod_flag) {
            printf("\n");
            pos = pos - 25;
        }
        if (tam % 25 != 0 && pos == tam) {
            printf("\n");
            pos = pos - tam;
            mod_flag = 1;
        }
        if (i != 0 && i % 50 == 0) pos = pos + 25;
        if (i / (float) 50 - i / 50 < 0.5 && !mod_flag) {
            if (isprint(ptr[pos])) {
                printf("%3c", ptr[pos]);
            } else printf("%3c", ' ');
        } else {
            printf(" %02hhX", ptr[pos]);
        }
        pos++;
    }
    printf("\n");
    return 1;
}

int memfill_cmd(char **tokens, int ntokens) {
    int tam = 128;
    char c = 'A';
    if (ntokens == 0 || ntokens > 3) {
        printf("Error, check the arguments.");
        return -1;
    }
    char *ptr = (char *) strtol(tokens[0], NULL, 16);
    if (ntokens > 1) tam = strtol(tokens[1], NULL, 10);
    if (tam <= 0) tam = 128;
    if (ntokens == 3) c = strtol(tokens[2], NULL, 16);
    for (int i = 0; i < tam; i++) {
        ptr[i] = c;
    }
    return 1;
}

void recursive(int n) {
    char auto_array[4096];
    static char stat_array[4096];
    printf("Parameter %2d:\t %p\n", n, &n);
    printf("Auto array:   \t %p\n", auto_array);
    printf("Static array: \t %p\n", stat_array);
    printf("\n");
    if (n > 0) recursive(--n);
}

int recurse_cmd(char **tokens, int ntokens) {
    if (ntokens != 1) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    int n = strtol(tokens[0], NULL, 10);
    if (n <= 0) {
        printf("Number not valid. \n");
        return -1;
    }
    recursive(n);
    return 1;
}

#define LEERCOMPLETO ((ssize_t)-1)

ssize_t LeerFichero(char *fich, void *p, ssize_t n) {
    ssize_t nleidos, tam = n;
    int df, aux;
    struct stat s;
    if (stat(fich, &s) == -1 || (df = open(fich, O_RDONLY)) == -1)
        return ((ssize_t) -1);
    if (n == LEERCOMPLETO)
        tam = (ssize_t) s.st_size;
    if ((nleidos = read(df, p, tam)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return ((ssize_t) -1);
    }
    close(df);
    return (nleidos);
}

int readfile_cmd(char **tokens, int ntokens) {
    if (ntokens > 3 || ntokens < 2) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    ssize_t size = (ssize_t) -1;
    if (ntokens == 3) size = (ssize_t) strtol(tokens[2], NULL, 10);
    if (LeerFichero(tokens[0],
                    (void *) strtol(tokens[1], NULL, 16),
                    size) == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    }
    printf("%s successfully read into memory.\n", tokens[0]);
    return 1;
}

int toFileFromMem(char *file, void *p, ssize_t n, int o_flag) {
    int fd, aux;
    struct stat s;
    if (o_flag && stat(file, &s) == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    } else if (stat(file, &s) != -1 && !o_flag) {
        printf("File exists, use overwrite mode.\n");
        return -2;
    }
    if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
        return -1;
    if (write(fd, p, n) == -1) {
        aux = errno;
        close(fd);
        errno = aux;
        return -1;
    }
    close(fd);
    printf("Wrote file.\n");
    return 1;
}

int writefile_cmd(char **tokens, int ntokens) {
    if (ntokens < 3 || ntokens > 4) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    int i = 0, o_flag = 0;
    if (strcmp(tokens[0], "-o") == 0) {
        o_flag = 1;
        i++;
    }
    if (toFileFromMem(tokens[i],
                      (void *) strtol(tokens[i + 1], NULL, 16),
                      (ssize_t) strtol(tokens[i + 2], NULL, 10), o_flag) == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    }
    return 1;
}

int mem_alloc_malloc(char **tokens, int ntokens, mem_list ml) {
    if (ntokens > 1) {
        printf("Take a look at the arguments, expecting one.\n");
        return -1;
    }
    if (ntokens == 0) {
        print_memlist(ml, "malloc");
        return 1;
    }
    unsigned long size = strtol(tokens[0], NULL, 10);
    if (size == 0) {
        printf("That doesn't seem like a valid size\n");
        return -1;
    }
    void *ptr = malloc(size);
    printf("malloc in %p\n", ptr);
    insert_in_memlist(ml, ptr, size, "malloc", "");
    return 1;
}

void *mmapfich(char *fichero, int protection, mem_list ml) {
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    if (protection & PROT_WRITE) modo = O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED) {
        //printf("%s\n",strerror(errno));
        return NULL;
    }
    char *params = malloc(20);
    sprintf(params, "fd:%d - %s", df, fichero);
    insert_in_memlist(ml, p, s.st_size, "mmap", params);
    free(params);
    return p;
}

int mem_alloc_mmap(char **tokens, int ntokens, mem_list ml) {
    if (ntokens > 2) {
        printf("Take a look at the arguments, expecting one or two.\n");
        return -1;
    }
    if (ntokens == 0) {
        print_memlist(ml, "mmap");
        return 1;
    }
    char *perm;
    void *ptr;
    int prot = 0;
    if ((perm = tokens[1]) != NULL && strlen(perm) < 4) {
        if (strchr(perm, 'r') != NULL) prot |= PROT_READ;
        if (strchr(perm, 'w') != NULL) prot |= PROT_WRITE;
        if (strchr(perm, 'x') != NULL) prot |= PROT_EXEC;
    }
    if ((ptr = mmapfich(tokens[0], prot, ml)) == NULL)
        printf("Cannot map file.\n");
    else
        printf("mapped file in %p\n", ptr);
    return 1;
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam, mem_list ml) {
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;
    if (tam) /*si tam no es 0 la crea en modo exclusivo */
        flags = flags | IPC_CREAT | IPC_EXCL;
    /*if tam = 0 try to access */
    if (clave == IPC_PRIVATE) /*no nos vale*/
    {
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void *) -1) {
        aux = errno; /*si se ha creado y no se puede mapear*/
        if (tam) /*se borra */
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);
    char temp[40];
    int inttemp = clave;
    snprintf(temp, 40, "cl:%d", inttemp);
    if (tam == 0) {
        tam = get_size_of_key(ml, temp);
    }
    insert_in_memlist(ml, p, tam, "shared", temp);
    return (p);
}

int mem_alloc_createshared(char **tokens, int ntokens, mem_list ml) {
    key_t k;
    size_t tam = 0;
    void *p;
    if (ntokens == 0) {
        print_memlist(ml, "shared");
        return 0;
    }
    if (ntokens == 1 || ntokens > 2) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    k = (key_t) atoi(tokens[0]);
    if (tokens[1] != NULL)
        tam = (size_t) atoll(tokens[1]);
    if ((p = ObtenerMemoriaShmget(k, tam, ml)) == NULL)
        printf("Error, cannot allocate (maybe repeated key).\n");
    else
        printf("Allocated shared memory (cl:%d) asigned at %p\n", k, p);
    return 1;
}

int mem_alloc_shared(char **tokens, int ntokens, mem_list ml) {
    key_t k;
    void *p;
    if (ntokens == 0) {
        print_memlist(ml, "shared");
        return 0;
    } else {
        k = (key_t) atoi(tokens[0]);
        if ((p = ObtenerMemoriaShmget(k, 0, ml)) == NULL) {
            printf("Error, cannot allocate (maybe repeated key).\n");
            return -1;
        } else {
            printf("Allocated shared memory (cl:%d) asigned at %p\n", k, p);
            return 1;
        }
    }
}

int mem_dealloc_malloc(char **tokens, int ntokens, mem_list ml) {
    int pos;
    if (ntokens == 0) {
        print_memlist(ml, "malloc");
        return 0;
    } else if (ntokens == 1) {
        pos = pos_in_mem_list(ml, "malloc", tokens[0]);
        if (pos > -1 && pos < n_elements_in_memlist(ml)) {
            printf("deallocated: %s\n", tokens[0]);
            remove_from_memlist(ml, pos, 1);
            return 1;
        }
    }
    return 0;
}

int mem_dealloc_mmap(char **tokens, int ntokens, mem_list ml) {
    int pos;
    if (ntokens == 0) {
        print_memlist(ml, "mmap");
    } else if (ntokens == 1) {
        pos = pos_in_mem_list(ml, "mmap", tokens[0]);
        if (pos > -1 && pos < n_elements_in_memlist(ml)) {
            unmap_from_memlist(ml, pos);
            printf("deallocated: %s\n", tokens[0]);
            return 1;
        } else {
            printf("Error, file not found.\n");
            return -1;
        }
    }
    return 1;
}

int mem_dealloc_shared(char **tokens, int ntokens, mem_list ml) {
    if (ntokens > 1) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    if (ntokens == 0) {
        print_memlist(ml, "shared");
        return 1;
    }
    int pos = pos_in_mem_list(ml, "shared", tokens[0]);
    if (pos > -1 && pos < n_elements_in_memlist(ml)) {
        detachShared(ml, pos);
        printf("deallocated: %s\n", tokens[0]);
        return 1;
    } else {
        printf("Error, key not found.\n");
        return -1;
    }
}

int mem_dealloc_addr(char *address, mem_list ml) {
    int pos = findAddr(ml, (void *) strtol(address, NULL, 16));
    if (pos > -1 && pos < n_elements_in_memlist(ml)) {
        deallocAddr(ml, pos);
        printf("deallocated: %s\n", address);
        return 1;
    } else {
        printf("Error, address not found.\n");
        return -1;
    }
}

int mem_deletekey(char *key) {
    key_t clave;
    int id;
    if (key == NULL || (clave = (key_t) strtoul(key, NULL, 10)) == IPC_PRIVATE) {
        printf("Error, key not valid.\n");
        return 1;
    }
    if ((id = shmget(clave, 0, 0666)) == -1) {
        printf("shmget: cannot get key\n");
        return -1;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1) {
        printf("shmctl: cannot remove key\n");
        return -1;
    } else {
        printf("Key removed successfully.\n");
        return 1;
    }
}

int mem_show(char **tokens, int ntokens, mem_list ml) {
    int a, b, c;
    if (ntokens > 1) {
        printf("Error, check the arguments.\n");
        return -1;
    }
    if (ntokens == 0) {
        printf("Functions: \n");
        printf("\tmem_show():         %p \n", &mem_show);
        printf("\tmem_dealloc_mmap(): %p \n", &mem_dealloc_mmap);
        printf("\tLeerFichero():      %p \n", &LeerFichero);
        printf("Global vars: \n");
        printf("\terrno:   %p \n", &errno);
        printf("\tstdin:   %p \n", &stdin);
        printf("\tstdout:  %p \n", &stdin);
        printf("Local vars: \n");
        printf("\ta:   %p \n", &a);
        printf("\tb:   %p \n", &b);
        printf("\tc:   %p \n", &c);
        printf("Parameters (not needed): \n");
        printf("\ttokens:  %p \n", &tokens);
        printf("\tntokens: %p \n", &ntokens);
        printf("\tml:      %p \n", &ml);
        return 1;
    }
    if (ntokens == 1) {
        if (strcmp(tokens[0], "-malloc") == 0)
            print_memlist(ml, "malloc");
        if (strcmp(tokens[0], "-shared") == 0)
            print_memlist(ml, "shared");
        if (strcmp(tokens[0], "-mmap") == 0)
            print_memlist(ml, "mmap");
        if (strcmp(tokens[0], "-all") == 0)
            print_memlist(ml, NULL);
        return 1;
    }
    return -1;
}


int mem_show_vars() {
    int a, b, c;
    printf("Global vars: \n");
    printf("\terrno:   %p \n", &errno);
    printf("\tstdin:   %p \n", &stdin);
    printf("\tstdout:  %p \n", &stdin);
    printf("Local vars: \n");
    printf("\ta: %p \n", &a);
    printf("\tb: %p \n", &b);
    printf("\tc: %p \n", &c);
    return 0;
}

int mem_show_funcs() {
    printf("thatshell functions: \n");
    printf("\tmem_show():         %p \n", &mem_show);
    printf("\tmem_dealloc_mmap(): %p \n", &mem_dealloc_mmap);
    printf("\tLeerFichero():      %p \n", &LeerFichero);
    printf("libc functions: \n");
    printf("\tprintf(): %p \n", &printf);
    printf("\tgetpid(): %p \n", &getpid);
    printf("\tatoi():   %p \n", &atoi);
    return 0;
}

int mem_dopmap() {
    FILE *fp;
    char output[4096];
    char pid[10];
    sprintf(pid, "%d", getpid());
    char cmd[100] = "/usr/bin/pmap ";
    strcat(cmd, pid);
    fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("An error ocurred.\n");
        return -1;
    }
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }
    pclose(fp);
    return 1;
}
