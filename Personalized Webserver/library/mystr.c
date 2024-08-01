#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "strarray.h"
#include "mystr.h"

ssize_t mystr_indexof(const char *str, const char sep, size_t start) {
    for(size_t i = start; i < strlen(str); i++){
        if(str[i] == sep){
            return i;
        }
    }
    return -1;
}

strarray_t *mystr_split(const char *str, const char sep) {
    
    size_t counter = 0;

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] != sep && (i == 0 || str[i - 1] == sep)) {
            counter++;
        }
    }
    
    strarray_t *final = strarray_init(counter);
    size_t array_idx = 0;
    size_t temp_idx = 0;

    char *temp = malloc(strlen(str) + 1);
    assert(temp);

    for (size_t i = 0; i < strlen(str); i++){
        if (str[i] != sep){
            temp[temp_idx] = str[i];
            temp_idx++;
            if (str[i + 1] == sep || str[i + 1] == '\0'){
                temp[temp_idx] = '\0';
                final->data[array_idx] = malloc(strlen(temp) + 1);
                assert(final->data[array_idx]);
                strcpy(final->data[array_idx], temp);
                array_idx++;
                temp_idx = 0;
                temp[0] = '\0';
             }

        }
    }

    free(temp);
    return final;
}