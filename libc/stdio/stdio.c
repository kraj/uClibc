/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/* This is an implementation of the C standard IO package.
 *
 * Updates:
 * 29-Sep-2000 W. Greathouse    1. fgetc copying beyond end of buffer
 *                              2. stdout needs flushed when input requested on
 *                                 stdin.
 *                              3. bufend was set incorrectly to 4 bytes beyond
 *                                 bufin (sizeof a pointer) instead of BUFSIZ.
 *                                 This resulted in 4 byte buffers for full
 *                                 buffered stdin and stdout and an 8 byte
 *                                 buffer for the unbuffered stderr!
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


#define FIXED_STREAMS 3
#define FIXED_BUFFERS 2

struct fixed_buffer {
	unsigned char data[BUFSIZ];
	unsigned char used;
};

extern FILE *__IO_list;			/* For fflush at exit */
extern FILE _stdio_streams[FIXED_STREAMS];
extern struct fixed_buffer _fixed_buffers[FIXED_BUFFERS];

#if defined L__fopen || defined L_fclose || defined L_setvbuf
extern unsigned char *_alloc_stdio_buffer(size_t size);
extern void _free_stdio_buffer(unsigned char *buf);
#endif

#if defined L__fopen || defined L_fclose
extern void _free_stdio_stream(FILE *fp);
#endif

#ifdef L__stdio_buffer
unsigned char *_alloc_stdio_buffer(size_t size)
{
	if (size == BUFSIZ) {
		int i;

		for (i = 0; i < FIXED_BUFFERS; i++)
			if (!_fixed_buffers[i].used) {
				_fixed_buffers[i].used = 1;
				return _fixed_buffers[i].data;
			}
	}
	return malloc(size);
}

void _free_stdio_buffer(unsigned char *buf)
{
	int i;

	for (i = 0; i < FIXED_BUFFERS; i++) {
		if (buf == _fixed_buffers[i].data) {
			_fixed_buffers[i].used = 0;
			return;
		}
	}
	free(buf);
}
#endif

#ifdef L__stdio_init

#if FIXED_BUFFERS < 2
#error FIXED_BUFFERS must be >= 2
#endif

#define bufin (_fixed_buffers[0].data)
#define bufout (_fixed_buffers[1].data)
#define buferr (_stdio_streams[2].unbuf)		/* Stderr is unbuffered */

struct fixed_buffer _fixed_buffers[FIXED_BUFFERS];

#if FIXED_STREAMS < 3
#error FIXED_STREAMS must be >= 3
#endif

FILE _stdio_streams[FIXED_STREAMS] = {
	{bufin, bufin, bufin, bufin, bufin + BUFSIZ,
	 0, _IOFBF | __MODE_READ | __MODE_FREEBUF,
	 _stdio_streams + 1},
	{bufout, bufout, bufout, bufout, bufout + BUFSIZ,
	 1, _IOFBF | __MODE_WRITE | __MODE_FREEBUF,
	 _stdio_streams + 2},
	{buferr, buferr, buferr, buferr, buferr + sizeof(buferr),
	 2, _IONBF | __MODE_WRITE,
	 0},
};

FILE *_stdin = _stdio_streams + 0;
FILE *_stdout = _stdio_streams + 1;
FILE *_stderr = _stdio_streams + 2;

/*
 * Note: the following forces linking of the __init_stdio function if
 * any of the stdio functions are used since they all call fflush directly
 * or indirectly.
 */
FILE *__IO_list = _stdio_streams;			/* For fflush at exit */

/* Call the stdio initiliser; it's main job it to call atexit */

void __stdio_close_all(void)
{
	FILE *fp;

	for (fp = __IO_list; fp; fp = fp->next) {
		fflush(fp);
		close(fp->fd);
	}
}

void __init_stdio(void)
{
#if (FIXED_BUFFERS > 2) || (FIXED_STREAMS > 3)
	int i;
#endif
#if FIXED_BUFFERS > 2
	for ( i = 2 ; i < FIXED_BUFFERS ; i++ ) {
		_fixed_buffers[i].used = 0;
	}
#endif
#if FIXED_STREAMS > 3
	for ( i = 3 ; i < FIXED_STREAMS ; i++ ) {
		_stdio_streams[i].fd = -1;
	}
#endif

	_fixed_buffers[0].used = 1;
	_fixed_buffers[1].used = 1;

	if (isatty(1)) {
		stdout->mode |= _IOLBF;
	}

	/* Cleanup is now taken care of in __uClibc_main. */
	/* atexit(__stdio_close_all); */
}
#endif

#ifdef L_fputc
int fputc(ch, fp)
int ch;
FILE *fp;
{
	register int v;

	v = fp->mode;
	/* If last op was a read ... */
	if ((v & __MODE_READING) && fflush(fp))
		return EOF;

	/* Can't write or there's been an EOF or error then return EOF */
	if ((v & (__MODE_WRITE | __MODE_EOF | __MODE_ERR)) != __MODE_WRITE)
		return EOF;

	/* Buffer is full */
	if (fp->bufpos >= fp->bufend && fflush(fp))
		return EOF;

	/* Right! Do it! */
	*(fp->bufpos++) = ch;
	fp->mode |= __MODE_WRITING;

	/* Unbuffered or Line buffered and end of line */
	if (((ch == '\n' && (v & _IOLBF)) || (v & _IONBF))
		&& fflush(fp))
		return EOF;

	/* Can the macro handle this by itself ? */
	if (v & (_IOLBF | _IONBF))
		fp->bufwrite = fp->bufstart;	/* Nope */
	else
		fp->bufwrite = fp->bufend;	/* Yup */

	/* Correct return val */
	return (unsigned char) ch;
}
#endif

#ifdef L_fgetc
int fgetc(fp)
FILE *fp;
{
	int ch;

	if (fp->mode & __MODE_WRITING)
		fflush(fp);

	if ( (fp == stdin) && (stdout->fd != -1) 
		 && (stdout->mode & __MODE_WRITING) ) 
	    fflush(stdout);

	/* Can't read or there's been an EOF or error then return EOF */
	if ((fp->mode & (__MODE_READ | __MODE_EOF | __MODE_ERR)) !=
		__MODE_READ) return EOF;

	/* Nothing in the buffer - fill it up */
	if (fp->bufpos >= fp->bufread) {
		fp->bufpos = fp->bufread = fp->bufstart;
		ch = fread(fp->bufpos, 1, fp->bufend - fp->bufstart, fp);
		if (ch == 0)
			return EOF;
		fp->bufread += ch;
		fp->mode |= __MODE_READING;
		fp->mode &= ~__MODE_UNGOT;
	}
	ch = *(fp->bufpos++);

	return ch;
}
#endif

#ifdef L_fflush
int fflush(fp)
FILE *fp;
{
	int len, cc, rv;
	char *bstart;

	rv = 0;
	if (fp == NULL) {			/* On NULL flush the lot. */
		for (fp = __IO_list; fp; fp = fp->next) {
			if (fflush(fp)) {
				rv = EOF;
			}
		}
		return rv;
	}

	/* If there's output data pending */
	if (fp->mode & __MODE_WRITING) {
		len = fp->bufpos - fp->bufstart;

		if (len) {
			bstart = fp->bufstart;
			/*
			 * The loop is so we don't get upset by signals or partial writes.
			 */
			do {
				cc = write(fp->fd, bstart, len);
				if (cc > 0) {
					bstart += cc;
					len -= cc;
				}
			}
			while (cc > 0 || (cc == -1 && errno == EINTR));
			/*
			 * If we get here with len!=0 there was an error, exactly what to
			 * do about it is another matter ...
			 *
			 * I'll just clear the buffer.
			 */
			if (len) {
				fp->mode |= __MODE_ERR;
				rv = EOF;
			}
		}
	}
	/* If there's data in the buffer sychronise the file positions */
	else if (fp->mode & __MODE_READING) {
		/* Humm, I think this means sync the file like fpurge() ... */
		/* Anyway the user isn't supposed to call this function when reading */

		len = fp->bufread - fp->bufpos;	/* Bytes buffered but unread */
		/* If it's a file, make it good */
		if (len > 0 && lseek(fp->fd, (long) -len, 1) < 0) {
			/* Hummm - Not certain here, I don't think this is reported */
			/*
			 * fp->mode |= __MODE_ERR; return EOF;
			 */
		}
	}

	/* All done, no problem */
	fp->mode &=
		(~(__MODE_READING | __MODE_WRITING | __MODE_EOF | __MODE_UNGOT));
	fp->bufread = fp->bufwrite = fp->bufpos = fp->bufstart;
	return rv;
}
#endif

#ifdef L_fgets
/* Nothing special here ... */
char *fgets(s, count, f)
char *s;
int count;
FILE *f;
{
	char *ret;
	register size_t i;
	register int ch;

	ret = s;
	for (i = count-1; i > 0; i--) {
		ch = getc(f);
		if (ch == EOF) {
			if (s == ret)
				return 0;
			break;
		}
		*s++ = (char) ch;
		if (ch == '\n')
			break;
	}
	*s = 0;

	if (ferror(f))
		return 0;
	return ret;
}
#endif

#ifdef L_gets
char *gets(str) /* BAD function; DON'T use it! */
char *str;
{
	/* Auwlright it will work but of course _your_ program will crash */
	/* if it's given a too long line */
	register char *p = str;
	register int c;

	while (((c = getc(stdin)) != EOF) && (c != '\n'))
		*p++ = c;
	*p = '\0';
	return (((c == EOF) && (p == str)) ? NULL : str);	/* NULL == EOF */
}
#endif

#ifdef L_fputs
int fputs(str, fp)
const char *str;
FILE *fp;
{
	register int n = 0;

	while (*str) {
		if (putc(*str++, fp) == EOF)
			return (EOF);
		++n;
	}
	return (n);
}
#endif

#ifdef L_puts
int puts(str)
const char *str;
{
	register int n;

	if (((n = fputs(str, stdout)) == EOF)
		|| (putc('\n', stdout) == EOF))
		return (EOF);
	return (++n);
}
#endif

#ifdef L_fread
/*
 * fread will often be used to read in large chunks of data calling read()
 * directly can be a big win in this case. Beware also fgetc calls this
 * function to fill the buffer.
 */
size_t fread(buf, size, nelm, fp)
void *buf;
size_t size;
size_t nelm;
FILE *fp;
{
	int len, v;
	unsigned bytes, got = 0;

	v = fp->mode;

	/* Want to do this to bring the file pointer up to date */
	if (v & __MODE_WRITING)
		fflush(fp);

	/* Can't read or there's been an EOF or error then return zero */
	if ((v & (__MODE_READ | __MODE_EOF | __MODE_ERR)) != __MODE_READ)
		return 0;

	/* This could be long, doesn't seem much point tho */
	bytes = size * nelm;

	len = fp->bufread - fp->bufpos;
	if (len >= bytes) {			/* Enough buffered */
		memcpy(buf, fp->bufpos, (unsigned) bytes);
		fp->bufpos += bytes;
		return bytes;
	} else if (len > 0) {		/* Some buffered */
		memcpy(buf, fp->bufpos, len);
		got = len;
	}

	/* Need more; do it with a direct read */
	len = read(fp->fd, buf + got, (unsigned) (bytes - got));

	/* Possibly for now _or_ later */
	if (len < 0) {
		fp->mode |= __MODE_ERR;
		len = 0;
	} else if (len == 0)
		fp->mode |= __MODE_EOF;

	return (got + len) / size;
}
#endif

#ifdef L_fwrite
/*
 * Like fread, fwrite will often be used to write out large chunks of
 * data; calling write() directly can be a big win in this case.
 * 
 * But first we check to see if there's space in the buffer.
 */
size_t fwrite(buf, size, nelm, fp)
const void *buf;
size_t size;
size_t nelm;
FILE *fp;
{
	register int v;
	int len;
	unsigned bytes, put;

	v = fp->mode;
	/* If last op was a read ... */
	if ((v & __MODE_READING) && fflush(fp))
		return 0;

	/* Can't write or there's been an EOF or error then return 0 */
	if ((v & (__MODE_WRITE | __MODE_EOF | __MODE_ERR)) != __MODE_WRITE)
		return 0;

	/* This could be long, doesn't seem much point tho */
	bytes = size * nelm;

	len = fp->bufend - fp->bufpos;

	/* Flush the buffer if not enough room */
	if (bytes > len)
		if (fflush(fp))
			return 0;

	len = fp->bufend - fp->bufpos;
	if (bytes <= len) {			/* It'll fit in the buffer ? */
		fp->mode |= __MODE_WRITING;
		memcpy(fp->bufpos, buf, bytes);
		fp->bufpos += bytes;

		/* If we're not fully buffered */
		if (v & (_IOLBF | _IONBF))
			fflush(fp);

		return nelm;
	} else
		/* Too big for the buffer */
	{
		put = bytes;
		do {
			len = write(fp->fd, buf, bytes);
			if (len > 0) {
				buf += len;
				bytes -= len;
			}
		}
		while (len > 0 || (len == -1 && errno == EINTR));

		if (len < 0)
			fp->mode |= __MODE_ERR;

		put -= bytes;
	}

	return put / size;
}
#endif

#ifdef L_rewind
void rewind(fp)
FILE *fp;
{
	fseek(fp, (long) 0, 0);
	clearerr(fp);
}
#endif

#ifdef L_fseek
int fseek(fp, offset, ref)
FILE *fp;
long offset;
int ref;
{
#if 0
	/* FIXME: this is broken!  BROKEN!!!! */
	/* if __MODE_READING and no ungetc ever done can just move pointer */
	/* This needs testing! */

	if ((fp->mode & (__MODE_READING | __MODE_UNGOT)) == __MODE_READING &&
		(ref == SEEK_SET || ref == SEEK_CUR)) {
		long fpos = lseek(fp->fd, 0L, SEEK_CUR);

		if (fpos == -1)
			return EOF;

		if (ref == SEEK_CUR) {
			ref = SEEK_SET;
			offset = fpos + offset + fp->bufpos - fp->bufread;
		}
		if (ref == SEEK_SET) {
			if (offset < fpos
				&& offset >= fpos + fp->bufstart - fp->bufread) {
				fp->bufpos = offset - fpos + fp->bufread;
				return 0;
			}
		}
	}
#endif

	/* Use fflush to sync the pointers */
	if (fflush(fp) || (lseek(fp->fd, offset, ref) < 0)) {
		return EOF;
	}
	return 0;
}
#endif

#ifdef L_ftell
long ftell(fp)
FILE *fp;
{
	if (fflush(fp) == EOF)
		return EOF;
	return lseek(fp->fd, 0L, SEEK_CUR);
}
#endif

#ifdef L__fopen
/*
 * This Fopen is all three of fopen, fdopen and freopen. The macros in
 * stdio.h show the other names.
 */
FILE *__fopen(fname, fd, fp, mode)
const char *fname;
int fd;
FILE *fp;
const char *mode;
{
	FILE *nfp;
	int open_mode;
	int fopen_mode;
	int i;

	fopen_mode = 0;

	/* If we've got an fp close the old one (freopen) */
	if (fp) {					/* We don't want to deallocate fp. */
		fopen_mode |=
			(fp->mode & (__MODE_BUF | __MODE_FREEFIL | __MODE_FREEBUF));
		fp->mode &= ~(__MODE_FREEFIL | __MODE_FREEBUF);
		fclose(fp);
	}

	/* decode the new open mode */
	switch (*mode++) {
		case 'r':				/* read */
			fopen_mode |= __MODE_READ;
			open_mode = O_RDONLY;
			break;
		case 'w':				/* write (create or truncate)*/
			fopen_mode |= __MODE_WRITE;
			open_mode = (O_WRONLY | O_CREAT | O_TRUNC);
			break;
		case 'a':				/* write (create or append) */
			fopen_mode |= __MODE_WRITE;
			open_mode = (O_WRONLY | O_CREAT | O_APPEND);
			break;
		default:				/* illegal mode */
			return 0;
	}

	if ((*mode == 'b')) {		/* binary mode (nop for uClibc) */
		++mode;
	}

	if (*mode == '+') {			/* read-write */
		++mode;
		fopen_mode |= __MODE_RDWR;
		open_mode &= ~(O_RDONLY | O_WRONLY);
		open_mode |= O_RDWR;
	}

	while (*mode) {				/* ignore everything else except ... */
		if (*mode == 'x') {		/* open exclusive -- GNU extension */
			open_mode |= O_EXCL;
		}
		++mode;
	}

	nfp = 0;
	if (fp == 0) {				/* We need a FILE so allocate it before */
		for (i = 0; i < FIXED_STREAMS; i++) { /* we potentially call open. */
			if (_stdio_streams[i].fd == -1) {
				nfp = _stdio_streams + i;
				break;
			}
		}
		if ((i == FIXED_STREAMS) && (!(nfp = malloc(sizeof(FILE))))) {
			return 0;
		}
	}


	if (fname) {				/* Open the file itself */
		fd = open(fname, open_mode, 0666);
	}
	if (fd < 0) {				/* Error from open or bad arg. */
		if (nfp) {
			_free_stdio_stream(nfp);
		}
		return 0;
	}

	if (fp == 0) {				/* Not freopen so... */
		fp = nfp;				/* use newly created FILE and */
		fp->next = __IO_list;	/* add it to the list of open files. */
		__IO_list = fp;

		fp->mode = __MODE_FREEFIL;
		if (!(fp->bufstart = _alloc_stdio_buffer(BUFSIZ))) {
			/* Allocation failed so use 8 byte buffer in FILE structure */
			fp->bufstart = fp->unbuf;
			fp->bufend = fp->unbuf + sizeof(fp->unbuf);
		} else {
			fp->bufend = fp->bufstart + BUFSIZ;
			fp->mode |= __MODE_FREEBUF;
		}
	}

	if (isatty(fd)) {
		fp->mode |= _IOLBF;
	} else {					/* Note: the following should be optimized */
		fp->mode |= _IOFBF;		/* away since we should have _IOFBF = 0. */
	}

	/* Ok, file's ready clear the buffer and save important bits */
	fp->bufpos = fp->bufread = fp->bufwrite = fp->bufstart;
	fp->mode |= fopen_mode;
	fp->fd = fd;
	return fp;
}
#endif

#ifdef L_fclose
int fclose(fp)
FILE *fp;
{
	FILE *prev;
	FILE *ptr;
	int rv;

	assert(fp);					/* Shouldn't be NULL */
	assert(fp->fd >= 0);		/* Need file descriptor in valid range. */

	rv = fflush(fp);
	if (close(fp->fd)) {		/* Need to close even if fflush fails. */
		rv = EOF;
	}

	if (fp->mode & __MODE_FREEBUF) {
		_free_stdio_buffer(fp->bufstart);
	}

	if (fp->mode & __MODE_FREEFIL) {
		prev = 0;
		for (ptr = __IO_list; ptr ; ptr = ptr->next) {
			if (ptr == fp) {
				if (prev == 0) {
					__IO_list = fp->next;
				} else {
					prev->next = fp->next;
				}
				_free_stdio_stream(fp);
				break;
			}
			prev = ptr;
		}
	}

	return rv;
}

/* The following is only called by fclose and _fopen (which calls fclose) */
void _free_stdio_stream(FILE *fp)
{
	int i;

	for (i = 0; i < FIXED_STREAMS; i++) {
		if (fp == _stdio_streams + i) {
			fp->fd = -1;
			return;
		}
	}
	free(fp);
}
#endif

#ifdef L_setbuffer
/*
 * Rewritten   Feb 2001    Manuel Novoa III
 *
 * Just call setvbuf with appropriate args.
 */
void setbuffer(FILE *fp, char *buf, size_t size)
{
	int mode;

	mode = _IOFBF;
	if (!buf) {
		mode = _IONBF;
	}
	setvbuf(fp, buf, mode, size);
}
#endif

#ifdef L_setvbuf
/*
 * Rewritten   Feb 2001    Manuel Novoa III
 *
 * Bugs in previous version:
 *   No checking on mode arg.
 *   If alloc of new buffer failed, some FILE fields not set correctly.
 *   If requested buf is same size as current and buf is NULL, then
 *      don't free current buffer; just use it.
 */

int setvbuf(FILE *fp, char *buf, int mode, size_t size)
{
	int allocated_buf_flag;

	if (fflush(fp)) {			/* Standard requires no ops before setvbuf */
		return EOF;				/* called.  We'll try to be more flexible. */
	}

	if (mode & ~__MODE_BUF) {	/* Illegal mode. */
		return EOF;
	}

	if ((mode == _IONBF) || (size <= sizeof(fp->unbuf))) {
		size = sizeof(fp->unbuf); /* Either no buffering requested or */
		buf = fp->unbuf;		/*     requested buffer size very small. */
	}

	fp->mode &= ~(__MODE_BUF);	/* Clear current mode */
	fp->mode |= mode;			/*   and set new one. */

	allocated_buf_flag = 0;
	if ((!buf) && (size != (fp->bufend - fp->bufstart))) {
		/* No buffer supplied and requested size different from current. */
		allocated_buf_flag = __MODE_FREEBUF;
		if (!(buf = _alloc_stdio_buffer(size))) {
			return EOF;
		}
	}

	if (buf && (buf != (char *) fp->bufstart)) { /* Want different buffer. */
		if (fp->mode & __MODE_FREEBUF) {
			_free_stdio_buffer(fp->bufstart);
			fp->mode &= ~(__MODE_FREEBUF);
		}
		fp->mode |= allocated_buf_flag;
		fp->bufstart = buf;
		fp->bufend = buf + size;
		fp->bufpos = fp->bufread = fp->bufwrite = fp->bufstart;
	}

	return 0;
}
#endif

#ifdef L_setbuf
void setbuf(FILE *fp, char *buf)
{
	int mode;

	mode = _IOFBF;
	if (!buf) {
		mode = _IONBF;
	}
	setvbuf(fp, buf, mode, BUFSIZ);
}
#endif

#ifdef L_setlinebuf
void setlinebuf(FILE *fp)
{
	setvbuf(fp, NULL, _IOLBF, BUFSIZ);
}
#endif

#ifdef L_ungetc
int ungetc(c, fp)
int c;
FILE *fp;
{
	if (fp->mode & __MODE_WRITING)
		fflush(fp);

	/* Can't read or there's been an error then return EOF */
	if ((fp->mode & (__MODE_READ | __MODE_ERR)) != __MODE_READ)
		return EOF;

	/* Can't do fast fseeks */
	fp->mode |= __MODE_UNGOT;

	if (fp->bufpos > fp->bufstart)
		return *--fp->bufpos = (unsigned char) c;
	else if (fp->bufread == fp->bufstart)
		return *fp->bufread++ = (unsigned char) c;
	else
		return EOF;
}
#endif

#ifdef L_fopen
#undef fopen
FILE *fopen(const char *__restrict filename,
			const char *__restrict mode)
{
	return __fopen(filename, -1, NULL, mode);
}
#endif

#ifdef L_freopen
#undef freopen
FILE *freopen(__const char *__restrict filename,
			  __const char *__restrict mode, FILE *__restrict fp)
{
	return __fopen(filename, -1, fp, mode);
}
#endif

#ifdef L_fdopen
#undef fdopen
FILE *fdopen(int fd, __const char *mode)
{
	return __fopen(NULL, fd, NULL, mode);
}
#endif

#ifdef L_getchar
#undef getchar
int getchar(void)
{
	return getc(stdin);
}
#endif

#ifdef L_putchar
#undef putchar
int putchar(int c)
{
	return putc(c, stdout);
}
#endif

#ifdef L_clearerr
#undef clearerr
void clearerr(FILE *fp)
{
	assert(fp);

	fp->mode &= ~(__MODE_EOF|__MODE_ERR);
}
#endif

#ifdef L_feof
#undef feof
int feof(FILE *fp)
{
	assert(fp);

  	return ((fp->mode & __MODE_EOF) != 0);
}
#endif

#ifdef L_ferror
#undef ferror
int ferror(FILE *fp)
{
	assert(fp);

	return ((fp->mode & __MODE_ERR) != 0);
}
#endif

#ifdef L_fileno
int fileno(FILE *fp)
{
	if (!fp || (fp->fd < 0)) {
		return -1;
	}
	return fp->fd;
}
#endif

#ifdef L_fgetpos
int fgetpos(FILE *fp, fpos_t *pos)
{
	fpos_t p;

	if (!pos) {					/* NULL pointer. */
		errno = EINVAL;
		return -1;
	}

	if ((p = ftell(fp)) < 0) {	/* ftell failed. */
		return -1;				/* errno set by ftell. */
	}

	*pos = p;
	return 0;
}
#endif

#ifdef L_fsetpos
int fsetpos(FILE *fp, __const fpos_t *pos)
{
	if (pos) {					/* Pointer ok. */
		return fseek(fp, *pos, SEEK_SET);
	}
	errno = EINVAL;				/* NULL pointer. */
	return EOF;
}
#endif
