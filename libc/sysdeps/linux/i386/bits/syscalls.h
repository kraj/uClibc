/* Unlike the asm/unistd.h kernel header file (which this is partly based on),
 * this file must be able to cope with PIC and non-PIC code.  For some arches
 * there is no difference.  For x86 (which has far too few registers) there is
 * a difference.   Regardless, including asm/unistd.h is hereby officially
 * forbidden.  Don't do it.  It is bad for you.  */ 

#include <features.h>

#ifndef __UCLIBC_USE_UNIFIED_SYSCALL__

#undef __syscall_return
#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-125)) { \
		errno = -(res); \
		res = -1; \
	} \
	return (type) (res); \
} while (0)


#undef _syscall0
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

#undef _syscall1
#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1))); \
__syscall_return(type,__res); \
}

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx" \
	: "=a" (__res) \
	: "0" (__NR_##name),"r" ((long)(arg1)),"c" ((long)(arg2))); \
__syscall_return(type,__res); \
}

#undef _syscall3
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

#undef _syscall4
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

#undef _syscall5
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

#undef _syscall1
#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1))); \
__syscall_return(type,__res); \
}

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2))); \
__syscall_return(type,__res); \
}

#undef _syscall3
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

#undef _syscall4
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

#undef _syscall5
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


#else

#define unified_syscall_body(name) \
__asm__ ( \
".text\n.align 4\n.global "###name"\n.type "###name",@function\n" \
#name":\nmovb $"__STR_NR_##name \
",%al;\n jmp __uClibc_syscall\n.Lfe1"###name":\n.size "###name \
",.Lfe1"###name"-"###name \
)

#undef _syscall0
#define _syscall0(type,name) \
unified_syscall_body(name)

#undef _syscall1
#define _syscall1(type,name,type1,arg1) \
unified_syscall_body(name)

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2) \
unified_syscall_body(name)

#undef _syscall3
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
unified_syscall_body(name)

#undef _syscall4
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
unified_syscall_body(name)

#undef _syscall5
#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
unified_syscall_body(name)

#endif
