/* orginally from include/unistd.h, written by ndf@linux.mit.edu> */

#ifndef _M68K_VFORK_H
#define _M68K_VFORK_H	1

extern int _clone __P ((int (*fn)(void *arg), void *child_stack, int flags, void *arg));

#ifndef __NR_vfork
#define __NR_vfork __NR_fork /* uClinux-2.0 only has fork which is vfork */
#endif

#define vfork() ({						\
unsigned long __res;	\
__asm__ __volatile__ ("movel %1,%%d0;" \
                      "trap  #0;" \
					  "movel %%d0,%0"				\
                      : "=d" (__res)				\
                      : "0" (__NR_vfork)				\
                      : "%d0");					\
if (__res >= (unsigned long)-4096) {				\
	errno = -__res;						\
	__res = (pid_t)-1;					\
}								\
(pid_t)__res;							\
})


#define clone clone_not_available_use__clone

#endif /* _M68K_VFORK_H */

