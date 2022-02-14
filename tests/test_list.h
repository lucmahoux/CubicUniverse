#include <criterion/criterion.h>
#include "utils/list.h"

Test(List, init_free) {
    cubList* L = cub_utils_list(sizeof(float), CUB_REALLOC_DOUBLE, 0);
    cr_assert(L);
    cr_assert(L->len == 0);
    cr_assert(L->capacity == BASE_CAPACITY);
    cr_assert(L->memb_size == sizeof(float));
    cr_assert(L->realloc_fun ==
                (void(*)(void*))cub_utils_list_check_double_capacity);
    cub_utils_list_free(L);

    L = cub_utils_list(sizeof(double), CUB_REALLOC_PLUS_ONE, 4);
    cr_assert(L);
    cr_assert(L->len == 0);
    cr_assert(L->capacity == 4);
    cr_assert(L->memb_size == sizeof(double));
    cr_assert(L->realloc_fun ==
                (void(*)(void*))cub_utils_list_check_plus_one_realloc);
    cub_utils_list_free(L);
}

cubList* create_list() {
    cubList* L = cub_utils_list(sizeof(float), CUB_REALLOC_DOUBLE, 0);
    float arr[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
    for (uint8_t i = 0; i < 8; ++i)
        cub_utils_list_append(L, arr + i);
    return L;
}

Test(List, append) {
    cubList* L = cub_utils_list(sizeof(float), CUB_REALLOC_DOUBLE, 0);
    float arr[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    for (uint8_t i = 0; i < 8; ++i)
        cub_utils_list_append(L, arr + i);
    cr_assert(L->capacity == BASE_CAPACITY);
    cub_utils_list_append(L, arr + 8);
    cr_assert(L->capacity == 2 * BASE_CAPACITY);
    cr_assert(L->len == 9);
    cr_assert_arr_eq(L->data, arr, 9 * sizeof(float));
    cub_utils_list_free(L);
    L = cub_utils_list(sizeof(float), CUB_REALLOC_PLUS_ONE, 0);
    for (uint8_t i = 0; i < 8; ++i)
        cub_utils_list_append(L, arr + i);
    cr_assert(L->capacity == BASE_CAPACITY);
    cub_utils_list_append(L, arr + 8);
    cr_assert(L->capacity == 1 + BASE_CAPACITY);
    cr_assert(L->len == 9);
    cr_assert_arr_eq(L->data, arr, 9 * sizeof(float));
    cub_utils_list_free(L);
}

Test(List, get) {
    cubList* L = create_list();
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == 1.0f);
    ptr = cub_utils_list_get(L, 3);
    cr_assert(*ptr == 4.0f);
    ptr = cub_utils_list_get(L, 1);
    cr_assert(*ptr == 2.0f);
    cub_utils_list_free(L);
}

Test(List, set) {
    cubList* L = create_list();
    float elt = -1.0f;
    cub_utils_list_set(L, &elt, 0);
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == elt);
    cub_utils_list_set(L, &elt, 5);
    ptr = cub_utils_list_get(L, 5);
    cr_assert(*ptr == elt);
    cub_utils_list_set(L, &elt, L->len - 1);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == elt);
    cub_utils_list_free(L);
}

Test(List, insert) {
    cubList* L = create_list();
    float elt = -1.0f;
    cub_utils_list_insert(L, &elt, 0);
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == elt);
    elt = 9.0f;
    cub_utils_list_insert(L, &elt, L->len);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == elt);
    elt = 5.5f;
    cub_utils_list_insert(L, &elt, 5);
    ptr = cub_utils_list_get(L, 5);
    cr_assert(*ptr == elt);
    cub_utils_list_clear(L);
    cub_utils_list_insert(L, &elt, 0);
    ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == elt);
    cub_utils_list_free(L);
}

Test(List, remove) {
    cubList* L = create_list();
    cub_utils_list_remove(L, 0);
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == 8.0f);
    cr_assert(L->len == 7);
    cub_utils_list_remove(L, L->len - 1);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == 6.0f);
    cub_utils_list_remove(L, 1);
    ptr = cub_utils_list_get(L, 1);
    cr_assert(*ptr == 6.0f);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == 5.0f);
    cub_utils_list_free(L);
}

Test(List, remove_ordered) {
    cubList* L = create_list();
    cub_utils_list_remove_ordered(L, 0);
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == 2.0f);
    cr_assert(L->len == 7);
    cub_utils_list_remove_ordered(L, L->len - 1);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == 7.0f);
    cub_utils_list_remove_ordered(L, 1);
    ptr = cub_utils_list_get(L, 1);
    cr_assert(*ptr == 4.0f);
    ptr = cub_utils_list_get(L, L->len - 1);
    cr_assert(*ptr == 7.0f);
    cub_utils_list_free(L);
}

Test(List, pop) {
    cubList* L = create_list();
    float popped;
    cub_utils_list_pop(L, 0, &popped);
    cr_assert(popped == 1.0f);
    cr_assert(L->len == 7);
    float* ptr = cub_utils_list_get(L, 0);
    cr_assert(*ptr == 8.0f);
    cub_utils_list_pop(L, L->len - 1, &popped);
    cr_assert(popped == 7.0f);
    cr_assert(L->len == 6);
    cub_utils_list_free(L);
    L = create_list();
    for (uint8_t i = 0; i < 8; ++i)
        cub_utils_list_pop(L, 0, &popped);
    cr_assert(L->len == 0);
    cub_utils_list_free(L);
}
