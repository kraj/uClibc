
#include <time.h>
#include <sys/time.h>

char * ctime (const time_t *t)
{

    /* According to IEEE Std 1003.1-2001: The ctime() function shall
     * convert the time pointed to by clock, representing time in
     * seconds since the Epoch, to local time in the form of a string.
     * It shall be equivalent to: asctime(localtime(clock)) */
    return asctime (localtime (t));
}

