#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <linux/ioctl.h>
#include <features.h>
#include <termios.h>
#include <sys/socketio.h>

__BEGIN_DECLS

extern int	ioctl __P ((int __fildes, int __cmd, ...));
extern int	__ioctl __P ((int __fildes, int __cmd, ...));

__END_DECLS


#endif
