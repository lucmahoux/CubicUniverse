#include "utils/list.h"

void* cub_list_get(cubList* list, size_t i) {
    return (char*) list->data + i * list->memb_size;
}

cubList* cub_list(size_t memb_size, reallocType type,
                        size_t capacity) {
    cubList* list = malloc(sizeof(cubList));
    if (!list)
        errx(1, "cub_list: malloc failed!");
    list->data = malloc(memb_size * BASE_CAPACITY);
    if (!list->data) {
        free(list);
        errx(1, "cub_list: malloc failed!");
    }
    list->capacity = capacity ? capacity : BASE_CAPACITY;
    list->memb_size = memb_size;
    list->len = 0;
    list->realloc_fun = type == REALLOC_DOUBLE
                        ? (void(*)(void*))cub_list_check_double_capacity
                        : (void(*)(void*))cub_list_check_plus_one_realloc;
    return list;
}

void cub_list_set(cubList* list, void* elt, size_t i) {
    if (i >= list->len)
        errx(1, "cub_list_set: list index out of range!");
    memcpy(cub_list_get(list, i), elt, list->memb_size);
}

void cub_list_check_double_capacity(cubList* list) {
    if (list->len >= list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data, list->memb_size * list->capacity);
        if (!list->data)
            errx(1, "cub_list_check_double_capacity: realloc failed!");
    }
}

void cub_list_check_plus_one_realloc(cubList* list) {
    if (list->len >= list->capacity) {
        ++list->capacity;
        list->data = realloc(list->data, list->memb_size * list->capacity);
        if (!list->data)
            errx(1, "cub_list_check_plus_on: realloc failed!");
    }
}

void cub_list_append(cubList* list, void* elt) {
    list->realloc_fun(list);
    memcpy(cub_list_get(list, list->len), elt, list->memb_size);
    ++list->len;
}

void cub_list_insert(cubList* list, void* elt, size_t i) {
    if (i >= list->len + 1)
        printf("cub_list_insert: list index out of range\n!");
    else {
        list->realloc_fun(list);
        for (size_t k = list->len; k > i; --k)
            memcpy( cub_list_get(list, k),
                    cub_list_get(list, k - 1),
                    list->memb_size );
        memcpy(cub_list_get(list, i), elt, list->memb_size);
        ++list->len;
    }
}

void cub_list_remove_ordered(cubList* list, size_t i) {
    if (i >= list->len)
        printf("cub_list_remove_sorted: index {%lu} out of range!\n", i);
    else {
        for (size_t k = i + 1; k < list->len; ++k)
            memcpy( cub_list_get(list, k - 1),
                    cub_list_get(list, k),
                    list->memb_size );
        --list->len;
    }
}

void cub_list_remove(cubList* list, size_t i) {
    if (i >= list->len)
        printf("cub_list_remove: index {%lu} out of range!\n", i);
    else {
        --list->len;
        // Swap the last element with the one to remove
        void* dest = cub_list_get(list, i);
        void* src = cub_list_get(list, list->len);
        memcpy(dest, src, list->memb_size);
    }
}

void cub_list_pop(cubList* list, size_t i, void* removed) {
    if (i >= list->len)
        printf("cub_list_pop: index {%lu}out of range!\n", i);
    else {
        memcpy(removed, cub_list_get(list, i), list->memb_size);
        cub_list_remove(list, i);
    }
}

void cub_list_clear(cubList* list) {
    list->len = 0;
}

void cub_list_free(cubList* list) {
    free(list->data);
    free(list);
}
