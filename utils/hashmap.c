#include "utils/hashmap.h"

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

cubHashMap* cub_utils_hashmap(size_t len, size_t hash_nbr) {
    cubHashMap* HM = malloc(sizeof(cubHashMap));
    if (!HM)
        errx(1, "cub_utils_hashmap: Malloc failed!");
    HM->hash_table = malloc(len * sizeof(cubList*));
    if (!HM->hash_table) {
        free(HM);
        errx(1, "cub_utils_hashmap: Malloc failed!");
    }
    for (size_t i = 0; i < len; ++i)
        HM->hash_table[i] = NULL;
    HM->len = len;
    HM->hash_nbr = hash_nbr == 0 ? get_next_prime_nbr(len) : hash_nbr;
    return HM;
}

size_t cub_utils_hash_function(cubHashMap* HM, size_t key) {
    return (key % HM->hash_nbr) % HM->len;
}

void* cub_utils_hashmap_get(cubHashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[cub_utils_hash_function(HM, key)];
    if (!bucket_list)
        return NULL;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return (!bucket || bucket->key != key) ? NULL : bucket->value;
}

bool cub_utils_hashmap_in(cubHashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[cub_utils_hash_function(HM, key)];
    if (!bucket_list)
        return false;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return bucket && bucket->key == key;
}

void cub_utils_hashmap_set(cubHashMap* HM, size_t key, void* value) {
    cubList** bucket_list = &HM->hash_table[cub_utils_hash_function(HM, key)];
    if (!*bucket_list) {
        *bucket_list = cub_utils_list(sizeof(cubBucket), CUB_REALLOC_PLUS_ONE,
                                      CUB_HASHMAP_DEFAULT_LIST_CAPACITY);
        cubBucket elt = { .key = key, .value = value };
        cub_utils_list_append(*bucket_list, &elt);
    } else {
        size_t pos;
        cubBucket* bucket = binary_search_bucket((*bucket_list)->data,
                                                 (*bucket_list)->len,
                                                 key, &pos);
        if (bucket && bucket->key == key) {
            bucket->value = value;
        } else {
            cubBucket elt = { .key = key, .value = value };
            cub_utils_list_insert(*bucket_list, &elt, pos);
        }
    }
}

void cub_utils_hashmap_remove(cubHashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[cub_utils_hash_function(HM, key)];
    if (!bucket_list)
        return;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    if (bucket && bucket->key == key)
        cub_utils_list_remove_ordered(bucket_list, pos);
}

void cub_utils_hashmap_free(cubHashMap* HM) {
    for (size_t i = 0; i < HM->len; ++i)
        if (HM->hash_table[i])
            cub_utils_list_free(HM->hash_table[i]);
    free(HM->hash_table);
    free(HM);
}
