/* Trivial implementation for arches that lack vfork */
#include <unistd.h>
#include <sys/types.h>

libc_hidden_proto(fork)

pid_t vfork(void)
{
    return fork();
}
libc_hidden_proto(vfork)
libc_hidden_def(vfork)
