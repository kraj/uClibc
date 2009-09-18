#include "internal_errno.h"

int errno = 0;
int h_errno = 0;

#ifdef __UCLIBC_HAS_THREADS__
libc_hidden_def(errno)
weak_alias(errno, _errno)
libc_hidden_def(h_errno)
weak_alias(h_errno, _h_errno)
#endif
