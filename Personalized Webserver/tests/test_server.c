#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "test_util.h"
#include "network_util.h"
#include "server_test_util.h"

void test_hello(const char *bin, int port) {
    server_handle_t *h = server_spawn(bin, port);
    const char REQUEST[] = "GET /hello HTTP/1.1\r\n\r\n";
    const char EXPECTED[] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello world!";
    char *response = server_query(h, REQUEST);
    char *r = response;
    char *e = EXPECTED;
    assert_streq(response, (char *) EXPECTED);
    free(response);
    server_kill(h);
}

void test_multiple(const char *bin, int port) {
    const size_t NUM_QUERIES = 10;
    server_handle_t *h = server_spawn(bin, port);
    const char REQUEST[] = "GET /hello HTTP/1.1\r\n\r\n";
    const char EXPECTED[] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello world!";
    for (size_t i = 0; i < NUM_QUERIES; i++) {
        char *response = server_query(h, REQUEST);
        assert_streq(response, (char *) EXPECTED);
        free(response);
    }
    server_kill(h);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "USAGE:  %s <server> <server port>\n", argv[0]);
        exit(1);
    }
    const char *bin = argv[1];
    int port = atoi(argv[2]);

    bool all_tests = argc == 3;
    char **testnames = argv + 3;

    DO_TEST(test_hello, bin, port)
    DO_TEST(test_multiple, bin, port)
    puts("test_server PASS");
}
