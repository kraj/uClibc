
#include <time.h>

extern void __tm_conv();

/* Our static data lives in __time_static.c */
extern struct tm __tmb;

struct tm *gmtime(__const time_t *timep)
{
    __tm_conv(&__tmb, timep, 0L);
    return &__tmb;
}
