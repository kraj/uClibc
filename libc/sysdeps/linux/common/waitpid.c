#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

__pid_t waitpid(__pid_t pid, int *wait_stat, int options)
{
    return wait4(pid, wait_stat, options, NULL);
}
