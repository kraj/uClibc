//#include <errno.h>

int errno = 0;

int * __attribute__ ((__weak__)) __errno_location ( void )
{
  return &errno;
}

