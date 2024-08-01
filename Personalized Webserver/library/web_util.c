#include <string.h>

#include "web_util.h"

const char *PATH_PREFIX = "game/";

char *wutil_get_filename_ext(const char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

ssize_t wutil_get_file_size(FILE *f) {
    struct stat s;
    if (fstat(fileno(f), &s)) {
        fclose(f);
        return -1;
    }
    return s.st_size;
}

char *wutil_get_resolved_path(request_t *request) {
    size_t request_path_len = strlen(request->path);
    size_t path_len = strlen(PATH_PREFIX) + request_path_len;
    char *path = calloc(path_len + 1, sizeof(char));
    strcpy(path, PATH_PREFIX);
    strcat(path, request->path);
    // canonicalize the path
    char *resolved_path = realpath(path, NULL);
    free(path);
    return resolved_path;
}

response_code_t wutil_check_resolved_path(char* resolved_path) {
    char *expected_path_prefix = NULL;
    size_t expected_path_prefix_len = 0;
    if (!expected_path_prefix) {
        expected_path_prefix = realpath(PATH_PREFIX, NULL);
        if (!expected_path_prefix) {
            return HTTP_BAD_REQUEST;
        }
        expected_path_prefix_len = strlen(expected_path_prefix);
    }
    else if (strncmp(expected_path_prefix, resolved_path, expected_path_prefix_len) != 0) {
        // the canonicalized path doesn't start inside the data dir.
        // this means it is trying to access files it's not supposed to access
        return HTTP_FORBIDDEN;
    }
    return HTTP_OK;
}

mime_type_t wutil_get_mime_from_extension(char *ext) {
    mime_type_t mime = MIME_OCTET_STREAM;

    if (!strcmp(ext, "html")) {
        mime = MIME_HTML;
    }
    else if (!strcmp(ext, "js")) {
        mime = MIME_JS;
    }
    else if (!strcmp(ext, "json")) {
        mime = MIME_JSON;
    }
    else if (!strcmp(ext, "png")) {
        mime = MIME_PNG;
    }
    else if (!strcmp(ext, "wasm")) {
        mime = MIME_WASM;
    }
    return mime;
}
