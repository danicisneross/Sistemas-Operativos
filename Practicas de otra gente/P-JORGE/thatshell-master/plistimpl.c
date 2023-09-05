/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include <stdlib.h>
#include "plistimpl.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <wait.h>
#include <errno.h>

#define NOCHANGE -1
#define SIGN 1
#define NORM 2
#define CONT 3
#define STOP 4

plist create_plist() {
    plist h = malloc(sizeof(plist));
    h->info.pid = -1;
    return h;
}

bool createNode(posPL *pos) {
    *pos = malloc(sizeof(struct proccess_node));
    return (*pos != NULL);
}


bool insertItem(pinfo info, plist *pl) {
    posPL q, r;
    if (!createNode(&q))
        return false;
    else {
        q->info = info;
        q->next = NULL;
        if (isEmptyList(*pl)) {
            *pl = q;
        } else {
            r = last(*pl);
            r->next = q;
        }
        return true;
    }
}


posPL findItemByPID(plist pl, int pid) {
    posPL pos;
    pos = pl;
    while (pos != NULL) {
        if (pos->info.pid == pid) {
            return pos;
        }
        pos = pos->next;
    }
    pos = NULL;
    return pos;
}


void deleteAtPosition(posPL pos, plist *pl) {
    posPL q;
    if (pos == *pl)
        *pl = (*pl)->next;

    else if (pos->next == NULL) {
        for (q = *pl; q->next != pos; q = q->next);
        q->next = NULL;

    } else {
        q = pos->next;
        pos->info = q->info;
        pos->next = q->next;
        pos = q;
    }
    free(pos);// se libera el nodo pos};
}


void deleteList(plist *pl) {
    posPL p;
    while (*pl != NULL) {
        p = *pl;
        *pl = (*pl)->next;
        free(p);
    }
}

bool isEmptyList(plist pl) {
    return pl == NULL;
}


posPL last(plist pl) {
    posPL q;
    for (q = pl; q->next != NULL; q = q->next);
    return q;
}


pinfo createInfo(int pid, char *caller) {
    struct pinfo info;
    info.pid = pid;
    info.caller = caller;
    info.priority = 0;
    info.started_time = time(0);
    info.end_status = CONT;
    return info;
}


char *date(time_t time) {
    char *dateAndTime;
    dateAndTime = (char *) malloc(sizeof(char) * 49);
    struct tm *tm;
    tm = localtime(&time);
    strftime(dateAndTime, 80, "%d/%m/%Y at %H:%M:%S", tm);
    return dateAndTime;
}

void printInfo(pinfo info) {
    int which = PRIO_PROCESS;
    char *end;
    switch (info.end_status) {
        case SIGN:
            end = "Sign";
            break;
        case NORM:
            end = "Term";
            break;
        case STOP:
            end = "Stopped";
            break;
        case CONT:
            end = "Active";
            break;
        default:
            end = "Error?";
    }
    int prio = getpriority(which, info.pid);
    if (prio != -1) {
        info.priority = prio;
    }
    char *dateAndTime = date(info.started_time);
    printf("PID:%d  priority:%d started:%s end:%s %s\n", info.pid,
           info.priority, dateAndTime, end, info.caller);
    free(dateAndTime);
}

int check_status(int pid) {
    int status;
    int npid = waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (npid == pid) {
        if (WIFSIGNALED(status))
            return SIGN;
        if (WIFEXITED(status))
            return NORM;
        if (WIFCONTINUED(status))
            return CONT;
        if (WIFSTOPPED(status))
            return STOP;
    }
    return NOCHANGE;
}

void showList(plist pl) {
    posPL p;
    p = pl;
    if (isEmptyList(pl))
        printf("Proccess list is empty :(\n");
    while (p != NULL) {
        if (p->info.pid != -1) {
            int newstatus = check_status(p->info.pid);
            if (newstatus != NOCHANGE)
                p->info.end_status = newstatus;
            printInfo(p->info);
        }
        p = p->next;
    }
}