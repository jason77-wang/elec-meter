#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif

#if defined(HAVE_REGEX_H)
#include <regex.h>
#elif defined(HAVE_PCREPOSIX_H)
#include <pcreposix.h>
#endif

#ifdef HAVE_STRTOF_L
#include <locale.h>
#endif

#ifdef HAVE_SCHED_H
#include <sched.h>

#if defined(__linux__) && !defined(SCHED_RESET_ON_FORK)
#define SCHED_RESET_ON_FORK 0x40000000
#endif
#endif

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#ifdef HAVE_SYS_CAPABILITY_H
#include <sys/capability.h>
#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifndef ENOTSUP
#define ENOTSUP   135
#endif

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef HAVE_GRP_H
#include <grp.h>
#endif

#ifdef HAVE_LIBSAMPLERATE
#include <samplerate.h>
#endif

#ifdef __APPLE__
#include <xlocale.h>
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>
#endif

#ifdef HAVE_DBUS
#include "rtkit.h"
#endif

#if defined(__linux__) && !defined(__ANDROID__)
#include <sys/personality.h>
#endif

#include "xmalloc.h"
#include "core-util.h"

/* Not all platforms have this */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#define NEWLINE "\r\n"
#define WHITESPACE "\n\r \t"


/* Returns nonzero when *s starts with *pfx */
bool pa_startswith(const char *s, const char *pfx) {
    size_t l;

    pa_assert(s);
    pa_assert(pfx);

    l = strlen(pfx);

    return strlen(s) >= l && strncmp(s, pfx, l) == 0;
}

char *pa_strip(char *s) {
    char *e, *l = NULL;

    /* Drops trailing whitespace. Modifies the string in
     * place. Returns pointer to first non-space character */

    s += strspn(s, WHITESPACE);

    for (e = s; *e; e++)
        if (!strchr(WHITESPACE, *e))
            l = e;

    if (l)
        *(l+1) = 0;
    else
        *s = 0;

    return s;
}

/* The following function is based on an example from the GNU libc
 * documentation. This function is similar to GNU's asprintf(). */
char *pa_sprintf_malloc(const char *format, ...) {
    size_t size = 100;
    char *c = NULL;

    pa_assert(format);

    for(;;) {
        int r;
        va_list ap;

        c = pa_xrealloc(c, size);

        va_start(ap, format);
        r = vsnprintf(c, size, format, ap);
        va_end(ap);

        c[size-1] = 0;

        if (r > -1 && (size_t) r < size)
            return c;

        if (r > -1)    /* glibc 2.1 */
            size = (size_t) r+1;
        else           /* glibc 2.0 */
            size *= 2;
    }
}

/* Set the FD_CLOEXEC flag for a fd */
void pa_make_fd_cloexec(int fd) {

#ifdef FD_CLOEXEC
    int v;
    pa_assert(fd >= 0);

    pa_assert_se((v = fcntl(fd, F_GETFD, 0)) >= 0);

    if (!(v & FD_CLOEXEC))
        pa_assert_se(fcntl(fd, F_SETFD, v|FD_CLOEXEC) >= 0);
#endif

}

FILE* pa_fopen_cloexec(const char *path, const char *mode) {
    FILE *f;
    char *m;

    m = pa_sprintf_malloc("%se", mode);

    errno = 0;
    if ((f = fopen(path, m))) {
        pa_xfree(m);
        goto finish;
    }

    pa_xfree(m);

    if (errno != EINVAL)
        return NULL;

    if (!(f = fopen(path, mode)))
        return NULL;

finish:
    pa_make_fd_cloexec(fileno(f));
    return f;
}
