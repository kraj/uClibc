
#include <time.h>
#include <errno.h>

extern void __asctime();


char * asctime (__const struct tm *timeptr)
{
    static char __time_buf[26];
    if (timeptr == NULL) {
	__set_errno (EINVAL);
	return NULL;
    }
    __asctime(__time_buf, timeptr);
    return __time_buf;
}

