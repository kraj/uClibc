#define lseek __normal_lseek
#include <errno.h>
#include <unistd.h>
#include <syscall.h>
#undef lseek

static inline
_syscall3(off_t,lseek,int,fildes,off_t,offset,int,origin)

off_t tell(int);

off_t
tell (int fildes)
{
  return lseek (fildes, 0, SEEK_CUR);
}
