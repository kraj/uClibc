/* vi: set sw=4 ts=4: */
/*
 * epoll_create() / epoll_ctl() / epoll_wait() / epoll_pwait() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/epoll.h>

#ifdef __NR_epoll_create
_syscall1(int, epoll_create, int, size)
#endif

#ifdef __NR_epoll_create1
_syscall1(int, epoll_create1, int, flags)
#endif

#ifdef __NR_epoll_ctl
_syscall4(int, epoll_ctl, int, epfd, int, op, int, fd, struct epoll_event *, event)
#endif

#ifdef __NR_epoll_wait
_syscall4(int, epoll_wait, int, epfd, struct epoll_event *, events, int, maxevents, int, timeout)
/* TODO: add cancellation for epoll_wait */
#endif
