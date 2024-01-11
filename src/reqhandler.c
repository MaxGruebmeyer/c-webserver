#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "reqhandler.h"

struct Route {
    char *route;
    char *html;
};

static struct Route *routes = NULL;
static int route_len = 10;

static int send_dummy_response(char *res, const unsigned res_size);

int route_init()
{
    int i = 0;
    struct Route default_route;
    default_route.route = "/";
    default_route.html = "<h1>404 NOT FOUND</h1>";

    routes = calloc(route_len, sizeof(struct Route));
    routes[i++] = default_route;

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
    return 1;
}

/* Temporarily disable -Wunused-parameter warning since we always return the same response currently. */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* TODO (GM): Add routing! */
int handle_request(char *req, char *res, const unsigned int res_size)
{
    return send_dummy_response(res, res_size);
}

#pragma GCC diagnostic error "-Wunused-parameter"

/* Temporarily disable -Wunused-parameter warning since we don't yet use res_size. */
/* TODO (GM): Introduce res_size check! */
#pragma GCC diagnostic ignored "-Wunused-parameter"

static int send_dummy_response(char *res, const unsigned int res_size)
{
    /* char *body = "<h1>Hallo Minesweeper-Enthusiasten</h1>\n"
        "<b>Ich hab nen Webserver, was habt ihr?</b>\n";*/
    char *body = routes[0].html;

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
