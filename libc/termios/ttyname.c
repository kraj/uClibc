#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

static char * __check_dir_for_tty_match(char * dirname, struct stat *st)
{
    DIR *fp;
    struct stat dst;
    struct dirent *d;
    static char name[NAME_MAX];

    fp = opendir(dirname);
    if (fp == 0)
	return 0;
    strcpy(name, dirname);
    strcat(name, "/");

    while ((d = readdir(fp)) != 0) {
	strcpy(name + strlen(dirname) + 1, d->d_name);
	if (stat(name, &dst) == 0
		&& st->st_dev == dst.st_dev && st->st_ino == dst.st_ino) {
	    closedir(fp);
	    return name;
	}
    }
    closedir(fp);
    return NULL;
}

/* This is a failly slow approach.  We do a linear search through
 * some directories looking for a match.  Yes this is lame.  But 
 * it should work, should be small, and will return names that match
 * what is on disk.  
 *
 * Another approach we could use would be to use the info in /proc/self/fd */
char *ttyname(fd)
int fd;
{
    char *the_name = NULL;
    struct stat st;
    int noerr = errno;

    if (fstat(fd, &st) < 0)
	return 0;

    if (!isatty(fd)) {
	noerr = ENOTTY;
	goto cool_found_it;
    }

    /* Lets try /dev/vc first (be devfs compatible) */
    if ( (the_name=__check_dir_for_tty_match("/dev/vc", &st))) 
	goto cool_found_it;

    /* Lets try /dev/tts next (be devfs compatible) */
    if ( (the_name=__check_dir_for_tty_match("/dev/tts", &st))) 
	goto cool_found_it;

    /* Lets try /dev/pts next */
    if ( (the_name=__check_dir_for_tty_match("/dev/pts", &st))) 
	goto cool_found_it;

    /* Lets try walking through /dev last */
    if ( (the_name=__check_dir_for_tty_match("/dev", &st))) 
	goto cool_found_it;

cool_found_it:
    __set_errno(noerr);
    return the_name;
}
