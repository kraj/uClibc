/* unistd.h <ndf@linux.mit.edu> */
#include <features.h>
#include <sys/types.h>

#ifndef __UNISTD_H
#define __UNISTD_H

#include <errno.h>
#ifdef __mc68000__
#include <asm/unistd.h>
#endif
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


/* NULL-terminated array of "NAME=VALUE" environment variables.  */
extern char **__environ;
extern char **environ;


extern int close __P ((int));
extern size_t read __P ((int __fd, char * __buf, size_t __nbytes));
extern size_t write __P ((int __fd, __const char * __buf, size_t __n));
extern off_t lseek __P ((int __fd, off_t __n, int __whence));
extern int pipe __P ((int __pipedes[2]));
extern unsigned int alarm __P ((unsigned int __seconds));
extern int sleep __P ((unsigned int __seconds));
extern void usleep __P ((unsigned long __microseconds));
extern int pause __P ((void));
extern char*    crypt __P((__const char *__key, __const char *__salt));
extern int isatty __P ((int __fd));
extern int readlink __P ((__const char *__path, char *__buf, size_t __len));
extern int link __P ((__const char *__from, __const char *__to));
extern int symlink __P ((__const char *__from, __const char *__to));
extern int readlink __P ((__const char *__path, char *__buf, size_t __len));
extern int unlink __P ((__const char *__name));
extern char *getcwd __P ((char *__buf, size_t __size));
/* Duplicate FD, returning a new file descriptor on the same file.  */
extern int dup __P ((int __fd));
/* Duplicate FD to FD2, closing FD2 and making it open on the same file.  */
extern int dup2 __P ((int __fd, int __fd2));
extern int fchdir __P ((int __fd));
extern int chdir __P ((__const char *__path));
extern int chown __P ((__const char *__file,
                       uid_t __owner, gid_t __group));

extern int fchown __P ((int __fd,
                       uid_t __owner, gid_t __group));

extern int fsync __P ((int __fd));

extern int sync __P ((void));

extern int rmdir __P ((__const char *__path));

extern int rename __P((__const char* _old, __const char* _new));

extern int access __P ((__const char *__name, int __type));

extern int      reboot __P ((int __magic, int __magic_too, int __flag));

extern int mkstemp __P ((char * __template));
extern char * mktemp __P ((char * __template));

extern int _clone __P ((int (*fn)(void *arg), void *child_stack, int flags, void *arg));


/* Make PATH be the root directory (the starting point for absolute paths).
   This call is restricted to the super-user.  */
extern int chroot __P ((__const char *__path));




/* Execute PATH with all arguments after PATH until
   a NULL pointer and environment from `environ'.  */
extern int execl __P ((__const char *__path, __const char *__arg, ...));

/* Execute FILE, searching in the `PATH' environment variable if
   it contains no slashes, with all arguments after FILE until a
   NULL pointer and environment from `environ'.  */
extern int execlp __P ((__const char *__file, __const char *__arg, ...));

/* Execute PATH with arguments ARGV and environment from `environ'.  */
extern int execv __P ((__const char *__path, char *__const __argv[]));

/* Replace the current process, executing PATH with arguments ARGV and
   environment ENVP.  ARGV and ENVP are terminated by NULL pointers.  */
extern int execve __P ((__const char *__path, char *__const __argv[],
			char *__const __envp[]));
/* Execute FILE, searching in the `PATH' environment variable if it contains
   no slashes, with arguments ARGV and environment from `environ'.  */
extern int execvp __P ((__const char *__file, char *__const __argv[]));


/* Execute PATH with arguments ARGV and environment ENVP. */
extern int execvep __P ((__const char *path, char *__const __argv[], 
	    char *__const __envp[]));

/* Terminate program execution with the low-order 8 bits of STATUS.  */
extern void _exit __P ((int __status)) __attribute__ ((__noreturn__));

/* Clone the calling process, creating an exact copy.
 *    Return -1 for errors, 0 to the new process,
 *       and the process ID of the new process to the old process.  */
extern __pid_t __fork __P ((void));
extern __pid_t fork __P ((void));

/* Clone the calling process, but without copying the whole address space.
 *    The calling process is suspended until the new process exits or is
 *       replaced by a call to `execve'.  Return -1 for errors, 0 to the new process,
 *          and the process ID of the new process to the old process.  */
extern __pid_t vfork __P ((void));


#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#ifndef R_OK
#define	R_OK	4		/* Test for read permission.  */
#define	W_OK	2		/* Test for write permission.  */
#define	X_OK	1		/* Test for execute permission.  */
#define	F_OK	0		/* Test for existence.  */
#endif

#endif /* __UNISTD_H */


