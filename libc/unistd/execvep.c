
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int execvep(const char *path, char *__const argv[], char *__const envp[])
{
	if (!strchr(path, '/')) {
		char partial[128];
		char *p = getenv("PATH");
		char *p2;

		if (!p)
			p = "/bin:/usr/bin";

		for (; p && *p;) {

			strcpy(partial, p);

			p2 = strchr(partial, ':');
			if (p2)
				*p2 = '\0';

			if (strlen(partial))
				strcat(partial, "/");
			strcat(partial, path);

			execve(partial, argv, envp);

			if (errno != ENOENT)
				return -1;

			p2 = strchr(p, ':');
			if (p2)
				p = p2 + 1;
			else
				p = 0;
		}
		return -1;
	} else
		return execve(path, argv, envp);
}
