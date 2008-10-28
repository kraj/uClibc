#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

_syscall1 (__ptr_t, sram_free, __ptr_t, addr)

