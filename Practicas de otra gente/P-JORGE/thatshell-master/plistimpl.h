/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include<stdbool.h>
#include<time.h>
#include<string.h> 
#include<stdio.h>


typedef struct proccess_node * posPL;

typedef struct pinfo{
    int pid;
    char* caller;
    int priority;
    time_t started_time;
    int end_status;
}pinfo;

typedef struct proccess_node {
    posPL next;
    struct pinfo info;
}proccess_node;


typedef posPL plist;

plist create_plist();
pinfo createInfo(int pid, char* caller);
bool insertItem(struct pinfo info, plist *pl);
void deleteAtPosition(posPL pos, plist *pl);
void deleteList(plist *pl);
bool isEmptyList(plist pl);
posPL last(plist pl);
void showList(plist pl);
void printInfo(struct pinfo info);
void removeByStatus(plist* pl, int signal);
posPL findItemByPID(plist pl, int pid);
int check_status(int);