#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ll.h"


typedef struct node {
    entry_t entry;
    struct node *next;
} node_t;

typedef struct ll_map {
    size_t length;
    node_t *head;
} ll_map_t;

/**
 * Initialize a new node with the given key and value.
 * 
 * The returned node should be heap-allocated with malloc and it takes ownership
 * of the arguments so the caller should not modify or free them afterward.
 */
static node_t *node_init(char *key, char* value);

/**
 * Frees a linked list node and all nodes it points to.
 * 
 * Also frees the keys and values since they are owned by the node.
 * 
 * Passing NULL to this function is valid (and will do nothing).
 */
static void node_free(node_t *curr);

static node_t *node_init(char *key, char* value) {
    node_t *node = malloc(sizeof(node_t));
    assert(node);
    node->entry.key = key;
    node->entry.value = value;
    node->next = NULL;
    return node;
}

static void node_free(node_t *curr) {
    if (curr == NULL){
        return;
    }

    node_free(curr->next);

    free(curr->entry.key);
    free(curr->entry.value);
    free(curr);
    return;
}

ll_map_t *ll_init(void) {
    ll_map_t *linked_list = malloc(sizeof(ll_map_t));
    assert(linked_list);

    linked_list->length = 0;
    linked_list->head = NULL;
    return linked_list;
}

void ll_free(ll_map_t *dict) {
    node_free(dict->head);
    free(dict);
}

char *ll_put(ll_map_t *dict, char *key, char *value) {
    node_t *curr = dict->head; 
    if (curr != NULL){
        while (curr->next != NULL){
            if (strcmp(curr->entry.key, key) == 0){
                free(key);
                char *old_value = curr->entry.value;
                curr->entry.value = value;
                return old_value;
            }
            curr = curr->next;
        }
        if (strcmp(curr->entry.key, key) == 0){
            free(key);
                char *old_value = curr->entry.value;
                curr->entry.value = value;
                return old_value;
        }
        else {
            curr->next = node_init(key, value);
            dict->length++;
            return NULL;
        }
    }

    else {
        dict->head = node_init(key, value);
        dict->length++;
        return NULL;
    }
}

char *ll_get(ll_map_t *dict, char *key) {
    node_t *curr = dict->head;
   
    if (curr == NULL){
        return NULL;
    }

    while (curr != NULL){
        if (strcmp(curr->entry.key, key) == 0){
            char *value = curr->entry.value;
            return value;
        }
        curr = curr -> next;
    }

    return NULL;
}

strarray_t *ll_get_keys(ll_map_t *dict) {
    strarray_t *keys = strarray_init(dict -> length);
    node_t *curr = dict -> head;

    for (size_t i = 0; i < dict->length; i++){
        keys->data[i] = malloc(strlen(curr->entry.key) + 1);
        assert(keys->data[i]);
        strcpy(keys->data[i], curr->entry.key);
        curr = curr->next;
    }

    return keys;
}
