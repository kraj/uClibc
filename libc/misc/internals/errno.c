#include <features.h>
#undef errno

#if 0
/* Unfortunately, this doesn't work... */
int h_errno __attribute__ ((section  (".bss"))) = 0;
int errno __attribute__ ((section  (".bss"))) = 0;
#else
int errno = 0;
int h_errno = 0;
#endif

weak_alias (errno, _errno)
weak_alias(h_errno, _h_errno);
