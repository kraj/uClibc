/* Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sysdep.h>
#include <lowlevelcond.h>
#include <tcb-offsets.h>

#ifdef UP
# define LOCK
#else
# define LOCK lock
#endif

#define FUTEX_WAIT		0
#define FUTEX_WAKE		1


	.text

/* int pthread_cond_wait (pthread_cond_t *cond, pthread_mutex_t *mutex)  */
	.globl	__pthread_cond_wait
	.type	__pthread_cond_wait, @function
	.align	16
__pthread_cond_wait:
.LSTARTCODE:

	pushl	%edi
.Lpush_edi:
	pushl	%esi
.Lpush_esi:
	pushl	%ebx
.Lpush_ebx:

	xorl	%esi, %esi
	movl	16(%esp), %ebx

	/* Get internal lock.  */
	movl	$1, %edx
	xorl	%eax, %eax
	LOCK
#if cond_lock == 0
	cmpxchgl %edx, (%ebx)
#else
	cmpxchgl %edx, cond_lock(%ebx)
#endif
	jnz	1f

	/* Store the reference to the mutex.  If there is already a
	   different value in there this is a bad user bug.  */
2:	cmpl	$-1, dep_mutex(%ebx)
	movl	20(%esp), %eax
	je	15f
	movl	%eax, dep_mutex(%ebx)

	/* Unlock the mutex.  */
15:	xorl	%edx, %edx
	call	__pthread_mutex_unlock_usercnt

	testl	%eax, %eax
	jne	12f

	addl	$1, total_seq(%ebx)
	adcl	$0, total_seq+4(%ebx)
	addl	$1, cond_futex(%ebx)
	addl	$(1 << clock_bits), cond_nwaiters(%ebx)

#define FRAME_SIZE 16
	subl	$FRAME_SIZE, %esp
.Lsubl:

	/* Get and store current wakeup_seq value.  */
	movl	wakeup_seq(%ebx), %edi
	movl	wakeup_seq+4(%ebx), %edx
	movl	broadcast_seq(%ebx), %eax
	movl	%edi, 4(%esp)
	movl	%edx, 8(%esp)
	movl	%eax, 12(%esp)

8:	movl	cond_futex(%ebx), %edi

	/* Unlock.  */
	LOCK
#if cond_lock == 0
	subl	$1, (%ebx)
#else
	subl	$1, cond_lock(%ebx)
#endif
	jne	3f

.LcleanupSTART:
4:	call	__pthread_enable_asynccancel
	movl	%eax, (%esp)

	movl	%esi, %ecx	/* movl $FUTEX_WAIT, %ecx */
	movl	%edi, %edx
	addl	$cond_futex, %ebx
.Ladd_cond_futex:
	movl	$SYS_futex, %eax
	ENTER_KERNEL
	subl	$cond_futex, %ebx
.Lsub_cond_futex:

	movl	(%esp), %eax
	call	__pthread_disable_asynccancel
.LcleanupEND:

	/* Lock.  */
	movl	$1, %edx
	xorl	%eax, %eax
	LOCK
#if cond_lock == 0
	cmpxchgl %edx, (%ebx)
#else
	cmpxchgl %edx, cond_lock(%ebx)
#endif
	jnz	5f

6:	movl	broadcast_seq(%ebx), %eax
	cmpl	12(%esp), %eax
	jne	16f

	movl	woken_seq(%ebx), %eax
	movl	woken_seq+4(%ebx), %ecx

	movl	wakeup_seq(%ebx), %edi
	movl	wakeup_seq+4(%ebx), %edx

	cmpl	8(%esp), %edx
	jne	7f
	cmpl	4(%esp), %edi
	je	8b

7:	cmpl	%ecx, %edx
	jne	9f
	cmp	%eax, %edi
	je	8b

9:	addl	$1, woken_seq(%ebx)
	adcl	$0, woken_seq+4(%ebx)

	/* Unlock */
16:	subl	$(1 << clock_bits), cond_nwaiters(%ebx)

	/* Wake up a thread which wants to destroy the condvar object.  */
	movl	total_seq(%ebx), %eax
	andl	total_seq+4(%ebx), %eax
	cmpl	$0xffffffff, %eax
	jne	17f
	movl	cond_nwaiters(%ebx), %eax
	andl	$~((1 << clock_bits) - 1), %eax
	jne	17f

	addl	$cond_nwaiters, %ebx
	movl	$SYS_futex, %eax
	movl	$FUTEX_WAKE, %ecx
	movl	$1, %edx
	ENTER_KERNEL
	subl	$cond_nwaiters, %ebx

17:	LOCK
#if cond_lock == 0
	subl	$1, (%ebx)
#else
	subl	$1, cond_lock(%ebx)
#endif
	jne	10f

11:	movl	20+FRAME_SIZE(%esp), %eax
	call	__pthread_mutex_cond_lock
	addl	$FRAME_SIZE, %esp
.Laddl:

14:	popl	%ebx
.Lpop_ebx:
	popl	%esi
.Lpop_esi:
	popl	%edi
.Lpop_edi:

	/* We return the result of the mutex_lock operation.  */
	ret

	/* Initial locking failed.  */
1:
.LSbl1:
#if cond_lock == 0
	movl	%ebx, %ecx
#else
	leal	cond_lock(%ebx), %ecx
#endif
	call	__lll_mutex_lock_wait
	jmp	2b

	/* Unlock in loop requires waekup.  */
3:
.LSbl2:
#if cond_lock == 0
	movl	%ebx, %eax
#else
	leal	cond_lock(%ebx), %eax
#endif
	call	__lll_mutex_unlock_wake
	jmp	4b

	/* Locking in loop failed.  */
5:
#if cond_lock == 0
	movl	%ebx, %ecx
#else
	leal	cond_lock(%ebx), %ecx
#endif
	call	__lll_mutex_lock_wait
	jmp	6b

	/* Unlock after loop requires wakeup.  */
10:
#if cond_lock == 0
	movl	%ebx, %eax
#else
	leal	cond_lock(%ebx), %eax
#endif
	call	__lll_mutex_unlock_wake
	jmp	11b

	/* The initial unlocking of the mutex failed.  */
12:
.LSbl3:
	LOCK
#if cond_lock == 0
	subl	$1, (%ebx)
#else
	subl	$1, cond_lock(%ebx)
#endif
	jne	14b

	movl	%eax, %esi
#if cond_lock == 0
	movl	%ebx, %eax
#else
	leal	cond_lock(%ebx), %eax
#endif
	call	__lll_mutex_unlock_wake

	movl	%esi, %eax
	jmp	14b
	.size	__pthread_cond_wait, .-__pthread_cond_wait
weak_alias(__pthread_cond_wait, pthread_cond_wait)


	.type	__condvar_w_cleanup2, @function
__condvar_w_cleanup2:
	subl	$cond_futex, %ebx
	.size	__condvar_w_cleanup2, .-__condvar_w_cleanup2
.LSbl4:
	.type	__condvar_w_cleanup, @function
__condvar_w_cleanup:
	movl	%eax, %esi

	/* Get internal lock.  */
	movl	$1, %edx
	xorl	%eax, %eax
	LOCK
#if cond_lock == 0
	cmpxchgl %edx, (%ebx)
#else
	cmpxchgl %edx, cond_lock(%ebx)
#endif
	jz	1f

#if cond_lock == 0
	movl	%ebx, %ecx
#else
	leal	cond_lock(%ebx), %ecx
#endif
	call	__lll_mutex_lock_wait

1:	movl	broadcast_seq(%ebx), %eax
	cmpl	12(%esp), %eax
	jne	3f

	addl	$1, wakeup_seq(%ebx)
	adcl	$0, wakeup_seq+4(%ebx)

	addl	$1, cond_futex(%ebx)

	addl	$1, woken_seq(%ebx)
	adcl	$0, woken_seq+4(%ebx)

3:	subl	$(1 << clock_bits), cond_nwaiters(%ebx)

	/* Wake up a thread which wants to destroy the condvar object.  */
	xorl	%edi, %edi
	movl	total_seq(%ebx), %eax
	andl	total_seq+4(%ebx), %eax
	cmpl	$0xffffffff, %eax
	jne	4f
	movl	cond_nwaiters(%ebx), %eax
	andl	$~((1 << clock_bits) - 1), %eax
	jne	4f

	addl	$cond_nwaiters, %ebx
	movl	$SYS_futex, %eax
	movl	$FUTEX_WAKE, %ecx
	movl	$1, %edx
	ENTER_KERNEL
	subl	$cond_nwaiters, %ebx
	movl	$1, %edi

4:	LOCK
#if cond_lock == 0
	subl	$1, (%ebx)
#else
	subl	$1, cond_lock(%ebx)
#endif
	je	2f

#if cond_lock == 0
	movl	%ebx, %eax
#else
	leal	cond_lock(%ebx), %eax
#endif
	call	__lll_mutex_unlock_wake

	/* Wake up all waiters to make sure no signal gets lost.  */
2:	testl	%edi, %edi
	jnz	5f
	addl	$cond_futex, %ebx
	movl	$FUTEX_WAKE, %ecx
	movl	$SYS_futex, %eax
	movl	$0x7fffffff, %edx
	ENTER_KERNEL

5:	movl	20+FRAME_SIZE(%esp), %eax
	call	__pthread_mutex_cond_lock

	movl	%esi, (%esp)
.LcallUR:
	call	_Unwind_Resume
	hlt
.LENDCODE:
	.size	__condvar_w_cleanup, .-__condvar_w_cleanup
