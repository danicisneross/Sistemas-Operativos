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

#include "hist_list.h"
#include "mem_list.h"
#include "bg_list.h"
#include "file.h"

extern char **environ;

/**
 * Function: prompt
 * ----------------------
 * Prints the prompt "#> ".
 *
 * @return void.
 */
void prompt() {
    printf("#> ");
}

/**
 * Function: input
 * ----------------------
 * Saves the string entered by standard input in
 * command with a max size of MAX_CHARS.
 *
 * @param command string initialized
 * @return void.
 */
void input(char *command) {
    fgets(command, MAX_CHARS, stdin);
}

/**
 * Function: guardarComando
 * ----------------------
 * The current command is added lastC in a command-only list.
 *
 * @param L list of type tListC (include in library "Hist_List.h")
 * @param command string with the command entered by standard input
 * @param commandNumber integer counting number of commands
 * @return void.
 */
void guardarComando(tListC *L, char *command, int *commandNumber) {
    tItemC item;

    /* se reserva memoria para guardar el comando en la lista */
    item.CommandName = (char *) malloc(strlen(command) + 1);

    /* se copia el comando en una variable de tipo tItem */
    strcpy(item.CommandName, command);

    /* se incrementa el contador de comandos
     * y se añade el item a la lista */
    *commandNumber = *commandNumber + 1;
    insertItemC(item, L);
}

int main(int argc, char *argv[], char *env[]) {
    int commandNumber = 1; /* se inicializa el contador de comandos */
    char command[MAX_CHARS]; /* se declara un array de chars de tamaño MAX_CHARS */
    char *std_error;
    tListC CommandList; /* se declara una lista para guardar los comandos */
    tListM MemoryList; /* se declara una lista para guardar los comandos */
    tListE EnvironmentList; /* se declara una lista para guardar variables de entorno */
    tListP ProcessList; /* se declara una lista para guardar los procesos en background */
    bool exit = false; /* se inicializa una variable bool para saber cuando cerrar el programa */

    std_error = malloc(14);
    strcpy(std_error, "Initial setup");

    /* se crea una lista vacía para guardar
     * los comandos en orden de entrada */
    createEmptyListC(&CommandList);

    /* se crea una lista vacía para guardar
     * los datos de memoria necesarios */
    createEmptyListM(&MemoryList);

    /* se crea una lista vacía para guardar las direcciones
     * de las variables de entorno modificadas */
    createEmptyListE(&EnvironmentList);

    /* se crea una lista vacía para guardar los procesos
     * que se fueron creando en background */
    createEmptyListP(&ProcessList);

    /* bucle del shell para introducir los
     * comandos hasta que se cierre el programa */
    while (!exit) {
        prompt();
        input(command);
        guardarComando(&CommandList, command, &commandNumber);
        /* procedimiento encargado de procesar correctamente
         * cada comando (incluido en la librería "commands.h") */
        procesarEntrada(command, &exit, &CommandList, &commandNumber, &MemoryList,
                        &EnvironmentList, env, environ, &std_error, &ProcessList);
    }

    return 0;
}