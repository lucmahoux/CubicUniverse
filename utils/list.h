#ifndef CUB_UTILS_LIST_H
#define CUB_UTILS_LIST_H

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BASE_CAPACITY 8

typedef struct cubList {
    void* data;
    size_t len;
    size_t capacity;
    size_t memb_size;
} cubList;

cubList* cub_utils_list(size_t memb_size);

void* cub_utils_list_get(cubList* list, size_t i);

void cub_utils_list_set(cubList* list, void* elt, size_t i);

void cub_utils_list_check_double_capacity(cubList* list);

void cub_utils_list_append(cubList* list, void* elt);

void cub_utils_list_insert(cubList* list, void* elt, size_t i);

void cub_utils_list_remove(cubList* list, size_t i);

void cub_utils_list_pop(cubList* list, size_t i, void* removed);

void cub_utils_list_clear(cubList* list);

void cub_utils_list_free(cubList* list);

#endif
