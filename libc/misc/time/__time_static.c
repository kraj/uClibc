
#include <time.h>

/* These globals are exported by the C library */
char *__tzname[2] = { (char *) "GMT", (char *) "GMT" };
int __daylight = 0;
long int __timezone = 0L;
/* Grumble */
weak_alias (__tzname, tzname);
weak_alias (__daylight, daylight);
weak_alias (__timezone, timezone);


/* static data for gmtime() and localtime() */
struct tm __tmb;


