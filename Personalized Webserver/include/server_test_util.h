#ifndef __SERVER_TEST_UTILS_H
#define __SERVER_TEST_UTILS_H
#include <unistd.h>

/**
 * Represents a handle on the spawned server used to connect to and close it.
 */
typedef struct server_handle server_handle_t;

/**
 * Spawns a copy of `bin`, passing stringified `port` as first arg.
 * 
 * Returns the pid of the spawned child.
 */
server_handle_t *server_spawn(const char *bin, int port);

/**
 * Kills the server corresponding to `pid_t`. Wrapper for `kill` which doesn't
 * require the user to include `signal.h`
 */
void server_kill(server_handle_t *handle);

/**
 * Attempts to connect to the server and, on success, sends it `request`.
 * 
 * Returns (the first two \r\n\r\n terminated chunks of) the server's response 
 * or NULL on failure.
 */
char *server_query(server_handle_t *handle, const char *request);

#endif // __SERVER_TEST_UTILS_H