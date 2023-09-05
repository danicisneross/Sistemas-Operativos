/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 18 / 11 / 2021
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/resource.h>

#include "aux_file.h"
#include "aux_mem.h"
#include "aux_proc.h"
#include "prints.h"

/**
 * Function: print_error
 * ----------------------
 * Shows an error message depending
 * on what happened in the program.
 *
 * @return void.
 */
void print_error() {
    perror("Error");
}

/**
 * Function: print_pid
 * ----------------------
 * Prints the (parent) proccess id.
 *
 * @param pid integer value of the pid
 * @param p_pid integer value of the parent pid
 * @param param string of the option
 * @return void.
 */
void print_pid(int pid, int p_pid, char *param) {
    if (param == NULL) {
        printf("Process id: %d\n", pid);
    } else if (strcmp(param, "-p") == 0) {
        printf("Parent process id: %d\n", p_pid);
    }
}

/**
 * Function: print_time
 * ----------------------
 * Prints the time in YYYY/MM/DD-HH:mm format
 *
 * @param timer needed for function 'localtime'
 * @return void.
 */
void print_time(const time_t *timer) {
    char time[20];
    strftime(time, 20, "%Y/%m/%d-%H:%M", localtime(timer));
    printf("%s ", time);
}

/**
 * Function: print_current_path
 * ----------------------
 * Prints the current working directory
 *
 * @return 0 if the path was found,
 *         -1 if there was an error.
 */
int print_current_path() {
    char ruta[255];

    /* Se comprueba la existencia de la ruta */
    if (getcwd(ruta, sizeof(ruta)) == NULL) {
        print_error();
        return -1;
    }
    printf("%s\n", ruta);
    return 0;
}

/**
 * Function: print_name_and_size
 * ----------------------
 * Prints the name and the size of a file
 *
 * @param filename string with the name of the file
 * @return 0 if printed successfully,
 *         -1 if there was an error.
 */
int print_name_and_size(char *filename) {
    DIR *d;
    struct dirent *dir;

    if (filename == NULL) {
        return -1;
    }

    d = opendir("."); /* se accede al directorio: */
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            /* si se encuentra el archivo: */
            if (strcmp(dir->d_name, filename) == 0) {
                printf("%10ld  %s", find_size(filename), filename);
                closedir(d);
                return 0;
            }
        }
        closedir(d);
    }
    return -1;
}

/**
 * Function: print_files
 * ----------------------
 * Prints all the contents of a directory
 *
 * @param trozos array of strings containing the command
 * @param path string with the name of the directory
 * @param opAcc, opLink, opLong, opHid booleans for the possible options
 * @return void.
 */
void
print_files(char *trozos[], char *path, bool opAcc, bool opLink, bool opLong, bool opHid, bool opReca, bool opRecb) {
    DIR *d;
    struct dirent *dir;

    d = opendir(path); /* se accede al directorio: */
    if (d) {
        chdir(path);
        printf("************%s:\n", path);
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] == '.' && !opHid) {
                continue;
            }
            fix_trozos(trozos, dir->d_name, opAcc, opLink, opLong, opHid, opReca, opRecb);
        }
        if (strcmp(path, ".") != 0 && strcmp(path, "..") != 0) {
            chdir("..");
        }
        closedir(d);
    }
}

/**
 * Function: print_files_rec
 * ----------------------
 * Prints all the files of a directory. Then, subdirectories
 * will be listed recursively before or after all the files
 * in the directory, depending on the chosen option.
 *
 * @param trozos array of strings containing the command
 * @param path string with the name of the directory
 * @param c char to choose between option "-reca" and "-recb"
 * @param opAcc, opLink, opLong, opHid booleans for the possible options
 * @return void.
 */
void print_files_rec(char *trozos[], char *path, char c, bool opAcc, bool opLink, bool opLong, bool opHid) {
    DIR *d;
    struct dirent *dir;
    struct stat filestat;

    /* para la opción "-reca", primero lista
     * los archivos del directorio: */
    if (c == 'a') {
        print_files(trozos, path, opAcc, opLink, opLong, opHid, true, false);
    }

    d = opendir(path); /* se accede al directorio: */
    if (d) {
        chdir(path);
        while ((dir = readdir(d)) != NULL) {
            /* control de archivos ocultos: */
            if (dir->d_name[0] == '.' && !opHid) {
                continue;
            }

            /* no se muestran "." y ".." para evitar recursión infinita: */
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }
            stat(dir->d_name, &filestat);
            /* si se encuentra una carpeta: (recursividad) */
            if (LetraTF(filestat.st_mode) == 'd') {
                print_files_rec(trozos, dir->d_name, c, opAcc, opLink, opLong, opHid);
            }
        }
        if (strcmp(path, ".") != 0 && strcmp(path, "..") != 0) {
            chdir("..");
        }
        closedir(d);
    }

    /* para la opción "-recb", lista los
     * archivos del directorio al final: */
    if (c == 'b') {
        print_files(trozos, path, opAcc, opLink, opLong, opHid, false, true);
    }
}

void invalid_nargs() {
    printf("* Invalid number of arguments *\n");
}

void cmd_not_found() {
    printf("* Command not found *\n");
}

void invalid_arg() {
    printf("* Invalid argument *\n");
}

void print_malloc(tListM L) {
    tPosM pos;
    tItemM item;

    printf("******* List of assigned blocks (malloc) for process %d *******\n", getpid());

    if (isEmptyListM(L)) {
        printf("(Empty list)\n");
        return;
    }

    for (pos = firstM(L); (pos != LNULL); pos = nextM(pos, L)) {
        item = getItemM(pos, L);
        if (item.cmdType == MALLOC) {
            printf("%p: size:%zu. malloc %s\n", item.dir, item.size, item.date);
        }
    }
}

void print_mmap(tListM L) {
    tPosM pos;
    tItemM item;

    printf("******* List of assigned blocks (mmap) for process %d *******\n", getpid());

    if (isEmptyListM(L)) {
        printf("(Empty list)\n");
        return;
    }

    for (pos = firstM(L); (pos != LNULL); pos = nextM(pos, L)) {
        item = getItemM(pos, L);
        if (item.cmdType == MMAP) {
            printf("%p: size:%zu. mmap %s (fd: %d) %s\n", item.dir, item.size,
                   item.Union.fich.filename, item.Union.fich.fd, item.date);
        }
    }
}

void print_shared(tListM L) {
    tPosM pos;
    tItemM item;

    printf("******* List of assigned blocks (shared) for process %d *******\n", getpid());

    if (isEmptyListM(L)) {
        printf("(Empty list)\n");
        return;
    }

    for (pos = firstM(L); (pos != LNULL); pos = nextM(pos, L)) {
        item = getItemM(pos, L);
        if (item.cmdType == SHARED) {
            printf("%p: size:%zu. shared memory (key %d) %s\n", item.dir, item.size,
                   item.Union.key, item.date);
        }
    }
}

void print_memoryList(tListM L) {
    tPosM pos;
    tItemM item;

    printf("******* List of assigned blocks for process %d *******\n", getpid());

    if (isEmptyListM(L)) {
        printf("(Empty list)\n");
        return;
    }

    for (pos = firstM(L); pos != LNULL; pos = nextM(pos, L)) {
        item = getItemM(pos, L);
        if (item.cmdType == MALLOC) {
            printf("%p: size:%zu. malloc %s\n", item.dir, item.size, item.date);
        } else if (item.cmdType == MMAP) {
            printf("%p: size:%zu. mmap %s (fd: %d) %s\n", item.dir, item.size,
                   item.Union.fich.filename, item.Union.fich.fd, item.date);
        } else if (item.cmdType == SHARED) {
            printf("%p: size:%zu. shared memory (key %d) %s\n", item.dir, item.size,
                   item.Union.key, item.date);
        }
    }
}

void print_mem(char *p, int bytes) {
    int i, j;
    int char_pos;
    int cnt = 0, lines = 0;

    for (i = 0; i < bytes; i++) {
        /* checks if it is a printable char
         * if not, prints a space */
        printable_char(p[i]);

        cnt++; /* counts the nº of chars for each row */

        if ((cnt == bytes - 25 * lines) || (cnt == 25)) {
            printf("\n");

            char_pos = i + 1 - cnt; /* saves the position of the first char in the row */

            /* loop for the code in hexadecimal format: */
            for (j = char_pos; j < char_pos + cnt; ++j) {
                /* checks if it is a printable char
                 * if not, prints a space */
                printable_hex(p[j]);
            }

            printf("\n");
            lines++; /* counts the lines of chars that have been filled */
            cnt = 0; /* reset the count of chars for each row */
        }
    }
}

void print_var(char *env[], char *name) {
    int i;

    for (i = 0; env[i] != NULL; i++) {
        printf("%p->%s[%d]=(%p) %s\n", &env[i], name, i, env[i], env[i]);
    }
}

void print_env_addr(char *env[], char *environ[]) {
    printf("main arg3: %p (stored in %p)\n", env, &env);
    printf("environ: %p (stored in %p)\n", environ, &environ);
}

void print_job(tItemP item) {
    char *status = check_status(item.state);

    if (item.state == STOPPED || item.state == KILLED) {
        printf("%d\t%s p=%d %s %s (%s) %s\n", item.pid, item.user, getpriority(PRIO_PROCESS, item.pid), item.time,
               status, NombreSenal(item.end), item.command);
    } else {
        printf("%d\t%s p=%d %s %s (%d) %s\n", item.pid, item.user, getpriority(PRIO_PROCESS, item.pid), item.time,
               status, item.end, item.command);
    }
}

void empty_list() {
    printf("Error: empty list\n");
}

void print_end(tItemP item) {

    switch (item.state) {
        case EXITED:
            printf("Process %d was exited with %d\n", item.pid, item.end);
            break;
        case RUNNING:
            printf("Process %d is running\n", item.pid);
            break;
        case STOPPED:
            printf("Process %d was stopped with signal %d (%s)\n", item.pid, item.end, NombreSenal(item.end));
            break;
        case KILLED:
            printf("Process %d was killed with signal %d (%s)\n", item.pid, item.end, NombreSenal(item.end));
            break;
        default:
            printf("Process %d' state ", item.pid);
    }
}