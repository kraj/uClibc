#if 0
#include <errno.h>
#endif
#define __check_errno(__res)    ((__res) >= 0)

#ifdef __mc68000__
#include <m68k/syscall.h>
#else
#include <i386/syscall.h>
#endif

#include <sys/syscall.h>
