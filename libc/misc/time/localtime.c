
#include <time.h>
#include <sys/time.h>

/* Our static data lives in __time_static.c */
extern struct tm __tmb;


extern void __tm_conv();

struct tm *localtime(__const time_t *timep)
{
    struct timezone tz;
    time_t offt;

    gettimeofday((void *) 0, &tz);

    offt = -tz.tz_minuteswest * 60L;

    /* tmb.tm_isdst = ? */
    __tm_conv(&__tmb, timep, offt);

    return &__tmb;
}
