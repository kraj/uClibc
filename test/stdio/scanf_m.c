#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	const char *buf = "hello world";
	char *ps = NULL, *pc = NULL;
	char s[6], c;

	/* Check that %[...]/%c work. */
	sscanf(buf, "%[a-z] %c", s, &c);
	/* Check that %m[...]/%mc work. */
	sscanf(buf, "%m[a-z] %mc", &ps, &pc);

	if (strcmp(ps, "hello") != 0 || *pc != 'w' ||
	    strcmp(s, "hello") != 0 || c != 'w')
		return 1;

	free(ps);
	free(pc);

	return 0;
}
