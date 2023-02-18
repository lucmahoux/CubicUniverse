#include <criterion/criterion.h>
#include "data_struct/list.h"

Test(List, init_free) {
    List* L = list_new(sizeof(float), REALLOC_DOUBLE, 0);
    cr_assert(L);
    cr_assert(L->len == 0);
    cr_assert(L->capacity == BASE_CAPACITY);
    cr_assert(L->memb_size == sizeof(float));
    cr_assert(L->realloc_fun ==
                (void(*)(void*))list_check_double_capacity);
    list_free(L);

    L = list_new(sizeof(double), REALLOC_INCREMENT, 4);
    cr_assert(L);
    cr_assert(L->len == 0);
    cr_assert(L->capacity == 4);
    cr_assert(L->memb_size == sizeof(double));
    cr_assert(L->realloc_fun ==
                (void(*)(void*))list_check_plus_one_realloc);
    list_free(L);
}

List* create_list() {
    List* L = list_new(sizeof(float), REALLOC_DOUBLE, 0);
    float arr[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
    for (uint8_t i = 0; i < 8; ++i)
        list_append(L, arr + i);
    return L;
}

Test(List, append) {
    List* L = list_new(sizeof(float), REALLOC_DOUBLE, 0);
    float arr[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    for (uint8_t i = 0; i < 8; ++i)
        list_append(L, arr + i);
    cr_assert(L->capacity == BASE_CAPACITY);
    list_append(L, arr + 8);
    cr_assert(L->capacity == 2 * BASE_CAPACITY);
    cr_assert(L->len == 9);
    cr_assert_arr_eq(L->data, arr, 9 * sizeof(float));
    list_free(L);
    L = list_new(sizeof(float), REALLOC_INCREMENT, 0);
    for (uint8_t i = 0; i < 8; ++i)
        list_append(L, arr + i);
    cr_assert(L->capacity == BASE_CAPACITY);
    list_append(L, arr + 8);
    cr_assert(L->capacity == 1 + BASE_CAPACITY);
    cr_assert(L->len == 9);
    cr_assert_arr_eq(L->data, arr, 9 * sizeof(float));
    list_free(L);
}

Test(List, get) {
    List* L = create_list();
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == 1.0f);
    ptr = list_get(L, 3);
    cr_assert(*ptr == 4.0f);
    ptr = list_get(L, 1);
    cr_assert(*ptr == 2.0f);
    list_free(L);
}

Test(List, set) {
    List* L = create_list();
    float elt = -1.0f;
    list_set(L, &elt, 0);
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == elt);
    list_set(L, &elt, 5);
    ptr = list_get(L, 5);
    cr_assert(*ptr == elt);
    list_set(L, &elt, L->len - 1);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == elt);
    list_free(L);
}

Test(List, insert) {
    List* L = create_list();
    float elt = -1.0f;
    list_insert(L, &elt, 0);
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == elt);
    elt = 9.0f;
    list_insert(L, &elt, L->len);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == elt);
    elt = 5.5f;
    list_insert(L, &elt, 5);
    ptr = list_get(L, 5);
    cr_assert(*ptr == elt);
    list_clear(L);
    list_insert(L, &elt, 0);
    ptr = list_get(L, 0);
    cr_assert(*ptr == elt);
    list_free(L);
}

Test(List, remove) {
    List* L = create_list();
    list_remove(L, 0);
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == 8.0f);
    cr_assert(L->len == 7);
    list_remove(L, L->len - 1);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == 6.0f);
    list_remove(L, 1);
    ptr = list_get(L, 1);
    cr_assert(*ptr == 6.0f);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == 5.0f);
    list_free(L);
}

Test(List, remove_ordered) {
    List* L = create_list();
    list_remove_ordered(L, 0);
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == 2.0f);
    cr_assert(L->len == 7);
    list_remove_ordered(L, L->len - 1);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == 7.0f);
    list_remove_ordered(L, 1);
    ptr = list_get(L, 1);
    cr_assert(*ptr == 4.0f);
    ptr = list_get(L, L->len - 1);
    cr_assert(*ptr == 7.0f);
    list_free(L);
}

Test(List, pop) {
    List* L = create_list();
    float popped;
    list_pop(L, 0, &popped);
    cr_assert(popped == 1.0f);
    cr_assert(L->len == 7);
    float* ptr = list_get(L, 0);
    cr_assert(*ptr == 8.0f);
    list_pop(L, L->len - 1, &popped);
    cr_assert(popped == 7.0f);
    cr_assert(L->len == 6);
    list_free(L);
    L = create_list();
    for (uint8_t i = 0; i < 8; ++i)
        list_pop(L, 0, &popped);
    cr_assert(L->len == 0);
    list_free(L);
}
