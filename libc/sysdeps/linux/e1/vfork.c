#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>

_syscall0(pid_t, vfork);
