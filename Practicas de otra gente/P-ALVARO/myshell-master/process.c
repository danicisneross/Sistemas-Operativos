/**
 * TITLE: Operative Systems
 * SUBTITLE: Lab Assignment 3
 * @author Mateo Díaz Allegue @login mateo.diaz
 * @author Álvaro Freire Ares @login alvaro.freirea
 * GROUP: 4.2
 * DATE: 22 / 11 / 2021
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

#include "process.h"
#include "prints.h"
#include "aux_proc.h"

void cmdCambiarvar(char *trozos[], int n, char *arg3[], char *environ[], tListE *EnvironmentList) {
    int i;
    char *var;
    tItemE item;

    if (n == 4) {
        /* se reserva memoria para concatenar los strings
         * correspondientes con el formato "name=value" */
        var = malloc(strlen(trozos[2]) + strlen(trozos[3]) + 4);
        strcpy(var, trozos[2]);
        strcat(var, "=");
        strcat(var, trozos[3]);
        /* se comprueba el modo en el que se va a acceder a la variable de entorno */
        if (strcmp(trozos[1], "-a") == 0) {
            /* se busca la variable en el array arg3 del main */
            if ((i = BuscarVariable(trozos[2], arg3)) == -1) {
                printf("Environment variable \"%s\" does not exist\n", trozos[2]);
                free(var);
                return;
            }
            /* se hace que el puntero de la variable correspondiente
             * apunte al string con el nuevo valor */
            arg3[i] = var;
        } else if (strcmp(trozos[1], "-e") == 0) {
            /* se busca la variable en el array environ del main */
            if ((i = BuscarVariable(trozos[2], environ)) == -1) {
                printf("Environment variable \"%s\" does not exist\n", trozos[2]);
                free(var);
                return;
            }
            /* se hace que el puntero de la variable correspondiente
             * apunte al string con el nuevo valor */
            environ[i] = var;
        } else if (strcmp(trozos[1], "-p") == 0) {
            /* si la variable existe se cambia su valor, si no existe se crea */
            if (putenv(var) != 0) {
                print_error();
                free(var);
                return;
            }
        } else {
            cmd_not_found();
            free(var);
            return;
        }
        printf("Environment variable \"%s\" changed its value to \"%s\"\n", trozos[2], trozos[3]);
        /* se añade la dirección del string a una lista para luego liberar su memoria reservada */
        item.name = var;
        insertItemE(item, EnvironmentList);
    } else {
        invalid_nargs();
    }
}

void cmdEntorno(char *trozos[], int n, char *arg3[], char *environ[]) {

    if (n == 1) {
        /* se imprime por pantalla el array del
         * tercer argumento del main (env) */
        print_var(arg3, "main arg3");
    } else if (n == 2 && strcmp(trozos[1], "-environ") == 0) {
        /* se imprime por pantalla el array de la
         * variable externa del main (environ) */
        print_var(environ, "environ");
    } else if (n == 2 && strcmp(trozos[1], "-addr") == 0) {
        /* se imprimen las direcciones de los dos
         * arrays mencionados anteriormente */
        print_env_addr(arg3, environ);
    } else {
        cmd_not_found();
    }
}

void cmdMostrarvar(char *trozos[], int n, char *arg3[], char *environ[]) {
    int i, j;
    char *value;

    if (n == 1) {
        /* se imprime por pantalla el array del
         * tercer argumento del main (arg3) */
        print_var(arg3, "main arg3");
    } else if (n == 2) {
        /* se obtiene el valor de la variable de entorno "trozos[1]" */
        value = getenv(trozos[1]);
        if (value == NULL) {
            printf("Variable \"%s\" does not exists\n", trozos[1]);
            return;
        }
        /* se busca en los arrays (3er argumento del main y variable
         * externa) la variable de entorno mencionada en el comando */
        if ((i = BuscarVariable(trozos[1], arg3)) == -1) {
            print_error();
        }
        if ((j = BuscarVariable(trozos[1], environ)) == -1) {
            print_error();
        }

        /* se imprimen valores y direcciones correspondientes de cada array */
        printf("With arg3 main:\t%s (%p) %p\n", arg3[i], arg3[i], &arg3[i]);
        printf("With environ:\t%s (%p) %p\n", environ[j], environ[j], &environ[j]);
        printf("With getenv:\t%s (%p)\n", value, &value);
    } else {
        cmd_not_found();
    }
}

void cmdPriority(char *trozos[], int n) {
    int priority, value;
    pid_t pid;
    /* se inicializa el errno a 0 para detectar
     * posibles errores más adelante */
    errno = 0;

    if (n == 1) {
        /* se intenta obtener la prioridad del proceso actual */
        if ((priority = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
            print_error();
            return;
        }
        pid = getpid();
        printf("Priority of the current process (%d): %d\n", pid, priority);
    } else if (n == 2) {
        /* se intenta convertir el argumento del pid a número */
        if ((pid = (pid_t) atoi(trozos[1])) == 0) {
            invalid_arg();
            return;
        }
        /* se intenta obtener la prioridad del proceso actual teniendo en
         * cuenta que puede devolver -1 pero no haberse producido un error */
        if ((priority = getpriority(PRIO_PROCESS, pid)) == -1 && errno != 0) {
            print_error();
            return;
        }
        printf("Priority of process %d: %d\n", pid, priority);
    } else if (n == 3) {
        /* se intenta convertir el argumento del pid a número */
        if ((pid = (pid_t) atoi(trozos[1])) == 0) {
            invalid_arg();
            return;
        }
        if ((value = set_priority(trozos[2], pid)) == -1) {
            print_error();
            return;
        }
        printf("Priority of process %d changed to %d\n", pid, value);
    } else {
        cmd_not_found();
    }
}

void cmdRederr(char *trozos[], int n, char **std_error) {
    int fd;

    if (n == 1) {
        /* se imprime el file descriptor en el que se encuentra
         * direccionada la salida estándar de errores */
        printf("Standard error in file \"%s\"\n", *std_error);
    } else if (n == 2) {
        if (strcmp(trozos[1], "-reset") == 0) {
            /* se restablece el fd de la salida estándar de errores al
             * valor predeterminado (salida por pantalla) */
            if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1) {
                printf("It was not possible to reset standard error\n");
                return;
            }
            if (strcmp(*std_error, "Initial setup") != 0) {
                *std_error = realloc(*std_error, 14);
                strcpy(*std_error, "Initial setup");
                printf("Standard error was reset successfully\n");
            } else {
                printf("Standard error was already with initial setup\n");
            }
        } else {
            /* se abre el archivo indicado en el comando y se guarda su fd */
            if ((fd = open(trozos[1], O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
                print_error();
                return;
            }
            /* con el fd del archivo se restablece el fd de la salida estándar de errores */
            if (dup2(fd, STDERR_FILENO) == -1) {
                print_error();
                return;
            }
            *std_error = realloc(*std_error, strlen(trozos[1]) + 1);
            strcpy(*std_error, trozos[1]);
            printf("Standard error was changed to file \"%s\"\n", trozos[1]);
        }
    } else {
        invalid_nargs();
    }
}

void cmdUid(char *trozos[], int n) {
    uid_t uid;
    char *login;

    if (n == 1) {
        MostrarUidsProceso();
    } else if (n == 2) {
        if (strcmp(trozos[1], "-get") == 0 || strcmp(trozos[1], "-set") == 0) {
            MostrarUidsProceso();
        } else {
            invalid_arg();
        }
    } else if (n == 3) {
        if (strcmp(trozos[1], "-set") == 0) {
            uid = atoi(trozos[2]);
            login = NombreUsuario(uid);
            if (CambiarUidLogin(login) == 0) {
                printf("Uid changed to: %d (%s)\n", uid, login);
            }
        } else {
            invalid_arg();
        }
    } else if (n == 4) {
        if (strcmp(trozos[1], "-set") == 0 && strcmp(trozos[2], "-l") == 0) {
            uid = UidUsuario(trozos[3]);
            if (CambiarUidLogin(trozos[3]) == 0) {
                printf("Uid changed to: %d (%s)\n", uid, trozos[3]);
            }
        } else {
            invalid_arg();
        }
    } else {
        cmd_not_found();
    }
}

void cmdFork(int n) {
    pid_t pid;

    if (n == 1) {
        if ((pid = fork()) == -1) {
            print_error();
        } else if (pid == 0) {
            /* el proceso hijo continúa ejecutándose */
            printf("Running process: %d\n", getpid());
        } else {
            /* el proceso padre espera a que termine el hijo */
            waitpid(pid, NULL, 0);
        }
    } else {
        invalid_nargs();
    }
}

void cmdEjec(char *trozos[], int n) {

    if (n == 1) {
        invalid_nargs();
    } else {
        if (execvp(trozos[1], &trozos[1]) == -1) {
            print_error();
        }
    }
}

void cmdEjecpri(char *trozos[], int n) {

    if (n < 3) {
        invalid_nargs();
    } else {
        if (set_priority(trozos[1], 0) == -1) {
            print_error();
            return;
        }
        if (execvp(trozos[2], &trozos[2]) == -1) {
            print_error();
        }
    }
}

void cmdFg(char *trozos[], int n, int start) {
    pid_t pid;

    if (n == 1 && start == 1) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            /* se ejecuta el programa en el proceso hijo */
            if (execvp(trozos[start], &trozos[start]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* el proceso padre espera a que termine la ejecución del proceso hijo */
            waitpid(pid, NULL, 0);
        }
    }
}

void cmdFgpri(char *trozos[], int n) {
    pid_t pid;

    if (n < 3) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            if (set_priority(trozos[1], 0) == -1) {
                print_error();
                exit(1);
            }
            /* se ejecuta el programa en el proceso hijo */
            if (execvp(trozos[2], &trozos[2]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* el proceso padre espera a que termine la ejecución del proceso hijo */
            waitpid(pid, NULL, 0);
        }
    }
}

void cmdBack(char *trozos[], int n, tListP *ProcessList, int start) {
    int i;
    pid_t pid;
    tItemP item;
    time_t tiempo;
    char date[128], *login;
    uid_t uid;

    if (n == 1 && start == 1) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            /* se ejecuta el programa en el proceso hijo */
            if (execvp(trozos[start], &trozos[start]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* el proceso padre continúa ejecutándose paralelamente al proceso hijo */
        }
        tiempo = time(0);
        strftime(date, 128, "%d/%m/%y %H:%M:%S", localtime(&tiempo));

        item.pid = pid;

        item.time = malloc(strlen(date) + 1);
        strcpy(item.time, date);

        item.end = 0;
        item.state = RUNNING;

        uid = geteuid();
        login = NombreUsuario(uid);
        /* nombre del usuario ejecutando el proceso */
        item.user = malloc(strlen(login) + 1);
        strcpy(item.user, login);

        /* se reserva la memoria necesaria para guardar el comando */
        item.command = malloc(strlen(trozos[start]) + 2);
        strcpy(item.command, trozos[start]);
        strcat(item.command, " ");
        for (i = start + 1; i < n; i++) {
            item.command = realloc(item.command, strlen(item.command) + strlen(trozos[i]) + 2);
            strcat(item.command, trozos[i]);
            strcat(item.command, " ");
        }

        insertItemP(item, ProcessList);
    }
}

void cmdBackpri(char *trozos[], int n, tListP *ProcessList) {
    int i;
    pid_t pid;
    uid_t uid;
    tItemP item;
    time_t tiempo;
    char date[128], *login;

    if (n < 3) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            if (set_priority(trozos[1], 0) == -1) {
                print_error();
                exit(1);
            }
            /* se ejecuta el programa en el proceso hijo */
            if (execvp(trozos[2], &trozos[2]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* el proceso padre continúa ejecutándose paralelamente al proceso hijo */
        }
        tiempo = time(0);
        strftime(date, 128, "%d/%m/%y %H:%M:%S", localtime(&tiempo));

        item.pid = pid;

        item.time = malloc(strlen(date) + 1);
        strcpy(item.time, date);

        item.end = 0;
        item.state = RUNNING;

        uid = geteuid();
        login = NombreUsuario(uid);
        /* nombre del usuario ejecutando el proceso */
        item.user = malloc(strlen(login) + 1);
        strcpy(item.user, login);

        /* se reserva la memoria necesaria para guardar el comando */
        item.command = malloc(strlen(trozos[2]) + 2);
        strcpy(item.command, trozos[2]);
        strcat(item.command, " ");
        for (i = 3; i < n; i++) {
            item.command = realloc(item.command, strlen(item.command) + strlen(trozos[i]) + 2);
            strcat(item.command, trozos[i]);
            strcat(item.command, " ");
        }

        insertItemP(item, ProcessList);
    }
}

void cmdEjecas(char *trozos[], int n) {

    if (n < 3) {
        invalid_nargs();
    } else {
        if (CambiarUidLogin(trozos[1]) == 0) {
            if (execvp(trozos[2], &trozos[2]) == -1) {
                print_error();
            }
        }
    }
}

void cmdFgas(char *trozos[], int n) {
    pid_t pid;

    if (n < 3) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            /* proceso hijo */
            if (CambiarUidLogin(trozos[1]) == -1) {
                /* error al cambiar el usuario */
                exit(1);
            }
            if (execvp(trozos[2], &trozos[2]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* proceso padre */
            waitpid(pid, NULL, 0);
        }
    }
}

void cmdBgas(char *trozos[], int n, tListP *ProcessList) {
    int i;
    pid_t pid;
    tItemP item;
    time_t tiempo;
    char date[128];

    if (n < 3) {
        invalid_nargs();
    } else {
        if ((pid = fork()) == -1) {
            print_error();
            return;
        }
        if (pid == 0) {
            if (CambiarUidLogin(trozos[1]) == -1) {
                /* error al cambiar el usuario */
                exit(1);
            }
            /* se ejecuta el programa en el proceso hijo */
            if (execvp(trozos[2], &trozos[2]) == -1) {
                print_error();
                exit(1);
            }
        } else {
            /* el proceso padre continúa ejecutándose paralelamente al proceso hijo */
        }
        tiempo = time(0);
        strftime(date, 128, "%d/%m/%y %H:%M:%S", localtime(&tiempo));

        item.pid = pid;

        /* nombre del usuario ejecutando el proceso */
        item.user = malloc(strlen(trozos[1]) + 1);
        strcpy(item.user, trozos[1]);

        item.time = malloc(strlen(date) + 1);
        strcpy(item.time, date);

        item.end = 0;
        item.state = RUNNING;

        /* se reserva la memoria necesaria para guardar el comando */
        item.command = malloc(strlen(trozos[2]) + 2);
        strcpy(item.command, trozos[2]);
        strcat(item.command, " ");
        for (i = 3; i < n; i++) {
            item.command = realloc(item.command, strlen(item.command) + strlen(trozos[i]) + 2);
            strcat(item.command, trozos[i]);
            strcat(item.command, " ");
        }

        insertItemP(item, ProcessList);
    }
}

void cmdListjobs(int n, tListP *ProcessList) {
    tPosP pos;
    tItemP item;

    if (n != 1) {
        invalid_nargs();
        return;
    }

    for (pos = firstP(*ProcessList); pos != LNULL; pos = nextP(pos, *ProcessList)) {
        item = getItemP(pos, *ProcessList);
        item = update_status(item, 1);
        updateItem(item, pos, ProcessList);
        print_job(item);
    }
}

void cmdJob(char *trozos[], int n, tListP *ProcessList) {
    pid_t pid;
    tPosP pos;
    tItemP item;

    if (n == 1) {
        cmdListjobs(1, ProcessList);
    } else if (n == 2) {
        pid = atoi(trozos[1]);
        if ((pos = findPosP(pid, *ProcessList)) == LNULL) {
            cmdListjobs(1, ProcessList);
        } else {
            item = getItemP(pos, *ProcessList);
            item = update_status(item, 1);
            updateItem(item, pos, ProcessList);
            print_job(item);
        }
    } else if (n == 3) {
        pid = atoi(trozos[2]);
        if (strcmp(trozos[1], "-fg") != 0) {
            invalid_arg();
            return;
        }

        if ((pos = findPosP(pid, *ProcessList)) == LNULL) {
            printf("Process %d is not in background processes list\n", pid);
            return;
        }

        item = getItemP(pos, *ProcessList);

        /* el proceso se pasa a foreground */
        item = update_status(item, 0);
        print_end(item);
        free(item.command);
        free(item.time);
        free(item.user);
        deleteItemP(pos, ProcessList);
    } else {
        invalid_nargs();
    }
}

void cmdBorrarjobs(char *trozos[], int n, tListP *ProcessList) {
    int nOptions;
    bool opTerm = false, opSig = false, opAll = false, opClear = false;
    tPosP pos, aux;
    tItemP item;

    checkoptions_borrar(trozos, n, &opTerm, &opSig, &opAll, &opClear);

    nOptions = opTerm + opSig + opAll + opClear;

    if (isEmptyListP(*ProcessList)) {
        empty_list();
        return;
    }

    if (n != nOptions + 1) {
        invalid_nargs();
        return;
    }

    if (opClear) {
        liberarProcessCommand(*ProcessList);
        clearListP(ProcessList);
        printf("List is now empty\n");
        return;
    }

    if (opAll) {
        opTerm = true;
        opSig = true;
    }

    for (pos = firstP(*ProcessList); pos != LNULL; pos = aux) {
        aux = nextP(pos, *ProcessList);
        item = getItemP(pos, *ProcessList);
        item = update_status(item, 2);
        if (opTerm) {
            if (item.state == EXITED) {
                free(item.command);
                free(item.time);
                free(item.user);
                deleteItemP(pos, ProcessList);
            }
        }
        if (opSig) {
            if (item.state == KILLED) {
                free(item.command);
                free(item.time);
                free(item.user);
                deleteItemP(pos, ProcessList);
            }
        }
    }
}

void cmdLinux(char *trozos[], int n, tListP *ProcessList) {

    if (strcmp(trozos[n - 1], "&") == 0) {
        trozos[--n] = NULL;
        cmdBack(trozos, n, ProcessList, 0);
    } else {
        cmdFg(trozos, n, 0);
    }
}