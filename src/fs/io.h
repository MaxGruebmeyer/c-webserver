#ifndef FSIO_HEADERS

#define FSIO_HEADERS

#include <stdlib.h>

int get_file_len(char *name);
int fs_read(char *name, char *buf, size_t buf_len);

#endif
