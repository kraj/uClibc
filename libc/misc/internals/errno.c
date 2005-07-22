#include <features.h>
#undef errno

#if __PTHREADS_NATIVE__
#include <tls.h>
extern int errno;
extern __thread int _h_errno;
int _errno = 0;
__thread int _h_errno;
#else
extern int errno;
extern int h_errno;
int _errno = 0;
int _h_errno = 0;
#endif

weak_alias (_errno, errno)
weak_alias(_h_errno, h_errno);
