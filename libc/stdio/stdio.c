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

/*
 * Feb 27, 2001            Manuel Novoa III
 *
 * Most of the core functionality has been completely rewritten.
 * A number of functions have been added as well, as mandated by C89.
 *
 * An extension function "fsfopen" has been added:
 *   Open a file using an automatically (stack) or statically allocated FILE.
 *   The FILE * returned behaves just as any other FILE * with respect to the
 *   stdio functions, but be aware of the following:
 *   NOTE: The buffer used for the file is FILE's builtin 2-byte buffer, so
 *         setting a new buffer is probably advisable.
 *   NOTE: This function is primarily intended to be used for stack-allocated
 *         FILEs when uClibc stdio has no dynamic memory support.
 *         For the statically allocated case, it is probably better to increase
 *         the value of FIXED_STREAMS in stdio.c.
 *   WARNING: If allocated on the stack, make sure you call fclose before the
 *            stack memory is reclaimed!
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
#include <limits.h>

extern off_t _uClibc_fwrite(const unsigned char *buf, off_t bytes, FILE *fp);
extern off_t _uClibc_fread(unsigned char *buf, off_t bytes, FILE *fp);

/* Used internally to actually open files */
extern FILE *__fopen __P((__const char *__restrict __filename, int __fd,
	                FILE *__restrict __stream, __const char *__restrict __mode,
			int extra_modes));

/* Note: This def of READING is ok since 1st ungetc puts in buf. */
#define READING(fp) (fp->bufstart < fp->bufread)
#define WRITING(fp) (fp->bufwrite > fp->bufstart)

#define READABLE(fp) (fp->bufread != 0)
#define WRITEABLE(fp) (fp->bufwrite != 0)
#define EOF_OR_ERROR(fp) (fp->mode & (__MODE_EOF | __MODE_ERR))

/***********************************************************************/
/* BUILD TIME OPTIONS                                                  */
/***********************************************************************/
/*
 * FIXED_STREAMS must be >= 3 and FIXED_BUFFERS must be >= 2.
 * As a feature, these can be increased, although this is probably
 * only useful if DISABLE_DYNAMIC is set to 1 below.
 */

#define FIXED_STREAMS 3
#define FIXED_BUFFERS 2

/*
 * As a feature, you can build uClibc with no dynamic allocation done
 * by the stdio package.  Just set DISABLE_DYNAMIC to nonzero.  Note that
 * use of asprintf, getdelim, or getline will pull malloc into the link.
 *
 * Note: You can't trust FOPEN_MAX if DISABLE_DYNAMIC != 0.
 */
#define DISABLE_DYNAMIC 0

/*
 * As a feature, you can try to allow setvbuf calls after file operations.
 * Setting FLEXIBLE_SETVBUF to nonzero will cause setvbuf to try to fflush
 * any buffered writes or sync the file position for buffered reads.  If it
 * is successful, the buffer change can then take place.
 */
#define FLEXIBLE_SETVBUF 0
/***********************************************************************/

#if DISABLE_DYNAMIC != 0
#undef malloc
#undef free
#define malloc(x) 0
#define free(x)
#endif

extern FILE *__IO_list;			/* For fflush. */
extern FILE *_free_file_list;
extern char _free_buffer_index;
extern FILE _stdio_streams[FIXED_STREAMS];
extern unsigned char _fixed_buffers[FIXED_BUFFERS * BUFSIZ];

extern unsigned char *_alloc_stdio_buffer(size_t size);
extern void _free_stdio_buffer_of_file(FILE *fp);
extern void _free_stdio_stream(FILE *fp);

#ifdef L__alloc_stdio_buffer
unsigned char *_alloc_stdio_buffer(size_t size)
{
	unsigned char *buf;

	if ((size == BUFSIZ) && (_free_buffer_index < FIXED_BUFFERS)) {
		buf = _fixed_buffers + ((unsigned int)_free_buffer_index) * BUFSIZ;
		_free_buffer_index = *buf;
		return buf;
	}
	return malloc(size);
}
#endif

#ifdef L__free_stdio_buffer_of_file
void _free_stdio_buffer_of_file(FILE *fp)
{
	unsigned char *buf;

	if (!(fp->mode & __MODE_FREEBUF)) {
		return;
	}
	fp->mode &= ~(__MODE_FREEBUF);
	buf = fp->bufstart;

	if ((buf >= _fixed_buffers) 
	 && (buf < _fixed_buffers + (FIXED_BUFFERS * BUFSIZ))) {
		*buf = _free_buffer_index;
		_free_buffer_index = (buf - _fixed_buffers)/BUFSIZ;
		return;
	}
	free(buf);
}
#endif

#ifdef L__stdio_init

#if FIXED_BUFFERS < 2
#error FIXED_BUFFERS must be >= 2
#endif

#if FIXED_BUFFERS >= UCHAR_MAX
#error FIXED_BUFFERS must be < UCHAR_MAX
#endif

#define bufin (_fixed_buffers)
#define bufout (_fixed_buffers + BUFSIZ)
#define buferr (_stdio_streams[2].unbuf) /* Stderr is unbuffered */

unsigned char _fixed_buffers[FIXED_BUFFERS * BUFSIZ];

#if FIXED_STREAMS < 3
#error FIXED_STREAMS must be >= 3
#endif

FILE _stdio_streams[FIXED_STREAMS] = {
	{bufin, bufin,      0, bufin, bufin + BUFSIZ,
	 _stdio_streams + 1,
	 0, _IOFBF | __MODE_FREEFIL | __MODE_FREEBUF | __MODE_TIED },
	{bufout,    0, bufout, bufout, bufout + BUFSIZ,
	 _stdio_streams + 2,
	 1, _IOFBF | __MODE_FREEFIL | __MODE_FREEBUF | __MODE_TIED },
	{buferr,    0, buferr, buferr, buferr + 1,
	 NULL,
	 2, _IONBF | __MODE_FREEFIL }
};

FILE *stdin = _stdio_streams + 0;
FILE *stdout = _stdio_streams + 1;
FILE *stderr = _stdio_streams + 2;

/*
 * Note: the following forces linking of the __init_stdio function if
 * any of the stdio functions are used since they all call fflush directly
 * or indirectly.
 */
FILE *__IO_list = _stdio_streams;			/* For fflush. */

FILE *_free_file_list = 0;
char _free_buffer_index = FIXED_BUFFERS;

/*
 * __stdio_close_all is automatically when exiting if stdio is used.
 * See misc/internals/__uClibc_main.c and and stdlib/atexit.c.
 */
void __stdio_close_all(void)
{
	fflush(NULL);				/* Files will be closed on _exit call. */
}

/*
 * __init_stdio is automatically by __uClibc_main if stdio is used.
 */
void __init_stdio(void)
{
#if (FIXED_BUFFERS > 2) || (FIXED_STREAMS > 3)
	int i;
#endif
#if FIXED_BUFFERS > 2
	_free_buffer_index = 2;
	for ( i = 2 ; i < FIXED_BUFFERS ; i++ ) {
		_fixed_buffers[i*BUFSIZ] = i;
	}
#endif
#if FIXED_STREAMS > 3
	_free_file_list = _stdio_streams + 3;
	for ( i = 3 ; i < FIXED_STREAMS-1 ; i++ ) {
		_stdio_streams[i].next = _stdio_streams + i + 1;
	}
	_stdio_streams[i].next = 0;
#endif

#if _IOFBF != 0 || _IOLBF != 1
#error Assumption violated -- values of _IOFBF and/or _IOLBF
/* This asssumption is also made in _fopen. */
#endif

	/* stdout uses line buffering when connected to a tty. */
	_stdio_streams[1].mode |= isatty(1);
}
#endif

#ifdef L_fputc
int fputc(int c, FILE *fp)
{
	unsigned char buf[1];

	*buf = (unsigned char) c;

	if (_uClibc_fwrite(buf, 1, fp)) {
		return (unsigned char) c;
	}
	return EOF;
}
#endif

#ifdef L_fgetc
int fgetc(FILE *fp)
{
	unsigned char buf[1];

	if (_uClibc_fread(buf, 1, fp)) {
		return *buf;
	}
	return EOF;
}
#endif

#ifdef L_fflush
int fflush(FILE *fp)
{
	int rv;

	rv = 0;

	if (fp == NULL) {			/* On NULL flush the lot. */
		for (fp = __IO_list; fp; fp = fp->next) {
			if (WRITEABLE(fp) && fflush(fp)) {
				rv = EOF;
			}
		}
	} else if (WRITING(fp)) {	/* Output buffer contents. */
		_uClibc_fwrite(NULL, 0, fp);
		if (fp->mode & __MODE_ERR) {
			rv = -1;
		}
	} else if (!WRITEABLE(fp)) { /* File opened read-only!!! */
		/*
		 * According to info, glibc returns an error when the file is opened
		 * in read-only mode.
		 * ANSI says behavior in this case is undefined but also says you
		 * shouldn't flush a stream you were reading from.
		 */
		__set_errno(EBADF);			/* Should we set stream error indicator? */
		rv = -1;
	}

	return rv;
}
#endif

#ifdef L_fgets
/* Nothing special here ... */
char *fgets(char *s, int count, FILE *fp)
{
	int ch;
	char *p;
	
	p = s;
	while (count-- > 1) {		/* Guard against count arg == INT_MIN. */
		ch = getc(fp);
		if (ch == EOF) {
			break;
		}
		*p++ = ch;
		if (ch == '\n') {
			break;
		}
	}
	if (ferror(fp) || (s == p)) {
		return 0;
	}
	*p = 0;
	return s;
}
#endif

#ifdef L_gets
link_warning (gets, "the `gets' function is dangerous and should not be used.")
char *gets(char *str) /* This is an UNSAFE function! */
{
	/* 
	 * Strictly speaking, this implementation is incorrect as the number
	 * of chars gets can read should be unlimited.  However, I can't
	 * imagine anyone wanting to gets() into a buffer bigger than INT_MAX.
	 *
	 * Besides, this function is inherently unsafe and shouldn't be used.
	 */
	return fgets(str, INT_MAX, stdin);
}
#endif

#ifdef L_fputs
int fputs(const char *str, FILE *fp)
{
	int n;

	n = strlen(str);

	_uClibc_fwrite((const unsigned char *)str, n, fp);
	if (fp->mode & __MODE_ERR) {
		n = EOF;
	}
	return n;
}
#endif

#ifdef L_puts
int puts(const char *str)
{
	int n;

	n = fputs(str, stdout);	/* Let next fputc handle EOF or error. */
	if (fputc('\n', stdout) == EOF) { /* Don't use putc since we want to */
		return EOF;				/* fflush stdout if it is line buffered. */
	}
	return n + 1;
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
	unsigned char *p;
	unsigned char *q;

#warning TODO: handle possible overflow of size * nelm
	p = (unsigned char *) buf;
	q = p + (size * nelm);

	while ((p < q) && !EOF_OR_ERROR(fp)) {
		p += _uClibc_fread(p, q - p, fp);
	}
	return (p - (unsigned char *) buf)/size;
}
#endif

#ifdef L__uClibc_fread
off_t _uClibc_fread(unsigned char *buf, off_t bytes, FILE *fp)
{
	unsigned char *p;
	off_t len;

	if (!READABLE(fp)) {
		fp->mode |= __MODE_ERR;
	} else if (WRITING(fp)) {
		fflush(fp);
	} else if (fp->mode & stdout->mode & __MODE_TIED) {
		fflush(stdout);
	}
	if (EOF_OR_ERROR(fp) || (bytes <= 0)) {
		return 0;
	}

	p = (unsigned char *) buf;

	if (fp->mode & __MODE_UNGOT) { /* If we had an ungetc'd char, */
		fp->mode ^= __MODE_UNGOT; /* reset the flag and return it. */
		*p++ = fp->ungot;
		--bytes;
	}

 FROM_BUF:
	len = fp->bufread - fp->bufpos;
	if (len > bytes) {			/* Enough buffered */
		len = bytes;
	}
	
	bytes -= len;
	while (len--) {
		*p++ = *fp->bufpos++;
	}

	if (bytes && !EOF_OR_ERROR(fp)) { /* More requested but buffer empty. */
		if (bytes < fp->bufend - fp->bufstart) {
			fp->bufpos = fp->bufread = fp->bufstart; /* Reset pointers. */
			fp->bufread += _uClibc_fread(fp->bufstart,
										 fp->bufend - fp->bufstart, fp);
			goto FROM_BUF;
		}

	TRY_READ:
		len = read(fp->fd, p, (unsigned) bytes);
		if (len < 0) {
			if (errno == EINTR) { /* We were interrupted, so try again. */
				goto TRY_READ;
			}
			fp->mode |= __MODE_ERR;
		} else {
			p += len;
			if (len == 0) {
				fp->mode |= __MODE_EOF;
			}
		}
	}

	return (p - (unsigned char *)buf);

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
	off_t bytes;

#warning TODO: handle possible overflow for bytes
	bytes = size * nelm;		/* How many bytes do we want? */

	bytes = _uClibc_fwrite((const unsigned char *)buf, bytes, fp);

	return bytes/size;
}
#endif

#ifdef L__uClibc_fwrite
/*
 * If buf == NULL, fflush.
 * If buf != NULL, (fflush and) write
 * Returns number of chars written from fp buffer _OR_ from buf.
 */

off_t _uClibc_fwrite(const unsigned char *buf, off_t bytes, FILE *fp)
{
	unsigned char *p;
	int rv, had_newline;

	/*
	 * Fail if stream isn't writable, if we were reading and get an error
	 * changing over to write mode (ie. can't update stream position),
	 * or if the stream was already in an error state.
	 */
	if (!WRITEABLE(fp)) {		/* Fail if stream isn't writable. */
		fp->mode |= __MODE_ERR;
	} else if (READING(fp)) {	/* If read buffer isn't empty, */
		fseek(fp, 0, SEEK_CUR); /* stop reading and update position. */
	} else if (READABLE(fp)) {
		fp->bufread = fp->bufstart;	/* Reset start of read buffer. */
	}
	if (EOF_OR_ERROR(fp)) {
		return 0;
	}

	p = (unsigned char *)buf;
	if (p && (fp->bufpos + bytes <= fp->bufend)) { /* Enough buffer space? */
		had_newline = 0;
		while (bytes--) {
			if (*p == '\n') {
				had_newline = 1;
			}
			*fp->bufpos++ = *p++;
		}
		if (fp->bufpos < fp->bufend) { /* Buffer is not full. */
			fp->bufwrite = fp->bufend;
			if ((fp->mode & __MODE_BUF) == _IOLBF) {
				fp->bufwrite = fp->bufpos;
				if (had_newline) {
					goto FFLUSH;
				}
			}
			goto DONE;
		}
	FFLUSH:
		/* If we get here, either buffer is full or we need to flush anyway. */
		buf = fp->bufpos - (p - (unsigned char *)buf);
		p = NULL;
	}
	if (!p) {					/* buf == NULL means fflush */
		p = fp->bufstart;
		bytes = fp->bufpos - p;
		fp->bufpos = fp->bufwrite = p;
	} else if (fp->bufpos > fp->bufstart) {	/* If there are buffered chars, */
		_uClibc_fwrite(NULL, 0, fp); /* write them. */
		if (ferror(fp)) {
			return 0;
		}
	}

	while (bytes) {
		if ((rv = write(fp->fd, p, bytes)) < 0) {
			rv = 0;
			if (errno != EINTR) {
				break;
			}
		}
		p += rv;
		bytes -= rv;
	}
	if (bytes) {
		fp->mode |= __MODE_ERR;
	}

 DONE:
	return (p - (unsigned char *)buf);
}
#endif

#ifdef L_rewind
void rewind(fp)
FILE *fp;
{
	clearerr(fp);				/* Clear errors first, then seek in case */
	fseek(fp, 0, SEEK_SET);		/* there is an error seeking. */
}
#endif

#ifdef L_fseek
int fseek(FILE *fp, long int offset, int ref)
{
#if SEEK_SET != 0 || SEEK_CUR != 1 || SEEK_END != 2
#error Assumption violated -- values of SEEK_SET, SEEK_CUR, SEEK_END
#endif

	if ((ref < 0) || (ref > 2)) {
		__set_errno(EINVAL);
		return -1;
	}

	if (WRITING(fp)) {
		fflush(fp);				/* We'll deal with errors below. */
		/* After fflush, bufpos is at CUR. */
	} else if (READING(fp)) {
		if (ref == SEEK_CUR) {
			/* Correct offset to take into account position in buffer. */
			offset -= (fp->bufread - fp->bufpos);
			if (fp->mode & __MODE_UNGOT) { /* If we had an ungetc'd char, */
				--offset;			/* adjust offset (clear flag below). */
			}
		}
	}

	if ((fp->mode & __MODE_ERR) || 
		(((ref != SEEK_CUR) || offset) && (lseek(fp->fd, offset, ref) < 0))) {
		return -1;
	}

	if (READING(fp)) {
		fp->bufpos = fp->bufread = fp->bufstart;
	}
	fp->mode &=	~(__MODE_EOF | __MODE_UNGOT);

	return 0;
}
#endif

#ifdef L_ftell
long ftell(fp)
FILE *fp;
{
	/* Note: can't do fflush here since it would discard any ungetc's. */
	off_t pos;

    pos = lseek(fp->fd, 0, SEEK_CUR); /* Get kernels idea of position. */
	if (pos < 0) {
		return -1;
	}

	if (WRITING(fp)) {
		pos += (fp->bufpos - fp->bufstart);	/* Adjust for buffer position. */
	} else if (READING(fp)) {
	    pos -= (fp->bufread - fp->bufpos);	/* Adjust for buffer position. */
		if (fp->mode & __MODE_UNGOT) {
			--pos;
		}
		if (pos < 0) {			/* ungetcs at start of file? */
			__set_errno(EIO);
			pos = -1;
		}
	}

	return pos;
}
#endif

#ifdef L__fopen
/*
 * This Fopen is all three of fopen, fdopen and freopen. The macros in
 * stdio.h show the other names.
 */
static __inline FILE *_alloc_stdio_stream(void)
{
	FILE *fp;

	if (_free_file_list) {
		fp = _free_file_list;
		_free_file_list = fp->next;
	} else if (!(fp = malloc(sizeof(FILE)))) {
		return 0;
	}
	fp->mode = __MODE_FREEFIL | _IOFBF;
	/* Initially set to use builtin buffer of FILE structure. */
	fp->bufstart = fp->unbuf;
	fp->bufend = fp->unbuf + sizeof(fp->unbuf);
	return fp;
}

FILE *__fopen(fname, fd, fp, mode, extra_modes)
const char *fname;
int fd;
FILE *fp;
const char *mode;
int extra_modes;
{
	FILE *nfp;
	unsigned char *p;
	int open_mode;
	int cur_mode;

	nfp = fp;

	/* Parse the mode string arg. */
	switch (*mode++) {
		case 'r':				/* read */
			open_mode = O_RDONLY | extra_modes;
			break;
		case 'w':				/* write (create or truncate)*/
			open_mode = (O_WRONLY | O_CREAT | O_TRUNC | extra_modes);
			break;
		case 'a':				/* write (create or append) */
			open_mode = (O_WRONLY | O_CREAT | O_APPEND | extra_modes);
			break;
		default:				/* illegal mode */
			__set_errno(EINVAL);
			goto _fopen_ERROR;
	}

	if ((*mode == 'b')) {		/* binary mode (nop for uClibc) */
		++mode;
	}


#if O_RDONLY != 0 || O_WRONLY != 1 || O_RDWR != 2
#error Assumption violated concerning open mode constants!
#endif

	if (*mode == '+') {			/* read-write */
		++mode;
		open_mode &= ~(O_RDONLY | O_WRONLY);
		open_mode |= O_RDWR;
	}

	while (*mode) {				/* ignore everything else except ... */
		if (*mode == 'x') {		/* open exclusive -- GNU extension */
			open_mode |= O_EXCL;
		}
		++mode;
	}

	if (fp == 0) {				/* We need a FILE so allocate it before */
		if (!(nfp = _alloc_stdio_stream())) {
			return 0;
		}
	}

	if (fname) {				/* Open the file itself */
		fd = open(fname, open_mode, 0666);
	} else {					/* fdopen -- check mode is compatible. */
#if O_ACCMODE != 3 || O_RDONLY != 0 || O_WRONLY != 1 || O_RDWR != 2
#error Assumption violated - mode constants
#endif
		cur_mode = fcntl(fd, F_GETFL);
		if (cur_mode == -1) {
			fd = -1;
		} else if (!(cur_mode & O_RDWR) 
				   && ((cur_mode ^ open_mode) & O_ACCMODE)) {
			__set_errno(EINVAL);
			fd = -1;
		}
	}

	if (fd < 0) {				/* Error from open or bad arg passed. */
	_fopen_ERROR:
		if (nfp) {
			_free_stdio_stream(nfp);
		}
		return 0;
	}

	nfp->fd = fd;				/* Set FILE's fd before adding to open list. */

	if (fp == 0) {				/* Not freopen so... */
		nfp->next = __IO_list;	/* use newly created FILE and */
		__IO_list = nfp;		/* add it to the list of open files. */

		if ((p = _alloc_stdio_buffer(BUFSIZ)) != 0) {
			nfp->bufstart = p;
			nfp->bufend = p + BUFSIZ;
			nfp->mode |= __MODE_FREEBUF;
		}
	}

	/* Ok, file's ready clear the buffer and save important bits */
	nfp->bufpos = nfp->bufstart;
	nfp->mode |= isatty(fd);
	nfp->bufread = nfp->bufwrite = 0;
	if (!(open_mode & O_WRONLY)) {
		nfp->bufread = nfp->bufstart;
	}
	if (open_mode & (O_WRONLY | O_RDWR)) {
		nfp->bufwrite = nfp->bufstart;
	}

	return nfp;
}
#endif

#ifdef L_fclose
int fclose(fp)
FILE *fp;
{
	FILE *prev;
	FILE *ptr;
	int rv;

	rv = 0;
	if (WRITING(fp)) {			/* If there are buffered chars to write... */
		rv = fflush(fp);		/* write them. */
	}
	if (close(fp->fd)) {		/* Need to close even if fflush fails. */
		rv = EOF;
	}

	prev = 0;					/* Remove file from open list. */
	for (ptr = __IO_list; ptr ; ptr = ptr->next) {
		if (ptr == fp) {
			if (prev == 0) {
				__IO_list = fp->next;
			} else {
				prev->next = fp->next;
			}
			break;
		}
		prev = ptr;
	}

	_free_stdio_stream(fp);		/* Finally free the stream if necessary. */

	return rv;
}
#endif

#ifdef L__free_stdio_stream
/* The following is only called by fclose and _fopen. */
void _free_stdio_stream(FILE *fp)
{
	_free_stdio_buffer_of_file(fp);	/* Free buffer if necessary. */

	if (!(fp->mode & __MODE_FREEFIL)) {
		return;
	}

	/* Note: we generally won't bother checking for bad pointers here. */
	if ((fp >= _stdio_streams) && (fp < _stdio_streams + FIXED_STREAMS)) {
		assert( (fp - _stdio_streams) % ((_stdio_streams+1) -_stdio_streams)
				== 0 );
		fp->next = _free_file_list;
		_free_file_list = fp;
		return;
	}
	free(fp);
}
#endif

#ifdef L_setbuffer
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
int setvbuf(FILE *fp, char *buf, int mode, size_t size)
{
	int allocated_buf_flag;

	if ((mode < 0) || (mode > 2)) {	/* Illegal mode. */
		return EOF;
	}

#if FLEXIBLE_SETVBUF
	/* C89 standard requires no ops before setvbuf, but we can be flexible. */
	/* NOTE: This will trash any chars ungetc'd!!! */
	if (fseek(fp, 0, SEEK_CUR)) {
		return EOF;
	}
#endif

	/* Note: If size == 2 we could use FILE's builting buffer as well, but */
	/* I don't think the benefit is worth the code size increase. */
	if ((mode == _IONBF) || (size < 1)) {
		size = 1;				/* size == 1 _REQUIRED_ for _IONBF!!! */
		buf = fp->unbuf;
	}

	fp->mode &= ~(__MODE_BUF);	/* Clear current mode */
	fp->mode |= mode;			/*   and set new one. */

	allocated_buf_flag = 0;
	if ((!buf) && (size != (fp->bufend - fp->bufstart))) {
		/* No buffer supplied and requested size different from current. */
		allocated_buf_flag = __MODE_FREEBUF;
		if (!(buf = _alloc_stdio_buffer(size))) {
			return EOF;			/* Keep current buffer. */
		}
	}

	if (buf && (buf != (char *) fp->bufstart)) { /* Want different buffer. */
		_free_stdio_buffer_of_file(fp);	/* Free the old buffer. */
		fp->mode |= allocated_buf_flag;	/* Allocated? or FILE's builtin. */
		fp->bufstart = buf;
		fp->bufend = buf + size;
		fp->bufpos = fp->bufstart;
		if (READABLE(fp)) {
			fp->bufread = fp->bufstart;
		}
		if (WRITEABLE(fp)) {
			fp->bufwrite = fp->bufstart;
		}
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
/* 
 * NOTE: Only one character of pushback is guaranteed, although sometimes
 * it is possible to do more.  You have 1 plus as many characters of pushback
 * as have been read since that last buffer-fill.
 */
int ungetc(c, fp)
int c;
FILE *fp;
{
	unsigned char *p;

	/* If can't read or there's been an error, or c == EOF, or ungot slot
	 * already filled, then return EOF */
		/*
		 * This can only happen if an fgetc triggered a read (that filled
		 * the buffer for case 2 above) and then we ungetc 3 chars.
		 */
	if (!READABLE(fp) || (fp->mode & (__MODE_UNGOT | __MODE_ERR))
		|| (c == EOF) ) {
		return EOF;
	}

	if (WRITING(fp)) {			/* Commit any write-buffered chars. */
		fflush(fp);
	}

	if (fp->bufpos > fp->bufstart) { /* We have space before bufpos. */
		p = --fp->bufpos;
	} else if (fp->bufread == fp->bufpos) { /* Buffer is empty. */
		p = fp->bufread++;
	} else {
		fp->mode |= __MODE_UNGOT;
		p = &(fp->ungot);
	}
	fp->mode &= ~(__MODE_EOF);	/* Clear EOF indicator. */

	if (*p != (unsigned char) c) { /* Don't store if same, because could */
		*p = (unsigned char) c;	/* be sscanf from a const string!!! */
	}

	return c;
}
#endif

#ifdef L_fopen
#undef fopen
FILE *fopen(const char *__restrict filename,
			const char *__restrict mode)
{
	return __fopen(filename, -1, NULL, mode, 0);
}
#endif

#ifdef L_freopen
FILE *freopen(__const char *__restrict filename,
			  __const char *__restrict mode, FILE *__restrict fp)
{
	/* fflush file, close the old fd, and reset modes. */
	if (WRITING(fp)) {			/* If anything in the write buffer... */
		fflush(fp);				/* write it. */
	}
	close(fp->fd);				/* Close the file. */
	fp->mode &= (__MODE_FREEFIL | __MODE_FREEBUF); /* Reset the FILE modes. */
	fp->mode |= _IOFBF;

	return __fopen(filename, -1, fp, mode, 0);
}
#endif

#ifdef L_fsfopen
FILE *fsfopen(__const char *__restrict filename,
			  __const char *__restrict mode, FILE *__restrict fp)
{
	fp->mode = _IOFBF;
	fp->bufstart = fp->unbuf;
	fp->bufend = fp->unbuf + sizeof(fp->unbuf);

	return __fopen(filename, -1, fp, mode, 0);
}
#endif

#ifdef L_fdopen
#undef fdopen
FILE *fdopen(int fd, __const char *mode)
{
	return __fopen(NULL, fd, NULL, mode, 0);
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
	fp->mode &= ~(__MODE_EOF | __MODE_ERR);
}
#endif

#ifdef L_feof
#undef feof
int feof(FILE *fp)
{
  	return fp->mode & __MODE_EOF;
}
#endif

#ifdef L_ferror
#undef ferror
int ferror(FILE *fp)
{
	return fp->mode & __MODE_ERR;
}
#endif

#ifdef L_fileno
int fileno(FILE *fp)
{
	return fp->fd;
}
#endif

#ifdef L_fgetpos
int fgetpos(FILE *fp, fpos_t *pos)
{
	fpos_t p;

	if (!pos) {					/* NULL pointer. */
		__set_errno(EINVAL);
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
	__set_errno(EINVAL);				/* NULL pointer. */
	return EOF;
}
#endif

#ifdef L_fopen64
#ifdef __UCLIBC_HAVE_LFS__
#ifndef O_LARGEFILE
#define O_LARGEFILE	0100000
#endif
FILE *fopen64(const char *__restrict filename,
			const char *__restrict mode)
{
	return __fopen(filename, -1, NULL, mode, O_LARGEFILE);
}
#endif /* __UCLIBC_HAVE_LFS__ */
#endif

