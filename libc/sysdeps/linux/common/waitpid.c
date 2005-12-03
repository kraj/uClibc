#define wait4 __wait4

#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

__pid_t attribute_hidden __waitpid(__pid_t pid, int *wait_stat, int options)
{
    return __wait4(pid, wait_stat, options, NULL);
}
strong_alias(__waitpid,waitpid)
weak_alias(__waitpid,__libc_waitpid)
