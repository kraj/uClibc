#ifndef	__PWD_H
#define	__PWD_H

#include <sys/types.h>
#include <features.h>
#include <stdio.h>

__BEGIN_DECLS

/* The passwd structure.  */
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Password.  */
  uid_t pw_uid;			/* User ID.  */
  gid_t pw_gid;			/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};


extern void setpwent __P ((void));
extern void endpwent __P ((void));
extern struct passwd * getpwent __P ((void));

extern int putpwent __P ((const struct passwd * __p, FILE * __f));
extern int getpw __P ((uid_t uid, char *buf));

extern struct passwd * fgetpwent __P ((FILE * file));

extern struct passwd * getpwuid __P ((const uid_t));
extern struct passwd * getpwnam __P ((const char *));


extern int getpwent_r __P ((struct passwd *__restrict __resultbuf,
			    char *__restrict __buffer, size_t __buflen,
			    struct passwd **__restrict __result));
extern int getpwuid_r __P ((uid_t __uid,
			    struct passwd *__restrict __resultbuf,
			    char *__restrict __buffer, size_t __buflen,
			    struct passwd **__restrict __result));
extern int getpwnam_r __P ((const char *__restrict __name,
			    struct passwd *__restrict __resultbuf,
			    char *__restrict __buffer, size_t __buflen,
			    struct passwd **__restrict __result));
extern int fgetpwent_r __P ((FILE *__restrict __stream,
			     struct passwd *__restrict __resultbuf,
			     char *__restrict __buffer, size_t __buflen,
			     struct passwd **__restrict __result));

#ifdef _LIBC
/* This is used internally to uClibc */
extern int __getpwent_r(struct passwd * passwd, char * line_buff, 
	size_t buflen, int pwd_fd);
#endif

__END_DECLS

#endif /* pwd.h  */



