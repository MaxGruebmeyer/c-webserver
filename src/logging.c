#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

/* TODO (GM): Can we reduce duplication in these methods? */
void log_fatal(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_FATAL > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}

void log_error(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_ERROR > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}

void log_warn(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_WARN > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}

void log_info(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_INFO > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}

void log_debug(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_DEBUG > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}

void log_trace(char *fmt, ...)
{
    va_list ap;
    long arg1, arg2, arg3, arg4, arg5, arg6;

    if (LOGLEVEL_TRACE > APPLICATION_LOGLEVEL) {
        return;
    }

    va_start(ap, fmt);
    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);
    printf(fmt, arg1, arg2, arg3, arg4, arg5, arg6);
}
