/* based originally on one the clone tests in the LTP */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>

int child_fn(void *arg)
{
	fprintf(stderr, "in child_fn\n");
	exit(1);
}

int main(void)
{
	int r_clone, ret_errno;

	r_clone = clone(child_fn, NULL, (int) NULL, NULL);
	ret_errno = errno;
	if (ret_errno != EINVAL) {
		fprintf(stderr, "clone: res=%d (%d) errno=%d %m\n",
			r_clone, (int) NULL, errno);
		return 1;
	}

	return 0;
}
