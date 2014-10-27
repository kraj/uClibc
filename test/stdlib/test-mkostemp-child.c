#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;

    /* This file gets built and run as a test, but its
     * really just a helper for test-mkostemp-O_CLOEXEC.c.
     * So, we'll always return succcess.
     */
    if(argc != 2)
        exit(EXIT_SUCCESS);

    sscanf(argv[1], "%d", &fd);

    if(write(fd, "test\0", 5) == -1)
        ; /* Don't Panic!  Failure is okay here. */

    close(fd);
    exit(EXIT_SUCCESS);
}
