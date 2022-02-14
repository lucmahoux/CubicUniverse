#ifndef CUB_UTILS_HASHMAP_H
#define CUB_UTILS_HASHMAP_H

#include <stdbool.h>
#include "utils/list.h"
#define CUB_HASHMAP_DEFAULT_LIST_CAPACITY 2

typedef struct cubBucket {
    size_t key;
    void* value;
} cubBucket;

typedef struct cubHashMap {
    cubList** hash_table;
    size_t len;
    size_t hash_nbr;
} cubHashMap;

/* Initialises a hashmap of length 'len'.
 * If 'hash_nbr' is 0, the next prime number following 'len' is chosen
 * as the 'hash_nbr'.
 * Otherwise, it is set to the specified value. */
cubHashMap* cub_utils_hashmap(size_t len, size_t hash_nbr);

/* Function used to hash the keys */
size_t cub_utils_hash_function(cubHashMap* HM, size_t key);

/* Return the value associated to a key if it exists or NULL otherwise */
void* cub_utils_hashmap_get(cubHashMap* HM, size_t key);

/* Returns TRUE if the 'key' is in the hashmap */
bool cub_utils_hashmap_in(cubHashMap* HM, size_t key);

/* Add a new key-value pair to the hashmap if the key does not exist.
 * Otherwise, it replaces the value by 'value'. */
void cub_utils_hashmap_set(cubHashMap* HM, size_t key, void* value);

/* Removes the key from the hashmap if it exists */
void cub_utils_hashmap_remove(cubHashMap* HM, size_t key);

/* Called when freeing the hashmap is required */
void cub_utils_hashmap_free(cubHashMap* HM);

#endif
