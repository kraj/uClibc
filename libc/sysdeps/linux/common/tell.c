#include <errno.h>
#include <unistd.h>
#include <syscall.h>

off_t tell(int);

off_t tell (int fildes)
{
  return lseek (fildes, 0, SEEK_CUR);
}
