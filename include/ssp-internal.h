/*
 * Distributed under the terms of the GNU Lesser General Public License
 *  $Header: $
 */

#ifndef _SSP_INTERNAL_H
#define _SSP_INTERNAL_H 1

#ifdef __SSP__
#error "file must not be compiled with stack protection enabled on it. Use -fno-stack-protector"
#endif

#ifdef __PROPOLICE_BLOCK_SEGV__
# define SSP_SIGTYPE SIGSEGV
#else
# define SSP_SIGTYPE SIGABRT
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/unistd.h>

#ifndef __SSP_QUICK_CANARY__
#define __NR___kernel_open		__NR_open
static __always_inline _syscall3(int,__kernel_open,const char *,path,int,flags,__kernel_mode_t,mode);
#define OPEN(path, flags, mode)		__kernel_open(path, flags, mode)

/* void * = __ptr_t */
#define __NR___kernel_read		__NR_read
static __always_inline _syscall3(ssize_t,__kernel_read,int,fd,void *,buf,size_t,count);
#define READ(fd, buf, count)		__kernel_read(fd, buf, count)

#define __NR___kernel_close		__NR_close
static __always_inline _syscall1(int,__kernel_close,int,fd);
#define CLOSE(fd)			__kernel_close(fd)
#endif

/* const void * = const __ptr_t */
#define __NR___kernel_write		__NR_write
static __always_inline _syscall3(ssize_t,__kernel_write,int,fd,const void *,buf,size_t,count);
#define WRITE(fd, buf, count)		__kernel_write(fd, buf, count)

/* not using __NR_ */
#define __NR___kernel_gettimeofday	__NR_gettimeofday
static __always_inline _syscall2(int,__kernel_gettimeofday,struct timeval *,tv,struct timezone *,tz);
#define GETTIMEOFDAY(tv, tz)		__kernel_gettimeofday(tv, tz)

#define __NR___kernel_getpid		__NR_getpid
static __always_inline _syscall0(pid_t,__kernel_getpid);
#define GETPID()			__kernel_getpid()

//#ifdef __NR_rt_sigaction
//#define __NR___kernel_sigaction	__NR_rt_sigaction
//static __always_inline _syscall4(...);
//#else
#define __NR___kernel_sigaction		__NR_sigaction
static __always_inline _syscall3(int,__kernel_sigaction,int,signum,const struct sigaction *,act,struct sigaction *,oldact);
//#endif
#define SIGACTION(signum, act, oldact)	__kernel_sigaction(signum, act, oldact)

//#ifdef __NR_rt_sigprocmask
//#define __NR___kernel_sigprocmask	__NR_rt_sigprocmask
//static __always_inline _syscall4(...);
//#else
#define __NR___kernel_sigprocmask	__NR_sigprocmask
static __always_inline _syscall3(int,__kernel_sigprocmask,int,how,const sigset_t *,set,sigset_t *,oldset);
//#endif
#define SIGPROCMASK(how, set, oldset)	__kernel_sigprocmask(how, set, oldset)

#define __NR___kernel_kill		__NR_kill
static __always_inline _syscall2(int,__kernel_kill,__kernel_pid_t,pid,int,sig);
#define KILL(pid, sig)			__kernel_kill(pid, sig)

#define __NR___kernel_exit		__NR_exit
static __always_inline _syscall1(void,__kernel_exit,int,status);
#define EXIT(status)			__kernel_exit(status)

#endif /* _SSP_INTERNAL_H */
