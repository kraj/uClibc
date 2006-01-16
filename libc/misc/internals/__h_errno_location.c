#define __FORCE_GLIBC
#include <features.h>
#include <netdb.h>
#undef h_errno

libc_hidden_proto(__h_errno_location)
int * weak_const_function __h_errno_location (void)
{
    return &h_errno;
}
libc_hidden_def(__h_errno_location)
