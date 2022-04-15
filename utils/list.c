#include "utils/list.h"

void* list_get(List* list, size_t i) {
    return (char*) list->data + i * list->memb_size;
}

List* list_new(size_t memb_size, ReallocType type, size_t capacity) {
    List* list = malloc(sizeof(List));
    if (!list) errx(EXIT_FAILURE, "cub_list: Malloc failed!");
    list->data = malloc(memb_size * BASE_CAPACITY);
    if (!list->data) {
        free(list);
        errx(1, "cub_list: malloc failed!");
    }
    list->capacity = capacity ? capacity : BASE_CAPACITY;
    list->memb_size = memb_size;
    list->len = 0;
    list->realloc_fun = type == REALLOC_DOUBLE
                        ? (void(*)(void*))list_check_double_capacity
                        : (void(*)(void*))list_check_plus_one_realloc;
    return list;
}

void list_set(List* list, void* elt, size_t i) {
#ifdef CUB_DEBUG
    if (i >= list->len)
        errx(EXIT_FAILURE, "list_set: list index out of range!");
#endif
    memcpy(list_get(list, i), elt, list->memb_size);
}

void list_check_double_capacity(List* list) {
    if (list->len >= list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data, list->memb_size * list->capacity);
        if (!list->data)
            errx(EXIT_FAILURE, "list_check_double_capacity: realloc!");
    }
}

void list_check_plus_one_realloc(List* list) {
    if (list->len >= list->capacity) {
        ++list->capacity;
        list->data = realloc(list->data, list->memb_size * list->capacity);
        if (!list->data)
            errx(EXIT_FAILURE, "list_check_plus_on: realloc failed!");
    }
}

void list_append(List* list, void* elt) {
    list->realloc_fun(list);
    memcpy(list_get(list, list->len), elt, list->memb_size);
    ++list->len;
}

void list_insert(List* list, void* elt, size_t i) {
#ifdef CUB_DEBUG
    if (i >= list->len + 1)
        errx(EXIT_FAILURE, "list_insert: list index out of range\n!");
#endif
    list->realloc_fun(list);
    for (size_t k = list->len; k > i; --k)
        memcpy( list_get(list, k),
                list_get(list, k - 1),
                list->memb_size );
    memcpy(list_get(list, i), elt, list->memb_size);
    ++list->len;
}

void list_remove_ordered(List* list, size_t i) {
    if (i >= list->len)
        printf("list_remove_sorted: index {%lu} out of range!\n", i);
    else {
        for (size_t k = i + 1; k < list->len; ++k)
            memcpy( list_get(list, k - 1),
                    list_get(list, k),
                    list->memb_size );
        --list->len;
    }
}

void list_remove(List* list, size_t i) {
#ifdef CUB_DEBUG
    if (i >= list->len)
        errx(EXIT_FAILURE, "list_remove: index {%lu} out of range!\n", i);
#endif
    --list->len;
    // Swap the last element with the one to remove
    void* dest = list_get(list, i);
    void* src = list_get(list, list->len);
    memcpy(dest, src, list->memb_size);
}

void list_pop(List* list, size_t i, void* removed) {
#ifdef CUB_DEBUG
    if (i >= list->len)
        errx(EXIT_FAILURE, "list_pop: index {%lu}out of range!\n", i);
#endif
    memcpy(removed, list_get(list, i), list->memb_size);
    list_remove(list, i);
}

void list_clear(List* list) {
    list->len = 0;
}

void list_free(List* list) {
    free(list->data);
    free(list);
}
