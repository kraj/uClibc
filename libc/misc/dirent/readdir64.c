#include <features.h>
#ifdef __UCLIBC_HAVE_LFS__
#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS != 64 
#undef _FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS   64
#endif
#ifndef __USE_LARGEFILE64
# define __USE_LARGEFILE64	1
#endif
/* We absolutely do _NOT_ want interfaces silently
 * renamed under us or very bad things will happen... */
#ifdef __USE_FILE_OFFSET64
# undef __USE_FILE_OFFSET64
#endif
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "dirstream.h"

extern int getdents64 __P ((unsigned int fd, struct dirent64 *dirp, unsigned int count));


struct dirent64 *readdir64(DIR * dir)
{
	int result;
	struct dirent64 *de;

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
		result = getdents64(dir->dd_fd, dir->dd_buf, dir->dd_max);

		/* We assume we have getdents64 (). */
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

	de = (struct dirent64 *) (((char *) dir->dd_buf) + dir->dd_nextloc);

	/* Am I right? H.J. */
	dir->dd_nextloc += de->d_reclen;

	/* We have to save the next offset here. */
	dir->dd_nextoff = de->d_off;

	return de;
}

#endif /* __UCLIBC_HAVE_LFS__ */
