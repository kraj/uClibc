#define __FORCE_GLIBC
#include <features.h>
#include <netdb.h>
#undef h_errno

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
extern __thread int h_errno;
#else
extern int h_errno;
#endif

int * weak_const_function __h_errno_location (void)
{
    return &h_errno;
}

