#ifndef CUB_UTILS_LIST_H
#define CUB_UTILS_LIST_H

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BASE_CAPACITY 8

typedef void (*reallocFunc) (void*);

typedef struct cubList {
    void* data;
    size_t len;
    size_t capacity;
    size_t memb_size;
    reallocFunc realloc_fun;
} cubList;

typedef enum reallocType {
    REALLOC_DOUBLE,
    REALLOC_PLUS_ONE
} reallocType;

/* Creates a list to store elements of size 'memb_size' and initializes
 * the capacity of the list to 'capacity' if it is not 0 or to BASE_CAPACITY.
 * The realloc type 'type' defines how the list will manage the reallocs when
 * its capacity is exceeded. */
cubList* cub_list(size_t memb_size, reallocType type,
                        size_t capacity);

/* Returns a pointer to the element at index 'i' in the list */
void* cub_list_get(cubList* list, size_t i);

/* Set the element at index 'i' in the list to 'elt' */
void cub_list_set(cubList* list, void* elt, size_t i);

/* Verify if the length of the list does not exceed or equals its capacity.
 * If so, a realloc is performed & the capacity is doubled. */
void cub_list_check_double_capacity(cubList* list);

/* If the length of the list exceeds or is equal to its capacity,
 * a realloc is performed & the capacity is incremented by one */
void cub_list_check_plus_one_realloc(cubList* list);

/* Adds the element 'elt' at the end of the list */
void cub_list_append(cubList* list, void* elt);

void cub_list_insert(cubList* list, void* elt, size_t i);

/* Removes the element at index 'i' and preserve the order of the other
 * elements in the list */
void cub_list_remove_ordered(cubList* list, size_t i);

/* Removes the element at index 'i', the last element of the list is moved
 * to this position 'i', this the order is not preserved. */
void cub_list_remove(cubList* list, size_t i);

/* Pops the element of the list at index and copy it at the position pointed
 * to by 'removed'. It uses the unordered remove approach. */
void cub_list_pop(cubList* list, size_t i, void* removed);

/* Reset the length of the list to 0 */
void cub_list_clear(cubList* list);

/* Free the list*/
void cub_list_free(cubList* list);

#endif
