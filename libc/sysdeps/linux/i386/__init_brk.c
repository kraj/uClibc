/* From libc-5.3.12 */

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

void * ___brk_addr = 0;

int
__init_brk ()
{
    if (___brk_addr == 0)
    {
#if defined(__PIC__) || defined (__pic__)
	__asm__ volatile ("pushl %%ebx\n\t"
			  "movl $0,%%ebx\n\t"
			  "int $0x80\n\t"
			  "popl %%ebx"
		:"=a" (___brk_addr)
		:"0" (SYS_brk));
#else
	__asm__ volatile ("int $0x80"
		:"=a" (___brk_addr)
		:"0" (SYS_brk),"b" (0));
#endif
	if (___brk_addr == 0)
	{
	  errno = ENOMEM;
	  return -1;
	}
    }
    return 0;
}
