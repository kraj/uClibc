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
		errno = EBADF;
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
				errno = result;
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

	/* convert */
	dir->dd_buf->d_ino = de->d_ino;
	dir->dd_buf->d_off = de->d_off;
	dir->dd_buf->d_reclen = de->d_reclen;
	dir->dd_buf->d_type = 0;
	if (strlen((char *) &de->d_type) > 10)
		de->d_name[10] = 0;
	strcpy(dir->dd_buf->d_name, (char *) &de->d_name);
	errno = 0;

	return dir->dd_buf;
}
