/* From libc-5.3.12 */

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

extern void * ___brk_addr;

extern int __init_brk (void);

void *sbrk(intptr_t increment)
{
    if (__init_brk () == 0)
    {
	void * tmp = ___brk_addr+increment;
#if defined(__PIC__) || defined (__pic__)
	__asm__ volatile ("pushl %%ebx\n\t"
			  "movl %%ecx,%%ebx\n\t"
                          "int $0x80\n\t"
                          "popl %%ebx"
		:"=a" (___brk_addr)
		:"0" (__NR_brk),"c" (tmp));
#else
	__asm__ volatile ("int $0x80"
		:"=a" (___brk_addr)
		:"0" (__NR_brk),"b" (tmp));
#endif
	if (___brk_addr == tmp)
		return tmp-increment;
	__set_errno(ENOMEM);
	return ((void *) -1);
    }
    return ((void *) -1);
}
