#ifndef __UTIME_H
#define __UTIME_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

struct utimbuf {
	time_t actime;
	time_t modtime;
};

extern int utime __P ((const char *__filename, struct utimbuf *__utimebuf));

__END_DECLS

#endif

