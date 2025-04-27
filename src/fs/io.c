#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "../log/logging.h"
#include "../err/errorhandler.h"

static int close_file_after_failure(FILE *f);

int get_file_len(char *name)
{
    FILE *f = fopen(name, "r");
    if (!f) {
        log_warn("File '%s' not found!\n", name);
        return -1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        handle_fseek_err();
        return close_file_after_failure(f);
    }

    int fsize = ftell(f);
    if (fsize == -1) {
        handle_ftell_err();
        return close_file_after_failure(f);
    }

    if (fseek(f, 0, SEEK_SET)) {
        handle_fseek_err();
        return close_file_after_failure(f);
    }

    if (fclose(f) != 0) {
        handle_fclose_err();
    }

    return fsize;
}

int fs_read(char *name, char *buf, size_t buf_len)
{
    // TODO (GM): What about race conditions? File already open?
    // TODO (GM): Implement a cache?
    FILE *f = fopen(name, "r");
    if (!f) {
        log_warn("File '%s' not found!\n", name);
        return -1;
    }

    // Checking number of chars read makes no sense since we always read until EOF
    fread(buf, buf_len, 1, f);
    if (ferror(f)) {
        handle_fread_err();
        free(buf);
        return close_file_after_failure(f);
    }

    if (fclose(f) != 0) {
        handle_fclose_err();
    }

    return 0;
}

static int close_file_after_failure(FILE *f)
{
    if (fclose(f) != 0) {
        return handle_fclose_err();
    }

    return -1;
}
