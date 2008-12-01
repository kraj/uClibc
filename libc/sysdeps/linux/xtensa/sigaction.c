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
					  struct sigaction *oact)
{
	struct kernel_sigaction kact, koact;
	int result;
	enum {
		SIGSET_MIN_SIZE = sizeof(kact.sa_mask) < sizeof(act->sa_mask)
				? sizeof(kact.sa_mask) : sizeof(act->sa_mask)
	};

	if (act) {
		kact.k_sa_handler = act->sa_handler;
		memcpy(&kact.sa_mask, &act->sa_mask, SIGSET_MIN_SIZE);
		kact.sa_flags = act->sa_flags;

		if (kact.sa_flags & SA_RESTORER) {
			kact.sa_restorer = act->sa_restorer;
		} else {
			kact.sa_restorer = __default_sa_restorer;
			kact.sa_flags |= SA_RESTORER;
		}
	}

	/* NB: kernel (as of 2.6.25) will return EINVAL
	 * if sizeof(kact.sa_mask) does not match kernel's sizeof(sigset_t) */
	result = __syscall_rt_sigaction(signum,
			act ? __ptrvalue (&kact) : NULL,
			oact ? __ptrvalue (&koact) : NULL,
			sizeof(kact.sa_mask));

	if (oact && result >= 0) {
		oact->sa_handler = koact.k_sa_handler;
		memcpy(&oact->sa_mask, &koact.sa_mask, SIGSET_MIN_SIZE);
		oact->sa_flags = koact.sa_flags;
		oact->sa_restorer = koact.sa_restorer;
	}

	return result;
}

#ifndef LIBC_SIGACTION
libc_hidden_proto (sigaction)
weak_alias (__libc_sigaction, sigaction)
libc_hidden_weak (sigaction)
#endif
