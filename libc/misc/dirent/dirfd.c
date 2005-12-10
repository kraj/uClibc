#include <dirent.h>
#include <errno.h>
#include "dirstream.h"

int attribute_hidden __dirfd(DIR * dir)
{
	if (!dir || dir->dd_fd == -1) {
		__set_errno(EBADF);
		return -1;
	}

	return dir->dd_fd;
}
strong_alias(__dirfd,dirfd)
