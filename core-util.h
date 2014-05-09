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

#define pa_log_debug //
#define pa_log_warn //

#define pa_memzero(x,l) (memset((x), 0, (l)))
#define pa_zero(x) (pa_memzero(&(x), sizeof(x)))

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

int pa_atoi(const char *s, int32_t *ret_i);
int pa_atou(const char *s, uint32_t *ret_u);
int pa_atol(const char *s, long *ret_l);
int pa_atod(const char *s, double *ret_d);
int pa_parse_boolean(const char *v);

ssize_t pa_loop_write(int fd, const void*data, size_t size, int *type);

#endif
