/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#ifndef P3_SO_AUX_MEM_H
#define P3_SO_AUX_MEM_H

#include <stdio.h>
#include <stdbool.h>

#include "mem_list.h"
#include "hist_list.h"

void borrarFilenameM(tListM *L);

bool dealloc_mem(tPosM pos, tListM *L, bool exit);

void checkoptions_mem(char *trozos[], bool *opBlocks, bool *opVars, bool *opFuncs,
                      bool *opAll, bool *opPmap, int n);

void liberarMemoria(tListM *L);

void *Mmap(char *arg[], int *fd);

void printable_char(char c);

void printable_hex(char c);

void *SharedCreate(char *arg[], tListM L);

int SharedDelkey(char *args[]);

ssize_t LeerFichero(char *fich, void *p, ssize_t n);

void dopmap(void);

void llenar_mem(char *p, int nbytes, char c);

void doRecursiva(int n);


#endif //P3_SO_AUX_MEM_H
