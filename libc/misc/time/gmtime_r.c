
#include <time.h>

extern void __tm_conv();

struct tm *
gmtime_r(timep, tp)
__const time_t * timep;
struct tm * tp;
{
   __tm_conv(tp, timep, 0L);
   return tp;
}

