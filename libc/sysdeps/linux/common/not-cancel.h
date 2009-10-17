/* By default we have none.  Map the name to the normal functions.  */
#define open_not_cancel(name, flags, mode) \
  open (name, flags, mode)
#define open_not_cancel_2(name, flags) \
  open (name, flags)
#define close_not_cancel(fd) \
  close (fd)
#define close_not_cancel_no_status(fd) \
  (void) close (fd)
#define read_not_cancel(fd, buf, n) \
  read (fd, buf, n)
#define write_not_cancel(fd, buf, n) \
  write (fd, buf, n)
#define writev_not_cancel_no_status(fd, iov, n) \
  (void) writev (fd, iov, n)
#define fcntl_not_cancel(fd, cmd, val) \
  fcntl (fd, cmd, val)
# define waitpid_not_cancel(pid, stat_loc, options) \
  waitpid (pid, stat_loc, options)
