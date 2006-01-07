#include <features.h>

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
__thread int errno;
__thread int h_errno;

#ifdef SHARED
/*
 * FIXME: Add usage of hidden attribute for both of these when used in
 *        the shared library. It currently crashes the linker when doing
 *        section relocations.
 */
extern __thread int __libc_errno __attribute__ ((alias ("errno")));
extern __thread int __libc_h_errno __attribute__ ((alias ("h_errno")));
#else
extern __thread int __libc_errno __attribute__ ((alias ("errno")));
extern __thread int __libc_h_errno __attribute__ ((alias ("h_errno")));
#endif
#define h_errno __libc_h_errno

#else
extern int errno;
extern int h_errno;

#if 0
/* Unfortunately, this doesn't work... */
int h_errno __attribute__ ((section  (".bss"))) = 0;
int errno __attribute__ ((section  (".bss"))) = 0;
#else
int _errno = 0;
int _h_errno = 0;
#endif

weak_alias(_errno, errno)
weak_alias(_h_errno, h_errno)
#endif
