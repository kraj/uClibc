#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

extern int __pthread_return_0(void);

void dltest(uint32_t **value1, uint32_t **value2)
{
	*value1 = (uint32_t *) __pthread_return_0;
	*value2 = (uint32_t *) pthread_self;
#if 0
	printf("dltest: __pthread_return_0=%p\n", __pthread_return_0);
	printf("dltest: pthread_self=%p\n", pthread_self);
#endif
}

