#include <fcntl.h>
#include <sys/stat.h>

#ifndef TST_FALLOCATE64
# define stat64 stat
# define fstat64 fstat
# else
# ifndef O_LARGEFILE
#  error no O_LARGEFILE but you want to test with LFS enabled
# endif
#endif

static void do_prepare(void);
static int do_test(void);
#define PREPARE(argc, argv) do_prepare ()
#define TEST_FUNCTION do_test ()
#include <test-skeleton.c>

static int fd;
static void
do_prepare (void)
{
  fd = create_temp_file ("tst-fallocate.", NULL);
  if (fd == -1)
    {
      printf ("cannot create temporary file: %m\n");
      exit (1);
    }
}


static int
do_test (void)
{
  struct stat64 st;
  int c;
  char garbage[4096];
  blkcnt_t blksb4;

  if (fstat64 (fd, &st) != 0)
    {
      puts ("1st fstat failed");
      return 1;
    }

  if (st.st_size != 0)
    {
      puts ("file not created with size 0");
      return 1;
    }

  /* This is the default mode which is identical to posix_fallocate().
     Note: we need a few extra blocks for FALLOC_FL_PUNCH_HOLE below.
     While block sizes vary, we'll assume eight 4K blocks for good measure. */
  if (fallocate (fd, 0, 8 * 4096, 128) != 0)
    {
      puts ("1st fallocate call failed");
      return 1;
    }

  if (fstat64 (fd, &st) != 0)
    {
      puts ("2nd fstat failed");
      return 1;
    }

  if (st.st_size != 8 * 4096 + 128)
    {
      printf ("file size after 1st fallocate call is %llu, expected %u\n",
	      (unsigned long long int) st.st_size, 8u * 4096u + 128u);
      return 1;
    }

  /* Without FALLOC_FL_KEEP_SIZE, this would increaste the size of the file. */
  if (fallocate (fd, FALLOC_FL_KEEP_SIZE, 0, 16 * 4096) != 0)
    {
      puts ("2nd fallocate call failed");
      return 1;
    }

  if (fstat64 (fd, &st) != 0)
    {
      puts ("3rd fstat failed");
      return 1;
    }

  if (st.st_size != 8 * 4096 + 128)
    {
      printf ("file size changed in 2nd fallocate call to %llu, expected %u\n",
	      (unsigned long long int) st.st_size, 8u * 4096u + 128u);
      return 1;
    }

  /* Let's fill up the first eight 4k blocks with 'x' to force some allocations. */

  memset(garbage, 'x', 4096);
  for(c=0; c < 8; c++)
    if(write(fd, garbage, 4096) == -1)
      {
        puts ("write failed");
        return 1;
      }

  if (fstat64 (fd, &st) != 0)
    {
      puts ("4th fstat failed");
      return 1;
    }

  blksb4 = st.st_blocks;

  /* Let's punch a hole in the entire file, turning it effectively into a sparse file. */
  if (fallocate (fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, 0, 8 * 4096 + 128) != 0)
    {
      puts ("3rd fallocate call failed");
      return 1;
    }

  if (fstat64 (fd, &st) != 0)
    {
      puts ("5th fstat failed");
      return 1;
    }

  if (st.st_size != 8 * 4096 + 128)
    {
      printf ("file size after 3rd fallocate call is %llu, expected %u\n",
	      (unsigned long long int) st.st_size, 8u * 4096u + 128u);
      return 1;
    }

  /* The number of allocated blocks should decrease.  I hope this works on
     all filesystems! */
  if (st.st_blocks >= blksb4)
    {
      printf ("number of blocks after 3rd fallocate call is %lu, expected less than %lu\n",
	      (unsigned long int) st.st_blocks, blksb4);
      return 1;
    }

#ifdef TST_FALLOCATE64
  /* We'll just do a mode = 0 test for fallocate64() */
  if (fallocate64 (fd, 0, 4097ULL, 4294967295ULL + 2ULL) != 0)
    {
      puts ("1st fallocate64 call failed");
      return 1;
    }

  if (fstat64 (fd, &st) != 0)
    {
      puts ("6th fstat failed");
      return 1;
    }

  if (st.st_size != 4097ULL + 4294967295ULL + 2ULL)
    {
      printf ("file size after 1st fallocate64 call is %llu, expected %llu\n",
	      (unsigned long long int) st.st_size, 4097ULL + 4294967295ULL + 2ULL);
      return 1;
    }
#endif
  close (fd);

  return 0;
}

