#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <router.h>

#include "network_util.h"
#include "http_request.h"
#include "http_response.h"
#include "web_util.h"

char *HELLO_RESPONSE = "Hello, world!";
char *ERROR_MESSAGE_ONE = "Path is Null";
char *ERROR_MESSAGE_TWO = "Wrong response code";
const char *HELLO_PATH = "/hello";
const char *ROLL_PATH = "/roll";
int DICE_NUMBER = 6;
int TO_ASCII = 49;


bytes_t *hello_handler() {
    bytes_t *body = bytes_init(strlen(HELLO_RESPONSE), HELLO_RESPONSE);
    return response_type_format(HTTP_OK, MIME_HTML, body);
}

bytes_t *roll_handler() {
    char random = (rand() % DICE_NUMBER) + TO_ASCII;
    // add 49 to get to the ascii value
    bytes_t *body = bytes_init(1, &random);
    return response_type_format(HTTP_OK, MIME_HTML, body);
}

bytes_t *default_handler(request_t *req) {
    char *path = wutil_get_resolved_path(req);
    if (path == NULL) {
        bytes_t *body = bytes_init(strlen(ERROR_MESSAGE_ONE), ERROR_MESSAGE_ONE);
        return response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, body);
    }

    response_code_t response = wutil_check_resolved_path(path);
    if (response != HTTP_OK) {
        bytes_t *body = bytes_init(strlen(ERROR_MESSAGE_TWO), ERROR_MESSAGE_TWO);
        return response_type_format(response, MIME_PLAIN, body);
    }

    FILE *f = fopen(path, "r");
    ssize_t file_size = wutil_get_file_size(f);
    char *copy = malloc(sizeof(char) * file_size);
    fread(copy, sizeof(char), file_size, f);

    char *name = wutil_get_filename_ext(path);
    mime_type_t mime = wutil_get_mime_from_extension(name);
    bytes_t *body = bytes_init(file_size, copy);
    return response_type_format(response, mime, body);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "USAGE:  %s <server port>\n", argv[0]);
        exit(1);
    }
    
    int port = atoi(argv[1]);

    router_t *router = router_init(2, default_handler);
    router_register(router, HELLO_PATH, hello_handler);
    router_register(router, ROLL_PATH, roll_handler);

    while (1){
        char *input = NULL;
        connection_t *log_in = nu_wait_client(port);

        while (input == NULL){
            input = nu_try_read_header(log_in);
        }
        
        request_t *parsed_input = request_parse(input);
        bytes_t *dispatch = router_dispatch(router, parsed_input);
        nu_send_bytes(log_in, dispatch->data, dispatch->len);

        free(input);
        nu_close_connection(log_in);
    }

    return 0;
}

