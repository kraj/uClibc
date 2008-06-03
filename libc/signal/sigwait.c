/* vi: set sw=4 ts=4: */
/* sigwait
 *
 * Copyright (C) 2003 by Erik Andersen <andersen@uclibc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.  */

#include <errno.h>
#include <signal.h>
#include <string.h>

#if defined __UCLIBC_HAS_REALTIME__
libc_hidden_proto(sigwaitinfo)

int __sigwait (const sigset_t *set, int *sig) attribute_hidden;
int __sigwait (const sigset_t *set, int *sig)
{
	int ret = 1;
	if ((ret = sigwaitinfo(set, NULL)) != -1) {
		*sig = ret;
		return 0;
	}
	return 1;
}
#else /* __UCLIBC_HAS_REALTIME__ */
/* variant without REALTIME extensions */
libc_hidden_proto(sigfillset)
libc_hidden_proto(sigaction)
libc_hidden_proto(sigsuspend)

static int was_sig; /* obviously not thread-safe */
static void ignore_signal(int sig)
{
	was_sig = sig;
}
int __sigwait (const sigset_t *set, int *sig) attribute_hidden;
int __sigwait (const sigset_t *set, int *sig)
{
  sigset_t tmp_mask;
  struct sigaction saved[NSIG];
  struct sigaction action;
  int save_errno;
  int this;

  /* Prepare set.  */
  sigfillset (&tmp_mask);

  /* Unblock all signals in the SET and register our nice handler.  */
  action.sa_handler = ignore_signal;
  action.sa_flags = 0;
  sigfillset (&action.sa_mask);       /* Block all signals for handler.  */

  /* Make sure we recognize error conditions by setting WAS_SIG to a
     value which does not describe a legal signal number.  */
  was_sig = -1;

  for (this = 1; this < NSIG; ++this)
    if (__sigismember (set, this))
      {
        /* Unblock this signal.  */
        __sigdelset (&tmp_mask, this);

        /* Register temporary action handler.  */
        if (sigaction (this, &action, &saved[this]) != 0)
          goto restore_handler;
      }

  /* Now we can wait for signals.  */
  sigsuspend (&tmp_mask);

 restore_handler:
  save_errno = errno;

  while (--this >= 1)
    if (__sigismember (set, this))
      /* We ignore errors here since we must restore all handlers.  */
      sigaction (this, &saved[this], NULL);

  __set_errno (save_errno);

  /* Store the result and return.  */
  *sig = was_sig;
  return was_sig == -1 ? -1 : 0;
}
#endif /* __UCLIBC_HAS_REALTIME__ */
libc_hidden_proto(sigwait)
weak_alias(__sigwait,sigwait)
libc_hidden_def(sigwait)
