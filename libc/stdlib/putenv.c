/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

#define ADD_NUM 4

int putenv (char *var)
{
	static char **mall_env = 0;
	static int extras = 0;
	char **p, **d;
	char *r;
	int len;

	r = strchr(var, '=');
	if (r == 0)
		len = strlen(var);
	else
		len = r - var;

	if (!__environ) {
		__environ = (char **) malloc(ADD_NUM * sizeof(char *));
		memset(__environ, 0, sizeof(char *) * ADD_NUM);

		extras = ADD_NUM;
	}

	for (p = __environ; *p; p++) {
		if (memcmp(var, *p, len) == 0 && (*p)[len] == '=') {
			while ((p[0] = p[1]))
				p++;
			extras++;
			break;
		}
	}
	if (r == 0)
		return 0;
	if (extras <= 0) {			/* Need more space */
		d = malloc((p - __environ + 1 + ADD_NUM) * sizeof(char *));

		if (d == 0)
			return -1;

		memcpy((void *) d, (void *) __environ,

			   (p - __environ + 1) * sizeof(char *));
		p = d + (p - __environ);
		extras = ADD_NUM;

		if (mall_env)
			free(mall_env);
		__environ = d;
		mall_env = d;
	}
	*p++ = strdup((char *) var);
	*p = '\0';
	extras--;

	return 0;
}
