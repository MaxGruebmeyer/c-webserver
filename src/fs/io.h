#ifndef FSIO_HEADERS

#define FSIO_HEADERS

#include <stdlib.h>

/*
 * Gets the file length of the file with the specified name.
 * Returns the file size if successful, -1 on failure.
 */
int get_file_len(const char *name);

/*
 * Reads buf_len bytes from the file with the specified name
 *  and stores them in buf.
 * Returns 0 on success, -1 on failure.
 */
int fs_read(const char *name, char *buf, const size_t buf_len);

#endif
