/*
 * This file contains the system call macros and syscall
 * numbers used by the shared library loader. Taken from
 * Linux/MIPS 2.4.17 version kernel.
 */

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#define new_stat stat
#include <asm/stat.h>

#define __NR_SYSCALL_BASE		4000

#define __NR_exit			(__NR_SYSCALL_BASE +   1)
#define __NR_read			(__NR_SYSCALL_BASE +   3)
#define __NR_write			(__NR_SYSCALL_BASE +   4)
#define __NR_open			(__NR_SYSCALL_BASE +   5)
#define __NR_close			(__NR_SYSCALL_BASE +   6)
#define __NR_getuid			(__NR_SYSCALL_BASE +  24)
#define __NR_getgid			(__NR_SYSCALL_BASE +  47)
#define __NR_geteuid			(__NR_SYSCALL_BASE +  49)
#define __NR_getegid			(__NR_SYSCALL_BASE +  50)
#define __NR_readlink			(__NR_SYSCALL_BASE +  85)
#define __NR_mmap			(__NR_SYSCALL_BASE +  90)
#define __NR_munmap			(__NR_SYSCALL_BASE +  91)
#define __NR_stat			(__NR_SYSCALL_BASE + 106)
#define __NR_mprotect			(__NR_SYSCALL_BASE + 125)

/* Here are the macros which define how this platform makes
 * system calls.  This particular variant does _not_ set
 * errno (note how it is disabled in __syscall_return) since
 * these will get called before the errno symbol is dynamicly
 * linked. */

#define _syscall0(type,name) \
type name(void) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a3 asm("$7"); \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"li\t$2, %2\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "=r" (__a3) \
	: "i" (__NR_##name) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return -1; \
}

#define _syscall1(type,name,atype,a) \
type name(atype a) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a0 asm("$4") = (unsigned long) a; \
	register unsigned long __a3 asm("$7"); \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"li\t$2, %3\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "=r" (__a3) \
	: "r" (__a0), "i" (__NR_##name) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return (type) -1; \
}

#define _syscall2(type,name,atype,a,btype,b) \
type name(atype a, btype b) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a0 asm("$4") = (unsigned long) a; \
	register unsigned long __a1 asm("$5") = (unsigned long) b; \
	register unsigned long __a3 asm("$7"); \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"li\t$2, %4\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "=r" (__a3) \
	: "r" (__a0), "r" (__a1), "i" (__NR_##name) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return (type) -1; \
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a, btype b, ctype c) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a0 asm("$4") = (unsigned long) a; \
	register unsigned long __a1 asm("$5") = (unsigned long) b; \
	register unsigned long __a2 asm("$6") = (unsigned long) c; \
	register unsigned long __a3 asm("$7"); \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"li\t$2, %5\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "=r" (__a3) \
	: "r" (__a0), "r" (__a1), "r" (__a2), "i" (__NR_##name) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return (type) -1; \
}

#define _syscall4(type,name,atype,a,btype,b,ctype,c,dtype,d) \
type name(atype a, btype b, ctype c, dtype d) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a0 asm("$4") = (unsigned long) a; \
	register unsigned long __a1 asm("$5") = (unsigned long) b; \
	register unsigned long __a2 asm("$6") = (unsigned long) c; \
	register unsigned long __a3 asm("$7") = (unsigned long) d; \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"li\t$2, %5\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "+r" (__a3) \
	: "r" (__a0), "r" (__a1), "r" (__a2), "i" (__NR_##name) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return (type) -1; \
}

#define _syscall6(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f) \
{ \
	register unsigned long __v0 asm("$2") = __NR_##name; \
	register unsigned long __a0 asm("$4") = (unsigned long) a; \
	register unsigned long __a1 asm("$5") = (unsigned long) b; \
	register unsigned long __a2 asm("$6") = (unsigned long) c; \
	register unsigned long __a3 asm("$7") = (unsigned long) d; \
	\
	__asm__ volatile ( \
	".set\tnoreorder\n\t" \
	"lw\t$2, %6\n\t" \
	"lw\t$8, %7\n\t" \
	"subu\t$29, 32\n\t" \
	"sw\t$2, 16($29)\n\t" \
	"sw\t$8, 20($29)\n\t" \
	"li\t$2, %5\t\t\t# " #name "\n\t" \
	"syscall\n\t" \
	"addiu\t$29, 32\n\t" \
	".set\treorder" \
	: "=&r" (__v0), "+r" (__a3) \
	: "r" (__a0), "r" (__a1), "r" (__a2), "i" (__NR_##name), \
	  "m" ((unsigned long)e), "m" ((unsigned long)f) \
	: "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24"); \
	\
	if (__a3 == 0) \
		return (type) __v0; \
	return (type) -1; \
}

#endif
