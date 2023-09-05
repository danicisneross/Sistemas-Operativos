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
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include "file.h"
#include "prints.h"


/**
 * Function: LetraTF
 * ----------------------
 * Checks the mode of the file and
 * returns the corresponding letter.
 *
 * @return mode letter
 */
char LetraTF(mode_t m) {
    switch (m & S_IFMT) { /* and bit a bit con los bits de formato, 0170000 */
        case S_IFSOCK:
            return 's'; /* socket */
        case S_IFLNK:
            return 'l'; /* symbolic link */
        case S_IFREG:
            return '-'; /* fichero normal */
        case S_IFBLK:
            return 'b'; /* block device */
        case S_IFDIR:
            return 'd'; /* directorio */
        case S_IFCHR:
            return 'c'; /* char device */
        case S_IFIFO:
            return 'p'; /* pipe */
        default:
            return '?'; /* desconocido, no deberia aparecer */
    }
}

/**
 * Function: ConvierteModo
 * ----------------------
 * Checks permissions of the file and returns the
 * corresponding string in drwxrxwrxw format.
 *
 * @param m file mode
 * @param permisos initialized string
 * @return updated string with the
 *         corresponding permissions
 */
char *ConvierteModo(mode_t m, char *permisos) {
    strcpy(permisos, "---------- ");
    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r'; /*propietario*/
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r'; /*grupo*/
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r'; /*resto*/
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's'; /*setuid, setgid y stickybit*/
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';

    return permisos;
}

/**
 * Function: find_size
 * ----------------------
 * Calculates the size of a file.
 *
 * @param path string of the file or
 *        directory to find its size
 * @return bytes of the file
 */
long int find_size(const char *path) {
    DIR *d;
    char firstpath[MAX_CHARS];
    struct dirent *dir;
    long int size = 0;
    struct stat filestat;
    stat(path, &filestat);
    getcwd(firstpath, sizeof(firstpath));

    /* se comprueba si es un directorio */
    if (LetraTF(filestat.st_mode) == 'd') {
        if (path == NULL) {
            return -1;
        }
        d = opendir(path); /* se abre el directorio */

        if (d) {
            /* se accede al directorio 'path' */
            if (chdir(path) != -1) {
                while ((dir = readdir(d)) != NULL) {
                    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                        stat(dir->d_name, &filestat); /* se guarda información del archivo */
                        size += find_size(dir->d_name); /* se añade el tamaño del fichero */
                    }
                }
                if (strcmp(path, ".") != 0) {
                    chdir(".."); /* se vuelve al directorio anterior */
                }
                if (strcmp(path, "..") == 0) {
                    chdir(firstpath); /* se vuelve al directorio principal */
                }
            }
            closedir(d); /* se cierra el directorio */
        }

        return size;
    } else {
        FILE *fp = fopen(path, "r"); /* se abre el fichero en modo lectura */

        /* se comprueba la existencia del fichero: */
        if (fp == NULL) {
            print_error();
            return -1;
        }

        fseek(fp, 0L, SEEK_END);

        size = ftell(fp); /* se obtiene el tamaño del fichero: */

        fclose(fp); /* se cierra el fichero */

        return size;
    }

}

/**
 * Function: remove_file
 * ----------------------
 * Removes a file or a directory
 *
 * @param c type of the file
 * @param filename name of the file
 * @return void.
 */
void remove_file(char c, char *filename) {
    if (c == '-') {
        if (remove(filename) == 0) {
            printf("Removed file \"%s\"\n", filename);
        } else {
            print_error();
        }
    } else if (c == 'd') {
        if (rmdir(filename) == 0) {
            printf("Removed directory \"%s\"\n", filename);
        } else {
            print_error();
        }
    } else {
        printf("Error: \"%s\" could not be removed\n", filename);
    }
}

/**
 * Function: copyfiles
 * ----------------------
 * Copies in trozos all the files
 * found in the directory path.
 *
 * @param path string with the path
 * @param trozos array of strings
 * @return number of strings copied in trozos
 */
int copyfiles(char *path, char *trozos[]) {
    DIR *d;
    struct dirent *dir;
    int cnt = 1;

    d = opendir(path); /* se abre el directorio */
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            } else {
                /* se reserva memoria para un string */
                trozos[cnt] = (char *) malloc(strlen(dir->d_name) + 1);
                /* se copia en trozos el nombre de cada archivo
                 * encontrado y se incrementa el contador */
                strcpy(trozos[cnt++], dir->d_name);
            }
        }
        closedir(d); /* se cierra el directorio */
    }
    return cnt; /* se devuelve el número de strings copiados */
}

/**
 * Function: trocearCadena
 * ----------------------
 * Copies in trozos all the tokens found
 * in the string separated by spaces.
 *
 * @param cadena string with the command
 * @param trozos array of strings
 * @return number of strings copied in trozos
 */
int trocearCadena(char *cadena, char *trozos[]) {
    int i = 1;

    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL) {
        return 0;
    }
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL) {
        i++;
    }

    return i;
}

/**
 * Function: checkoptions
 * ----------------------
 * Checks the options and changes its
 * value to the corresponding one.
 *
 * @param trozos array of strings
 * @param opLong, opLink, opAcc, opReca, opRecb, opHid booleans of the corresponding options
 * @param n number of arguments in trozos
 * @return void.
 */
void checkoptions(char *trozos[], bool *opLong, bool *opLink, bool *opAcc,
                  bool *opReca, bool *opRecb, bool *opHid, int n) {

    /* se identifican las opciones escogidas por el
     * usuario para los seis primeros parámetros: */
    for (int i = 1; i < n; i++) {
        if (trozos[i] == NULL) {
            break;
        } else if (strcmp(trozos[i], "-acc") == 0) {
            *opAcc = true;
        } else if (strcmp(trozos[i], "-link") == 0) {
            *opLink = true;
        } else if (strcmp(trozos[i], "-long") == 0) {
            *opLong = true;
        } else if (strcmp(trozos[i], "-reca") == 0) {
            *opReca = true;
        } else if (strcmp(trozos[i], "-recb") == 0) {
            *opRecb = true;
        } else if (strcmp(trozos[i], "-hid") == 0) {
            *opHid = true;
        }
    }
}

/**
 * Function: fix_trozos
 * ----------------------
 * Copies the command to an auxiliar array of strings
 * so that it only prints information about one file.
 *
 * @param trozos array of strings
 * @param file name of the file to print information about
 * @param opAcc, opLink, opLong, opHid, opReca, opRecb booleans of the corresponding options
 * @return void.
 */
void fix_trozos(char *trozos[], char *file,
                bool opAcc, bool opLink, bool opLong, bool opHid, bool opReca, bool opRecb) {
    int i = 1;
    char *aux_trozos[MAX_ARGS];

    /* se guarda en i el número de opciones a true: */
    i += opAcc + opLong + opLink + opHid + opReca + opRecb;

    /* se reserva en aux_trozos el espacio necesario para
     * copiar todas las opciones a true correspondientes */
    for (int j = 0; j < i; ++j) {
        aux_trozos[j] = (char *) malloc(strlen(trozos[j]) + 1);
        strcpy(aux_trozos[j], trozos[j]);
    }

    /* se copia en aux_trozos el archivo del
     * cual se quiere imprimir información */
    aux_trozos[i] = (char *) malloc(strlen(file) + 1);
    strcpy(aux_trozos[i], file);
    i++;

    /* se envía el nuevo array de strings a listfich: */
    cmdListfich(aux_trozos, i, true);

    /* se liberan todos los espacios de memoria
     * que se habían reservado anteriormente */
    for (int j = 0; j < i; ++j) {
        free(aux_trozos[j]);
    }
}

/**
 * Function: borrarComandos
 * ----------------------
 * The memory reserved for each command in the
 * list is freed.
 *
 * @param L command list
 * @return void.
 */
void borrarComandosC(tListC *L) {
    tItemC item;

    for (tPosC pos = firstC(*L); pos != NULL; pos = nextC(pos, *L)) {
        item = getItemC(pos, *L);
        free(item.CommandName);
    }
}