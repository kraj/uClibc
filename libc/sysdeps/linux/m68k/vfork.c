/* orginally from include/unistd.h, written by ndf@linux.mit.edu> */
#include <unistd.h>
#include <sys/types.h>
#include <asm/unistd.h>

#ifndef __NR_vfork
#define __NR_vfork __NR_fork /* uClinux-2.0 only has fork which is vfork */
#endif

pid_t vfork(void)
{
    pid_t __res;
	__asm__ __volatile__ ("movel %1,%%d0;"
		"trap  #0;"
		"movel %%d0,%0"
		: "=d" (__res)
		: "0" (__NR_vfork)
		: "%d0");
	if (__res >= (unsigned long)-4096) {
	    errno = -__res;
		__res = (pid_t)-1;
	}
    return(__res);
}

