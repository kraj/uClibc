/* vi: set sw=4 ts=4: */
/*
 * sigaction() for Xtensa uClibc
 *
 * Copyright (C) 2007, 2008 Tensilica Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>
#include <string.h>
#include <bits/kernel_sigaction.h>

#define SA_RESTORER	0x04000000

extern void __default_sa_restorer (void);

/* Experimentally off - libc_hidden_proto(memcpy) */

int __libc_sigaction (int signum, const struct sigaction *act,
					  struct sigaction *oldact)
{
	struct kernel_sigaction kact, koldact;
	int result;

	if (act) {
		kact.k_sa_handler = act->sa_handler;
		memcpy(&kact.sa_mask, &act->sa_mask, sizeof (kact.sa_mask));
		kact.sa_flags = act->sa_flags;

		if (kact.sa_flags & SA_RESTORER) {
			kact.sa_restorer = act->sa_restorer;
		} else {
			kact.sa_restorer = __default_sa_restorer;
			kact.sa_flags |= SA_RESTORER;
		}
	}

	result = __syscall_rt_sigaction(signum, act ? __ptrvalue (&kact) : NULL,
									oldact ? __ptrvalue (&koldact) : NULL,
									_NSIG / 8);

	if (oldact && result >= 0) {
		oldact->sa_handler = koldact.k_sa_handler;
		memcpy(&oldact->sa_mask, &koldact.sa_mask, sizeof(oldact->sa_mask));
		oldact->sa_flags = koldact.sa_flags;
		oldact->sa_restorer = koldact.sa_restorer;
	}

	return result;
}

#ifndef LIBC_SIGACTION
libc_hidden_proto (sigaction)
weak_alias (__libc_sigaction, sigaction)
libc_hidden_weak (sigaction)
#endif
