#ifndef _ERRNO_H
#define _ERRNO_H

#include <features.h>
#include <bits/errno.h>

extern int sys_nerr;
extern const char *const sys_errlist[];

#define _sys_nerr sys_nerr
#define _sys_errlist sys_errlist

#ifndef	errno
extern int	errno;
#endif
extern void	perror __P ((__const char* __s));
extern char*	strerror __P ((int __errno));


#endif
