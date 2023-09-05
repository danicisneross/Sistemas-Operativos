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
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include "file.h"
#include "memory.h"
#include "process.h"
#include "prints.h"
#include "aux_file.h"
#include "aux_mem.h"
#include "aux_proc.h"

#define COMANDO 0
#define PARAM 1

/**
 * Function: cmdAutores
 * ----------------------
 * Prints information about authors
 *
 * @param param string of the command
 * @param n number of words the command has
 * @return void.
 */
void cmdAutores(char *param, int n) {
    /*
     * Se comprueba si el comando es
     * válido y si tiene argumentos
     */

    if (n == 1) {
        printf("Name: Mateo Díaz\tLogin: mateo.diaz\n");
        printf("Name: Alvaro Freire\tLogin: alvaro.freirea\n");
    } else if (n == 2 && strcmp(param, "-n") == 0) {
        printf("Name: Mateo Diaz\nName: Alvaro Freire\n");
    } else if (n == 2 && strcmp(param, "-l") == 0) {
        printf("Login: mateo.diaz\nLogin: alvaro.freirea\n");
    } else if (n > 2)
        invalid_nargs();
    else {
        cmd_not_found();
    }
}

/**
 * Function: cmdAyuda
 * ----------------------
 * Shows information about a command.
 *
 * @param param string of the command
 * @param n number of words the command has
 * @return void.
 */
void cmdAyuda(char *param, int n) {

    if (n == 1) {
        printf("** ayuda: muestra información sobre un comando\n");
        printf("Uso: ayuda [comando]\n");
        printf("Comandos:\n\t-autores\n\t-ayuda\n\t-carpeta\n\t-comando\n\t"
               "-fecha\n\t-hist\n\t-infosis\n\t-pid\n\t-salir, fin o bye\n");

    } else if (strcmp(param, "autores") == 0) {
        printf("** autores: muestra nombre y login de los autores del shell\n");
        printf("Uso: autores [-l/-n]\n");
        printf("\t-l:\tmuestra el login de los autores\n");
        printf("\t-n:\tmuestra el nombre de los autores\n");

    } else if (strcmp(param, "pid") == 0) {
        printf("** pid: muestra el id del proceso que ejecuta el shell.\n");
        printf("Uso: pid [-p]\n");
        printf("\t-p:\tmuestra el id del proceso padre del shell");

    } else if (strcmp(param, "carpeta") == 0) {
        printf("** carpeta: muestra la ruta del directorio actual y permite cambiar de directorio.\n");
        printf("Uso: carpeta [dir]\n");
        printf("\tdir:\truta del directorio al que se desea acceder\n");

    } else if (strcmp(param, "fecha") == 0) {
        printf("** fecha: muestra fecha y hora actuales.\n");
        printf("Uso: fecha [-d/-h]\n");
        printf("\t-d:\tmuestra únicamente la fecha\n");
        printf("\t-h\tmuestra únicamente la hora\n");

    } else if (strcmp(param, "hist") == 0) {
        printf("** hist: muestra el historial de comandos.\n");
        printf("Uso: hist [-c/-N]\n");
        printf("\t-c:\tlimpia el historial de comandos\n");
        printf("\t-N:\tmuestra el historial de comandos hasta el número N\n");

    } else if (strcmp(param, "comando") == 0) {
        printf("** comando: repite el comando número 'n' (del historial de comandos)\n");
        printf("Uso: comando N\n");

    } else if (strcmp(param, "borrar") == 0) {
        printf("** borrar: elimina ficheros y/o directorios vacíos.\n");
        printf("Uso: borrar nombre1 nombre2 ...\n");

    } else if (strcmp(param, "borrarrec") == 0) {
        printf("** borrarrec: elimina ficheros y/o directorios no vacíos.\n");
        printf("Uso: borrarrec nombre1 nombre2 ...\n");

    } else if (strcmp(param, "crear") == 0) {
        printf("** crear: crea un fichero o directorio en la ruta actual.\n");
        printf("Uso: crear [-f] nombre\n");
        printf("\t-f:\tcon esta opción se creará un fichero y no un directorio.\n");

    } else if (strcmp(param, "listfich") == 0) {
        printf("** listfich: muestra información acerca de ficheros, directorios, dispositivos...\n");
        printf("Uso: listfich [-long] [-link] [-acc] nombre1 nombre2 ...\n");
        printf("\t-long:\tmostrará la última fecha de modificación (formato YYYY/MM/DD-HH:mm), número\n");
        printf("\t\tde links, owner, grupo, modo (formato drwx), tamaño y nombre del archivo.\n");
        printf("\t-link:\tsolo sirve con la opción -long. Si el fichero es un link simbólico, también\n");
        printf("\t\tmuestra el archivo al que apunta.\n");
        printf("\t-acc:\tsolo sirve con la opción -long. Se mostrará la fecha de último acceso en lugar\n");
        printf("\t\tde la fecha de última modificación.\n");

    } else if (strcmp(param, "listdir") == 0) {
        printf("** listdir: lista el contenido de directorios. Si no se trata de un directorio,\n");
        printf("\t    se mostrará su información.\n");
        printf("Uso: listdir [-reca] [-recb] [-hid] [-long] [-link] [-acc] nombre1 nombre2 ...\n");
        printf("\t-reca:\tlos subdirectorios se mostrarán recursivamente después de listar todo el\n");
        printf("\t\tcontenido del directorio.\n");
        printf("\t-recb:\tlos subdirectorios se mostrarán recursivamente antes de listar todo el\n");
        printf("\t\tcontenido del directorio.\n");
        printf("\t-hid:\tmostrará también los ficheros y/o directorios ocultos.\n");
        printf("\t-long:\tmostrará la última fecha de modificación (formato YYYY/MM/DD-HH:mm), número\n");
        printf("\t\tde links, owner, grupo, modo (formato drwx), tamaño y nombre del archivo.\n");
        printf("\t-link:\tsolo sirve con la opción -long. Si el fichero es un link simbólico, también\n");
        printf("\t\tmuestra el archivo al que apunta.\n");
        printf("\t-acc:\tsolo sirve con la opción -long. Se mostrará la fecha de último acceso en lugar\n");
        printf("\t\tde la fecha de última modificación.\n");

    } else if (strcmp(param, "infosis") == 0) {
        printf("** infosis: muestra información sobre el sistema\n");
        printf("Uso: infosis\n");

    } else if (strcmp(param, "fin") == 0 || strcmp(param, "salir") == 0 || strcmp(param, "bye") == 0) {
        printf("** %s: termina el programa. Salida del shell.\n", param);
        printf("Uso: %s\n", param);

    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdBorrar
 * ----------------------
 * Deletes files and empty directories.
 *
 * @param trozos array of strings of the command
 * @param n number of words the command has
 * @return void.
 */
void cmdBorrar(char *trozos[], int n) {
    int status;
    struct stat st_buf;

    if (n == 1) {
        print_current_path();
    } else {
        /* se comprueban todos los parámetros recibidos: */
        for (int i = 1; i < n; i++) {
            status = stat(trozos[i], &st_buf);
            if (status != 0) {
                print_error();
            } else {
                remove_file(LetraTF(st_buf.st_mode), trozos[i]);
            }
        }
    }
}

/**
 * Function: cmdBorrarrec
 * ----------------------
 * Deletes files and directories (empty and not empty).
 *
 * @param trozos array of strings of the command
 * @param n number of words the command has
 * @return void.
 */
void cmdBorrarrec(char *trozos[], int n) {
    int status, n2;
    struct stat st_buf;
    char c;
    char *filename;
    char *trozos2[MAX_ARGS];

    if (n == 1) {
        print_current_path();
    } else {
        /* se comprueban todos los parámetros recibidos: */
        for (int i = 1; i < n; ++i) {
            status = stat(trozos[i], &st_buf);
            if (status != 0) {
                print_error();
                return;
            }
            filename = trozos[i];
            c = LetraTF(st_buf.st_mode);

            /* se comprueba si el parámetro recibido es
             * un fichero o si es un directorio: */
            if (c == '-') {
                remove_file(c, filename);
            } else if (c == 'd') {
                if (rmdir(filename) == 0) {
                    printf("Removed directory \"%s\"\n", filename);
                } else if (errno == ENOTEMPTY) {
                    /* se guardan en un array los nombres de todos los archivos
                     * y directorios de la carpeta que se quiere eliminar */
                    n2 = copyfiles(filename, trozos2);
                    /* se cambia de directorio al que se va a realizar
                     * la eliminación recursiva de todos sus elementos */
                    if (chdir(filename) == 0) {
                        /* se elimina recursivamente todos
                         * los elementos de la carpeta */
                        cmdBorrarrec(trozos2, n2);
                        /* se regresa al directorio original */
                        chdir("..");
                        if (rmdir(filename) == 0) {
                            printf("Removed directory \"%s\"\n", trozos[i]);
                        } else {
                            print_error();
                        }
                    } else {
                        print_error();
                    }
                    if (n2 > 1) {
                        for (int j = 1; j < n2; j++) {
                            free(trozos2[j]);
                            trozos2[j] = NULL;
                        }
                    }
                } else {
                    print_error();
                }
            } else {
                printf("Error: \"%s\" could not be removed\n", trozos[i]);
            }
        }
    }
}

/**
 * Function: cmdCarpeta
 * ----------------------
 * Changes the current working directory.
 *
 * @param param string of target directory
 * @param n number of words the command has
 * @return void.
 */
void cmdCarpeta(char *param, int n) {
    if (n == 1 || (n == 2 && chdir(param) == 0)) {
        print_current_path();
    } else {
        print_error();
    }
}

/**
 * Function: cmdComando
 * ----------------------
 * Repeats a command from the history.
 *
 * @param param string with the number of the command
 * @param n number of words the command has
 * @param L pointer to the list of commands
 * @param commandNumber number of commands the history has
 * @return void.
 */
void cmdComando(char *param, int n, tListC *CommandList, int *commandNumber, tListM *MemoryList,
                tListE *EnvironmentList, char *arg3[], char *environ[], char **std_error, tListP *ProcessList) {
    int i;
    char *command;
    tPosC pos;
    tItemC item;

    /*
     * Se comprueba el número de argumentos del
     * comando para procesarlo adecuadamente
     */
    if (n == 2) {
        i = atoi(param);
        if (i == 0) {
            printf("* Invalid expression *\n");
        } else if ((pos = findItemC(i, *CommandList)) == LNULL) {
            printf("* Number does not belong to any command *\n");
        } else {
            item = getItemC(pos, *CommandList);
            command = (char *) malloc(strlen(item.CommandName) + 1);
            strcpy(command, item.CommandName);
            procesarEntrada(command, false, CommandList, commandNumber, MemoryList,
                            EnvironmentList, arg3, environ, std_error, ProcessList);
            free(command);
        }
    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdCrear
 * ----------------------
 * Creates a file or a directory.
 *
 * @param trozos array of strings with the command
 * @param n number of words the command has
 * @return void.
 */
void cmdCrear(char *trozos[], int n) {
    int fd;

    if (n == 1) {
        print_current_path();
    } else if (n == 2) {
        /* if no option is given, try to create a directory: */
        if (mkdir(trozos[1], 0777) == -1) {
            print_error();
        } else {
            printf("Directory \"%s\" was created successfully\n", trozos[1]);
        }
    } else if (n == 3 && strcmp(trozos[1], "-f") == 0) {
        /* if an option is given, check if it's
         * "-f" and create a file in that case: */
        fd = open(trozos[2], O_CREAT, 0777);
        if (fd == -1) {
            print_error();
            return;
        } else {
            printf("File \"%s\" was created successfully\n", trozos[2]);
            close(fd);
        }
    } else if (n > 3) {
        invalid_nargs();
    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdExit
 * ----------------------
 * Ends program.
 *
 * @param n number of words the command has
 * @param exit bool pointer to make the exit
 * @param L pointer to the list of commands
 * @return void.
 */
void cmdExit(int n, bool *exit, tListC *CommandList, tListM *MemoryList, tListE *EnvironmentList,
             tListP *ProcessList, char **std_error) {
    if (n == 1) {
        *exit = true;

        /* se libera la memoria reservada para cada lista */
        borrarComandosC(CommandList);
        borrarFilenameM(MemoryList);

        /* se libera la memoria sin liberar por el usuario */
        liberarMemoria(MemoryList);

        /* se libera la memoria reservada para las variables de entorno */
        liberarEnvironment(*EnvironmentList);

        /* se libera la memoria reservada para los comandos de los procesos en background */
        liberarProcessCommand(*ProcessList);

        free(*std_error);

        /* se elimina la lista definitivamente */
        deleteListC(CommandList);

        /* se elimina la lista de variables de entorno */
        deleteListE(EnvironmentList);

        /* se elimina la lista de procesos en background */
        deleteListP(ProcessList);

    } else {
        invalid_nargs();
    }
}

/**
 * Function: cmdFecha
 * ----------------------
 * Shows the local date and/or hour.
 *
 * @param param string of the command
 * @param n number of words the command has
 * @return void.
 */
void cmdFecha(char *param, int n) {
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char fecha[128];
    char hora[128];

    /*
     * Se guardan la fecha y hora actuales
     * en dos variables diferentes
     */
    strftime(fecha, 128, "Date: %d/%m/%y", tlocal);
    strftime(hora, 128, "Time: %H:%M:%S", tlocal);

    /*
     * Se comprueba el número de argumentos del
     * comando para procesarlo adecuadamente
     */
    if (n == 1) {
        printf("%s\n%s\n", fecha, hora);
    } else if (n == 2) {
        if (strcmp(param, "-d") == 0) {
            printf("%s\n", fecha);
        } else if (strcmp(param, "-h") == 0) {
            printf("%s\n", hora);
        }
    } else if (n > 2) {
        invalid_nargs();
    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdHist
 * ----------------------
 * Shows the history of commands.
 *
 * @param param string of the option given
 * @param n number of words the command has
 * @param L pointer to the list of commands
 * @param commandNumber number of commands the history has
 * @return void.
 */
void cmdHist(char *param, int n, tListC *CommandList, int *commandNumber) {
    tItemC item;
    char *number;
    int i, cnt = 1;
    tPosC aux;

    /*
     * Se comprueba si el comando es
     * válido y si tiene argumentos
     */
    if (n == 1) {

        /* Se recorre la lista para imprimirla por pantalla */
        for (tPosC pos = firstC(*CommandList); pos != LNULL; pos = nextC(pos, *CommandList)) {
            item = getItemC(pos, *CommandList);
            printf("%d->%s", cnt, item.CommandName);
            cnt++;
        }
    } else if (n == 2 && strcmp(param, "-c") == 0) {

        /* Se elimina la lista para limpiar el historial de comandos */
        deleteListC(CommandList);

        /* Se inicializa de nuevo para continuar usándola */
        createEmptyListC(CommandList);

        /* Se actualiza el número de comandos */
        *commandNumber = 1;
        printf("* Empty history *\n");
    } else if (n == 2 && param[0] == '-') {

        /* Se guarda la cadena de caracteres a continuación de '-' */
        number = strtok(param, "-");

        /* Se convierte a int la cadena de caracteres */
        i = atoi(number);

        /* Se comprueba si es un número válido */
        if (i == 0) {
            printf("* Invalid expression after '-' *\n");

            /* Se comprueba si el número corresponde a algún elemento de la lista */
        } else if ((aux = findItemC(i, *CommandList)) == LNULL) {
            printf("* Number does not belong to any command *\n");
        } else {

            /* Se recorre la lista para imprimirla por pantalla */
            for (tPosC pos = firstC(*CommandList); pos != nextC(aux, *CommandList); pos = nextC(pos, *CommandList)) {
                item = getItemC(pos, *CommandList);
                printf("%d->%s", cnt, item.CommandName);
                cnt++;
            }
        }
    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdInfosis
 * ----------------------
 * Shows the system information.
 *
 * @param n number of words the command has
 * @return void.
 */
void cmdInfosis(int n) {
    struct utsname info;

    /*
     * Se comprueba el número de argumentos del
     * comando para procesarlo adecuadamente
     */
    if (n == 1) {
        /* uname(&info) devuelve 0 si se obtuvo la
         * información adecuadamente. Se comprueba: */
        if (uname(&info) == 0) {
            printf("- Sysname: %s\n", info.sysname);
            printf("- Nodename: %s\n", info.nodename);
            printf("- Release: %s\n", info.release);
            printf("- Version: %s\n", info.version);
            printf("- Machine: %s\n", info.machine);
        } else {
            print_error();
        }
    } else {
        cmd_not_found();
    }
}

/**
 * Function: cmdListfich
 * ----------------------
 * Prints information about directories.
 *
 * @param trozos array of strings with the command
 * @param n number of words the command has
 * @return void.
 */
void cmdListdir(char *trozos[], int n) {
    int nOptions;
    struct stat filestat;
    bool opLong = false, opLink = false, opAcc = false;
    bool opReca = false, opRecb = false, opHid = false;

    checkoptions(trozos, &opLong, &opLink, &opAcc, &opReca, &opRecb, &opHid, n);
    nOptions = opLong + opLink + opAcc + opReca + opRecb + opHid;

    if (opReca && opRecb) {
        printf("Error: incompatible options -reca and -recb\n");
        return;
    }

    if (n == nOptions + 1) {
        print_current_path();
    } else {
        for (int i = nOptions + 1; i < n; i++) {
            if (stat(trozos[i], &filestat) == -1) {
                print_error();
                continue;
            }
            /* se comprueba si se trata de un directorio: */
            if (LetraTF(filestat.st_mode) == 'd') {
                /* se comprueba cuál fue la opción: */
                if (!opReca && !opRecb) {
                    print_files(trozos, trozos[i], opAcc, opLink, opLong, opHid, opReca, opRecb);
                }
                if (opRecb) {
                    print_files_rec(trozos, trozos[i], 'b', opAcc, opLink, opLong, opHid);
                }
                if (opReca) {
                    print_files_rec(trozos, trozos[i], 'a', opAcc, opLink, opLong, opHid);
                }
            } else {
                /* si se trata de un archivo se trata como con el comando Listfich: */
                fix_trozos(trozos, trozos[i], opAcc, opLink, opLong, opHid, opReca, opRecb);
            }
        }
    }
}

/**
 * Function: cmdListfich
 * ----------------------
 * Prints information about files.
 *
 * @param trozos array of strings with the command
 * @param n number of words the command has
 * @param aux_function bool to know if the call was made
 *        by the user or by an auxiliar function.
 * @return void.
 */
void cmdListfich(char *trozos[], int n, bool aux_function) {
    int nOptions, status;
    ssize_t bufsiz, nbytes;
    struct passwd *pwd;
    struct group *grp;
    char *permisos, *link_buf;
    struct stat filestat;
    bool opLong = false, opLink = false, opAcc = false;

    /* variables para el caso de llamada desde Listdir: */
    bool opReca = false, opRecb = false, opHid = false;

    checkoptions(trozos, &opLong, &opLink, &opAcc, &opReca, &opRecb, &opHid, n);
    nOptions = opLong + opLink + opAcc;

    if (aux_function) {
        nOptions += opHid + opReca + opRecb;
    } else if (opHid || opReca || opRecb) {
        invalid_arg();
        return;
    }

    /* si no se recibe ningún parámetro:
     * (p.ej.: "listfich -link" ) */
    if (n == nOptions + 1) {
        print_current_path();
    } else {
        for (int i = nOptions + 1; i < n; i++) {
            if (opLong) {
                status = lstat(trozos[i], &filestat);
                if (status == 0) {
                    if (opAcc) {
                        /* fecha del último acceso */
                        print_time(&filestat.st_atime);
                    } else {
                        /* fecha de la última modificación */
                        print_time(&filestat.st_mtime);
                    }
                    /* número de links */
                    printf("%lu ", filestat.st_nlink);
                    /* número de inodo */
                    printf("(%lu) ", filestat.st_ino);

                    /* User ID of owner */
                    pwd = getpwuid(filestat.st_uid);
                    printf("%s ", pwd->pw_name);

                    /* Group ID of owner */
                    grp = getgrgid(filestat.st_gid);
                    printf("%s ", grp->gr_name);

                    permisos = (char *) malloc(12);

                    /* se guardan y muestran los permisos del archivo */
                    permisos = ConvierteModo(filestat.st_mode, permisos);
                    printf("%s", permisos);

                    free(permisos);

                    /* nombre y tamaño del archivo o directorio */
                    if (print_name_and_size(trozos[i]) == -1) {
                        print_error();
                        continue;
                    }

                    /* se comprueba si la opción "-link" fue
                     * escrita y si el archivo es un link */
                    if (opLink && LetraTF(filestat.st_mode) == 'l') {
                        bufsiz = filestat.st_size;
                        link_buf = malloc(bufsiz);
                        nbytes = readlink(trozos[i], link_buf, bufsiz);
                        if (nbytes == -1) {
                            print_error();
                        } else {
                            printf(" -> %s\n", link_buf);
                        }
                        free(link_buf);
                    } else {
                        printf("\n");
                    }
                } else {
                    print_error();
                    continue;
                }
            } else {
                /* nombre y tamaño del archivo o directorio */
                if (print_name_and_size(trozos[i]) == -1) {
                    print_error();
                    continue;
                }
                printf("\n");
            }
        }
    }
}

/**
 * Function: cmdPid
 * ----------------------
 * Gets and prints the (parent) proccess id.
 *
 * @param param string with the option
 *              (if '-p' for parent pid).
 * @param n number of arguments of the command
 * @return void.
 */
void cmdPid(char *param, int n) {
    pid_t process_id;
    pid_t p_process_id;

    /* Se guarda el pid y el ppid */
    process_id = getpid();
    p_process_id = getppid();

    if (n == 1 || n == 2) {
        print_pid(process_id, p_process_id, param);
    } else {
        invalid_nargs();
    }
}

/**
 * Function: cmdSwitcher
 * ----------------------
 * Makes the call to the corresponding command.
 *
 * @param trozos array of strings with the command
 * @param n number of arguments of the command
 * @param exit bool pointer for cmdExit.
 * @param L pointer to the list of commands
 * @param commandNumber number of commands of the history
 * @return void.
 */
void cmdSwitcher(char *trozos[], int n, bool *exit, tListC *CommandList, int *commandNumber, tListM *MemoryList,
                 tListE *EnvironmentList, char *arg3[], char *environ[], char **std_error, tListP *ProcessList) {
    /*
     * Se comprueba de qué comando se trata y se
     * llama al procedimiento correspondiente
     */

    if (strcmp(trozos[COMANDO], "autores") == 0) {
        cmdAutores(trozos[PARAM], n);

    } else if (strcmp(trozos[COMANDO], "pid") == 0) {
        cmdPid(trozos[PARAM], n);

    } else if (strcmp(trozos[COMANDO], "carpeta") == 0 ||
               strcmp(trozos[COMANDO], "cd") == 0) {
        cmdCarpeta(trozos[PARAM], n);

    } else if (strcmp(trozos[COMANDO], "fecha") == 0) {
        cmdFecha(trozos[PARAM], n);

    } else if (strcmp(trozos[COMANDO], "hist") == 0) {
        cmdHist(trozos[PARAM], n, CommandList, commandNumber);

    } else if (strcmp(trozos[COMANDO], "comando") == 0) {
        cmdComando(trozos[PARAM], n, CommandList, commandNumber, MemoryList,
                   EnvironmentList, arg3, environ, std_error, ProcessList);

    } else if (strcmp(trozos[COMANDO], "infosis") == 0) {
        cmdInfosis(n);

    } else if (strcmp(trozos[COMANDO], "ayuda") == 0) {
        cmdAyuda(trozos[PARAM], n);

    } else if (strcmp(trozos[COMANDO], "fin") == 0 ||
               strcmp(trozos[COMANDO], "salir") == 0 ||
               strcmp(trozos[COMANDO], "bye") == 0 ||
               strcmp(trozos[COMANDO], "exit") == 0) {
        cmdExit(n, exit, CommandList, MemoryList, EnvironmentList, ProcessList, std_error);

    } else if (strcmp(trozos[COMANDO], "crear") == 0) {
        cmdCrear(trozos, n);

    } else if (strcmp(trozos[COMANDO], "borrar") == 0) {
        cmdBorrar(trozos, n);

    } else if (strcmp(trozos[COMANDO], "borrarrec") == 0) {
        cmdBorrarrec(trozos, n);

    } else if (strcmp(trozos[COMANDO], "listfich") == 0) {
        cmdListfich(trozos, n, false);

    } else if (strcmp(trozos[COMANDO], "listdir") == 0) {
        cmdListdir(trozos, n);

    } else if (strcmp(trozos[COMANDO], "malloc") == 0) {
        cmdMalloc(trozos, n, MemoryList);

    } else if (strcmp(trozos[COMANDO], "memoria") == 0) {
        cmdMemoria(trozos, n, *MemoryList);

    } else if (strcmp(trozos[COMANDO], "volcarmem") == 0) {
        cmdVolcarmem(trozos, n);

    } else if (strcmp(trozos[COMANDO], "llenarmem") == 0) {
        cmdLlenarmem(trozos, n);

    } else if (strcmp(trozos[COMANDO], "mmap") == 0) {
        cmdMmap(trozos, n, MemoryList);

    } else if (strcmp(trozos[COMANDO], "shared") == 0) {
        cmdShared(trozos, n, MemoryList);

    } else if (strcmp(trozos[COMANDO], "e-s") == 0) {
        if (strcmp(trozos[PARAM], "read") == 0) {
            cmdE_s_read(trozos, n);
        } else if (strcmp(trozos[PARAM], "write") == 0) {
            cmdE_s_write(trozos, n);
        } else {
            cmd_not_found();
        }

    } else if (strcmp(trozos[COMANDO], "dealloc") == 0) {
        cmdDealloc(trozos, n, MemoryList);

    } else if (strcmp(trozos[COMANDO], "recursiva") == 0) {
        cmdRecursiva(trozos, n);

    } else if (strcmp(trozos[COMANDO], "priority") == 0) {
        cmdPriority(trozos, n);

    } else if (strcmp(trozos[COMANDO], "rederr") == 0) {
        cmdRederr(trozos, n, std_error);

    } else if (strcmp(trozos[COMANDO], "entorno") == 0) {
        cmdEntorno(trozos, n, arg3, environ);

    } else if (strcmp(trozos[COMANDO], "mostrarvar") == 0) {
        cmdMostrarvar(trozos, n, arg3, environ);

    } else if (strcmp(trozos[COMANDO], "cambiarvar") == 0) {
        cmdCambiarvar(trozos, n, arg3, environ, EnvironmentList);

    } else if (strcmp(trozos[COMANDO], "uid") == 0) {
        cmdUid(trozos, n);

    } else if (strcmp(trozos[COMANDO], "fork") == 0) {
        cmdFork(n);

    } else if (strcmp(trozos[COMANDO], "ejec") == 0) {
        cmdEjec(trozos, n);

    } else if (strcmp(trozos[COMANDO], "ejecpri") == 0) {
        cmdEjecpri(trozos, n);

    } else if (strcmp(trozos[COMANDO], "fg") == 0) {
        cmdFg(trozos, n, 1);

    } else if (strcmp(trozos[COMANDO], "fgpri") == 0) {
        cmdFgpri(trozos, n);

    } else if (strcmp(trozos[COMANDO], "back") == 0) {
        cmdBack(trozos, n, ProcessList, 1);

    } else if (strcmp(trozos[COMANDO], "backpri") == 0) {
        cmdBackpri(trozos, n, ProcessList);

    } else if (strcmp(trozos[COMANDO], "ejecas") == 0) {
        cmdEjecas(trozos, n);

    } else if (strcmp(trozos[COMANDO], "fgas") == 0) {
        cmdFgas(trozos, n);

    } else if (strcmp(trozos[COMANDO], "bgas") == 0) {
        cmdBgas(trozos, n, ProcessList);

    } else if (strcmp(trozos[COMANDO], "listjobs") == 0) {
        cmdListjobs(n, ProcessList);

    } else if (strcmp(trozos[COMANDO], "job") == 0) {
        cmdJob(trozos, n, ProcessList);

    } else if (strcmp(trozos[COMANDO], "borrarjobs") == 0) {
        cmdBorrarjobs(trozos, n, ProcessList);

    } else {
        cmdLinux(trozos, n, ProcessList);
    }
}

/**
 * Function: procesarEntrada
 * ----------------------
 * Gets the input and processes it to
 * make the call to cmdSwitcher.
 *
 * @param command string with the whole command
 * @param exit bool pointer for cmdExit case
 * @param L pointer to the list of commands
 * @param commandNumber number of commands of the history
 * @return void.
 */
void procesarEntrada(char *command, bool *exit, tListC *CommandList, int *commandNumber, tListM *MemoryList,
                     tListE *EnvironmentList, char *arg3[], char *environ[], char **std_error, tListP *ProcessList) {
    char *trozos[MAX_ARGS];
    int n;

    /* Se separa el comando en trozos y se cuenta cuántos argumentos tiene */
    n = trocearCadena(command, trozos);

    /* Se comprueba si se escribió algún comando */
    if (n == 0) {

        /* Se elimina la última posición de la lista */
        deleteAtPositionC(lastC(*CommandList), CommandList);

        /* Se actualiza el número de comandos */
        *commandNumber = *commandNumber - 1;

    } else {
        cmdSwitcher(trozos, n, exit, CommandList, commandNumber, MemoryList,
                    EnvironmentList, arg3, environ, std_error, ProcessList);
    }
}
