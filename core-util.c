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
#include <sys/socket.h>
#include <dirent.h>
#include <time.h>

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
    int v = 0;
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

bool pa_is_path_absolute(const char *fn) {
    pa_assert(fn);

#ifndef OS_IS_WIN32
    return *fn == '/';
#else
    return strlen(fn) >= 3 && isalpha(fn[0]) && fn[1] == ':' && fn[2] == '\\';
#endif
}

/* Convert the string s to a signed integer in *ret_i */
int pa_atoi(const char *s, int32_t *ret_i) {
    long l;

    pa_assert(s);
    pa_assert(ret_i);

    if (pa_atol(s, &l) < 0)
        return -1;

    if ((int32_t) l != l) {
        errno = ERANGE;
        return -1;
    }

    *ret_i = (int32_t) l;

    return 0;
}

/* Convert the string s to an unsigned integer in *ret_u */
int pa_atou(const char *s, uint32_t *ret_u) {
    char *x = NULL;
    unsigned long l;

    pa_assert(s);
    pa_assert(ret_u);

    errno = 0;
    l = strtoul(s, &x, 0);

    if (!x || *x || errno) {
        if (!errno)
            errno = EINVAL;
        return -1;
    }

    if ((uint32_t) l != l) {
        errno = ERANGE;
        return -1;
    }

    *ret_u = (uint32_t) l;

    return 0;
}

/* Convert the string s to a signed long integer in *ret_l. */
int pa_atol(const char *s, long *ret_l) {
    char *x = NULL;
    long l;

    pa_assert(s);
    pa_assert(ret_l);

    errno = 0;
    l = strtol(s, &x, 0);

    if (!x || *x || errno) {
        if (!errno)
            errno = EINVAL;
        return -1;
    }

    *ret_l = l;

    return 0;
}

/* Try to parse a boolean string value.*/
int pa_parse_boolean(const char *v) {
    pa_assert(v);

    /* First we check language independent */
    if (pa_streq(v, "1") || !strcasecmp(v, "y") || !strcasecmp(v, "t")
            || !strcasecmp(v, "yes") || !strcasecmp(v, "true") || !strcasecmp(v, "on"))
        return 1;
    else if (pa_streq(v, "0") || !strcasecmp(v, "n") || !strcasecmp(v, "f")
                 || !strcasecmp(v, "no") || !strcasecmp(v, "false") || !strcasecmp(v, "off"))
        return 0;

#ifdef HAVE_LANGINFO_H
{
    const char *expr;
    /* And then we check language dependent */
    if ((expr = nl_langinfo(YESEXPR)))
        if (expr[0])
            if (pa_match(expr, v) > 0)
                return 1;

    if ((expr = nl_langinfo(NOEXPR)))
        if (expr[0])
            if (pa_match(expr, v) > 0)
                return 0;
}
#endif

    errno = EINVAL;
    return -1;
}

/** Similar to pa_read(), but handles writes */
ssize_t pa_write(int fd, const void *buf, size_t count, int *type) {

    if (!type || *type == 0) {
        ssize_t r;

        for (;;) {
            if ((r = send(fd, buf, count, MSG_NOSIGNAL)) < 0) {

                if (errno == EINTR)
                    continue;

                break;
            }

            return r;
        }

#ifdef OS_IS_WIN32
        if (WSAGetLastError() != WSAENOTSOCK) {
            errno = WSAGetLastError();
            return r;
        }
#else
        if (errno != ENOTSOCK)
            return r;
#endif

        if (type)
            *type = 1;
    }

    for (;;) {
        ssize_t r;

        if ((r = write(fd, buf, count)) < 0)
            if (errno == EINTR)
                continue;

        return r;
    }
}

/** Similar to pa_loop_read(), but wraps write() */
ssize_t pa_loop_write(int fd, const void*data, size_t size, int *type) {
    ssize_t ret = 0;
    int _type;

    pa_assert(fd >= 0);
    pa_assert(data);
    pa_assert(size);

    if (!type) {
        _type = 0;
        type = &_type;
    }

    while (size > 0) {
        ssize_t r;

        if ((r = pa_write(fd, data, size, type)) < 0)
            return r;

        if (r == 0)
            break;

        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }

    return ret;
}

bool is_file_exist(const char* filename)
{
	FILE *f;
	pa_assert(filename);

	if (!(f = fopen(filename, "r")))
		return 0;

	fclose(f);
	return 1;
}

char * get_current_date_time(void)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime); /* get the current time */
    timeinfo = localtime(&rawtime);

    return pa_sprintf_malloc("%4d-%02d-%02d %02d:%02d:%02d\n",
            1900+timeinfo->tm_year, /* year */
            1+timeinfo->tm_mon, /* month */
            timeinfo->tm_mday, /* date */
            timeinfo->tm_hour, /* hour */
            timeinfo->tm_min, /* minute */
            timeinfo->tm_sec); /* second */
}
