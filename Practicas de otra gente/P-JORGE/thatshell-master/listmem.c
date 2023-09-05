/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include "listmem.h"
#include <unistd.h>
#include <sys/shm.h>


struct struct_command {
    void *address;
    unsigned long size;
    char *date_and_time;
    char *type;
    char *param;
};


struct memory_struct {
    command *list[4096];
    int n_elem;
};


mem_list create_memlist() {
    mem_list historic = (mem_list) malloc(sizeof(struct memory_struct));
    historic->n_elem = 0;
    for (int i = 0; i < 4096; i++) {
        historic->list[i] = NULL;
    }
    return historic;
};

void remove_memlist(mem_list historic) {
    int size = historic->n_elem;
    for (int i = 0; i < size; i++) {
        if (historic->list[0] != NULL) {
            deallocAddr(historic, findAddr(historic, historic->list[0]->address));
        }
    }
    free(historic->list);
};

char *dateAndTime() {
    time_t t;
    struct tm *tm;
    char *dateAndTime;
    dateAndTime = (char *) malloc(sizeof(char) * 25);
    t = time(NULL);
    tm = localtime(&t);
    strftime(dateAndTime, 24, "%d/%m/%Y at %H:%M:%S", tm);
    return dateAndTime;
};

int pos_in_mem_list(mem_list historic, char *type, char *param) {
    for (int i = 0; i < n_elements_in_memlist(historic); i++) {
        if (strcmp(historic->list[i]->type, type) == 0) {
            if (strcmp(type, "malloc") == 0) {
                unsigned long int tmpInt = strtoul(param, NULL, 10);
                if (tmpInt == historic->list[i]->size) {
                    return i;
                }
            }
            if (strcmp(type, "mmap") == 0) {
                if (strcmp(param, historic->list[i]->param + 7) == 0) {
                    return i;
                }
            }
            if (strcmp(type, "shared") == 0) {
                if (strtoul(param, NULL, 10) == strtoul(historic->list[i]->param + 3, NULL, 10)) {
                    return i;
                }
            }
        }
    }
    return -1;
};

void insert_in_memlist(mem_list historic, void *memoryAddress, unsigned long size, char *type, char *params) {
    char *now = dateAndTime();
    if (historic->n_elem < 4096) {
        historic->list[historic->n_elem] = (command *) malloc(sizeof(command));
        historic->list[historic->n_elem]->address = memoryAddress;
        historic->list[historic->n_elem]->size = size;
        historic->list[historic->n_elem]->date_and_time = now;
        historic->list[historic->n_elem]->type = type;
        historic->list[historic->n_elem]->param = strdup(params);
        historic->n_elem++;
    }
};

void remove_from_memlist(mem_list historic, int position, int malloc_flag) {
    int last = n_elements_in_memlist(historic) - 1;
    if (historic->n_elem == 0) {
        printf("the list is empty, you cant remove");
    } else {
        if (position == last) {
            free(historic->list[position]->param);
            if (malloc_flag)
                free(historic->list[position]->address);
            free(historic->list[position]->date_and_time);
            free(historic->list[position]);
            historic->n_elem--;
        } else {
            free(historic->list[position]->param);
            if (malloc_flag)
                free(historic->list[position]->address);
            free(historic->list[position]->date_and_time);
            free(historic->list[position]);
            for (int i = position; i <= last; ++i) {
                historic->list[i] = historic->list[i + 1];
            }
            historic->n_elem--;
        }
    }
};

void unmap_from_memlist(mem_list historic, int position) {
    if (historic->n_elem == 0) {
        printf("the list is empty, you cant remove\n");
    } else {
        if (munmap(historic->list[position]->address, historic->list[position]->size) == 0)
            close(atoi(historic->list[position]->param + 3));
        remove_from_memlist(historic, position, 0);
    }

}

command *read_from_memlist(mem_list historic, int position) {
    return historic->list[position];
};

int n_elements_in_memlist(mem_list historic) {
    return historic->n_elem;
};

void print_memlist(mem_list ml, char *type) {
    for (int i = 0; i < n_elements_in_memlist(ml); ++i) {
        command *cmd = read_from_memlist(ml, i);
        if (type == NULL || strcmp(type, cmd->type) == 0) {
            printf("%p: size=%lu. %s (%s) %s\n", cmd->address, cmd->size, cmd->type, cmd->param, cmd->date_and_time);
        }
    }
}

void print_sharedmem_key_memlist(mem_list ml, char *key) {
    char type[] = "shared";
    for (int i = 0; i < n_elements_in_memlist(ml); ++i) {
        command *cmd = read_from_memlist(ml, i);
        if (type == NULL || strcmp(type, cmd->type) == 0) {
            if (strcmp(cmd->param, key) == 0)
                printf("%p: size=%lu. %s (key:%s) %s\n", cmd->address, cmd->size, cmd->type, cmd->param,
                       cmd->date_and_time);
        }
    }
}
int get_size_of_key(mem_list ml,char*key){
    char type[] = "shared";
    for (int i = 0; i < n_elements_in_memlist(ml); ++i) {
        if (strcmp(type, ml->list[i]->type) == 0){
            if (strcmp(ml->list[i]->param, key) == 0)
                return ml->list[i]->size;
        }
    }
    return -1;
}

void detachShared(mem_list historic, int position) {
    if (historic->n_elem == 0) {
        printf("the list is empty, you cant remove\n");
    } else {
        shmdt(historic->list[position]->address);
        remove_from_memlist(historic, position, 0);
    }
}

void deallocAddr(mem_list historic, int position) {
    if (historic->n_elem == 0) {
        printf("the list is empty, you cant remove\n");
    } else {
        if (strcmp(historic->list[position]->type, "malloc") == 0) {
            remove_from_memlist(historic, position, 1);
            return;
        } else if (strcmp(historic->list[position]->type, "mmap") == 0) {
            unmap_from_memlist(historic, position);
            return;
        } else if (strcmp(historic->list[position]->type, "shared") == 0) {
            detachShared(historic, position);
        }
    }

}


int findAddr(mem_list ml, void* addr) {
    for (int i = 0; i < n_elements_in_memlist(ml); ++i) {
        command *c = ml->list[i];
        if (addr == c->address) return i;
    }
    return -1;
}

