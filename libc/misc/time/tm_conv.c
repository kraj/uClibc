
/* This is adapted from glibc */
/* Copyright (C) 1991, 1993 Free Software Foundation, Inc */

#define SECS_PER_HOUR 3600L
#define SECS_PER_DAY  86400L

#include <features.h>
#include <time.h>
#include <sys/types.h>

/* This structure contains all the information about a
   timezone given in the POSIX standard TZ envariable.  */
typedef struct
{
    const char *name;

    /* When to change.  */
    enum { J0, J1, M } type;	/* Interpretation of:  */
    unsigned short int m, n, d;	/* Month, week, day.  */
    unsigned int secs;		/* Time of day.  */

    long int offset;		/* Seconds east of GMT (west if < 0).  */

    /* We cache the computed time of change for a
       given year so we don't have to recompute it.  */
    time_t change;	/* When to change to this zone.  */
    int computed_for;	/* Year above is computed for.  */
} tz_rule;

/* tz_rules[0] is standard, tz_rules[1] is daylight.  */
static tz_rule tz_rules[2];

/* Warning -- this function is a stub andd always does UTC
 * no matter what it is given */
void tzset (void)
{
    tz_rules[0].name = tz_rules[1].name = "UTC";
    tz_rules[0].type = tz_rules[1].type = J0;
    tz_rules[0].m = tz_rules[0].n = tz_rules[0].d = 0;
    tz_rules[1].m = tz_rules[1].n = tz_rules[1].d = 0;
    tz_rules[0].secs = tz_rules[1].secs = 0;
    tz_rules[0].offset = tz_rules[1].offset = 0L;
    tz_rules[0].change = tz_rules[1].change = (time_t) -1;
    tz_rules[0].computed_for = tz_rules[1].computed_for = 0;
}


static const unsigned short int __mon_lengths[2][12] = {
	/* Normal years.  */
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	/* Leap years.  */
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

void __tm_conv(struct tm *tmbuf, time_t *t, time_t offset)
{
	int isdst;
	long days, rem;
	register int y;
	register const unsigned short int *ip;

	timezone = -offset;

	days = *t / SECS_PER_DAY;
	rem = *t % SECS_PER_DAY;
	rem += offset;
	while (rem < 0) {
		rem += SECS_PER_DAY;
		--days;
	}
	while (rem >= SECS_PER_DAY) {
		rem -= SECS_PER_DAY;
		++days;
	}
	tmbuf->tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	tmbuf->tm_min = rem / 60;
	tmbuf->tm_sec = rem % 60;
	/* January 1, 1970 was a Thursday.  */
	tmbuf->tm_wday = (4 + days) % 7;
	if (tmbuf->tm_wday < 0)
		tmbuf->tm_wday += 7;
	y = 1970;
	while (days >= (rem = __isleap(y) ? 366 : 365)) {
		++y;
		days -= rem;
	}
	while (days < 0) {
		--y;
		days += __isleap(y) ? 366 : 365;
	}
	tmbuf->tm_year = y - 1900;
	tmbuf->tm_yday = days;
	ip = __mon_lengths[__isleap(y)];
	for (y = 0; days >= ip[y]; ++y)
		days -= ip[y];
	tmbuf->tm_mon = y;
	tmbuf->tm_mday = days + 1;
	isdst = (*t >= tz_rules[0].change && *t < tz_rules[1].change);
	tmbuf->tm_isdst = isdst;
	tmbuf->tm_zone = tzname[isdst];

}

