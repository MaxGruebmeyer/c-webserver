#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "reqhandler.h"
#include "../log/logging.h"
#include "../fs/io.h"

#define INDEX_ROUTE 0
#define NOT_FOUND_ROUTE 1

struct Route {
    char *route;
    char *html;
};

static struct Route *routes = NULL;
static int route_len = 10;

static int get_route(const char *req, char *buf, const int buf_size);
static int build_res(char *res, char *body, const unsigned res_size);

int route_init()
{
    int i = 0;
    struct Route index_route;
    struct Route not_found_route;

    index_route.route = "/";
    index_route.html = "<h1>Welcome to the index page!</h1>";

    not_found_route.route = "/notfound";
    not_found_route.html = "<h1>404 NOT FOUND</h1>";

    routes = calloc(route_len, sizeof(struct Route));
    routes[INDEX_ROUTE] = index_route;
    routes[NOT_FOUND_ROUTE] = not_found_route;

    log_trace("Added index and 404 route!\n");
    i += 2;

    while (i < route_len) {
        struct Route empty_route;
        empty_route.route = NULL;
        empty_route.html = NULL;

        routes[i++] = empty_route;
    }

    return routes ? 1 : 0;
}

void route_cleanup()
{
    free(routes);
}

/* TODO (GM): Check this method for off-by-one errors again! */
/* TODO (GM): Unit tests would be nice, wouldn't it? */
int add_route(char *route, char *html)
{
    /* TODO (GM): Replace this with something more performant, e.g. a hash map */
    /* Options include:
     * - Perfect Hashing
     * - BST
     * - Plain old Arrays
     */
    int i, j = i = 0;

    struct Route route_s;
    route_s.html = html;
    route_s.route = route;

    log_trace("Adding route '%s'...\n", route);
    while (routes[i].route && i < route_len) {
        ++i;
    }

    if (i == route_len) {
        routes = realloc(routes, route_len *= 2 * sizeof(struct Route));

        if (!routes) {
            log_error("Realloc failed!\n");
            return 0;
        }

        for (j = i; j < route_len; ++j) {
            struct Route empty_route;
            empty_route.route = NULL;
            empty_route.html = NULL;

            routes[j] = empty_route;
        }
    }

    routes[i] = route_s;
    log_trace("Route added!\n");

    return 1;
}

/* TODO (GM): Add routing! */
int handle_request(char *req, char *res, const unsigned int res_size)
{
    int i = 0;
    char parsed_route[MAX_ROUTE_LEN];

    // TODO (GM): This should definitly not return a 200 OK
    if (!get_route(req, parsed_route, MAX_ROUTE_LEN)) {
        log_warn("\033[31mCould not retrieve route from http request, request invalid!\n"
                "--- BEGIN REQUEST ---\n"
                "%s\n"
                "--- END REQUEST ---\033[0m\n"
                /* TODO (GM): Return generic 400 BAD_REQUEST instead? */
                "Returning 404 NOT_FOUND...\n",
                req);

        return build_res(res, routes[NOT_FOUND_ROUTE].html, res_size);
    }

    log_debug("Retrieved route '%s' from request!\n", parsed_route);
    for (i = 0; i < route_len; i++) {
        if (routes[i].route == NULL) {
            break;
        }

        if (strcmp(parsed_route, routes[i].route) == 0) {
            log_debug("Found matching route '%s'!\n", routes[i].route);
            return build_res(res, routes[i].html, res_size);
        }
    }

    log_warn("Route not found, trying in fs...\n");

    // +1 to get rid of the leading /
    char *filename = parsed_route + 1;

    int file_size = get_file_len(filename);
    if (file_size == -1) {
        log_error("Could not retrieve file size of file '%s'!\n", filename);

        log_info("No routes matched, returning default route...\n");
        return build_res(res, routes[NOT_FOUND_ROUTE].html, res_size);
    }

    char *buf = malloc(file_size + 1);
    if (!buf) {
        log_error("Could not allocate buffer of size %i!\n", file_size + 1);

        log_info("No routes matched, returning default route...\n");
        return build_res(res, routes[NOT_FOUND_ROUTE].html, res_size);
    }

    buf[file_size] = '\0';
    if (fs_read(filename, buf, file_size) == -1) {
        log_info("No routes matched, returning default route...\n");
        return build_res(res, routes[NOT_FOUND_ROUTE].html, res_size);
    }

    log_info("Route found in fs.\n");
    log_fatal("Hi direct: %s\n", buf);

    int size = build_res(res, buf, res_size);
    free(buf);

    return size;
}

/* TODO (GM): There has to be built-in functionality that does the same! */
static int get_route(const char *req, char *buf, const int buf_size)
{
    while (*req != ' ') {
        if (*req++ == '\0') {
            return 0;
        }
    }

    int i = 0;
    while (*++req != ' ') {
        if (*req == '\0') {
            return 0;
        }

        /* +2 because we need to fit the string terminating char as well. */
        if (i + 2 >= buf_size) {
            log_error("Could not parse route, route too big!\n");
            return 0;
        }

        buf[i++] = *req;
    }

    buf[i] = '\0';
    return 1;
}

/* Temporarily disable -Wunused-parameter warning since we don't yet use res_size. */
/* TODO (GM): Introduce res_size check! */
static int build_res(char *res, char *body, __attribute__((unused)) const unsigned int res_size)
{
    /* TODO (GM): Verify that res is large enough before using sprintf! */
    if(!sprintf(res, "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n"
                "Content-Encoding: UTF-8\r\n"
                "Content-Length: %lu\r\n"
                "\r\n"
                "%s"
                "\r\n\r\n",
                strlen(body), body)) {
        log_error("Could not assign to body!\n");
        return -1;
    }

    log_fatal("Hi: %s\n", res);

    return 0;
}
