
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>


int vfork(void)
{
	unsigned long __sc_ret, __sc_err;
	register unsigned long __sc_0 __asm__ ("r0");
	register unsigned long __sc_3 __asm__ ("r3");

	__sc_0 = __NR_vfork;
	__asm__ __volatile__
		("sc		\n\t"
		 "mfcr %1	"
		: "=&r" (__sc_3), "=&r" (__sc_0)
		: "0"   (__sc_3), "1"   (__sc_0)
		: __syscall_clobbers);
	__sc_ret = __sc_3;
	__sc_err = __sc_0;

	if((__sc_err & 0x10000000) && (__sc_ret == ENOSYS)){ 
		__sc_0 = __NR_fork;
		__asm__ __volatile__
			("sc		\n\t"
			 "mfcr %1	"
			: "=&r" (__sc_3), "=&r" (__sc_0)
			: "0"   (__sc_3), "1"   (__sc_0)
			: __syscall_clobbers);
		__sc_ret = __sc_3;
		__sc_err = __sc_0;
	}

	__syscall_return (pid_t);
}

