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

/* Return the current machine's Internet number.  */
extern long int gethostid __P ((void));

/* Put the name of the current host in no more than LEN bytes of NAME.
   The result is null-terminated if LEN is large enough for the full
   name and the terminator.  */
extern int gethostname __P ((char *__name, size_t __len));

/* Set the name of the current host to NAME, which is LEN bytes long.
   This call is restricted to the super-user.  */
extern int sethostname __P ((__const char *__name, size_t __len));


/* Set the user ID of the calling process to UID.
   If the calling process is the super-user, set the real
   and effective user IDs, and the saved set-user-ID to UID;
   if not, the effective user ID is set to UID.  */
extern int setuid __P ((__uid_t __uid));

/* Create a new session with the calling process as its leader.
   The process group IDs of the session and the calling process
   are set to the process ID of the calling process, which is returned.  */
extern __pid_t setsid __P ((void));

/* Get the real user ID of the calling process.  */
extern __uid_t getuid __P ((void));

/* Get the effective user ID of the calling process.  */
extern __uid_t geteuid __P ((void));

/* Get the real group ID of the calling process.  */
extern __gid_t getgid __P ((void));

/* Get the effective group ID of the calling process.  */
extern __gid_t getegid __P ((void));

/* If SIZE is zero, return the number of supplementary groups
   the calling process is in.  Otherwise, fill in the group IDs
   of its supplementary groups in LIST and return the number written.  */
extern int getgroups __P ((int __size, __gid_t __list[]));



/* Return the foreground process group ID of FD.  */
extern __pid_t tcgetpgrp __P ((int __fd));
/* Set the foreground process group ID of FD set PGRP_ID.  */
extern int tcsetpgrp __P ((int __fd, __pid_t __pgrp_id));

/* Get the process group ID of the calling process.
   This function is different on old BSD. */
extern __pid_t getpgrp __P ((void));
/* Set the process group ID of the process matching PID to PGID.
   If PID is zero, the current process's process group ID is set.
   If PGID is zero, the process ID of the process is used.  */
extern int setpgid __P ((__pid_t __pid, __pid_t __pgid));

/* Get the process ID of the calling process.  */
extern __pid_t getpid __P ((void));




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

/* Return the number of bytes in a page.  This is the system's page size,
   which is not necessarily the same as the hardware page size.  */
extern int getpagesize __P ((void)) __attribute__ ((__const__));

#ifdef	__USE_POSIX2
/* Get definitions and prototypes for functions to process the
   arguments in ARGV (ARGC of them, minus the program name) for
   options given in OPTS.  */
# define __need_getopt
# include <getopt.h>
#endif


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


