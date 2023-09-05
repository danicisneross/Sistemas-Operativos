/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#ifndef P1_SO_PRINTS_H
#define P1_SO_PRINTS_H

#include "mem_list.h"
#include "bg_list.h"

void print_error();

void print_pid(int pid, int p_pid, char *param);

void print_time(const time_t *timer);

int print_current_path();

int print_name_and_size(char *filename);

void print_files(char *trozos[], char *path, bool opAcc, bool opLink, bool opLong, bool opHid, bool opReca, bool opRecb);

void print_files_rec(char *trozos[], char *path, char c, bool opAcc, bool opLink, bool opLong, bool opHid);

void invalid_nargs();

void invalid_arg();

void cmd_not_found();

void print_malloc(tListM L);

void print_mmap(tListM L);

void print_mem(char *p, int bytes);

void print_memoryList(tListM L);

void print_shared(tListM L);

void print_var(char *env[], char *name);

void print_env_addr(char *env[], char *environ[]);

void print_job(tItemP item);

void empty_list();

void print_end(tItemP item);

#endif
