/* Copyright (C) 2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2006.

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

#include <signal.h>
#include <sys/syscall.h>
#include <sys/poll.h>
#define __need_NULL
#include <stddef.h>

#if defined __NR_ppoll && defined __UCLIBC_LINUX_SPECIFIC__
int
ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout,
       const sigset_t *sigmask)
{
	/* The Linux kernel can in some situations update the timeout value.
	   We do not want that so use a local variable.  */
	struct timespec tval;
	if (timeout != NULL) {
		tval = *timeout;
		timeout = &tval;
	}

	return INLINE_SYSCALL(ppoll, 5, fds, nfds, timeout, sigmask, _NSIG / 8);
}
libc_hidden_def(ppoll)
#endif
