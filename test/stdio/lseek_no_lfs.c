#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    FILE * f = fopen(argv[0], "rb");
    if (!f)
      {
	printf("Error: Can't open %s, reason: %s\n", argv[0], strerror(errno));
	return 1;
      }

    if (fseek(f, (unsigned)4096, (int)SEEK_SET) == -1)
      {
	printf("Test failed, fseek return fail code. errno=%u (%s)\n", errno, strerror(errno));
	return 1;
      }

    fclose(f);
    return 0;
}
