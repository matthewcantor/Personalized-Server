#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H
#include <stdlib.h>
#include <stdint.h>

/**
 * An enumeration of supported HTTP response codes.
 * 
 * See https://developer.mozilla.org/en-US/docs/Web/HTTP/Status for more details.
 */
typedef enum response_code {
    HTTP_OK = 200,          // brief: OK
    HTTP_BAD_REQUEST = 400, // brief: Bad Request
    HTTP_FORBIDDEN = 403,   // brief: Forbidden
    HTTP_NOT_FOUND = 404,   // brief: Not Found
} response_code_t;

/**
 * An enumeration of supported MIME types.
*/
typedef enum mime_type {
    MIME_PLAIN,        // text/plain
    MIME_HTML,         // text/html
    MIME_JS,           // text/javascript
    MIME_JSON,         // text/json
    MIME_WASM,         // application/wasm
    MIME_PNG,          // image/png
    MIME_OCTET_STREAM, // application/octet-stream
} mime_type_t;

typedef struct bytes {
    size_t len;
    char *data;
} bytes_t;

/**
 * Returns an owned bytes struct which should be freed with bytes_free made from
 * data and len. 
 * 
 * Takes ownership of `data`.
 */
bytes_t *bytes_init(size_t len, char *data);

/**
 * Frees a heap-allocated `bytes_t` struct and its associated data.
 */
void bytes_free(bytes_t *bytes);

/**
 * Returns an owned response for a given code and body.
 * 
 * `body` is borrowed and the caller is responsible for freeing if necessary.
 * 
 * The `type` specifies the content type header.
 * 
 * If `type` refers to a "text/" MIME type, then `body` must be a pointer to a
 * null-terminated string (i.e., it should be a `char *`). In this case the
 * returned bytes' data field will be a pointer to a null-terminated string.
 * The `len` field will be the *byte* length of the string, including the null-
 * terminator.
 * 
 * If `type` refers any other MIME type, then `body` must be be a pointer to a
 * `bytes_t` (i.e., it should be `bytes_t *`).
 * 
 * The response is formatted as 
 * [HTTP Version] [Response code] [Response brief]\r\n
 * [Header key 1]: [Value 1]\r\n
 * ...
 * [Header key n]: [Value n]\r\n
 * \r\n
 * [body]
 * 
 * This function only supports HTTP/1.1 for version and outputs only the headers
 * Content-Type: [type]
 * Content-Length: [length]
 * 
 * The human readable [Response briefs] are those documented in `response_code_t`.
 */
bytes_t *response_type_format(response_code_t code, mime_type_t type, bytes_t *body);

#endif // __HTTP_RESPONSE_H
