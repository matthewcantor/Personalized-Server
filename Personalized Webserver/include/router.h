#ifndef __ROUTER_H
#define __ROUTER_H
#include "http_request.h"
#include "http_response.h"

/**
 * A file system router for the web server. It stores handlers for paths and
 * dispatches requests to the appropriate handler. For example, the handler for
 * `/cat` might respond with a picture of a cat while `/roll` might respond with
 * a random dice roll.
 * 
 * If no matching path has been registered for a handler, it falls back to a
 * fallback handler.
 */
typedef struct router router_t;

/**
 * Represents a handler for a given route.
 * 
 * It accepts a request, borrowing it, and returns a owned bytes
 * which should be responded with.
 */
typedef bytes_t *(*route_handler_t)(request_t *);

/**
 * Initialize a router with a fallback handler, which will be called on all
 * requests which don't match a registered path.
 * 
 * Note that route_handler_t is a function pointer type and function pointers
 * are always borrowed.
 * 
 * `max_routes` specifies the maximum number of routes (not including the 
 * fallback handler) that the router can hold.
 */
router_t *router_init(size_t max_routes, route_handler_t fallback_handler);

/**
 * Registers the route `path` with the router such that requests with that path
 * will be send to `handler.`
 * 
 * If the route has already been registered, replaces the old handler.
 * 
 * If the max number of routes have already been registered and the route is not
 * a replacement route, does nothing (however, if the route is a replacement
 * route then the replacement is still performed even if the router is full).
 * 
 * This function does not take ownership of path and instead creates a copy.
 */
void router_register(router_t *router, const char *path, route_handler_t handler);

/**
 * Dispatch a request to the matching route handler, or the fallback if none
 * exists. Dispatching, here, simply means invoking the previously registered
 * route handler and returning the result, which is an owned array of bytes.
 * 
 * If the set handler returns `NULL,` the the request is instead sent to the
 * fallback handler.
 * 
 * Takes ownership of `request`.
 */
bytes_t *router_dispatch(router_t *router, request_t *request);

/**
 * Frees all resources associated with the router. Note that function pointers
 * are pointers to code, which is owned by the program like a string literal,
 * and are always borrowed (and, correspondingly, they shouldn't be freed).
 * 
 */
void router_free(router_t *router);

#endif
