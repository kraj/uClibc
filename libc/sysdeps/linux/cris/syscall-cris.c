/* syscall.c - generalized linux system call interface */

#include <stdarg.h>
#include <syscall.h>
#include <errno.h>

/* perform a Linux/CRIS system call with variable number of arguments
 * put them and the syscall number in the right registers and call
 * break 13.
 */

int
syscall(int number, ...)
{
	register long __a __asm__ ("r10");
	register long __b __asm__ ("r11");
	register long __c __asm__ ("r12");
	register long __d __asm__ ("r13");
	register long __e __asm__ ("r0");
        va_list args;

        va_start(args, number);
        __a = va_arg(args, int);
        __b = va_arg(args, int);
        __c = va_arg(args, int);
        __d = va_arg(args, int);
        __e = va_arg(args, int);
        va_end(args);

        __asm__ volatile ("movu.w %1,$r9\n\tbreak 13\n\t"
                : "=r" (__a)
                : "g" (number), "0" (__a), "r" (__b), "r" (__c), "r" (__d),
                          "r" (__e)
                : "r10", "r9");

        if (__a >= 0)
		return __a;

	errno = -__a;
        return -1;
}
