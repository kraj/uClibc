#include <features.h>
#include <ctype.h>
#include <langinfo.h>
#include <string.h>
#include <time.h>

/*
 * Internal ascii conversion routine, avoid use of printf, it's a bit big!
 */

/*
 * Modified      Manuel Novoa III       Jan 2001
 *
 * Removed static function "hit" and did time-field fills inline and
 * put day, hour, min, and sec conversions in a loop using a small
 * table to reduce code size.
 *
 * Made daysp[] and mons[] const to move them from bss to text.
 *
 * Also fixed day conversion ... ANSI says no leading 0.
 *
 * Modified	Erik Andersen	    May 2002
 * Changed to optionally support real locales.
 *
 */

#ifdef __UCLIBC_HAS_LOCALE__
/* This is defined in locale/C-time.c in the GNU libc.  */
extern const struct locale_data _nl_C_LC_TIME;
#define __ab_weekday_name(DAY) (_nl_C_LC_TIME.values[_NL_ITEM_INDEX (ABDAY_1)+(DAY)].string)
#define __ab_month_name(MON)   (_nl_C_LC_TIME.values[_NL_ITEM_INDEX (ABMON_1)+(MON)].string)
#else
extern char const __ab_weekday_name[][4];
extern char const __ab_month_name[][4];
#define __ab_weekday_name(DAY) (__ab_weekday_name[DAY])
#define __ab_month_name(MON)   (__ab_month_name[MON])
#endif

void __asctime(register char *buffer, struct tm *ptm)
{
	char *p;
	int tmp, i, tm_field[4];
	/*                              012345678901234567890123456 */
	static const char template[] = "??? ??? 00 00:00:00 0000\n";

	/* Since we need memcpy below, use it here instead of strcpy. */
	memcpy(buffer, template, sizeof(template));

	if ((ptm->tm_wday >= 0) && (ptm->tm_wday <= 6)) {
		memcpy(buffer, __ab_weekday_name(ptm->tm_wday), 3);
	}

	if ((ptm->tm_mon >= 0) && (ptm->tm_mon <= 11)) {
		memcpy(buffer + 4, __ab_month_name(ptm->tm_mon), 3);
	}

	tm_field[0] = ptm->tm_mday;
	tm_field[1] = ptm->tm_hour;
	tm_field[2] = ptm->tm_min;
	tm_field[3] = ptm->tm_sec;

	p = buffer + 9;
	for (i=0 ; i<4 ; i++) {
		tmp = tm_field[i];
		*p-- += tmp % 10;
		*p += (tmp/10) % 10;
		p += 4 ;				/* skip to end of next field */
	}

	tmp = ptm->tm_year + 1900;
	p = buffer + 23;
	for (i=0 ; i<4 ; i++) {
		*p-- += tmp % 10;
		tmp /= 10;
	}

	if (buffer[8] == '0') {
		buffer[8] = ' ';
	}
}
