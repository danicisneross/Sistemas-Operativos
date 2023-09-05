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
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>

#include "aux_proc.h"
#include "prints.h"

/****************************** SEÑALES ******************************/

static struct SEN sigstrnum[] = {
        {"HUP", SIGHUP},
        {"INT", SIGINT},
        {"QUIT", SIGQUIT},
        {"ILL", SIGILL},
        {"TRAP", SIGTRAP},
        {"ABRT", SIGABRT},
        {"IOT", SIGIOT},
        {"BUS", SIGBUS},
        {"FPE", SIGFPE},
        {"KILL", SIGKILL},
        {"USR1", SIGUSR1},
        {"SEGV", SIGSEGV},
        {"USR2", SIGUSR2},
        {"PIPE", SIGPIPE},
        {"ALRM", SIGALRM},
        {"TERM", SIGTERM},
        {"CHLD", SIGCHLD},
        {"CONT", SIGCONT},
        {"STOP", SIGSTOP},
        {"TSTP", SIGTSTP},
        {"TTIN", SIGTTIN},
        {"TTOU", SIGTTOU},
        {"URG", SIGURG},
        {"XCPU", SIGXCPU},
        {"XFSZ", SIGXFSZ},
        {"VTALRM", SIGVTALRM},
        {"PROF", SIGPROF},
        {"WINCH", SIGWINCH},
        {"IO", SIGIO},
        {"SYS", SIGSYS},

/*senales que no hay en todas partes*/
#ifdef SIGPOLL
        {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
        {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
        {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
        {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
        {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
        {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
        {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
        {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
        {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
        {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
        {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
        {"WAITING", SIGWAITING},
#endif
        {NULL, -1},
};  /*fin array sigstrnum */

char *NombreSenal(int sen) {    /*devuelve el nombre senal a partir de la senal*/
    /* para sitios donde no hay sig2str*/
    int i;

    for (i = 0; sigstrnum[i].nombre != NULL; i++) {
        if (sen == sigstrnum[i].senal) {
            return sigstrnum[i].nombre;
        }
    }

    return ("SIGUNKNOWN");
}

/*******************************************************/


#define MAXVAR 255

void liberarEnvironment(tListE EnvironmentList) {
    tPosE pos;

    for (pos = firstE(EnvironmentList); pos != LNULL; pos = nextE(pos, EnvironmentList)) {
        free(getItemE(pos, EnvironmentList).name);
    }
}

int BuscarVariable(char *var, char *e[]) {
    int pos = 0;
    char aux[MAXVAR];

    strcpy(aux, var);
    strcat(aux, "=");

    while (e[pos] != NULL) {
        if (!strncmp(e[pos], aux, strlen(aux))) {
            return (pos);
        } else {
            pos++;
        }
    }

    errno = ENOENT; /* no hay tal variable */
    return (-1);
}

char *NombreUsuario(uid_t uid) {
    struct passwd *p;

    if ((p = getpwuid(uid)) == NULL) {
        return ("??????");
    }

    return p->pw_name;
}

uid_t UidUsuario(char *nombre) {
    struct passwd *p;

    if ((p = getpwnam(nombre)) == NULL) {
        return (uid_t) -1;
    }

    return p->pw_uid;
}

void MostrarUidsProceso(void) {
    uid_t real = getuid(), efec = geteuid();

    printf("Real credential: %d (%s)\n", real, NombreUsuario(real));
    printf("Effective credential: %d (%s)\n", efec, NombreUsuario(efec));
}

int CambiarUidLogin(char *login) {
    uid_t uid;

    if ((uid = UidUsuario(login)) == (uid_t) -1) {
        printf("Invalid login: %s\n", login);
        return -1;
    }

    if (setuid(uid) == -1) {
        print_error();
        return -1;
    }
    return 0;
}

int set_priority(char *val, pid_t pid) {
    int value;

    /* se intenta convertir el argumento
     * del valor de prioridad a número */
    value = atoi(val);
    /* se cambia la prioridad del proceso con el pid introducido
     * por comando al valor correspondiente */
    if (setpriority(PRIO_PROCESS, pid, value) == -1) {
        return -1;
    }

    return value;
}

void liberarProcessCommand(tListP ProcessList) {
    tPosP pos;
    tItemP item;

    for (pos = firstP(ProcessList); pos != LNULL; pos = nextP(pos, ProcessList)) {
        item = getItemP(pos, ProcessList);
        free(item.command);
        free(item.time);
        free(item.user);
    }
}

char *check_status(int status) {

    switch (status) {
        case EXITED:
            return "EXITED";
        case RUNNING:
            return "RUNNING";
        case STOPPED:
            return "STOPPED";
        case KILLED:
            return "KILLED";
        default:
            return "UNKNOWN";
    }
}

tItemP update_status(tItemP item, int options) {
    int status;

    if (options != 0) {
        options = WNOHANG | WUNTRACED | WCONTINUED;
    }

    if (waitpid(item.pid, &status, options) == item.pid) {
        /* el estado del proceso ha cambiado desde la última revisión */
        if (WIFEXITED(status)) {
            item.state = EXITED;
            item.end = WEXITSTATUS(status);
        } else if (WIFCONTINUED(status)) {
            item.state = RUNNING;
        } else if (WIFSTOPPED(status)) {
            item.state = STOPPED;
            item.end = WSTOPSIG(status);
        } else if (WIFSIGNALED(status)) {
            item.state = KILLED;
            item.end = WTERMSIG(status);
        } else {
            item.state = UNKNOWN;
        }
    }

    return item;
}

void checkoptions_borrar(char *trozos[], int n, bool *opTerm, bool *opSig, bool *opAll, bool *opClear) {

    /* se identifican las opciones escogidas por el
     * usuario para los seis primeros parámetros: */
    for (int i = 1; i < n; i++) {
        if (trozos[i] == NULL) {
            break;
        } else if (strcmp(trozos[i], "-term") == 0) {
            *opTerm = true;
        } else if (strcmp(trozos[i], "-sig") == 0) {
            *opSig = true;
        } else if (strcmp(trozos[i], "-all") == 0) {
            *opAll = true;
        } else if (strcmp(trozos[i], "-clear") == 0) {
            *opClear = true;
        }
    }
}
