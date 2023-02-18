#include "data_struct/hashmap.h"

// Get the next prime number starting from start - not optimized
size_t get_next_prime_nbr(size_t start) {
    while (++start) {
        size_t prev;
        for (prev = 3; prev < start; ++prev)
            if (start % prev == 0)
                break;
        if (prev == start)
            return start;
    }
    return 1;
}

cubBucket* binary_search_bucket(cubBucket* buckets, size_t len,
                                size_t key, size_t* pos) {
    size_t left = 0, right = len;
    while (left < right) {
        size_t mid = left + ((right - left) >> 1);
        if ((buckets + mid)->key < key)
            left = mid + 1;
        else
            right = mid;
    }
    *pos = left;
    return left < len ? (buckets + left) : NULL;
}

HashMap* hashmap_new(size_t len, size_t hash_nbr) {
    HashMap* HM = malloc(sizeof(HashMap));
    if (!HM) errx(EXIT_FAILURE, "hashmap_new: Malloc failed!");
    HM->hash_table = calloc(len, sizeof(List*));
    if (!HM->hash_table) {
        free(HM);
        errx(EXIT_FAILURE, "hashmap_new: Malloc failed!");
    }
    HM->len = len;
    HM->nb_keys = 0;
    HM->hash_nbr = hash_nbr == 0 ? get_next_prime_nbr(len) : hash_nbr;
    return HM;
}

size_t hash_function(HashMap* HM, size_t key) {
    return (key % HM->hash_nbr) % HM->len;
}

void* hashmap_get(HashMap* HM, size_t key) {
    List* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return NULL;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return (!bucket || bucket->key != key) ? NULL : bucket->value;
}

bool hashmap_in(HashMap* HM, size_t key) {
    List* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return false;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return bucket && bucket->key == key;
}

void hashmap_set(HashMap* HM, size_t key, void* value) {
    List** bucket_list = &HM->hash_table[hash_function(HM, key)];
    if (!*bucket_list) {
        *bucket_list = list_new(sizeof(cubBucket), REALLOC_INCREMENT,
                                HASHMAP_DEFAULT_LIST_CAPACITY);
        cubBucket elt = { .key = key, .value = value };
        list_append(*bucket_list, &elt);
        ++HM->nb_keys;
    } else {
        size_t pos;
        cubBucket* bucket = binary_search_bucket((*bucket_list)->data,
                                                 (*bucket_list)->len,
                                                 key, &pos);
        if (bucket && bucket->key == key) {
            bucket->value = value;
        } else {
            cubBucket elt = { .key = key, .value = value };
            list_insert(*bucket_list, &elt, pos);
            ++HM->nb_keys;
        }
    }
}

void hashmap_remove(HashMap* HM, size_t key) {
    List* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    if (bucket && bucket->key == key) {
        list_remove_ordered(bucket_list, pos);
        --HM->nb_keys;
    }
}

void hashmap_get_keys(HashMap* HM, size_t* keys) {
    size_t i = 0;
    for (size_t k = 0; k < HM->len; ++k) {
        if (HM->hash_table[k]) {
            List* bucket_list = HM->hash_table[k];
            for (size_t l = 0; l < bucket_list->len; ++l)
                keys[i++] = ((cubBucket*)
                            list_get(bucket_list, l))->key;
        }
    }
}

void hashmap_get_keys_uint(HashMap* HM, uint32_t* keys) {
    size_t i = 0;
    for (size_t k = 0; k < HM->len; ++k) {
        if (HM->hash_table[k]) {
            List* bucket_list = HM->hash_table[k];
            for (size_t l = 0; l < bucket_list->len; ++l)
                keys[i++] = (uint32_t)((cubBucket*)
                            list_get(bucket_list, l))->key;
        }
    }
}

void hashmap_free(HashMap* HM) {
    for (size_t i = 0; i < HM->len; ++i)
        if (HM->hash_table[i])
            list_free(HM->hash_table[i]);
    free(HM->hash_table);
    free(HM);
}
