#include <dirent.h>
#include <errno.h>

int skip_all(const struct dirent *dirbuf)
{
	__set_errno(EBADF);
	return 0;
}

int main()
{
	struct dirent **namelist;
	int n;

	n = scandir(".", &namelist, skip_all, 0);
	if (n < 0) {
		perror("scandir");
		return 1;
	}
	return 0;
}
