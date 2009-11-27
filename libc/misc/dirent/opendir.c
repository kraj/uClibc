/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <dirent.h>
#include "dirstream.h"

libc_hidden_proto(opendir)
libc_hidden_proto(open)
libc_hidden_proto(fcntl)
libc_hidden_proto(close)
libc_hidden_proto(stat)
libc_hidden_proto(fstat)

static DIR *fd_to_DIR(int fd, __blksize_t size)
{
	DIR *ptr;

	ptr = malloc(sizeof(*ptr));
	if (!ptr)
		return NULL;

	ptr->dd_fd = fd;
	ptr->dd_nextloc = ptr->dd_size = ptr->dd_nextoff = 0;
	ptr->dd_max = size;
	if (ptr->dd_max < 512)
		ptr->dd_max = 512;

	ptr->dd_buf = calloc(1, ptr->dd_max);
	if (!ptr->dd_buf) {
		free(ptr);
		return NULL;
	}
	__pthread_mutex_init(&ptr->dd_lock, NULL);

	return ptr;
}

DIR *fdopendir(int fd)
{
	int flags;
	struct stat st;

	if (fstat(fd, &st))
		return NULL;
	if (!S_ISDIR(st.st_mode)) {
		__set_errno(ENOTDIR);
		return NULL;
	}

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return NULL;
	if ((flags & O_ACCMODE) == O_WRONLY) {
		__set_errno(EINVAL);
		return NULL;
	}

	return fd_to_DIR(fd, st.st_blksize);
}

/* opendir just makes an open() call - it return NULL if it fails
 * (open sets errno), otherwise it returns a DIR * pointer.
 */
DIR *opendir(const char *name)
{
	int fd;
	struct stat statbuf;
	DIR *ptr;

#ifndef O_DIRECTORY
	/* O_DIRECTORY is linux specific and has been around since like 2.1.x */
	if (stat(name, &statbuf))
		return NULL;
	if (!S_ISDIR(statbuf.st_mode)) {
		__set_errno(ENOTDIR);
		return NULL;
	}
# define O_DIRECTORY 0
#endif
	if ((fd = open(name, O_RDONLY|O_NDELAY|O_DIRECTORY)) < 0)
		return NULL;

	/* Note: we should check to make sure that between the stat() and open()
	 * call, 'name' didnt change on us, but that's only if O_DIRECTORY isnt
	 * defined and since Linux has supported it for like ever, i'm not going
	 * to worry about it right now (if ever). */
	if (fstat(fd, &statbuf) < 0)
		goto close_and_ret;

	/* According to POSIX, directory streams should be closed when
	 * exec. From "Anna Pluzhnikov" <besp@midway.uchicago.edu>.
	 */
	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
		int saved_errno;
close_and_ret:
		saved_errno = errno;
		close(fd);
		__set_errno(saved_errno);
		return NULL;
	}

	ptr = fd_to_DIR(fd, statbuf.st_blksize);
	if (!ptr) {
		close(fd);
		__set_errno(ENOMEM);
	}
	return ptr;
}
libc_hidden_def(opendir)
