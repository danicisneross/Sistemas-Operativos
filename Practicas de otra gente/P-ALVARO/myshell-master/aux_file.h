/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#ifndef P1_SO_AUXILIARS_H
#define P1_SO_AUXILIARS_H

#include "mem_list.h"
#include "hist_list.h"

void borrarComandosC(tListC *L);

void checkoptions(char *trozos[], bool *opLong, bool *opLink, bool *opAcc,
                  bool *opReca, bool *opRecb, bool *opHid, int n);

char *ConvierteModo(mode_t m, char *permisos);

int copyfiles(char *param, char *trozos[]);

long int find_size(const char *path);

void remove_file(char c, char *filename);

char LetraTF(mode_t m);

int trocearCadena(char *cadena, char *trozos[]);

void fix_trozos(char *trozos[], char *file, bool opAcc, bool opLink,
                bool opLong, bool opHid, bool opReca, bool opRecb);

#endif
