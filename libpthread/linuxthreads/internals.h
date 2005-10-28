/* Linuxthreads - a simple clone()-based implementation of Posix        */
/* threads for Linux.                                                   */
/* Copyright (C) 1996 Xavier Leroy (Xavier.Leroy@inria.fr)              */
/*                                                                      */
/* This program is free software; you can redistribute it and/or        */
/* modify it under the terms of the GNU Library General Public License  */
/* as published by the Free Software Foundation; either version 2       */
/* of the License, or (at your option) any later version.               */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU Library General Public License for more details.                 */

#ifndef _INTERNALS_H
#define _INTERNALS_H	1

/* Internal data structures */

/* Includes */

#include <features.h>
#include <bits/libc-tsd.h> /* for _LIBC_TSD_KEY_N */
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <bits/stackinfo.h>
#include <sys/types.h>
#include "pt-machine.h"
#include <ucontext.h>
#include <bits/sigcontextinfo.h>
#include "semaphore.h"
#ifdef __UCLIBC_HAS_XLOCALE__
#include <bits/uClibc_locale.h>
#endif /* __UCLIBC_HAS_XLOCALE__ */
#include "descr.h"

/* Use a funky version in a probably vein attempt at preventing gdb 
 * from dlopen()'ing glibc's libthread_db library... */
#define STRINGIFY(s) STRINGIFY2 (s)
#define STRINGIFY2(s) #s
#define VERSION STRINGIFY(__UCLIBC_MAJOR__) "." STRINGIFY(__UCLIBC_MINOR__) "." STRINGIFY(__UCLIBC_SUBLEVEL__)

#ifndef THREAD_GETMEM
# define THREAD_GETMEM(descr, member) descr->member
#endif
#ifndef THREAD_GETMEM_NC
# define THREAD_GETMEM_NC(descr, member) descr->member
#endif
#ifndef THREAD_SETMEM
# define THREAD_SETMEM(descr, member, value) descr->member = (value)
#endif
#ifndef THREAD_SETMEM_NC
# define THREAD_SETMEM_NC(descr, member, value) descr->member = (value)
#endif

#if !defined NOT_IN_libc && defined FLOATING_STACKS
# define LIBC_THREAD_GETMEM(descr, member) THREAD_GETMEM (descr, member)
# define LIBC_THREAD_SETMEM(descr, member, value) \
  THREAD_SETMEM (descr, member, value)
#else
# define LIBC_THREAD_GETMEM(descr, member) descr->member
# define LIBC_THREAD_SETMEM(descr, member, value) descr->member = (value)
#endif

typedef void (*destr_function)(void *);

struct pthread_key_struct {
  int in_use;                   /* already allocated? */
  destr_function destr;         /* destruction routine */
};


#define PTHREAD_START_ARGS_INITIALIZER(fct) \
  { (void *(*) (void *)) fct, NULL, {{0, }}, 0, { 0 } }


/* The type of thread handles. */

typedef struct pthread_handle_struct * pthread_handle;

struct pthread_handle_struct {
  struct _pthread_fastlock h_lock; /* Fast lock for sychronized access */
  pthread_descr h_descr;        /* Thread descriptor or NULL if invalid */
  char * h_bottom;              /* Lowest address in the stack thread */
};

/* The type of messages sent to the thread manager thread */

struct pthread_request {
  pthread_descr req_thread;     /* Thread doing the request */
  enum {                        /* Request kind */
    REQ_CREATE, REQ_FREE, REQ_PROCESS_EXIT, REQ_MAIN_THREAD_EXIT,
    REQ_POST, REQ_DEBUG, REQ_KICK, REQ_FOR_EACH_THREAD
  } req_kind;
  union {                       /* Arguments for request */
    struct {                    /* For REQ_CREATE: */
      const pthread_attr_t * attr; /* thread attributes */
      void * (*fn)(void *);     /*   start function */
      void * arg;               /*   argument to start function */
      sigset_t mask;            /*   signal mask */
    } create;
    struct {                    /* For REQ_FREE: */
      pthread_t thread_id;      /*   identifier of thread to free */
    } free;
    struct {                    /* For REQ_PROCESS_EXIT: */
      int code;                 /*   exit status */
    } exit;
    void * post;                /* For REQ_POST: the semaphore */
    struct {			/* For REQ_FOR_EACH_THREAD: callback */
      void (*fn)(void *, pthread_descr);
      void *arg;
    } for_each;
  } req_args;
};



typedef void (*arch_sighandler_t) (int, SIGCONTEXT);
union sighandler
{
  arch_sighandler_t old;
  void (*rt) (int, struct siginfo *, struct ucontext *);
};
extern union sighandler __sighandler[NSIG];


/* Signals used for suspend/restart and for cancellation notification.  */

extern int __pthread_sig_restart;
extern int __pthread_sig_cancel;

/* Signal used for interfacing with gdb */

extern int __pthread_sig_debug;

/* Global array of thread handles, used for validating a thread id
   and retrieving the corresponding thread descriptor. Also used for
   mapping the available stack segments. */

extern struct pthread_handle_struct __pthread_handles[PTHREAD_THREADS_MAX];

/* Descriptor of the main thread */

extern pthread_descr __pthread_main_thread;

/* File descriptor for sending requests to the thread manager.
   Initially -1, meaning that __pthread_initialize_manager must be called. */

extern int __pthread_manager_request;

/* Other end of the pipe for sending requests to the thread manager. */

extern int __pthread_manager_reader;

#ifdef FLOATING_STACKS
/* Maximum stack size.  */
extern size_t __pthread_max_stacksize;
#endif

/* Pending request for a process-wide exit */

extern int __pthread_exit_requested, __pthread_exit_code;

/* Set to 1 by gdb if we're debugging */

extern volatile int __pthread_threads_debug;

/* Globally enabled events.  */
extern volatile td_thr_events_t __pthread_threads_events;

/* Pointer to descriptor of thread with last event.  */
extern volatile pthread_descr __pthread_last_event;

/* Flag which tells whether we are executing on SMP kernel. */
extern int __pthread_smp_kernel;

/* Return the handle corresponding to a thread id */

static inline pthread_handle thread_handle(pthread_t id)
{
  return &__pthread_handles[id % PTHREAD_THREADS_MAX];
}

/* Validate a thread handle. Must have acquired h->h_spinlock before. */

static inline int invalid_handle(pthread_handle h, pthread_t id)
{
  return h->h_descr == NULL || h->h_descr->p_tid != id || h->h_descr->p_terminated;
}

static inline int nonexisting_handle(pthread_handle h, pthread_t id)
{
  return h->h_descr == NULL || h->h_descr->p_tid != id;
}

/* Fill in defaults left unspecified by pt-machine.h.  */

/* We round up a value with page size. */
#ifndef page_roundup
#define page_roundup(v,p) ((((size_t) (v)) + (p) - 1) & ~((p) - 1))
#endif

/* The page size we can get from the system.  This should likely not be
   changed by the machine file but, you never know.  */
extern size_t __pagesize;
#include <bits/uClibc_page.h>
#ifndef PAGE_SIZE
#define PAGE_SIZE  (sysconf (_SC_PAGE_SIZE))
#endif

/* The initial size of the thread stack.  Must be a multiple of PAGE_SIZE.  */
#ifndef INITIAL_STACK_SIZE
#define INITIAL_STACK_SIZE  (4 * __pagesize)
#endif

/* Size of the thread manager stack. The "- 32" avoids wasting space
   with some malloc() implementations. */
#ifndef THREAD_MANAGER_STACK_SIZE
#define THREAD_MANAGER_STACK_SIZE  (2 * __pagesize - 32)
#endif

/* The base of the "array" of thread stacks.  The array will grow down from
   here.  Defaults to the calculated bottom of the initial application
   stack.  */
#ifndef THREAD_STACK_START_ADDRESS
#define THREAD_STACK_START_ADDRESS  __pthread_initial_thread_bos
#endif

/* If MEMORY_BARRIER isn't defined in pt-machine.h, assume the
   architecture doesn't need a memory barrier instruction (e.g. Intel
   x86).  Still we need the compiler to respect the barrier and emit
   all outstanding operations which modify memory.  Some architectures
   distinguish between full, read and write barriers.  */

#ifndef MEMORY_BARRIER
#define MEMORY_BARRIER() asm ("" : : : "memory")
#endif
#ifndef READ_MEMORY_BARRIER
#define READ_MEMORY_BARRIER() MEMORY_BARRIER()
#endif
#ifndef WRITE_MEMORY_BARRIER
#define WRITE_MEMORY_BARRIER() MEMORY_BARRIER()
#endif

/* Max number of times we must spin on a spinlock calling sched_yield().
   After MAX_SPIN_COUNT iterations, we put the calling thread to sleep. */

#ifndef MAX_SPIN_COUNT
#define MAX_SPIN_COUNT 50
#endif

/* Max number of times the spinlock in the adaptive mutex implementation
   spins actively on SMP systems.  */

#ifndef MAX_ADAPTIVE_SPIN_COUNT
#define MAX_ADAPTIVE_SPIN_COUNT 100
#endif

/* Duration of sleep (in nanoseconds) when we can't acquire a spinlock
   after MAX_SPIN_COUNT iterations of sched_yield().
   With the 2.0 and 2.1 kernels, this MUST BE > 2ms.
   (Otherwise the kernel does busy-waiting for realtime threads,
    giving other threads no chance to run.) */

#ifndef SPIN_SLEEP_DURATION
#define SPIN_SLEEP_DURATION 2000001
#endif

/* Debugging */

#ifdef DEBUG
#include <assert.h>
#define ASSERT assert
#define MSG __pthread_message
#else
#define ASSERT(x)
#define MSG(msg,arg...)
#endif

/* Internal global functions */

void __pthread_destroy_specifics(void);
void __pthread_perform_cleanup(void);
int __pthread_initialize_manager(void);
void __pthread_message(char * fmt, ...);
int __pthread_manager(void *reqfd);
int __pthread_manager_event(void *reqfd);
void __pthread_manager_sighandler(int sig);
void __pthread_reset_main_thread(void);
void __fresetlockfiles(void);
void __pthread_manager_adjust_prio(int thread_prio);
void __pthread_initialize_minimal (void);

extern int __pthread_attr_setguardsize __P ((pthread_attr_t *__attr,
					     size_t __guardsize));
extern int __pthread_attr_getguardsize __P ((__const pthread_attr_t *__attr,
					     size_t *__guardsize));
extern int __pthread_attr_setstackaddr __P ((pthread_attr_t *__attr,
					     void *__stackaddr));
extern int __pthread_attr_getstackaddr __P ((__const pthread_attr_t *__attr,
					     void **__stackaddr));
extern int __pthread_attr_setstacksize __P ((pthread_attr_t *__attr,
					     size_t __stacksize));
extern int __pthread_attr_getstacksize __P ((__const pthread_attr_t *__attr,
					     size_t *__stacksize));
extern int __pthread_getconcurrency __P ((void));
extern int __pthread_setconcurrency __P ((int __level));
extern int __pthread_mutexattr_gettype __P ((__const pthread_mutexattr_t *__attr,
					     int *__kind));
extern void __pthread_kill_other_threads_np __P ((void));

extern void __pthread_restart_old(pthread_descr th);
extern void __pthread_suspend_old(pthread_descr self);
extern int __pthread_timedsuspend_old(pthread_descr self, const struct timespec *abs);

extern void __pthread_restart_new(pthread_descr th);
extern void __pthread_suspend_new(pthread_descr self);
extern int __pthread_timedsuspend_new(pthread_descr self, const struct timespec *abs);

extern void __pthread_wait_for_restart_signal(pthread_descr self);

/* Global pointers to old or new suspend functions */

extern void (*__pthread_restart)(pthread_descr);
extern void (*__pthread_suspend)(pthread_descr);

/* Prototypes for the function without cancelation support when the
   normal version has it.  */
extern int __libc_close (int fd);
extern int __libc_nanosleep (const struct timespec *requested_time,
			     struct timespec *remaining);
extern ssize_t __libc_read (int fd, void *buf, size_t count);
extern pid_t __libc_waitpid (pid_t pid, int *stat_loc, int options);
extern ssize_t __libc_write (int fd, const void *buf, size_t count);

/* Prototypes for some of the new semaphore functions.  */
extern int __new_sem_post (sem_t * sem);

/* The functions called the signal events.  */
extern void __linuxthreads_create_event (void);
extern void __linuxthreads_death_event (void);
extern void __linuxthreads_reap_event (void);

#endif /* internals.h */
