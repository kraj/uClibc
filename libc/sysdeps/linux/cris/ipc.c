#include <syscall.h>

_syscall6(int, ipc, unsigned int, call, int, first, int, second, int, third,
          void *, ptr, long, fifth);
