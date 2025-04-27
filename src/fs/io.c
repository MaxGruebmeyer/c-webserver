#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "../log/logging.h"
#include "../err/errorhandler.h"

static int get_file_len(FILE *f);
static int close_file_after_failure(FILE *f);

int fs_read(char *name, char *buf)
{
    // TODO (GM): What about race conditions? File already open?
    // TODO (GM): Implement a cache?
    FILE *f = fopen(name, "r");
    if (!f) {
        log_warn("File '%s' not found!\n", name);
        return -1;
    }

    int file_size = get_file_len(f);
    if (file_size == -1) {
        return close_file_after_failure(f);
    }

    buf = malloc(file_size + 1);
    if (!buf) {
        log_error("Could not allocate buffer of size %i!\n", file_size + 1);
        return close_file_after_failure(f);
    }

    buf[file_size] = '\0';

    // Checking number of chars read makes no sense since we always read until EOF
    fread(buf, file_size, 1, f);
    if (ferror(f)) {
        handle_fread_err();
        free(buf);
        return close_file_after_failure(f);
    }

    if (fclose(f) != 0) {
        // TODO (GM): Good to just swallow this?
        handle_fclose_err();
    }

    return file_size;
}

static int get_file_len(FILE *f)
{
    if (fseek(f, 0, SEEK_END) != 0) {
        return handle_fseek_err();
    }

    int fsize = ftell(f);
    if (fsize == -1) {
        return handle_ftell_err();
    }

    if (fseek(f, 0, SEEK_SET)) {
        return handle_fseek_err();
    }

    return fsize;
}

static int close_file_after_failure(FILE *f)
{
    if (fclose(f) != 0) {
        return handle_fclose_err();
    }

    return -1;
}
