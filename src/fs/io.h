#ifndef FSIO_HEADERS

#define FSIO_HEADERS

/*
 * Reads the file with the specified name into buf (which will be allocated)
 * and returns the length of the file.
 * Returns -1 on error.
 */
int fs_read(char *name, char *buf);

#endif
