
#include <time.h>
#include <sys/time.h>

char * ctime_r(const time_t *t, char *buf)
{
    struct tm tm;
    return asctime_r(localtime_r(t, &tm), buf);
} 

