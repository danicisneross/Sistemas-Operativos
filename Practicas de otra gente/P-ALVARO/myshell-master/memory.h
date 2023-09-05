/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#ifndef P2_SO_MEMORY_H
#define P2_SO_MEMORY_H

#include "file.h"

void cmdDealloc(char *trozos[], int n, tListM *MemoryList);

void cmdE_s_read(char *trozos[], int n);

void cmdE_s_write(char *trozos[], int n);

void cmdLlenarmem(char *trozos[], int n);

void cmdMalloc(char *trozos[], int n, tListM *L);

void cmdMemoria(char *trozos[], int n, tListM MemoryList);

void cmdMmap(char *trozos[], int n, tListM *MemoryList);

void cmdRecursiva(char *trozos[], int n);

void cmdShared(char *trozos[], int n, tListM *MemoryList);

void cmdVolcarmem(char *trozos[], int n);

#endif //P2_SO_MEMORY_H
