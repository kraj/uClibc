/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2007 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>

#if defined __USE_SVID || defined __USE_XOPEN
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/syscall.h>

# ifdef __NR_waitid
/* The waitid() POSIX interface takes 4 arguments, but the kernel function
 * actually takes 5.  The fifth is a pointer to struct rusage.  Make sure
 * we pass NULL rather than letting whatever was in the register bleed up.
 */
#define __NR_waitid5 __NR_waitid
static _syscall5(int, waitid5, idtype_t, idtype, id_t, id, siginfo_t*, infop,
                 int, options, struct rusage*, ru)
# endif

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
# ifdef __NR_waitid
	return waitid5(idtype, id, infop, options, NULL);
# else
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
# endif
}

#endif
