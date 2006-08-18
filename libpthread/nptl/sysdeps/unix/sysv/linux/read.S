#include <sysdep-cancel.h>
PSEUDO (__libc_read, read, 3)
ret
PSEUDO_END (__libc_read)
libc_hidden_def (__libc_read)
weak_alias (__libc_read, __read)
libc_hidden_weak (__read)
weak_alias (__libc_read, read)
libc_hidden_weak (read)
