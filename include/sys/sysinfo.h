#ifndef _SYS_SYSINFO_H
#define _SYS_SYSINFO_H

#include <features.h>

#include <linux/kernel.h>

__BEGIN_DECLS

extern int sysinfo __P((struct sysinfo *__info));
extern int __sysinfo __P((struct sysinfo *__info));

__END_DECLS

#endif
