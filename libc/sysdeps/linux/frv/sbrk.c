/* From libc-5.3.12 */

#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

libc_hidden_proto(sbrk)

extern void * __curbrk;
libc_hidden_proto(__curbrk)

extern int __init_brk (void);
libc_hidden_proto(__init_brk)
extern void *_brk(void *ptr) attribute_hidden;

void *
sbrk(intptr_t increment)
{
    if (__init_brk () == 0)
    {
		char * tmp = (char*)__curbrk+increment;
		__curbrk = _brk(tmp);
		if (__curbrk == tmp)
			return tmp-increment;
		__set_errno(ENOMEM);
		return ((void *) -1);
    }
    return ((void *) -1);
}
libc_hidden_def(sbrk)
