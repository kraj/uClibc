#define setpgid __setpgid

#include <syscall.h>
#include <unistd.h>

int setpgrp(void)
{
	return setpgid(0,0);
}
