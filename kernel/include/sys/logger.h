#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <libc/stdio/printf.h>
#include <stdarg.h>

void debug_log(const char *file, const int line, const char *function,
               const char *fmt, ...);
void cdebug_log(const char *function, const char *fmt, ...);

void pdebug_log(const char *file, const int line, const char *function,
                const char *fmt, ...);
void pcdebug_log(const char *function, const char *fmt, ...);

void plog_ok(const char *fmt, ...);

void plog_fail(const char *fmt, ...);
void plog_fatal(const char *fmt, ...);
void plog_warn(const char *fmt, ...);

#endif  // __LOGGER_H__