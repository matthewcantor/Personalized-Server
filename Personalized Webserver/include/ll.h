#ifndef __LL__H
#define __LL__H
#include "mystr.h"

typedef struct {
    char *key;
    char *value;
} entry_t;

typedef struct ll_map ll_map_t;

/**
 * Creates a new, empty linked list-based string dict. The caller must then
 * free the created dictionary using `ll_free`.
 */
ll_map_t *ll_init(void);

/**
 * Frees a string dict created by `ll_init` and all of its keys and values.
 */
void ll_free(ll_map_t *dict);

/**
 * Adds a new key-value pair to the dict. If the key already exists, the value
 * is replaced and the old value is returned. Otherwise, NULL is returned.
 * 
 * Both the key and the value should be heap-allocated strings (created via 
 * malloc, calloc, strdup, or similar) and this function will take ownership of
 * them. The caller should not free or modify the key or value after passing
 * them to this function.
 * 
 * This function stores the string `value` as-is, rather than creating a new
 * allocation and copying it.
 * 
 * If an old value is returned, it should be exactly the string last placed at
 * this key by a previous `ll_put` call.
 * 
 * If the returned string is not NULL, it is heap-allocated and owned by the 
 * caller, so it is the caller's responsibility to free it when it is no longer 
 * needed.
 * 
 * Example:
 * ```
 * ll_map_t *dict = ll_init();
 * 
 * char *key = strdup("key_string");
 * char *value = strdup("value_string");
 * char *old_value = ll_put(dict, key, value);
 * assert(old_value == NULL); // we just created the map so the key can't exist
 * 
 * char *new_key = strdup("key_string");
 * char *new_value = strdup("new_value_string");
 * old_value = ll_put(dict, new_key, new_value); // this frees either `key` or `new_key`
 * 
 * assert(strcmp(old_value, "value_string") == 0); // the old value was returned
 * 
 * free(old_value) // ownership of old_value was returned from the dict by `ll_put`
 * 
 * ll_free(dict); // this frees `new_value` and whichever of `key` and `new_key` 
 *                // wasn't freed by `ll_put` because the dict took ownership of them
 * ```
 */
char *ll_put(ll_map_t *dict, char *key, char *value);

/**
 * Returns the value associated with the given key in the dict. If the key does
 * not exist in the dict, NULL is returned. The dict does not take ownership of
 * the key and the key still needs to be freed by the caller.
 * 
 * The returned string is owned by the dict and should not be freed by the caller.
 * 
 * Returns exactly the string last placed at this key by `ll_put`, not a copy
 * of it.
 * 
 * Example:
 * ```
 * ll_map_t *dict = ll_init();
 * char *key = strdup("key_string");
 * char *value = strdup("value_string");
 * ll_put(dict, key, value);
 * char *retrieved_value = ll_get(dict, key);
 * assert(strcmp(retrieved_value, "value_string") == 0);
 * ll_free(dict); // this frees `key` and `value` because the dict took
 *                // ownership of them.
 *                // `retrieved_value` is no longer valid but does not need to
 *                // be separately freed because it is owned by the dict.
 * ```
 */
char *ll_get(ll_map_t *dict, char *key);

/**
 * Returns an array of all the keys in the dict. The array should
 * be freed with `strarray_free` when it is no longer needed. See `str_util.h` 
 * for more information on `strarray_t`.
 */
strarray_t *ll_get_keys(ll_map_t *dict);

#endif /* __LL_H */
