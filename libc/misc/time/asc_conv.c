
#include <time.h>
#include <string.h>
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
 */

void __asctime(buffer, ptm)
register char *buffer;
struct tm *ptm;
{
	static const char days[] = "SunMonTueWedThuFriSat";
	static const char mons[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	/*                              012345678901234567890123456 */
	static const char template[] = "Err Err 00 00:00:00 0000\n";
	int tm_field[4];
	int tmp, i;
	char *p;

	/* Since we need memcpy below, use it here instead of strcpy. */
	memcpy(buffer, template, sizeof(template));

	if ((ptm->tm_wday >= 0) && (ptm->tm_wday <= 6)) {
		memcpy(buffer, days + 3 * (ptm->tm_wday), 3);
	}

	if ((ptm->tm_mon >= 0) && (ptm->tm_mon <= 11)) {
		memcpy(buffer + 4, mons + 3 * (ptm->tm_mon), 3);
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
