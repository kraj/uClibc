
#include <unistd.h>

extern char **environ;

int execvp(__const char *path, char *__const argv[])
{
	return execvep(path, argv, environ);
}
