#include <criterion/criterion.h>
#include "utils/hashmap.h"

Test(HashMap, init_free) {
    HashMap* HM = hashmap_new(1024, 0);
    cr_assert(HM->len == 1024);
    cr_assert(HM->hash_nbr == 1031);
    for (size_t i = 0; i < HM->len; ++i)
        cr_assert(HM->hash_table[i] == NULL);
    hashmap_free(HM);

    HM = hashmap_new(10, 15);
    cr_assert(HM->len == 10);
    cr_assert(HM->hash_nbr == 15);
    for (size_t i = 0; i < HM->len; ++i)
        cr_assert(HM->hash_table[i] == NULL);
    hashmap_free(HM);
}

HashMap* create_hm() {
    return hashmap_new(10, 0);
}

Test(HashMap, hash_function) {
    HashMap* HM = create_hm();
    cr_assert(hash_function(HM, 1024) == 1);
    cr_assert(hash_function(HM, 10) == 0);
    cr_assert(hash_function(HM, 87) == 0);
    cr_assert(hash_function(HM, 85) == 8);
    hashmap_free(HM);
}

Test(HashMap, set) {
    HashMap* HM = create_hm();
    float elts[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    hashmap_set(HM, 1024, elts);
    cr_assert(HM->hash_table[1] != NULL);
    cr_assert(HM->hash_table[1]->len == 1);
    cr_assert(HM->hash_table[1]->capacity == 2);

    hashmap_set(HM, 1, elts + 1);
    cr_assert(HM->hash_table[1]->len == 2);
    cr_assert(HM->hash_table[1]->capacity == 2);

    hashmap_set(HM, 1, elts + 2);
    cr_assert(HM->hash_table[1]->len == 2);
    cr_assert(HM->hash_table[1]->capacity == 2);

    hashmap_set(HM, 12, elts + 3);
    cr_assert(HM->hash_table[1]->len == 3);
    cr_assert(HM->hash_table[1]->capacity == 3);
    hashmap_free(HM);
}

Test(HashMap, in) {
    HashMap* HM = create_hm();
    float elts[] = {1.0f, 2.0f, 3.0f, 4.0f };
    hashmap_set(HM, 0, elts);
    hashmap_set(HM, 1, elts);
    hashmap_set(HM, 12, elts);
    hashmap_set(HM, 5, elts);
    cr_assert(hashmap_in(HM, 0));
    cr_assert(hashmap_in(HM, 1));
    cr_assert(hashmap_in(HM, 12));
    cr_assert(hashmap_in(HM, 5));
    cr_assert(hashmap_in(HM, 1024) == false);
    cr_assert(hashmap_in(HM, 45) == false);
    hashmap_free(HM);
}

Test(HashMap, get) {
    HashMap* HM = create_hm();
    float elts[] = {1.0f, 2.0f, 3.0f, 4.0f };
    hashmap_set(HM, 0, elts);
    hashmap_set(HM, 1, elts + 1);
    hashmap_set(HM, 12, elts + 2);
    hashmap_set(HM, 5, elts + 3);
    float* ptr;
    ptr = hashmap_get(HM, 0);
    cr_assert(*ptr == 1.0f);
    ptr = hashmap_get(HM, 1);
    cr_assert(*ptr == 2.0f);
    ptr = hashmap_get(HM, 12);
    cr_assert(*ptr == 3.0f);
    ptr = hashmap_get(HM, 5);
    cr_assert(*ptr == 4.0f);
    hashmap_set(HM, 5, elts + 2);
    ptr = hashmap_get(HM, 5);
    cr_assert(*ptr == 3.0f);
    hashmap_free(HM);
}

Test(HashMap, remove) {
    HashMap* HM = create_hm();
    float elts[] = {1.0f, 2.0f, 3.0f, 4.0f };
    hashmap_set(HM, 0, elts);
    hashmap_set(HM, 12, elts + 1);
    hashmap_set(HM, 23, elts + 2);
    hashmap_set(HM, 1, elts + 3);
    hashmap_set(HM, 5, elts + 3);

    cr_assert(hashmap_in(HM, 12));
    hashmap_remove(HM, 12);
    cr_assert(!hashmap_in(HM, 12));

    cr_assert(hashmap_in(HM, 23));
    hashmap_remove(HM, 23);
    cr_assert(!hashmap_in(HM, 23));

    cr_assert(hashmap_in(HM, 1));
    hashmap_remove(HM, 1);
    cr_assert(!hashmap_in(HM, 1));

    hashmap_remove(HM, 0);
    hashmap_remove(HM, 5);
    cr_assert(!hashmap_in(HM, 0));
    cr_assert(!hashmap_in(HM, 5));
    hashmap_remove(HM, 25312);
    hashmap_free(HM);
}
