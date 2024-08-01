#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include "ll.h"

/**
 * Struct representing an HTTP request.
 * 
 * The `method`, `http_version`, and `path` fields are heap-allocated strings
 * that are owned by the struct and are freed by `request_free`.
 * 
 * The `headers` is a linked list dictionary of headers, also freed by
 * `request_free`.
 */
typedef struct {
    char *method;
    char *http_version;
    char *path;
    ll_map_t *headers;
} request_t;

/**
 * Initializes a new request struct with the given method, path, and HTTP version.
 * 
 * The `method`, `http_version`, and `path` strings are COPIED into the struct,
 * so the caller still owns the original strings and must free them once they
 * are no longer needed. I.e., they are all borrowed.
 * 
 * ```
 * // fine to use string literals, since they are copied
 * request_t *req = request_init("GET", "/index.html", "HTTP/1.1");
 * assert(strcmp(req->method, "GET") == 0);
 * assert(strcmp(req->path, "/index.html") == 0);
 * assert(strcmp(req->http_version, "HTTP/1.1") == 0);
 * request_free(req);
 * ```
 * ```
 * char *method = strdup("GET");
 * char *path = strdup("/index.html");
 * char *http_version = strdup("HTTP/1.1");
 * request_t *req = request_init(method, path, http_version);
 * assert(strcmp(req->method, "GET") == 0);
 * assert(strcmp(req->path, "/index.html") == 0);
 * assert(strcmp(req->http_version, "HTTP/1.1") == 0);
 * request_free(req);
 * // must also free the original strings
 * free(method);
 * free(path);
 * free(http_version);
 * 
 * The `headers` field is initialized as an empty linked list dictionary.
 */
request_t *request_init(const char *method, const char *path, const char *http_version);

/**
 * Frees the given request struct and all the strings inside it.
 * 
 * The `method`, `http_version`, and `path` strings are freed using free, and
 * the `headers` linked list dictionary is freed using `ll_free`.
 * 
 * ```
 * request_t *req = request_init("GET", "/index.html", "HTTP/1.1");
 * request_free(req);
 * ```
 */
void request_free(request_t *req);

/**
 * Parses an HTTP request from the given string contents, creating a new request
 * struct.
 * 
 * The function expects the contents to be a valid HTTP request with headers
 * separated by '\r\n'. The first line should contain the method, path, and
 * HTTP version separated by spaces. The headers should be key-value pairs
 * separated by a colon and a space.
 * 
 * It should be terminated by an additional '\r\n' after the headers.
 * 
 * The contents string is borrowed and not touched or modified by this function 
 * and so it freeing it, if necessary, is the callers responsibility.
 * 
 * The returned request should be freed using `request_free` when it is no
 * longer needed.
 */
request_t *request_parse(const char *contents);

#endif /* __HTTP_REQUEST_H */
