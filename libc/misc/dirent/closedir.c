#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "dirstream.h"

int attribute_hidden __closedir(DIR * dir)
{
	int fd;

	if (!dir) {
		__set_errno(EBADF);
		return -1;
	}

	/* We need to check dd_fd. */
	if (dir->dd_fd == -1) {
		__set_errno(EBADF);
		return -1;
	}
	__pthread_mutex_lock(&(dir->dd_lock));
	fd = dir->dd_fd;
	dir->dd_fd = -1;
	__pthread_mutex_unlock(&(dir->dd_lock));
	free(dir->dd_buf);
	free(dir);
	return __close(fd);
}
strong_alias(__closedir,closedir)
