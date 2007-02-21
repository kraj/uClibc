#include "internal_errno.h"

#ifdef __UCLIBC_HAS_THREADS__
libc_hidden_proto(errno)
libc_hidden_proto(h_errno)
#endif
int errno = 0;
int h_errno = 0;

#ifdef __UCLIBC_HAS_THREADS__
//weak_alias(_errno, errno)
libc_hidden_def(errno)
//weak_alias(_h_errno, h_errno)
libc_hidden_def(h_errno)
#endif
