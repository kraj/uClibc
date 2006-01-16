#include <features.h>
#undef errno

extern int errno;
extern int h_errno;

libc_hidden_proto(errno)
libc_hidden_proto(h_errno)

#if 0
/* Unfortunately, this doesn't work... */
int h_errno __attribute__ ((section  (".bss"))) = 0;
int errno __attribute__ ((section  (".bss"))) = 0;
#else
int _errno = 0;
int _h_errno = 0;
#endif

strong_alias(_errno,errno)
libc_hidden_def(errno)
strong_alias(_h_errno,h_errno)
libc_hidden_def(h_errno)
