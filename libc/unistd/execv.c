
#include <unistd.h>

extern char **environ;

int execv(__const char *path, char *__const argv[])
{
	return execve(path, argv, environ);
}
