/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2007 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>

#if defined __USE_SVID || defined __USE_XOPEN
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/syscall.h>
# ifdef __NR_waitid
_syscall4(int, waitid, idtype_t, idtype, id_t, id, siginfo_t*, infop, int, options)
# else
#  include <string.h>
libc_hidden_proto(waitpid)
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
	switch (idtype) {
		case P_PID:
			if (id <= 0)
				goto invalid;
			break;
		case P_PGID:
			if (id < 0 || id == 1)
				goto invalid;
			id = -id;
			break;
		case P_ALL:
			id = -1;
			break;
		default:
		invalid:
			__set_errno(EINVAL);
			return -1;
	}

	memset(infop, 0, sizeof *infop);
	infop->si_pid = waitpid(id, &infop->si_status, options
#  ifdef WEXITED
					   &~ WEXITED
#  endif
					  );
	if (infop->si_pid < 0)
		return infop->si_pid;
	return 0;
}
# endif
#endif
