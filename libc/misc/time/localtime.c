
#include <time.h>
#include <sys/time.h>

/* These globals are exported by the C library */
char *__tzname[2] = { (char *) "GMT", (char *) "GMT" };
int __daylight = 0;
long int __timezone = 0L;
weak_alias (__tzname, tzname);
weak_alias (__daylight, daylight);
weak_alias (__timezone, timezone);


extern void __tm_conv();

struct tm *localtime(timep)
__const time_t *timep;
{
	static struct tm tmb;
	struct timezone tz;
	time_t offt;

	gettimeofday((void *) 0, &tz);

	offt = -tz.tz_minuteswest * 60L;

	/* tmb.tm_isdst = ? */
	__tm_conv(&tmb, timep, offt);

	return &tmb;
}
