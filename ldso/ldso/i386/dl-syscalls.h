/*
 * This file contains the system call macros and syscall 
 * numbers used by the shared library loader.
 */

#define __NR_exit		  1
#define __NR_read		  3
#define __NR_write		  4
#define __NR_open		  5
#define __NR_close		  6
#define __NR_getuid		 24
#define __NR_geteuid		 49
#define __NR_getgid		 47
#define __NR_getegid		 50
#define __NR_mmap		 90
#define __NR_munmap		 91
#define __NR_stat		106
#define __NR_mprotect		125

/* Here are the macros which define how this platform makes
 * system calls.  This particular variant does _not_ set 
 * errno (note how it is disabled in __syscall_return) since
 * these will get called before the errno symbol is dynamicly 
 * linked. */

#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-125)) { \
		/*errno = -(res); */ \
		res = -1; \
	} \
	return (type) (res); \
} while (0)

#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
__syscall_return(type,__res); \
}

#if defined(__PIC__)

/*
 * PIC uses %ebx, so we need to save it during system calls
 */

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1))); \
__syscall_return(type,__res); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1)),"c" ((long)(arg2))); \
__syscall_return(type,__res); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1)),"c" ((long)(arg2)), \
		"d" ((long)(arg3))); \
__syscall_return(type,__res); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4))); \
__syscall_return(type,__res); \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"m" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5))); \
__syscall_return(type,__res); \
}

#else  /* not doing __PIC__ */

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1))); \
__syscall_return(type,__res); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2))); \
__syscall_return(type,__res); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
		  "d" ((long)(arg3))); \
__syscall_return(type,__res); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4))); \
__syscall_return(type,__res); \
} 

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
	  "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5))); \
__syscall_return(type,__res); \
}


#endif /* __PIC__ */


