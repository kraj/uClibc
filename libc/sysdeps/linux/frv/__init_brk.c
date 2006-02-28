/* From libc-5.3.12 */

#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

extern void * __curbrk;
libc_hidden_proto(__curbrk)
void * __curbrk = 0;
libc_hidden_data_def(__curbrk)

#define __NR__brk __NR_brk
attribute_hidden _syscall1(void *, _brk, void *, ptr);

extern int __init_brk(void);
libc_hidden_proto(__init_brk)
int
__init_brk (void)
{
    if (__curbrk == 0)
    {
		__curbrk = _brk(0);
		if (__curbrk == 0)
		{
		  __set_errno(ENOMEM);
		  return -1;
		}
    }
    return 0;
}
libc_hidden_def(__init_brk)
