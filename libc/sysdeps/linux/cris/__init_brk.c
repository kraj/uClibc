/* From libc-5.3.12 */

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "sysdep.h"

extern void *__curbrk;
libc_hidden_proto(__curbrk)
void * __curbrk = 0;
libc_hidden_data_def(__curbrk)

extern int __init_brk (void);
libc_hidden_proto(__init_brk)
int
__init_brk (void)
{
    if (__curbrk == 0) {
	    /* Notice that we don't need to save/restore the GOT
	     * register since that is not call clobbered by the syscall.
	     */
	    asm ("clear.d $r10\n\t"
		 "movu.w " STR(__NR_brk) ",$r9\n\t"
		 "break 13\n\t"
		 "move.d $r10, %0"
		 : "=r" (__curbrk)
		 :
		 : "r9", "r10");

	    if (__curbrk == 0) {
		    __set_errno(ENOMEM);
		    return -1;
	    }
    }
    return 0;
}
libc_hidden_def(__init_brk)
