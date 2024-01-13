#ifndef DEBUG_HEADERS

#define DEBUG_HEADERS

#define LOGLEVEL_FATAL 1
#define LOGLEVEL_ERROR 2
#define LOGLEVEL_WARN 3
#define LOGLEVEL_INFO 4
#define LOGLEVEL_DEBUG 5
#define LOGLEVEL_TRACE 6

/* TODO (GM): Read this from config file instead of making it a compile time constant? */
#define APPLICATION_LOGLEVEL LOGLEVEL_DEBUG

/* TODO (GM): Print log messages in different colours depending on severity or prefix with e.g. [FATAL] */
void log_fatal(char *fmt, ...);
void log_error(char *fmt, ...);
void log_warn(char *fmt, ...);
void log_info(char *fmt, ...);
void log_debug(char *fmt, ...);
void log_trace(char *fmt, ...);

#endif
