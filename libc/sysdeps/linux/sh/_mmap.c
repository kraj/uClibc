#include <unistd.h>
#include <sys/mman.h>

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5, type6,arg6) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
{ \
register long __sc3 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
register long __sc6 __asm__ ("r6") = (long) arg3; \
register long __sc7 __asm__ ("r7") = (long) arg4; \
register long __sc0 __asm__ ("r0") = (long) arg5; \
register long __sc1 __asm__ ("r1") = (long) arg6; \
__asm__ __volatile__ ("trapa	#0x15" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4), "r" (__sc5), "r" (__sc6), "r" (__sc7),  \
	  "r" (__sc3), "r" (__sc1) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

_syscall6(__ptr_t, mmap, __ptr_t, addr, size_t, len, int, prot, int, flags, int, fd, __off_t, offset);


