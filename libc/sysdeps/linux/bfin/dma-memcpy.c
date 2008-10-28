#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

_syscall3 (__ptr_t, dma_memcpy, __ptr_t, dest, __ptr_t, src, size_t, len)

