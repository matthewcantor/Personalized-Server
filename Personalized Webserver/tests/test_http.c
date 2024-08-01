#include "test_util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "http_request.h"
#include "http_response.h"

char* response_format(response_code_t code, char *resp) {
    bytes_t *to_send;
    if (!resp) {
        to_send = bytes_init(0, "");
    }
    else {
        to_send = bytes_init(strlen(resp), resp);
    }
    bytes_t *_resp = response_type_format(code, MIME_HTML, to_send);
    char *str = _resp->data;
    free(_resp);
    free(to_send);
    return str;
}

void test_init_simple() {
    request_t *req = request_init("GET", "/index.html", "HTTP/1.1");
    assert_streq("GET", req->method);
    assert_streq("/index.html", req->path);
    assert_streq("HTTP/1.1", req->http_version);
    strarray_t *header_keys = ll_get_keys(req->headers);
    assert(header_keys->length == 0);
    strarray_free(header_keys);
    request_free(req);
}

void test_init_copy() {
    char *method = strdup("GET");
    char *path = strdup("/index.html");
    char *http_version = strdup("HTTP/1.1");
    request_t *req = request_init(method, path, http_version);
    
    // Check that the values were copied by emptying the original strings
    method[0] = '\0';
    path[0] = '\0';
    http_version[0] = '\0';
    assert_streq("GET", req->method);
    assert_streq("/index.html", req->path);
    assert_streq("HTTP/1.1", req->http_version);
    strarray_t *header_keys = ll_get_keys(req->headers);
    assert(header_keys->length == 0);
    strarray_free(header_keys);
    request_free(req);
    // if init is copying, this isn't a double-free
    free(method);
    free(path);
    free(http_version);
}

void test_init_multuple() {
    request_t *req1 = request_init("GET", "/index.html", "HTTP/1.1");
    request_t *req2 = request_init("POST", "/index.html", "HTTP/1.1");
    assert_streq("GET", req1->method);
    assert_streq("/index.html", req1->path);
    assert_streq("HTTP/1.1", req1->http_version);
    strarray_t *header_keys = ll_get_keys(req1->headers);
    assert(header_keys->length == 0);
    strarray_free(header_keys);
    assert_streq("POST", req2->method);
    assert_streq("/index.html", req2->path);
    assert_streq("HTTP/1.1", req2->http_version);
    header_keys = ll_get_keys(req2->headers);
    assert(header_keys->length == 0);
    strarray_free(header_keys);
    request_free(req1);
    request_free(req2);
}

void test_init() {}

void test_parse_simple() {
    char *contents = "GET /index.html HTTP/1.1\r\n"
                     "Host: localhost:8080\r\n"
                     "User-Agent: curl/7.68.0\r\n"
                     "Accept: */*\r\n"
                     "\r\n";
    request_t *req = request_parse(contents);
    assert_streq("GET", req->method);
    assert_streq("/index.html", req->path);
    assert_streq("HTTP/1.1", req->http_version);
    assert_streq("localhost:8080", ll_get(req->headers, "Host"));
    assert_streq("curl/7.68.0", ll_get(req->headers, "User-Agent"));
    assert_streq("*/*", ll_get(req->headers, "Accept"));
    request_free(req);
}

void test_parse_copy() {
    char *contents = strdup(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "Accept: */*\r\n"
        "\r\n");
    request_t *req = request_parse(contents);

    // Check that the values were copied by emptying the original string
    contents[0] = '\0';
    assert_streq("GET", req->method);
    assert_streq("/index.html", req->path);
    assert_streq("HTTP/1.1", req->http_version);
    assert_streq("localhost:8080", ll_get(req->headers, "Host"));
    assert_streq("curl/7.68.0", ll_get(req->headers, "User-Agent"));
    assert_streq("*/*", ll_get(req->headers, "Accept"));
    request_free(req);
    // if parse is copying, this isn't a double-free
    free(contents);
}

void test_parse_no_headers() {
    char *contents = "A BBB CCCCC\r\n"
                     "\r\n";
    request_t *req = request_parse(contents);

    assert_streq("A", req->method);
    assert_streq("BBB", req->path);
    assert_streq("CCCCC", req->http_version);
    request_free(req);
}

void test_parse_many_headers() {
    const size_t NUM_HEADERS = 1000;
    char prefix[] = "A B C\r\n";
    assert(NUM_HEADERS < 10000);
    size_t contents_len = sizeof(prefix) - 1 + strlen("k####: v####\r\n") * NUM_HEADERS + strlen("\r\n");
    char *contents = malloc(contents_len + 1);
    strcpy(contents, prefix);
    size_t written = sizeof(prefix) - 1;
    for (size_t i = 0; i < NUM_HEADERS; i++) {
        written += snprintf(contents + written, contents_len - written + 1, "k%zu: v%zu\r\n", i, i);
    }
    strcpy(contents + written, "\r\n");
    request_t *req = request_parse(contents);
    assert_streq("A", req->method);
    assert_streq("B", req->path);
    assert_streq("C", req->http_version);
    for (size_t i = 0; i < NUM_HEADERS; i++) {
        char key[6], value[6];
        snprintf(key, sizeof(key), "k%zu", i);
        snprintf(value, sizeof(value), "v%zu", i);
        assert_streq(value, ll_get(req->headers, key));
    }
    request_free(req);
    free(contents);
}

void test_parse_long_prefix() {
    const size_t PREFIX_PIECE_LEN = 100000;
    const size_t PREFIX_LEN = 3 * PREFIX_PIECE_LEN + strlen("  \r\n");
    char prefix_piece_1[PREFIX_PIECE_LEN + 1];
    char prefix_piece_2[PREFIX_PIECE_LEN + 1];
    char prefix_piece_3[PREFIX_PIECE_LEN + 1];
    memset(prefix_piece_1, 'A', PREFIX_PIECE_LEN);
    memset(prefix_piece_2, 'B', PREFIX_PIECE_LEN);
    memset(prefix_piece_3, 'C', PREFIX_PIECE_LEN);
    prefix_piece_1[PREFIX_PIECE_LEN] = '\0';
    prefix_piece_2[PREFIX_PIECE_LEN] = '\0';
    prefix_piece_3[PREFIX_PIECE_LEN] = '\0';

    char contents[PREFIX_LEN + 1];
    snprintf(contents, sizeof(contents), "%s %s %s\r\n", prefix_piece_1, prefix_piece_2, prefix_piece_3);

    request_t *req = request_parse(contents);
    assert_streq(prefix_piece_1, req->method);
    assert_streq(prefix_piece_2, req->path);
    assert_streq(prefix_piece_3, req->http_version);
    request_free(req);
}

void test_parse() {}

void test_response_status() {
    char *resp_ok = response_format(HTTP_OK, "");
    char *resp_bad_request = response_format(HTTP_BAD_REQUEST, "");
    char *resp_forbidden = response_format(HTTP_FORBIDDEN, "");
    char *resp_not_found = response_format(HTTP_NOT_FOUND, "");
    char exp_status_ok[] = "HTTP/1.1 200 OK";
    char exp_status_bad_request[] = "HTTP/1.1 400 Bad Request";
    char exp_status_forbidden[] = "HTTP/1.1 403 Forbidden";
    char exp_status_not_found[] = "HTTP/1.1 404 Not Found";
    resp_ok[sizeof(exp_status_ok) - 1] = 0;
    resp_bad_request[sizeof(exp_status_bad_request) - 1] = 0;
    resp_forbidden[sizeof(exp_status_forbidden) - 1] = 0;
    resp_not_found[sizeof(exp_status_not_found) - 1] = 0;
    assert_streq(resp_ok, exp_status_ok);
    assert_streq(resp_bad_request, exp_status_bad_request);
    assert_streq(resp_forbidden, exp_status_forbidden);
    assert_streq(resp_not_found, exp_status_not_found);
    free(resp_ok);
    free(resp_bad_request);
    free(resp_forbidden);
    free(resp_not_found);
}

void test_response_headers() {
    char *resp1 = response_format(HTTP_OK, "");
    char exp_resp1[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    assert_streq(resp1, exp_resp1);
    free(resp1);
    char *resp2 = response_format(HTTP_BAD_REQUEST, "");
    char exp_resp2[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    assert_streq(resp2, exp_resp2);
    free(resp2);
    char *resp3 = response_format(HTTP_FORBIDDEN, "");
    char exp_resp3[] = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    assert_streq(resp3, exp_resp3);
    free(resp3);
    char *resp4 = response_format(HTTP_NOT_FOUND, "");
    char exp_resp4[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    assert_streq(resp4, exp_resp4);
    free(resp4);
}

void test_response_null() {
    char *resp = response_format(HTTP_OK, NULL);
    char exp_resp[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    assert_streq(resp, exp_resp);
    free(resp);
}

void test_response_body() {
    char *resp = response_format(HTTP_OK, "Hello, world!");
    char exp_resp[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\nHello, world!";
    assert_streq(resp, exp_resp);
    free(resp);
}

void test_response_long_body() {
    const size_t BODY_LEN = 100000;
    char *body = malloc(BODY_LEN + 1);
    body[BODY_LEN] = 0;
    memset(body, 'a', BODY_LEN);
    const char EXP_RESP_PREFIX[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 100000\r\n\r\n";
    const size_t EXP_RESP_LEN = BODY_LEN + sizeof(EXP_RESP_PREFIX) - 1;
    char *exp_resp = malloc(EXP_RESP_LEN + 1);
    strcpy(exp_resp, EXP_RESP_PREFIX);
    strcat(exp_resp, body);
    char *resp = response_format(HTTP_OK, body);
    // assert(streq()) instead of assert_streq() because latter prints on error
    assert(streq(resp, exp_resp));
    free(resp);
    free(body);
    free(exp_resp);
}

void invalid_status(void *aux) {
    (void) aux;
    char *resp = response_format(7, "");
    free(resp);
}

void test_response_invalid_status() {
    assert(test_assert_fail(invalid_status, NULL));
}

void test_response() {}

// TODO: Test parsing more rigorously

int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    char **testnames = argv + 1;

    DO_TEST(test_init_simple)
    DO_TEST(test_init_copy)
    DO_TEST(test_init)
    DO_TEST(test_parse_simple)
    DO_TEST(test_parse_copy)
    DO_TEST(test_parse_no_headers)
    DO_TEST(test_parse_many_headers)
    DO_TEST(test_parse_long_prefix)
    DO_TEST(test_parse)
    DO_TEST(test_response_status)
    DO_TEST(test_response_headers)
    DO_TEST(test_response_null)
    DO_TEST(test_response_body)
    DO_TEST(test_response_long_body)
    DO_TEST(test_response_invalid_status)
    DO_TEST(test_response)
    puts("test_http PASS");

}

