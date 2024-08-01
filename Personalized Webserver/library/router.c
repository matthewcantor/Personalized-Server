#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "router.h"

typedef struct route {
    char *path;
    route_handler_t handler;
} route_t;

struct router {
    route_handler_t fallback;
    size_t max_routes;
    size_t num_routes;
    route_t routes[];
};

router_t *router_init(size_t max_routes, route_handler_t fallback) {
    router_t *ret = malloc(sizeof(router_t) + (sizeof(route_t) * max_routes));
    assert(ret);

    ret->max_routes = max_routes;
    ret->fallback = fallback;
    ret->num_routes = 0;
    
    return ret;
}

void router_register(router_t *router, const char *path, route_handler_t handler) {
    for (size_t i = 0; i < router->num_routes; i++) {
        if (strcmp(path, router->routes[i].path) == 0) {
            router->routes[i].handler = handler;
        }
    }

    if (router->num_routes < router->max_routes) {
        router->routes[router->num_routes].handler = handler;

        char *path_cpy = malloc(sizeof(char) * (strlen(path) + 1));
        assert(path_cpy);
        
        strcpy(path_cpy, path);
        router->routes[router->num_routes].path = path_cpy;
        router->num_routes++;
    }
}

bytes_t *router_dispatch(router_t *router, request_t *request) {
    for (size_t i = 0; i < router->num_routes; i++) {
        if (strcmp(request->path, router->routes[i].path) == 0) {
            bytes_t *ret = router->routes[i].handler(request);
            if (ret == NULL) {
                ret = router->fallback(request);
            }
            request_free(request);
            return ret;
        }
    }

    bytes_t *ret = router->fallback(request); 
    request_free(request);
    return ret;
}

void router_free(router_t *router) {
    for (size_t i = 0; i < router->num_routes; i++) {
        route_t curr = router->routes[i];
        free(curr.path);
    }
    free(router);
}