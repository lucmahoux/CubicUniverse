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

hashMap* hashmap_new(size_t len, size_t hash_nbr) {
    hashMap* HM = malloc(sizeof(hashMap));
    if (!HM)
        errx(1, "hashmap_new: Malloc failed!");
    HM->hash_table = malloc(len * sizeof(cubList*));
    if (!HM->hash_table) {
        free(HM);
        errx(1, "hashmap_new: Malloc failed!");
    }
    for (size_t i = 0; i < len; ++i)
        HM->hash_table[i] = NULL;
    HM->len = len;
    HM->nb_keys = 0;
    HM->hash_nbr = hash_nbr == 0 ? get_next_prime_nbr(len) : hash_nbr;
    return HM;
}

size_t hash_function(hashMap* HM, size_t key) {
    return (key % HM->hash_nbr) % HM->len;
}

void* hashmap_get(hashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return NULL;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return (!bucket || bucket->key != key) ? NULL : bucket->value;
}

bool hashmap_in(hashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return false;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    return bucket && bucket->key == key;
}

void hashmap_set(hashMap* HM, size_t key, void* value) {
    cubList** bucket_list = &HM->hash_table[hash_function(HM, key)];
    if (!*bucket_list) {
        *bucket_list = cub_utils_list(sizeof(cubBucket), CUB_REALLOC_PLUS_ONE,
                                      HASHMAP_DEFAULT_LIST_CAPACITY);
        cubBucket elt = { .key = key, .value = value };
        cub_utils_list_append(*bucket_list, &elt);
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
            cub_utils_list_insert(*bucket_list, &elt, pos);
            ++HM->nb_keys;
        }
    }
}

void hashmap_remove(hashMap* HM, size_t key) {
    cubList* bucket_list = HM->hash_table[hash_function(HM, key)];
    if (!bucket_list)
        return;
    size_t pos;
    cubBucket* bucket = binary_search_bucket(bucket_list->data,
                                             bucket_list->len,
                                             key, &pos);
    if (bucket && bucket->key == key) {
        cub_utils_list_remove_ordered(bucket_list, pos);
        --HM->nb_keys;
    }
}

void hashmap_get_keys(hashMap* HM, size_t* keys) {
    size_t i = 0;
    for (size_t k = 0; k < HM->len; ++k) {
        if (HM->hash_table[k]) {
            cubList* bucket_list = HM->hash_table[k];
            for (size_t l = 0; l < bucket_list->len; ++l)
                keys[i++] = ((cubBucket*)
                            cub_utils_list_get(bucket_list, l))->key;
        }
    }
}

void hashmap_get_keys_uint(hashMap* HM, uint32_t* keys) {
    size_t i = 0;
    for (size_t k = 0; k < HM->len; ++k) {
        if (HM->hash_table[k]) {
            cubList* bucket_list = HM->hash_table[k];
            for (size_t l = 0; l < bucket_list->len; ++l)
                keys[i++] = (uint32_t)((cubBucket*)
                            cub_utils_list_get(bucket_list, l))->key;
        }
    }
}

void hashmap_free(hashMap* HM) {
    for (size_t i = 0; i < HM->len; ++i)
        if (HM->hash_table[i])
            cub_utils_list_free(HM->hash_table[i]);
    free(HM->hash_table);
    free(HM);
}
