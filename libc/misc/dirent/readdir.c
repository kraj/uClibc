#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "dirstream.h"

#undef readdir
struct dirent attribute_hidden *__readdir(DIR * dir)
{
	ssize_t bytes;
	struct dirent *de;

	if (!dir) {
		__set_errno(EBADF);
		return NULL;
	}

	__pthread_mutex_lock(&(dir->dd_lock));

	do {
	    if (dir->dd_size <= dir->dd_nextloc) {
		/* read dir->dd_max bytes of directory entries. */
		bytes = __getdents(dir->dd_fd, dir->dd_buf, dir->dd_max);
		if (bytes <= 0) {
		    de = NULL;
		    goto all_done;
		}
		dir->dd_size = bytes;
		dir->dd_nextloc = 0;
	    }

	    de = (struct dirent *) (((char *) dir->dd_buf) + dir->dd_nextloc);

	    /* Am I right? H.J. */
	    dir->dd_nextloc += de->d_reclen;

	    /* We have to save the next offset here. */
	    dir->dd_nextoff = de->d_off;

	    /* Skip deleted files.  */
	} while (de->d_ino == 0);

all_done:
	__pthread_mutex_unlock(&(dir->dd_lock));
	return de;
}
strong_alias(__readdir,readdir)
