#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

static int __check_dir_for_tty_match(char * dirname, struct stat *st, char *buf, size_t buflen)
{
    DIR *fp;
    int len;
    struct stat dst;
    struct dirent *d;

    fp = opendir(dirname);
    if (fp == NULL)
	return errno;
    strncpy(buf, dirname, buflen);
    strncat(buf, "/", buflen);
    len = strlen(dirname) + 1;

    while ((d = readdir(fp)) != 0) {
	strncpy(buf+len, d->d_name, buflen);
	buf[buflen]='\0';
	if (stat(buf, &dst) == 0 && st->st_dev == dst.st_dev 
		&& st->st_ino == dst.st_ino) 
	{
	    closedir(fp);
	    return 0;
	}
    }
    closedir(fp);
    return ENOTTY;
}

/* This is a fairly slow approach.  We do a linear search through some
 * directories looking for a match.  Yes this is lame.  But it should
 * work, should be small, and will return names that match what is on
 * disk.  Another approach we could use would be to use the info in
 * /proc/self/fd, but that is even more lame since it requires /proc */

char *ttyname(int fd)
{
    static char name[NAME_MAX];
    ttyname_r(fd, name, NAME_MAX);
    return(name);
}

int ttyname_r(int fd, char *buf, size_t buflen)
{
    int noerr;
    struct stat st;

    noerr = errno;
    if (!buf) {
	noerr = EINVAL;
	goto cool_found_it;
    }
    /* Make sure we have enough space to return "/dev/pts/0" */
    if (buflen < 10) {
	noerr = ERANGE;
	goto cool_found_it;
    }
    if (!isatty (fd)) {
	noerr = ENOTTY;
	goto cool_found_it;
    }
    if (fstat(fd, &st) < 0)
	return errno;
    if (!isatty(fd)) {
	noerr = ENOTTY;
	goto cool_found_it;
    }

    /* Lets try /dev/vc first (be devfs compatible) */
    if ( (noerr=__check_dir_for_tty_match("/dev/vc", &st, buf, buflen))) 
	goto cool_found_it;

    /* Lets try /dev/tts next (be devfs compatible) */
    if ( (noerr=__check_dir_for_tty_match("/dev/tts", &st, buf, buflen))) 
	goto cool_found_it;

    /* Lets try /dev/pts next */
    if ( (noerr=__check_dir_for_tty_match("/dev/pts", &st, buf, buflen))) 
	goto cool_found_it;

    /* Lets try walking through /dev last */
    if ( (noerr=__check_dir_for_tty_match("/dev", &st, buf, buflen))) 
	goto cool_found_it;

cool_found_it:
    __set_errno(noerr);
    return noerr;
}

