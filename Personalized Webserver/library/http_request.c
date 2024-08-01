#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "http_request.h"
#include "ll.h"
#include "mystr.h"

request_t *request_init(const char *method, const char *path , const char *http_version) {
    request_t *req = malloc(sizeof(request_t));
    req->headers = ll_init();

    char *http_copy = malloc(sizeof(char) * strlen(http_version) + 1);
    strcpy(http_copy, http_version);

    char *path_copy = malloc(sizeof(char) * strlen(path) + 1);
    strcpy(path_copy, path);

    char *method_copy = malloc(sizeof(char) * strlen(method) + 1);
    strcpy(method_copy, method);

    req->http_version = http_copy;
    req->method = method_copy;
    req->path = path_copy;
    
    return req;
}

request_t *request_parse(const char *contents) {
    strarray_t *line = mystr_split(contents, '\n');
    strarray_t *first_line = mystr_split(line->data[0], ' ');

    char *final_method = malloc(sizeof(char) * (strlen(first_line->data[0]) + 1));
    assert(final_method);
    strcpy(final_method, first_line->data[0]);

    char *final_path = malloc(sizeof(char) * (strlen(first_line->data[1]) + 1));
    assert(final_path);
    strcpy(final_path, first_line->data[1]);

    //don't need the plus 1 because it has a \r at the back of the string
    char *final_version = malloc(sizeof(char) * strlen(first_line->data[2]));
    assert(final_version);
    strncpy(final_version, first_line->data[2], strlen(first_line->data[2]) - 1);
    final_version[strlen(first_line->data[2]) - 1] = '\0';
    
    request_t *final = request_init(final_method, final_path, final_version);
    
    free(final_method);
    free(final_path);
    free(final_version);
    strarray_free(first_line);

    for (size_t i = 1; i < line->length - 1; i++){
        strarray_t *key_line = mystr_split(line->data[i], ' ');
        char *temp_key = malloc(sizeof(char) * strlen(key_line->data[0]));
        assert(temp_key);
        strncpy(temp_key, key_line->data[0], strlen(key_line->data[0]) - 1);
        temp_key[strlen(key_line->data[0]) - 1] = '\0';

        char *temp_value = malloc(sizeof(char) * strlen(key_line->data[1]));
        assert(temp_value);
        strncpy(temp_value, key_line->data[1], strlen(key_line->data[1]) - 1);
        temp_value[strlen(key_line->data[1]) - 1] = '\0';

        ll_put(final->headers, temp_key, temp_value);

        strarray_free(key_line);
    }

    strarray_free(line);

    return final;
}

void request_free(request_t *req) {
    ll_free(req->headers);
    free(req->http_version);
    free(req->method);
    free(req->path);
    free(req);
}