#ifndef foocoreutilhfoo
#define foocoreutilhfoo

#include <sys/types.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif


#define PA_PATH_SEP "/"
#define PA_PATH_SEP_CHAR '/'

typedef int bool;

/* Does exactly nothing */
#define pa_nop() do {} while (0)

/* pa_assert() is an assert that may be optimized away by defining
 * NDEBUG. pa_assert_fp() is an assert that may be optimized away by
 * defining FASTPATH. It is supposed to be used in inner loops. It's
 * there for extra paranoia checking and should probably be removed in
 * production builds. */

#define pa_assert(expr) pa_nop()
#define pa_assert_fp(expr) pa_nop()
#define pa_assert_se(expr) pa_nop()

#define pa_log printf

#define pa_streq(a,b) (!strcmp((a),(b)))

static inline const char *pa_strna(const char *x) {
    return x ? x : "n/a";
}

char *pa_strip(char *s);

char *pa_sprintf_malloc(const char *format, ...);

FILE* pa_fopen_cloexec(const char *path, const char *mode);

bool pa_startswith(const char *s, const char *pfx);

#endif