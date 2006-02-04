#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define _DTIFY(DT) [DT] #DT
const char const *types[] = {
	_DTIFY(DT_UNKNOWN),
	_DTIFY(DT_FIFO),
	_DTIFY(DT_CHR),
	_DTIFY(DT_DIR),
	_DTIFY(DT_BLK),
	_DTIFY(DT_REG),
	_DTIFY(DT_LNK),
	_DTIFY(DT_SOCK),
	_DTIFY(DT_WHT)
};

int main(int argc, char **argv)
{
	DIR *dirh;
	struct dirent *de;
	struct dirent64 *de64;
	const char const mydir[] = ".";

	if ((dirh = opendir(mydir)) == NULL) {
		perror("opendir");
		return 1;
	}

	printf("readdir() says:\n");
	while ((de = readdir(dirh)) != NULL)
		printf("\tdir entry %s: %s\n", types[de->d_type], de->d_name);

	rewinddir(dirh);

	printf("readdir64() says:\n");
	while ((de64 = readdir64(dirh)) != NULL)
		printf("\tdir entry %s: %s\n", types[de64->d_type], de64->d_name);

	closedir(dirh);


	return 0;
}
