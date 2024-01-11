#ifndef REQHANDLER_HEADERS

#define REQHANDLER_HEADERS

#define MAX_ROUTE_LEN 256

int route_init();
void route_cleanup();

int add_route(char *route, char *html);
int handle_request(char *req, char *res, const unsigned int res_size);

#endif
