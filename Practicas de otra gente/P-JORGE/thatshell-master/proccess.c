/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "proccess.h"
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <wait.h>

#define GETPRIORITY 20
#define SETPRIORITY 21
#define GETUID 22
#define SETUID 23
#define FORK 24
#define EXECUTE 25
#define FOREGROUND 26
#define BACKGROUND 27
#define RUNAS 28
#define EXECUTEAS 29
#define LISTPROCS 30
#define PROC 31
#define DELETEPROCS 32
#define EXITED_NORMALY 1111
#define EXITED_SIGNAL 1000


void getpriority_cmd(char *pid, int flag_pid);

void setpriority_cmd(char **tokens, int ntokens);

void getuid_cmd();

void setuid_cmd(char **tokens);

void fork_cmd();

void execute_cmd(char **tokens, int ntokens);

void foreground_cmd(char **tokens, int ntokens);

void background_cmd(char **tokens, int ntokens, plist pl);

void runas_cmd(char **tokens, int ntokens, plist pl);

void executeas_cmd(char **tokens, int ntokens);

void listprocs_cmd(plist pl);

void proc_cmd(char **tokens, int ntokens, plist pl);

void deleteprocs_cmd(char *token, plist pl);


/** COPIED FUNCTIONS **/


char *NombreUsuario(uid_t uid) {
    struct passwd *p;
    if ((p = getpwuid(uid)) == NULL)
        return (" ??????");
    return p->pw_name;
}

uid_t UidUsuario(char *nombre) {
    struct passwd *p;
    if ((p = getpwnam(nombre)) == NULL)
        return (uid_t) -1;
    return p->pw_uid;
}

void Cmd_getuid(char *tr[]) {
    uid_t real = getuid(), efec = geteuid();
    printf("Credencial real: %d, (%s)\n", real, NombreUsuario(real));
    printf("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario(efec));
}

void Cmd_setuid(char *tr[]) {
    uid_t uid;
    int u;
    if (tr[0] == NULL || (!strcmp(tr[0], "-l") && tr[1] == NULL)) {
        Cmd_getuid(tr);
        return;
    }
    if (!strcmp(tr[0], "-l")) {
        if ((uid = UidUsuario(tr[1])) == (uid_t) -1) {
            printf("Usuario no existente %s\n", tr[1]);
            return;
        }
    } else if ((uid = (uid_t) ((u = atoi(tr[0])) < 0) ? -1 : u) == (uid_t) -1) {
        printf("Valor no valido de la credencial %s\n", tr[0]);
        return;
    }
    if (setuid(uid) == -1)
        printf("Imposible cambiar credencial: %s\n", strerror(errno));
}

int proccess_router(char **tokens, int ntokens, int cmd_index, plist pl) {
    switch (cmd_index) {
        case GETPRIORITY:
            if (ntokens > 1)
                printf("Error, check the arguments.");
            else
                getpriority_cmd(tokens[0], ntokens);
            break;
        case SETPRIORITY:
            if (ntokens > 2)
                printf("Error, check the arguments.");
            else
                setpriority_cmd(tokens, ntokens);
            break;
        case GETUID:
            if (ntokens)
                printf("Error, check the arguments.");
            else
                getuid_cmd();
            break;
        case SETUID:
            if (ntokens == 0 || ntokens > 2)
                printf("Error, check the arguments.");
            else
                setuid_cmd(tokens);
            break;
        case FORK:
            if (ntokens)
                printf("Error, check the arguments.");
            else
                fork_cmd();
            break;
        case EXECUTE:
            if (ntokens == 0)
                printf("Error, check the arguments.");
            else
                execute_cmd(tokens, ntokens);
            break;
        case FOREGROUND:
            if (ntokens == 0)
                printf("Error, check the arguments.");
            else
                foreground_cmd(tokens, ntokens);
            break;
        case BACKGROUND:
            if (ntokens == 0)
                printf("Error, check the arguments.");
            else
                background_cmd(tokens, ntokens, pl);
            break;
        case RUNAS:
            if (ntokens < 2)
                printf("Error, check the arguments.");
            else
                runas_cmd(tokens, ntokens, pl);
            break;
        case EXECUTEAS:
            if (ntokens < 2)
                printf("Error, check the arguments.");
            else
                executeas_cmd(tokens, ntokens);
            break;
        case LISTPROCS:
            if (ntokens)
                printf("Error, check the arguments.");
            else
                listprocs_cmd(pl);
            break;
        case PROC:
            if (ntokens == 0 || ntokens > 2)
                printf("Error, check the arguments.");
            else
                proc_cmd(tokens, ntokens, pl);
            break;
        case DELETEPROCS:
            if (ntokens != 1)
                printf("Error, check the arguments.");
            else
                deleteprocs_cmd(tokens[0],pl);
            break;
    }
    return 0;
}

void exec_default(char** tokens, int ntokens, plist pl) {
    int back_flag = 0;
    if (tokens[ntokens - 1][0] == '&') {
        back_flag = 1;
        tokens[ntokens - 1] = NULL;
        ntokens--;
    }
    pid_t pid = fork();
    if (pid < 0) {
        printf("something went wrong :(\n");
        return;
    }
    if (pid > 0) {
        if (back_flag) {
            char* caller= (char*) malloc(sizeof(char)*90);
            strcpy(caller,tokens[0]);
            for(int i =1 ; i<ntokens ; i++){

                if(strstr(tokens[i],"@")==NULL) {
                    strcat(caller, tokens[i]);
                    strcat(caller, " ");
                }
            }
            insertItem(createInfo(pid,caller),&pl);
            sleep(1);
        }else
            waitpid(pid, NULL, 0);
    }else
        execute_cmd(tokens, ntokens);
}

void deleteprocs_cmd(char *token, plist pl){
    posPL pos;
    pos = pl;
    if(isEmptyList(pl)) {
        printf("empty list, nothing to remove");
        return;
    } else {
        if (0 == strcmp(token, "-term")) {
            while(pos!=NULL){
                int newstatus = check_status(pos->info.pid);
                if (newstatus != -1)
                    pos->info.end_status = newstatus;
                if(pos->info.end_status == 2){
                    deleteAtPosition(pos,&pl);
                }
                pos=pos->next;
            }
        }
        if (0 == strcmp(token, "-sig")) {
            while(pos!=NULL){
                int newstatus = check_status(pos->info.pid);
                if (newstatus != -1)
                    pos->info.end_status = newstatus;
                if(pos->info.end_status==1){
                    deleteAtPosition(pos,&pl);
                }
                pos=pos->next;
            }
        }
    }
}

void proc_cmd(char **tokens, int ntokens, plist pl){
    posPL pos;
    if(ntokens==1){
        pos=findItemByPID(pl,atoi(tokens[0]));
        if (pos==NULL){
            printf("this PID doesnt exit :(\n");
            showList(pl);
            return;
        }
        printInfo(pos->info);
    }else{
        if(strcmp(tokens[0],"-fg")==0){
            //TODO
        }else{
            printf("check the params!");
        }
    }
}

void listprocs_cmd(plist pl) {
    showList(pl);
}

void executeas_cmd(char **tokens, int ntokens) {
    uid_t uid;
    if ((uid = UidUsuario(tokens[0])) == (uid_t) -1) {
        printf("Usuario no existente %s\n", tokens[0]);
        return;
    }
    if (setuid(uid) == -1) {
        printf("Imposible cambiar credencial: %s\n", strerror(errno));
        return;
    }
    execute_cmd(tokens + 1, ntokens - 1);
}

void runas_cmd(char **tokens, int ntokens, plist pl) {
    int back_flag = 0;
    if (tokens[ntokens - 1][0] == '&') {
        back_flag = 1;
        tokens[ntokens - 1] = NULL;
        ntokens--;
    }
    pid_t pid = fork();
    if (pid < 0) {
        printf("something went wrong :(\n");
        return;
    }
    if (pid > 0) {
        if (back_flag) {//add to list
            char* caller= (char*) malloc(sizeof(char)*90);
            strcpy(caller,tokens[0]);
            for(int i =1 ; i<ntokens ; i++){

                if(strstr(tokens[i],"@")==NULL) {
                    strcat(caller, tokens[i]);
                    strcat(caller, " ");
                }
            }
            insertItem(createInfo(pid,caller),&pl);
            sleep(1);
        }else
            waitpid(pid, NULL, 0);
    } else {
        uid_t uid;
        if ((uid = UidUsuario(tokens[0])) == (uid_t) -1) {
            printf("Usuario no existente %s\n", tokens[0]);
            exit(-1);
        }
        if (setuid(uid) == -1) {
            printf("Imposible cambiar credencial: %s\n", strerror(errno));
            exit(-1);
        }
        execute_cmd(tokens + 1, ntokens - 1);
    }
}

void background_cmd(char **tokens, int ntokens, plist pl) {
    pid_t pid = fork();
    if (pid < 0) {
        printf("something went wrong :(\n");
        return;
    }
    if (pid == 0)
        execute_cmd(tokens, ntokens);
    char* caller= (char*) malloc(sizeof(char)*90);
    strcpy(caller,tokens[0]);
    for(int i =1 ; i<ntokens ; i++){
        if(strstr(tokens[i],"@")==NULL) {
            strcat(caller, tokens[i]);
            strcat(caller, " ");
        }
    }
    insertItem(createInfo(pid,caller),&pl);
    sleep(1);
}

void foreground_cmd(char **tokens, int ntokens) {
    pid_t pid = fork();
    if (pid < 0) {
        printf("something went wrong :(\n");
        return;
    }
    if (pid > 0)
        waitpid(pid, NULL, 0);
    else
        execute_cmd(tokens, ntokens);
}

void execute_cmd(char **tokens, int ntokens) {
    if (tokens[ntokens - 1][0] == '@') {
        setpriority_cmd(tokens + ntokens - 1, 1);
        tokens[ntokens - 1] = NULL;
    }
    execvp(tokens[0], tokens);
    printf("I'm not a magician, try a command that exists.\n");
    exit(1);
}

void fork_cmd() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("something went wrong :(\n");
        return;
    }
    if (pid > 0)
        waitpid(pid, NULL, 0);
}

void setuid_cmd(char **tokens) {
    Cmd_setuid(tokens);
}

void getuid_cmd() {
    uid_t real = getuid();
    uid_t eff = geteuid();
    printf("real uid: %d (%s)\n", real, NombreUsuario(real));
    printf("eff. uid: %d (%s)\n", eff, NombreUsuario(eff));
}

void setpriority_cmd(char **tokens, int ntokens) {
    int i_pid, prio, which = PRIO_PROCESS;
    if (ntokens == 0) {
        getpriority_cmd(NULL, 0);
        return;
    } else if (ntokens == 1) {
        i_pid = getpid();
        if (tokens[0][0] == '@')
            prio = atoi(tokens[0] + 1);
        else
            prio = atoi(tokens[0]);
    } else {
        i_pid = atoi(tokens[0]);
        prio = atoi(tokens[1]);
    }
    int err = setpriority(which, i_pid, prio);
    if (errno == 0 && err == 0)
        printf("priority changed pid=%d to prio=%d\n", i_pid, prio);
    else
        printf("priority not changed: %s\n", strerror(errno));
}

void getpriority_cmd(char *pid, int flag_pid) {
    int i_pid, which = PRIO_PROCESS;
    if (flag_pid)
        i_pid = atoi(pid);
    else
        i_pid = getpid();
    int prio = getpriority(which, i_pid);
    if (prio == -1 && errno != 0) {
        printf("something went wrong\n");
        return;
    }
    printf("priority of pid=%d is=%d\n", i_pid, prio);
}