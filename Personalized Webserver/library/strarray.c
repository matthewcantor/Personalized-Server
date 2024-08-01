#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"

strarray_t *strarray_init(size_t length) {
    strarray_t *stringarray = malloc(sizeof(strarray_t));
    char **data = malloc(sizeof(char *) * length);
    for (size_t i = 0; i < length; i++){
        data[i] = NULL;
    }
    stringarray->length = length;
    stringarray->data = data;

    return stringarray;
}

void strarray_free(strarray_t *arr) {
    for (size_t i = 0; i < arr->length; i++){
        free(arr->data[i]);
    }
    free(arr -> data);
    free(arr);
}
