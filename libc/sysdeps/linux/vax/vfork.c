#include <errno.h>
#include <features.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

libc_hidden_proto(vfork)
pid_t
vfork (void)
{
	return __fork ();
}
libc_hidden_def(vfork)

