#ifndef __ERRNO_H
#define __ERRNO_H

#include <features.h>
#include <linux/errno.h>

extern int sys_nerr;
extern const char *const sys_errlist[];

#define _sys_nerr sys_nerr
#define _sys_errlist sys_errlist

extern int	errno;
extern void	perror __P ((__const char* __s));
extern char*	strerror __P ((int __errno));

#endif
