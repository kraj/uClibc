
#include <time.h>
#include <errno.h>

extern void __asctime();

char *asctime_r(__const struct tm *timeptr, char *buf)
{
    if (timeptr == NULL || buf == NULL) {
	__set_errno (EINVAL);
	return NULL;
    }
    __asctime(buf, timeptr);
    return buf;
}

