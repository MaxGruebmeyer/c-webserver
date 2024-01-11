#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "reqhandler.h"

#define INDEX_ROUTE 0
#define NOT_FOUND_ROUTE 1

struct Route {
    char *route;
    char *html;
};

static struct Route *routes = NULL;
static int route_len = 10;

static int get_route(const char *req, char *buf);
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

    printf("Added index and 404 route!\n");
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
    int i, j = i = 0;

    struct Route route_s;
    route_s.html = html;
    route_s.route = route;

    printf("Adding route '%s'...\n", route);
    while (routes[i].route && i < route_len) {
        ++i;
    }

    if (i == route_len) {
        routes = realloc(routes, route_len *= 2 * sizeof(struct Route));

        if (!routes) {
            printf("Realloc failed!\n");
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
    printf("Route added!\n");

    return 1;
}

/* Temporarily disable -Wunused-parameter warning since we always return the same response currently. */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* TODO (GM): Add routing! */
int handle_request(char *req, char *res, const unsigned int res_size)
{
    int i = 0;
    char parsed_route[MAX_ROUTE_LEN];

    if (!get_route(req, parsed_route)) {
        printf("\033[31mCould not retrieve route from http request, request invalid!\n"
                "--- BEGIN REQUEST ---\n"
                "%s\n"
                "--- END REQUEST ---\033[0m\n",
                req);

        return -1;
    }

    for (i = 0; i < route_len; i++) {
        if (routes[i].route == NULL) {
            break;
        }

        if (strcmp(parsed_route, routes[i].route) == 0) {
            printf("Found matching route '%s'!\n", routes[i].route);
            return build_res(res, routes[i].html, res_size);
        }
    }

    printf("No routes matched, returning default route...\n");
    return build_res(res, routes[NOT_FOUND_ROUTE].html, res_size);
}

#pragma GCC diagnostic error "-Wunused-parameter"

/* TODO (GM): There has to be built-in functionality that does the same! */
static int get_route(const char *req, char *buf)
{
    while (*req != ' ') {
        if (*req++ == '\0') {
            return 0;
        }
    }

    while (*++req != ' ') {
        if (*req == '\0') {
            return 0;
        }

        *buf++ = *req;
    }

    *buf = '\0';
    return 1;
}

/* Temporarily disable -Wunused-parameter warning since we don't yet use res_size. */
/* TODO (GM): Introduce res_size check! */
#pragma GCC diagnostic ignored "-Wunused-parameter"

static int build_res(char *res, char *body, const unsigned int res_size)
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
        printf("Could not assign to body!\n");
        return -1;
    }

    return 0;
}

#pragma GCC diagnostic error "-Wunused-parameter"
