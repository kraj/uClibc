#include <string.h>

static char *save = 0;

char *strtok(char *s, const char *delim)
{
	return __strtok_r(s, delim, &save);
}
