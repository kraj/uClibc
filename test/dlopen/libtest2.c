#include <stdio.h>
#include <pthread.h>


extern int __pthread_mutex_init (void);

void __attribute__((constructor)) libtest2_ctor(void)
{
    printf("I am the libtest2 constructor!\n");
}

void __attribute__((destructor)) libtest2_dtor(void)
{
    printf("I am the libtest2 destructor!\n");
}

void function1(void)
{
    printf("libtest2: I am overriding function1!\n");
}

void __attribute__((weak)) function2(void)
{
    printf("libtest2: I am function2 from libtest2!\n");
}


int libtest2_func(const char *s)
{
    function1();
    function2();
    printf( "libtest2: function1 = %p\n"
	    "libtest2: function2 = %p\n"
	    "libtest2: message from main() = '%s'\n",
	    function1, function2, s);
    return 0;
}


