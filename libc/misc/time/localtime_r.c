
#include <time.h>

extern void __tm_conv();

struct tm *localtime_r(timep, tp)
__const time_t *timep;
struct tm *tp;
{
	struct timezone tz;
	time_t offt;

	gettimeofday((void *) 0, &tz);

	offt = -tz.tz_minuteswest * 60L;

	/* tmb.tm_isdst = ? */
	__tm_conv(tp, timep, offt);

	return tp;
}
