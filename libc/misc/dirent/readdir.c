#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "dirstream.h"

extern int getdents __P ((unsigned int fd, struct dirent *dirp, unsigned int count));


struct dirent *readdir(DIR * dir)
{
	int result;
	struct dirent *de;

	if (!dir) {
		__set_errno(EBADF);
		return NULL;
	}

	/* Are we running an old kernel? */
	if (dir->dd_getdents == no_getdents) {
		abort();
	}

	if (dir->dd_size <= dir->dd_nextloc) {
		/* read dir->dd_max bytes of directory entries. */
		result = getdents(dir->dd_fd, dir->dd_buf, dir->dd_max);

		/* We assume we have getdents (). */
		dir->dd_getdents = have_getdents;
		if (result <= 0) {
			result = -result;
			if (result > 0) {
				/* Are we right? */
				if (result == ENOSYS) {
					dir->dd_getdents = no_getdents;
					abort();
				}
				__set_errno(result);
			}

			return NULL;
		}

		dir->dd_size = result;
		dir->dd_nextloc = 0;
	}

	de = (struct dirent *) (((char *) dir->dd_buf) + dir->dd_nextloc);

	/* Am I right? H.J. */
	dir->dd_nextloc += de->d_reclen;

	/* We have to save the next offset here. */
	dir->dd_nextoff = de->d_off;

	__set_errno(0);

	return de;
}
