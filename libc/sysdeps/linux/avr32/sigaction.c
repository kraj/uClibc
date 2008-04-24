/*
 * Copyright (C) 2004-2007 Atmel Corporation
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License.  See the file "COPYING.LIB" in the main directory of this
 * archive for more details.
 */
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <bits/kernel_sigaction.h>

#define SA_RESTORER	0x04000000
extern void __default_rt_sa_restorer(void);

libc_hidden_proto(memcpy)

/*
 * If act is not NULL, change the action for sig to *act.
 * If oact is not NULL, put the old action for sig in *oact.
 */
int __libc_sigaction(int signum, const struct sigaction *act,
		     struct sigaction *oldact)
{
	struct kernel_sigaction kact, koact;
	int result;

	if (act) {
		kact.k_sa_handler = act->sa_handler;
		memcpy(&kact.sa_mask, &act->sa_mask, sizeof (kact.sa_mask));
		kact.sa_flags = act->sa_flags;
		if (kact.sa_flags & SA_RESTORER)
			kact.sa_restorer = act->sa_restorer;
		else
			kact.sa_restorer = __default_rt_sa_restorer;
		kact.sa_flags |= SA_RESTORER;
	}

	result = __syscall_rt_sigaction(signum, act ? __ptrvalue(&kact) : NULL,
					oldact ? __ptrvalue(&koact) : NULL,
					_NSIG / 8);

	if (oldact && result >= 0) {
		oldact->sa_handler = koact.k_sa_handler;
		memcpy(&oldact->sa_mask, &koact.sa_mask,
		       sizeof(oldact->sa_mask));
		oldact->sa_flags = koact.sa_flags;
		oldact->sa_restorer = koact.sa_restorer;
	}

	return result;
}

#ifndef LIBC_SIGACTION
libc_hidden_proto(sigaction)
weak_alias(__libc_sigaction, sigaction)
libc_hidden_weak(sigaction)
#endif
