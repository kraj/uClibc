#include <errno.h>

#define __check_errno(__res)    ((__res) >= 0)

#include <m68k/syscall.h>
#include <sys/syscall.h>
