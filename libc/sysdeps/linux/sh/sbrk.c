/* From libc-5.3.12 */

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

extern void * ___brk_addr;

extern int __init_brk (void);
extern void *_brk(void *ptr);

void *
sbrk(intptr_t increment)
{
    if (__init_brk () == 0)
    {
		void * tmp = ___brk_addr+increment;
		___brk_addr = _brk(tmp);
		if (___brk_addr == tmp)
			return tmp-increment;
		__set_errno(ENOMEM);
		return ((void *) -1);
    }
    return ((void *) -1);
}
