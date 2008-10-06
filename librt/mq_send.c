/*
 * mq_send.c - functions for sending to message queue.
 */

#include <errno.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <mqueue.h>

#ifdef __NR_mq_timedsend
#define __NR___syscall_mq_timedsend __NR_mq_timedsend
static _syscall5(int, __syscall_mq_timedsend, int, mqdes,
		 const char *, msg_ptr, size_t, msg_len, unsigned int,
		 msg_prio, const void *, abs_timeout);

# if defined __USE_XOPEN2K && defined __UCLIBC_HAS_ADVANCED_REALTIME__
/*
 * Add a message to queue. If O_NONBLOCK is set and queue is full, wait
 * for sufficient room in the queue until abs_timeout expires.
 */
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
		 unsigned int msg_prio, const struct timespec *abs_timeout)
{
	return __syscall_mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio,
				      abs_timeout);
}
# endif

/* Add a message to queue */
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
	    unsigned int msg_prio)
{
	return __syscall_mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, NULL);
}
#elif defined __UCLIBC_HAS_STUBS__
# if defined __USE_XOPEN2K && defined __UCLIBC_HAS_ADVANCED_REALTIME__
int mq_timedsend(mqd_t mqdes attribute_unused, const char *msg_ptr attribute_unused,
		 size_t msg_len attribute_unused, unsigned int msg_prio attribute_unused,
		 const struct timespec *abs_timeout attribute_unused)
{
	__set_errno(ENOSYS);
	return -1;
}
# endif
int mq_send(mqd_t mqdes attribute_unused, const char *msg_ptr attribute_unused,
	    size_t msg_len attribute_unused, unsigned int msg_prio attribute_unused)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif
