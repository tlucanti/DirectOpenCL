
#ifndef GUILIB_INTERNALS_H
# define GUILIB_INTERNALS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef READ_ONCE
# define READ_ONCE(__ref) (*(const volatile __typeof__(__ref) *)&(__ref))
#endif

#ifndef WRITE_ONCE
# define WRITE_ONCE(__ref, __val) (*(volatile typeof(__ref) *)&(__ref) = (__val))
#endif

#ifndef __always_inline
# define __always_inline inline __attribute__((__always_inline__))
#endif

#ifndef noinline
# define noinline __attribute__((__noinline__))
#endif

#ifndef __used
# define __used __attribute__((__used__))
#endif

#ifndef __unused
# define __unused __attribute__((__unused__))
#endif

#ifndef __noret
# define __noret __attribute__((__noreturn__))
#endif

#ifndef __cold
# define __cold __attribute__((__cold__))
#endif

#ifndef likely
# define likely(expr) __builtin_expect(!!(expr), 1)
#endif

#ifndef unlikely
# define unlikely(expr) __builtin_expect(!!(expr), 0)
#endif

#ifndef unreachable
# define unreachable() __builtin_unreachable()
#endif

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
#endif

#if CONFIG_GUILIB_VERBOSE
# define gui_perror(message) __gui_perror(__FUNCTION__, message)
#else
# define gui_perror(message) /* empty */
#endif

#define gui_panic(message) __gui_panic(__FILE__, __FUNCTION__, __LINE__, message)

static __always_inline void __gui_perror(const char *function, const char *message)
{
        fprintf(stderr, "%s: ", function);
        if (errno == 0) {
                fprintf(stderr, "%s\n", message);
        } else {
                perror(message);
        }
        errno = 0;
}

__cold __noret
static inline void __gui_panic(const char *file, const char *function, unsigned long line, const char *message)
{
        fflush(stdout);
        fprintf(stderr, "%s:%lu: GUILIB PANIC\n%s: ", file, line, function);
        perror(message);
        abort();
}

#endif /* GUILIB_INTERNALS_H */

