
/*
 * ANSI/ISO C99 says

 9 Although both text and binary wide­oriented streams are conceptually sequences of wide
 characters, the external file associated with a wide­oriented stream is a sequence of
 multibyte characters, generalized as follows:
 --- Multibyte encodings within files may contain embedded null bytes (unlike multibyte
 encodings valid for use internal to the program).
 --- A file need not begin nor end in the initial shift state. 225)

 * How do we deal with this?

 * Should auto_wr_transition init the mbstate object?
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#ifndef __STDIO_THREADSAFE

#ifdef __BCC__
#define UNLOCKED_STREAM(RETURNTYPE,NAME,PARAMS,ARGS,STREAM) \
asm(".text\nexport _" "NAME" "_unlocked\n_" "NAME" "_unlocked = _" "NAME"); \
RETURNTYPE NAME PARAMS
#else
#define UNLOCKED_STREAM(RETURNTYPE,NAME,PARAMS,ARGS,STREAM) \
strong_alias(NAME,NAME##_unlocked) \
RETURNTYPE NAME PARAMS
#endif

#define UNLOCKED(RETURNTYPE,NAME,PARAMS,ARGS) \
	UNLOCKED_STREAM(RETURNTYPE,NAME,PARAMS,ARGS,stream)

#ifdef __BCC__
#define UNLOCKED_VOID_RETURN(NAME,PARAMS,ARGS) \
asm(".text\nexport _" "NAME" "_unlocked\n_" "NAME" "_unlocked = _" "NAME"); \
void NAME PARAMS
#else
#define UNLOCKED_VOID_RETURN(NAME,PARAMS,ARGS) \
strong_alias(NAME,NAME##_unlocked) \
void NAME PARAMS
#endif

#define __STDIO_THREADLOCK_OPENLIST
#define __STDIO_THREADUNLOCK_OPENLIST

#else  /* __STDIO_THREADSAFE */

#include <pthread.h>

#define UNLOCKED_STREAM(RETURNTYPE,NAME,PARAMS,ARGS,STREAM) \
RETURNTYPE NAME PARAMS \
{ \
	RETURNTYPE retval; \
	__STDIO_THREADLOCK(STREAM); \
	retval = NAME##_unlocked ARGS ; \
	__STDIO_THREADUNLOCK(STREAM); \
	return retval; \
} \
RETURNTYPE NAME##_unlocked PARAMS

#define UNLOCKED(RETURNTYPE,NAME,PARAMS,ARGS) \
	UNLOCKED_STREAM(RETURNTYPE,NAME,PARAMS,ARGS,stream)

#define UNLOCKED_VOID_RETURN(NAME,PARAMS,ARGS) \
void NAME PARAMS \
{ \
	__STDIO_THREADLOCK(stream); \
	NAME##_unlocked ARGS ; \
	__STDIO_THREADUNLOCK(stream); \
} \
void NAME##_unlocked PARAMS

#define __STDIO_THREADLOCK_OPENLIST \
	pthread_mutex_lock(&_stdio_openlist_lock)

#define __STDIO_THREADUNLOCK_OPENLIST \
	pthread_mutex_unlock(&_stdio_openlist_lock)

#define __STDIO_THREADTRYLOCK_OPENLIST \
	pthread_mutex_trylock(&_stdio_openlist_lock)

#endif /* __STDIO_THREADSAFE */

#ifndef __STDIO_BUFFERS
#error stdio buffers are currently required for wide i/o
#endif

/**********************************************************************/
#ifdef L_fwide

/* TODO: According to SUSv3 should return EBADF if invalid stream. */

int fwide(register FILE *stream, int mode)
{
	__STDIO_THREADLOCK(stream);

	if (mode && !(stream->modeflags & (__FLAG_WIDE|__FLAG_NARROW))) {
		stream->modeflags |= ((mode > 0) ? __FLAG_WIDE : __FLAG_NARROW);
	}

	mode = (stream->modeflags & __FLAG_WIDE)
		- (stream->modeflags & __FLAG_NARROW);

	__STDIO_THREADUNLOCK(stream);

	return mode;
}

#endif
/**********************************************************************/
#ifdef L_fgetwc

static void munge_stream(register FILE *stream, unsigned char *buf)
{
#ifdef __STDIO_GETC_MACRO
	stream->bufgetc =
#endif
#ifdef __STDIO_PUTC_MACRO
	stream->bufputc =
#endif
	stream->bufpos = stream->bufread = stream->bufend = stream->bufstart = buf;
}

UNLOCKED(wint_t,fgetwc,(register FILE *stream),(stream))
{
	wint_t wi;
	wchar_t wc[1];
	int n;
	size_t r;
	unsigned char c[1];
	unsigned char sbuf[1];
	unsigned char ungot_width;	/* Support ftell after wscanf ungetwc. */

	wi = WEOF;					/* Prepare for failure. */

	if (stream->modeflags & __FLAG_NARROW) {
		stream->modeflags |= __FLAG_ERROR;
		__set_errno(EBADF);
		goto DONE;
	}
	stream->modeflags |= __FLAG_WIDE;

	if (stream->modeflags & __MASK_UNGOT) {/* Any ungetwc()s? */
		assert( (stream->modeflags & (__FLAG_READING|__FLAG_ERROR))
				== __FLAG_READING);
		wi = stream->ungot[(--stream->modeflags) & __MASK_UNGOT];
		stream->ungot[1] = 0;
		goto DONE;
	}

	if (!stream->bufstart) {	/* Ugh... stream isn't buffered! */
		/* Munge the stream temporarily to use a 1-byte buffer. */
		munge_stream(stream, sbuf);
		++stream->bufend;
	}

	ungot_width = 0;

 LOOP:
	if ((n = stream->bufread - stream->bufpos) == 0) {
		goto FILL_BUFFER;
	}

	r = mbrtowc(wc, stream->bufpos, n, &stream->state);
	if (((ssize_t) r) >= 0) {	/* Single byte... */
		if (r == 0) {			/* Nul wide char... means 0 byte for us so */
			++r;				/* increment r and handle below as single. */
		}
		stream->bufpos += r;
		stream->ungot_width[0] = ungot_width + r;
		wi = *wc;
		goto DONE;
	}

	if (r == ((size_t) -2)) {
		/* Potentially valid but incomplete and no more buffered. */
		stream->bufpos += n;	/* Update bufpos for stream. */
		ungot_width += n;
	FILL_BUFFER:
		if (_stdio_fread(c, (size_t) 1, stream) > 0) {
			assert(stream->bufpos == stream->bufstart + 1);
			*--stream->bufpos = *c;	/* Insert byte into buffer. */
			goto LOOP;
		}
		if (!__FERROR(stream)) { /* EOF with no error. */
			if (!stream->state.mask) {	/* No partially complete wchar. */
				goto DONE;
			}
			/* EOF but partially complete wchar. */
			/* TODO: should EILSEQ be set? */
			__set_errno(EILSEQ);
		}
	}

	/* If we reach here, either r == ((size_t)-1) and mbrtowc set errno
	 * to EILSEQ, or r == ((size_t)-2) and stream is in an error state
	 * or at EOF with a partially complete wchar.  Make sure stream's
	 * error indicator is set. */
	stream->modeflags |= __FLAG_ERROR;

 DONE:
	if (stream->bufstart == sbuf) {	/* Need to un-munge the stream. */
		munge_stream(stream, NULL);
	}

	return wi;
}

strong_alias(fgetwc_unlocked,getwc_unlocked);
strong_alias(fgetwc,getwc);

#endif
/**********************************************************************/
#ifdef L_getwchar

UNLOCKED_STREAM(wint_t,getwchar,(void),(),stdin)
{
	register FILE *stream = stdin; /* This helps bcc optimize. */

	return fgetwc_unlocked(stream);
}

#endif
/**********************************************************************/
#ifdef L_fgetws

UNLOCKED(wchar_t *,fgetws,(wchar_t *__restrict ws, int n,
						   FILE *__restrict stream),(ws, n, stream))
{
	register wchar_t *p = ws;
	wint_t wi;

	while ((n > 1)
		   && ((wi = fgetwc_unlocked(stream)) != WEOF)
		   && ((*p++ = wi) != '\n')
		   ) {
		--n;
	}
	if (p == ws) {
		/* TODO -- should we set errno? */
/*  		if (n <= 0) { */
/*  			errno = EINVAL; */
/*  		} */
		return NULL;
	}
	*p = 0;
	return ws;
}

#endif
/**********************************************************************/
#ifdef L_fputwc

UNLOCKED(wint_t,fputwc,(wchar_t wc, FILE *stream),(wc, stream))
{
#if 0
	size_t r;
	char buf[MB_LEN_MAX];

	if (stream->modeflags & __FLAG_NARROW) {
		stream->modeflags |= __FLAG_ERROR;
		__set_errno(EBADF);
		return WEOF;
	}
	stream->modeflags |= __FLAG_WIDE;

	/* TODO:
	 * If stream is in reading state with bad mbstate object, what to do?
	 * Should we check the state first?  Should we check error indicator?
	 * Should we check reading or even read-only?
	 */
	/* It looks like the only ANSI/ISO C99 - blessed way of manipulating
	 * the stream's mbstate object is through fgetpos/fsetpos. */
	r = wcrtomb(buf, wc, &stream->state);

	return (r != ((size_t) -1) && (r == _stdio_fwrite(buf, r, stream)))
		? wc : WEOF;

#elif 0

	/* this is broken if wc == 0 !!! */
	wchar_t wbuf[2];

	wbuf[0] = wc;
	wbuf[1] = 0;

	return (fputws_unlocked(wbuf, stream) > 0) ? wc : WEOF;

#else

	size_t n;
	char buf[MB_LEN_MAX];

	if (stream->modeflags & __FLAG_NARROW) {
		stream->modeflags |= __FLAG_ERROR;
		__set_errno(EBADF);
		return WEOF;
	}
	stream->modeflags |= __FLAG_WIDE;

	return (((n = wcrtomb(buf, wc, &stream->state)) != ((size_t)-1)) /* EILSEQ */
			&& (_stdio_fwrite(buf, n, stream) != n))/* Didn't write everything. */
		? wc : WEOF;

#endif
}

strong_alias(fputwc_unlocked,putwc_unlocked);
strong_alias(fputwc,putwc);

#endif
/**********************************************************************/
#ifdef L_putwchar

UNLOCKED_STREAM(wint_t,putwchar,(wchar_t wc),(wc),stdout)
{
	register FILE *stream = stdout; /* This helps bcc optimize. */

	return fputwc_unlocked(wc, stream);
}

#endif
/**********************************************************************/
#ifdef L_fputws

UNLOCKED(int,fputws,(const wchar_t *__restrict ws,
					 register FILE *__restrict stream),(ws, stream))
{
#if 1
	size_t n;
	char buf[64];

	if (stream->modeflags & __FLAG_NARROW) {
		stream->modeflags |= __FLAG_ERROR;
		__set_errno(EBADF);
		return -1;
	}
	stream->modeflags |= __FLAG_WIDE;

	while ((n = wcsrtombs(buf, &ws, sizeof(buf), &stream->state)) != 0) {
		/* Wasn't an empty wide string. */
		if ((n == ((size_t) -1))/* Encoding error! */
			 || (_stdio_fwrite(buf, n, stream) != n)/* Didn't write everything. */
			 ) {
			return -1;
		}
		if (!ws) {				/* Done? */
			break;
		}
	}

	return 1;




#elif 1
	int result;
	size_t n;
	size_t len;
	register char *s;
	unsigned char *bufend;
	char sbuf[MB_LEN_MAX];

	if (stream->modeflags & __FLAG_NARROW) {
	RETURN_BADF:
		stream->modeflags |= __FLAG_ERROR;
		__set_errno(EBADF);
		return -1;
	}
	stream->modeflags |= __FLAG_WIDE;

	/* Note: What follows is setup grabbed from _stdio_fwrite and modified
	 * slightly.  Since this is a wide stream, we can ignore bufgetc and
	 * bufputc if present.  They always == bufstart.
	 * It is unfortunate that we need to duplicate so much code here, but
	 * we need to do the stream setup before starting the wc->mb conversion. */

	if ((stream->modeflags & __FLAG_READONLY)
#ifndef __STDIO_AUTO_RW_TRANSITION
	/* ANSI/ISO requires either at EOF or currently not reading. */
		|| ((stream->modeflags & (__FLAG_READING|__FLAG_EOF))
			== __FLAG_READING)
#endif /* __STDIO_AUTO_RW_TRANSITION */
		) {
		/* TODO: This is for posix behavior if readonly.  To save space, we
		 * use this errno for write attempt while reading, as no errno is
		 * specified by posix for this case, even though the restriction is
		 * mentioned in fopen(). */
		goto RETURN_BADF;
	}

#ifdef __STDIO_AUTO_RW_TRANSITION
	/* If reading, deal with ungots and read-buffered chars. */
	if (stream->modeflags & __FLAG_READING) {
		if (((stream->bufpos < stream->bufread)
			 || (stream->modeflags & __MASK_UNGOT))
			/* If appending, we might as well seek to end to save a seek. */
			/* TODO: set EOF in fseek when appropriate? */
			&& fseek(stream, 0L, 
					 ((stream->modeflags & __FLAG_APPEND)
					  ? SEEK_END : SEEK_CUR))
			) {
			/* Note: This differs from glibc's apparent behavior of
			   not setting the error flag and discarding the buffered
			   read data. */
			stream->modeflags |= __FLAG_ERROR; /* fseek may not set this. */
			return -1;			/* Fail if we need to fseek but can't. */
		}
		/* Always reset even if fseek called (saves a test). */
		stream->bufpos = stream->bufread = stream->bufstart;
		stream->modeflags &= ~__FLAG_READING;
	}
#endif

	/* Ok, the boilerplate from _stdio_fwrite is done.  */

	if (stream->bufpos > stream->bufstart) { /* Pending writes.. */
		/* This is a performance penalty, but it simplifies the code below.
		 * If this is removed, the buffer sharing and while loop condition
		 * need to be modified below (at least).  We at least save a little
		 * on the overhead by calling _stdio_fwrite directly instead of
		 * fflush_unlocked. */
		if (_stdio_fwrite(NULL, 0, stream) > 0) {/* fflush incomplete! */
			return -1;
		}
	}

	stream->modeflags |= __FLAG_WRITING; /* Ensure Writing flag is set. */

	/* Next, we "steal" the stream's buffer and do the wc->mb conversion
	 * straight into it.  This will cause the equivalent of an fflush
	 * for each string write.  :-( */
	
	bufend = NULL;
	s = stream->bufstart;

	if ((len = stream->bufend - stream->bufstart) < MB_LEN_MAX) {
		/* Stream is unbuffered or buffer is too small, so deactivate. */
		bufend = stream->bufend;
		stream->bufend = stream->bufstart;
		s = sbuf;
		len = MB_LEN_MAX;
	}

	result = 1;					/* Assume success. */
	while (ws && (n = wcsrtombs(s, &ws, len, &stream->state)) != 0) {
		if ((n == ((size_t) -1)) /* Encoding error! */
			 /* TODO - maybe call write directly?  but what about custom streams? */
			 || (_stdio_fwrite(s, n, stream) != n)/* Didn't write everything. */
			 ) {
			result = -1;
			break;
		}
	}

	if (bufend) {				/* If deactivated stream buffer, renable it. */
		stream->bufend = bufend;
	}

	return result;

#else  /* slow, dumb version */
	while (*ws) {
		if (fputwc_unlocked(*ws, stream) == WEOF) {
			return -1;
		}
		++ws;
	}
	return 1;
#endif
}

#endif
/**********************************************************************/
#ifdef L_ungetwc
/*
 * Note: This is the application-callable ungetwc.  If wscanf calls this, it
 * should also set stream->ungot[1] to 0 if this is the only ungot.
 */

/* Reentrant. */

wint_t ungetwc(wint_t c, register FILE *stream)
{
	__STDIO_THREADLOCK(stream);

	__stdio_validate_FILE(stream); /* debugging only */

	if (stream->modeflags & __FLAG_NARROW) {
		stream->modeflags |= __FLAG_ERROR;
		c = WEOF;
		goto DONE;
	}
	stream->modeflags |= __FLAG_WIDE;

	/* If can't read or there's been an error, or c == EOF, or ungot slots
	 * already filled, then return EOF */
	if ((stream->modeflags
		 & (__MASK_UNGOT2|__FLAG_WRITEONLY
#ifndef __STDIO_AUTO_RW_TRANSITION
			|__FLAG_WRITING		/* Note: technically no, but yes in spirit */
#endif /* __STDIO_AUTO_RW_TRANSITION */
			))
		|| ((stream->modeflags & __MASK_UNGOT1) && (stream->ungot[1]))
		|| (c == WEOF) ) {
		c = WEOF;
		goto DONE;;
	}

/*  ungot_width */

#ifdef __STDIO_BUFFERS
								/* TODO: shouldn't allow writing??? */
	if (stream->modeflags & __FLAG_WRITING) {
		fflush_unlocked(stream); /* Commit any write-buffered chars. */
	}
#endif /* __STDIO_BUFFERS */

	/* Clear EOF and WRITING flags, and set READING FLAG */
	stream->modeflags &= ~(__FLAG_EOF|__FLAG_WRITING);
	stream->modeflags |= __FLAG_READING;
	stream->ungot[1] = 1;		/* Flag as app ungetc call; wscanf fixes up. */
	stream->ungot[(stream->modeflags++) & __MASK_UNGOT] = c;

	__stdio_validate_FILE(stream); /* debugging only */

 DONE:
	__STDIO_THREADUNLOCK(stream);

	return c;
}

#endif
/**********************************************************************/
