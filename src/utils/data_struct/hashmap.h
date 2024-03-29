#ifndef CUB_UTILS_HASHMAP_H
#define CUB_UTILS_HASHMAP_H

#include <stdbool.h>
#include "data_struct/list.h"
#define HASHMAP_DEFAULT_LIST_CAPACITY 2

typedef struct cubBucket {
    size_t key;
    void* value;
} cubBucket;

typedef struct cubHashMap {
    List** hash_table;
    size_t len;
    size_t nb_keys;
    size_t hash_nbr;
} HashMap;

/* Initialises a hashmap of length 'len'.
 * If 'hash_nbr' is 0, the next prime number following 'len' is chosen
 * as the 'hash_nbr'.
 * Otherwise, it is set to the specified value. */
HashMap* hashmap_new(size_t len, size_t hash_nbr);

/* Function used to hash the keys */
size_t hash_function(HashMap* HM, size_t key);

/* Return the value associated to a key if it exists or NULL otherwise */
void* hashmap_get(HashMap* HM, size_t key);

/* Returns TRUE if the 'key' is in the hashmap */
bool hashmap_in(HashMap* HM, size_t key);

/* Add a new key-value pair to the hashmap if the key does not exist.
 * Otherwise, it replaces the value by 'value'. */
void hashmap_set(HashMap* HM, size_t key, void* value);

/* Removes the key from the hashmap if it exists */
void hashmap_remove(HashMap* HM, size_t key);

/* Populates the array 'keys' which must have at least 'HM->nb_keys' elements,
 * with all the keys contained in the hashmap */
void hashmap_get_keys(HashMap* HM, size_t* keys);

/* Same as cub_utils_hashmap_get_keys() but instead of size_t keys, it fills
 * the 'keys' array with uint32_t keys */
void hashmap_get_keys_uint(HashMap* HM, uint32_t* keys);

/* Called when freeing the hashmap is required */
void hashmap_free(HashMap* HM);

#endif
