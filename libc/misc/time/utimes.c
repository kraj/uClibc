#include <stdlib.h>
#include <utime.h>
#include <sys/time.h>

int utimes (const char *file, const struct timeval tvp[2])
{
	struct utimbuf buf, *times;

	if (tvp) {
		times = &buf;
		times->actime = tvp[0].tv_sec;
		times->modtime = tvp[1].tv_sec;
	}
	else
		times = NULL;
	return utime(file, times);
}
