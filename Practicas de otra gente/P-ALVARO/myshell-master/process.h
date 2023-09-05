/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#ifndef P3_SO_PROCESS_H
#define P3_SO_PROCESS_H

#include "bg_list.h"
#include "env_list.h"

void cmdBack(char *trozos[], int n, tListP *ProcessList, int start);

void cmdBackpri(char *trozos[], int n, tListP *ProcessList);

void cmdBgas(char *trozos[], int n, tListP *ProcessList);

void cmdCambiarvar(char *trozos[], int n, char *arg3[], char *environ[], tListE *EnvironmentList);

void cmdEjec(char *trozos[], int n);

void cmdEjecas(char *trozos[], int n);

void cmdEjecpri(char *trozos[], int n);

void cmdEntorno(char *trozos[], int n, char *arg3[], char *environ[]);

void cmdFg(char *trozos[], int n, int start);

void cmdFgas(char *trozos[], int n);

void cmdFgpri(char *trozos[], int n);

void cmdFork(int n);

void cmdLinux(char *trozos[], int n, tListP *ProcessList);

void cmdListjobs(int n, tListP *ProcessList);

void cmdJob(char *trozos[], int n, tListP *ProcessList);

void cmdBorrarjobs(char *trozos[], int n, tListP *ProcessList);

void cmdMostrarvar(char *trozos[], int n, char *arg3[], char *environ[]);

void cmdPriority(char *trozos[], int n);

void cmdRederr(char *trozos[], int n, char **std_error);

void cmdUid(char *trozos[], int n);

#endif //P3_SO_PROCESS_H
