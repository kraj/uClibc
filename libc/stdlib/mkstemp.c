#include <string.h>
#include <features.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int mkstemp(template)
char *template;
{
	int i;
	int num __attribute__ ((unused));	/* UNINITIALIZED */
	int n2;
	int l = strlen(template);

	if (l < 6) {
		errno = EINVAL;
		return -1;
	}

	for (i = l - 6; i < l; i++)
		if (template[i] != 'X') {
			errno = EINVAL;
			return -1;
		}

  again:
	n2 = num;
	for (i = l - 1; i >= l - 6; i--) {
		template[i] = '0' + n2 % 10;
		n2 /= 10;
	}

	i = open(template, O_RDWR | O_EXCL | O_CREAT, 0666);

	if (i == -1) {
		if (errno == EEXIST) {
			num++;
			goto again;
		} else
			return -1;
	}

	return i;
}
