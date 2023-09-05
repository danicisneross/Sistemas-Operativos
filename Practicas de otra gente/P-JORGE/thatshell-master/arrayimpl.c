/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include <stdlib.h>
#include <memory.h>
#include "arrayimpl.h"


/**
 * Defines the historic struct
 *
 * list: array of 4096 elements that are references to chars
 * n_elem: number of elements in the array
 *
 */
struct historic_struct {
    char* list[4096];
    int n_elem;
};

/**
 * Function: create_historic
 * --------------------------
 * Allocates the space for the struct in memory and returns the pointer to it,
 * and initialize the
 *
 * returns: returns the pointer to the created struct
 */
historic create_historic() {
    historic h = malloc(sizeof(struct historic_struct));
    h->n_elem = 0;
    return h;
}

/**
 * Function: remove_historic
 * ------------------
 * Frees all the actual pointers contained in the array and
 * frees the actual struct
 *
 * h: the address to the struct
 *
 * returns: void
 */
void remove_historic(historic h) {
    for (int i = 0; i < h->n_elem; ++i) {
        free(h->list[i]);
    }
    free(h);
}


/**
 * Function: insert_in_historic
 * ------------------
 * Insert the received string in the last position of the array
 *
 * h: the address to the struct
 * e: the address to the string to insert
 *
 * returns: void
 */
void insert_in_historic(historic h, char* e) {
    h->list[h->n_elem] = strdup(e);
    h->n_elem++;
}


/**
 * Function: remove_from_historic
 * ------------------
 * Removes the element in the position received by moving all
 * next element to n - 1; frees the removed address and updates the
 * total number of elements in the array
 *
 * h: the address to the struct
 * n: the position of the element to delete
 *
 * returns: void
 */
void remove_from_historic(historic h, int n) {
    if (n >= h->n_elem) return;
    free(h->list[n]);
    for (int i = n; i < h->n_elem; ++i) {
        h->list[i] = h->list[i + 1];
    }
    h->n_elem--;
}


/**
 * Function: read_from_historic
 * ------------------
 * Reads in the array and returns the element in the given position
 *
 * h: the address to the struct
 * n: the position to read in the array
 *
 * returns: the address of the string on n position,
 *          if out of index, NULL
 */
char* read_from_historic(historic h, int n) {
    if (n >= h->n_elem) return NULL;
    return h->list[n];
}


/**
 * Function: n_elements_in_historic
 * ------------------
 * Gives the actual number of elements the array
 *
 * h: the address to the struct
 *
 * returns: the number of elements in the array
 */
int n_elements_in_historic(historic h) {
    return h->n_elem;
}


