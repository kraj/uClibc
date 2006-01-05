#define __FORCE_GLIBC
#include <features.h>
#include <netdb.h>

#ifndef __UCLIBC_HAS_THREADS_NATIVE__
#undef h_errno
extern int h_errno;
#endif

int * weak_const_function __h_errno_location (void)
{
    return &h_errno;
}

