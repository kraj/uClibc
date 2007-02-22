/*
 *
 */

#include <features.h>
#include <errno.h>
#include <netdb.h>

#undef errno
#undef h_errno

/* need to tweak libpthread
#ifdef __UCLIBC_HAS_THREADS__
# define errno __uclibc_errno
# define h_errno __uclibc_h_errno
#endif
*/

extern int h_errno;
libc_hidden_proto(h_errno)

extern int errno;
libc_hidden_proto(errno)
