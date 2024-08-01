#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "network_util.h"

#define INITIAL_BUFFER_SIZE 10000
#define RESIZE_MULTIPLIER 2
#define LISTENQ 50

#define max(a,b) (a > b ? a : b)

struct connection {
    int fd;
    size_t num_extra_chars;
    char extra_chars[INITIAL_BUFFER_SIZE];
};

static connection_t *connection_init(int fd) {
    connection_t *conn = calloc(1, sizeof(connection_t));
    conn->fd = fd;
    conn->num_extra_chars = 0;
    return conn;
}

int nu_server_listen(int port) {
    int myfd = 0;
    if ((myfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    int optval = 1;
    if (setsockopt(myfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(myfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "bind to %s:%d failed (%s)\n", "localhost", port, strerror(errno));
        close(myfd); 
        return -1;
    }
    if (listen(myfd, LISTENQ) < 0) {
        fprintf(stderr, "listen on %s:%d failed\n", "localhost", port);
        close(myfd);
        return -1;
    }
    return myfd;
}

connection_t *nu_wait_client(int port) {
    int myfd = nu_server_listen(port);
    if (myfd < 0) {
        return NULL;
    }

    struct sockaddr_in client_addr; 
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int clientfd = accept(myfd, (struct sockaddr *) &client_addr, &client_addr_size); 
    if (clientfd < 0) {
        fprintf(stderr, "accept failed\n");
        close(myfd);
        return NULL;
    }

    close(myfd);
    return connection_init(clientfd);
}

connection_t *nu_connect_server(const char *ip, int port) {
    int fd = 0;
    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return NULL;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "pton %s:%d failed (%s)\n", ip, port, strerror(errno));
        close(fd); 
        return NULL;
    }

    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "connect to %s:%d failed (%s)\n", ip, port, strerror(errno));
        close(fd); 
        return NULL;
    }
    return connection_init(fd);
}

int nu_send_str(connection_t *conn, const char *str) {
    int len = strlen(str) + 1;
    if (send(conn->fd, str, len, 0) < 0) {
        return -1;
    }
    return len;
}

int nu_send_bytes(connection_t *conn, const char *bytes, size_t bytes_len) {
    size_t sent = -1;
    while ((sent = send(conn->fd, bytes, bytes_len, 0)) < 0);
    return sent;
}

char *nu_check_for_terminator(char *buf, size_t len) {
    for (size_t i = 3; i < len; i++) {
        if (buf[i - 3] == '\r' && buf[i - 2] == '\n' && buf[i - 1] == '\r' && buf[i] == '\n') {
            return buf + i + 1;
        }
    }
    return NULL;
}

char *nu_read_header(connection_t *conn) {
    size_t current_buffer_size = INITIAL_BUFFER_SIZE;
    char *buf = malloc(current_buffer_size);

    memcpy(buf, conn->extra_chars, conn->num_extra_chars);
    char *curr = buf + conn->num_extra_chars;
    while (true) {
        /* If the block of bytes has a null terminator in it, then we need
         * to finish the string and save the rest for the next call to
         * this function. */
        char *terminator = nu_check_for_terminator(buf, current_buffer_size);
        if (terminator) {
            conn->num_extra_chars = terminator - buf + 1;
            memcpy(conn->extra_chars, terminator, conn->num_extra_chars);
            curr = terminator;
            *curr = '\0';
            return buf;
        }
        size_t to_read = INITIAL_BUFFER_SIZE - conn->num_extra_chars;
        conn->num_extra_chars = 0;
        ssize_t tried_read = read(conn->fd, conn->extra_chars, to_read);

        /* If there was an error, destroy the buffer and indicate 
         * an error has occurred. */
        if (tried_read < 0) {
            perror("recv");
            free(buf);
            return NULL;
        }
        size_t actually_read = tried_read;

        /* If we're out of space, realloc the buffer to include 
         * more space. */
        if ((curr - buf) + actually_read > current_buffer_size) {
            buf = realloc(buf, current_buffer_size * RESIZE_MULTIPLIER); 
            curr = buf + current_buffer_size; 
            current_buffer_size *= RESIZE_MULTIPLIER;
        }

        memcpy(curr, conn->extra_chars, actually_read);
        curr += actually_read;
    }

    // Execution should never reach here...
    assert(false);
}

char *nu_try_read_header(connection_t *conn) {
    fd_set rd;
    FD_ZERO(&rd);
    FD_SET(conn->fd, &rd);

    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    int result = -1;
    if ((result = select(conn->fd + 1, &rd, NULL, NULL, &tv)) < 0) {
        return NULL;
    }
    if (FD_ISSET(conn->fd, &rd)) {
        return nu_read_header(conn);
    }
    return NULL;
}

char *nu_read_bytes(connection_t *conn, size_t amount) {
    char *buf = malloc(amount);
    if (conn->num_extra_chars >= amount) {
        memcpy(buf, conn->extra_chars, amount);
        memmove(conn->extra_chars, conn->extra_chars + amount, conn->num_extra_chars - amount);
        conn->num_extra_chars -= amount;
        return buf;
    }
    memcpy(buf, conn->extra_chars, conn->num_extra_chars);
    char *curr = buf + conn->num_extra_chars;
    size_t to_read = amount - conn->num_extra_chars;
    conn->num_extra_chars = 0;
    while (to_read) {
        ssize_t tried_read = read(conn->fd, curr, to_read);
        if (tried_read < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            perror("recv");
            free(buf);
            return NULL;
        }
        to_read -= tried_read;
    }
    return buf;
}

char *nu_try_read_bytes(connection_t *conn, size_t amount) {
    fd_set rd;
    FD_ZERO(&rd);
    FD_SET(conn->fd, &rd);

    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    int result = -1;
    if ((result = select(conn->fd + 1, &rd, NULL, NULL, &tv)) < 0) {
        return NULL;
    }
    if (FD_ISSET(conn->fd, &rd)) {
        return nu_read_bytes(conn, amount);
    }
    return NULL;
}

void nu_close_connection(connection_t *conn) {
    close(conn->fd);
    free(conn);
}

int nu_multiplex(connection_t *local, connection_t *remote, nu_callback on_local_write, nu_callback on_remote_write) {
    fd_set rd;

    while (1) {
        FD_ZERO(&rd);
        FD_SET(local->fd, &rd);
        FD_SET(remote->fd, &rd);

        int result = -1;
        if ((result = select(max(local->fd, remote->fd) + 1, &rd, NULL, NULL, NULL)) < 0) {
            perror("select");
            return -1;
        }
        if (FD_ISSET(remote->fd, &rd)) {
            on_remote_write(remote->fd, nu_read_header(remote));
        }
        if (FD_ISSET(local->fd, &rd)) {
            on_local_write(remote->fd, nu_read_header(local));
        }
    }
}
