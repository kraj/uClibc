#include <stdio.h>

int howdy(const char *s)
{
	return printf("howdy: %s\n", s);
}

void __attribute__((constructor)) howdy_ctor(void)
{
	printf("I am the libhowdy constructor!\n");
}

void __attribute__((destructor)) howdy_dtor(void)
{
	printf("I am the libhowdy destructor!\n");
}



