#include <syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

static inline
_syscall4(__pid_t,wait4,__pid_t,pid,__WAIT_STATUS,status,int,options,struct rusage *,ru)

__pid_t wait(__WAIT_STATUS wait_stat)
{
	return wait4((-1) /* WAIT_ANY */, wait_stat, 0, NULL);
}
