#include "test_util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "http_request.h"
#include "router.h"

typedef struct route {
    char *path;
    route_handler_t handler;
} route_t;

typedef struct test_router {
    route_handler_t fallback;
    size_t max_routes;
    size_t num_routes;
    route_t routes[];
} test_router_t;

bytes_t *str_bytes(char *s) {
    return bytes_init(strlen(s), s);
}

bytes_t *strdup_bytes(char *s) {
    return bytes_init(strlen(s), strdup(s));
}

bytes_t *hello_world_handler(request_t *request) {
    (void) request;
    return strdup_bytes("Hello, world!");
}

bytes_t *cat_handler(request_t *request) {
    (void) request;
    return strdup_bytes("Cat");
}

bytes_t *method_handler(request_t *request) {
    return strdup_bytes(request->method);
}

bytes_t *num_handler(request_t *request) {
    (void) request;
    return strdup_bytes("7");
}

bytes_t *puppy_handler(request_t *request) {
    (void) request;
    return strdup_bytes("Puppy!");
}

void test_init() {
    router_t *r = router_init(5, hello_world_handler);
    test_router_t *tr = (test_router_t *) r;
    assert(tr->fallback == hello_world_handler);
    assert(tr->max_routes == 5);
    assert(tr->num_routes == 0);
    for (size_t i = 0; i < tr->max_routes; i++) {
        // if this was allocated incorrectly, asan will cause this to crash
        tr->routes[i] = (route_t) { 0 };
    }
    router_free(r); 
}

void test_fallback() {
    router_t *r = router_init(0, hello_world_handler);
    request_t *request = request_init("A", "C", "B");
    bytes_t *response = router_dispatch(r, request);
    assert_streq(response->data, "Hello, world!");
    bytes_free(response);
    router_free(r); 
}

void test_register_one() {
    router_t *r = router_init(1, hello_world_handler);
    router_register(r, "cat", cat_handler);
    request_t *request = request_init("A", "C", "B");
    request_t *cat_request = request_init("A", "cat", "B");
    bytes_t *response = router_dispatch(r, request);
    bytes_t *cat_response = router_dispatch(r, cat_request);
    assert_streq(response->data, "Hello, world!");
    assert_streq(cat_response->data, "Cat");
    bytes_free(response); 
    bytes_free(cat_response);
    router_free(r);
}

void test_register_several() {
    router_t *r = router_init(3, hello_world_handler);
    router_register(r, "cat", cat_handler);
    router_register(r, "method", method_handler);
    router_register(r, "num/seven", num_handler);
    request_t *requests[] = {
        request_init("A", "C", "B"),
        request_init("A", "cat", "B"),
        request_init("A", "method", "B"),
        request_init("GET", "method", "B"),
        request_init("GET", "num/seven", "B")
    };
    char *expected_responses[] = {
        "Hello, world!",
        "Cat",
        "A",
        "GET",
        "7"
    };
    for (size_t i = 0; i < 5; i++) {
        bytes_t *response = router_dispatch(r, requests[i]);
        assert_streq(response->data, expected_responses[i]);
        bytes_free(response);
    }
    router_free(r);
}

void test_register_replace() {
    router_t *r = router_init(1, hello_world_handler);
    router_register(r, "best_animal", cat_handler); 
    router_register(r, "best_animal", puppy_handler);
    request_t *request = request_init("A", "best_animal", "B");
    bytes_t *response = router_dispatch(r, request);
    assert_streq(response->data, "Puppy!");
    bytes_free(response);
    router_free(r);
}

void test_register_max() {
    router_t *r = router_init(0, hello_world_handler);
    router_register(r, "foo", cat_handler);
    request_t *request = request_init("A", "foo", "B");
    bytes_t *response = router_dispatch(r, request);
    assert_streq(response->data, "Hello, world!");
    bytes_free(response);
    router_free(r);
}


int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    char **testnames = argv + 1;
    
    DO_TEST(test_init);
    DO_TEST(test_fallback)
    DO_TEST(test_register_one)
    DO_TEST(test_register_several)
    DO_TEST(test_register_replace)
    DO_TEST(test_register_max)
    puts("test_router PASS");
}
