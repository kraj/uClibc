#define _XOPEN_SOURCE_EXTENDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#if !defined __ARCH_USE_MMU__
# define fork vfork
#endif

int main(int argc, char *argv[]) {
    int fd, status;
    char buff[5];
    char template[] = "/tmp/test-mkostemp.XXXXXX";

    fd = mkostemp(template, O_CLOEXEC);
    unlink(template);

    snprintf(buff, 5, "%d", fd);

    if(!fork())
        if(execl("./test-mkostemp-child", "test-mkostemp-child", buff, NULL) == -1)
            exit(EXIT_FAILURE);

    wait(&status);

    memset(buff, 0, 5);
    lseek(fd, 0, SEEK_SET);
    errno = 0;
    if(read(fd, buff, 5) == -1)
        exit(EXIT_FAILURE);

    if(!strncmp(buff, "test", 5))
        exit(EXIT_FAILURE);
    else
        exit(EXIT_SUCCESS);

    close(fd);
    exit(EXIT_SUCCESS);
}
