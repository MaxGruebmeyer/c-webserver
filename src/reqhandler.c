#include <string.h>
#include <stdio.h>

#include "reqhandler.h"

static int send_dummy_response(char *res, const unsigned res_size);

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
    char *body = "<h1>Hallo Minesweeper-Enthusiasten</h1>\n"
        "<b>Ich hab nen Webserver, was habt ihr?</b>\n";

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
