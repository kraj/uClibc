#include <stdio.h>
#include <errno.h>

/*
 * Manuel Novoa III           Feb 2001
 *
 * Replaced old version that did write(2,...)'s with a version using
 * stream functions.  If the program is calling perror, it's a safe
 * bet that printf and friends are used as well.  It is also possible
 * that the calling program could buffer stderr, or reassign it.
 * Also, the old version did not conform the standards when the 
 * passed char * was either NULL or pointed to an empty string.
 */

void perror(__const char *str)
{
	static const char perror_str[] = ": ";
	const char *sep;

	sep = perror_str;
	if (!(str && *str)) {		/* Caller did not supply a prefix message */
		sep += 2;				/* or passed an empty string. */
		str = sep;
	}
	fprintf(stderr, "%s%s%s\n", str, sep, strerror(errno));
}
