/* orginally from include/unistd.h, written by ndf@linux.mit.edu> */

#ifndef _M68K_VFORK_H
#define _M68K_VFORK_H	1

extern int _clone __P ((int (*fn)(void *arg), void *child_stack, int flags, void *arg));

#define vfork() ({						\
register unsigned long __res __asm__ ("%d0") = __NR_fork;	\
__asm__ __volatile__ ("trap  #0"				\
                      : "=g" (__res)				\
                      : "0" (__res)				\
                      : "%d0");					\
if (__res >= (unsigned long)-4096) {				\
	errno = -__res;						\
	__res = (pid_t)-1;					\
}								\
(pid_t)__res;							\
})


#define clone clone_not_available_use__clone

#endif /* _M68K_VFORK_H */

