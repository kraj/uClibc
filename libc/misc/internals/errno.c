#if 0
#include <features.h>
/* Unforunately, this produces noisy warnings... */
int errno __attribute__ ((section  (".bss")));
int h_errno __attribute__ ((section  (".bss")));
weak_alias(errno, _errno);
weak_alias(h_errno, _h_errno);
#else
__asm__("
.weak _errno;
_errno = errno
.weak _h_errno;
_h_errno = h_errno

.bss
.globl  errno
.type errno,%object
.size errno,4
errno:
    .space  4

.bss
.globl  h_errno
.type h_errno,%object
.size h_errno,4
h_errno:
    .space  4
");
#endif
