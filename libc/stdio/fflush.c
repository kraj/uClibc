/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

libc_hidden_proto(fflush_unlocked)

#ifdef __DO_UNLOCKED

#ifdef __UCLIBC_MJN3_ONLY__
#warning WISHLIST: Add option to test for undefined behavior of fflush.
#endif /* __UCLIBC_MJN3_ONLY__ */

#ifdef __UCLIBC_HAS_THREADS__
/* Even if the stream is set to user-locking, we still need to lock
 * when all (lbf) writing streams are flushed. */
#define MY_STDIO_THREADLOCK(STREAM) \
	if (_stdio_user_locking != 2) { \
		__STDIO_ALWAYS_THREADLOCK(STREAM); \
	}

#define MY_STDIO_THREADUNLOCK(STREAM) \
	if (_stdio_user_locking != 2) { \
		__STDIO_ALWAYS_THREADUNLOCK(STREAM); \
	}
#else
#define MY_STDIO_THREADLOCK(STREAM)		((void)0)
#define MY_STDIO_THREADUNLOCK(STREAM)	((void)0)
#endif


int fflush_unlocked(register FILE *stream)
{
#ifdef __STDIO_BUFFERS

	int retval = 0;
#ifdef __UCLIBC_MJN3_ONLY__
#warning REMINDER: should probably define a modeflags type
#endif
	unsigned short bufmask = __FLAG_LBF;

#ifndef NDEBUG
	if ((stream != NULL) && (stream != (FILE *) &_stdio_openlist)) {
		__STDIO_STREAM_VALIDATE(stream); /* debugging only */
	}
#endif

	if (stream == (FILE *) &_stdio_openlist) { /* Flush all lbf streams. */
		stream = NULL;
		bufmask = 0;
	}

	if (!stream) {				/* Flush all (lbf) writing streams. */
		__STDIO_THREADLOCK_OPENLIST;
		for (stream = _stdio_openlist; stream ; stream = stream->__nextopen) {
			MY_STDIO_THREADLOCK(stream);
			if (!(((stream->__modeflags | bufmask)
				   ^ (__FLAG_WRITING|__FLAG_LBF)
				   ) & (__FLAG_WRITING|__MASK_BUFMODE))
				) {
				if (!__STDIO_COMMIT_WRITE_BUFFER(stream)) {
					__STDIO_STREAM_DISABLE_PUTC(stream);
					__STDIO_STREAM_CLEAR_WRITING(stream);
				} else {
					retval = EOF;
				}
			}
			MY_STDIO_THREADUNLOCK(stream);
		}
		__STDIO_THREADUNLOCK_OPENLIST;
	} else if (__STDIO_STREAM_IS_WRITING(stream)) {
		if (!__STDIO_COMMIT_WRITE_BUFFER(stream)) {
			__STDIO_STREAM_DISABLE_PUTC(stream);
			__STDIO_STREAM_CLEAR_WRITING(stream);
		} else {
			retval = EOF;
		}
	}
#if 0
	else if (stream->__modeflags & (__MASK_READING|__FLAG_READONLY)) {
		/* ANSI/ISO says behavior in this case is undefined but also says you
		 * shouldn't flush a stream you were reading from.  As usual, glibc
		 * caters to broken programs and simply ignores this. */
		__UNDEFINED_OR_NONPORTABLE;
		__STDIO_STREAM_SET_ERROR(stream);
		__set_errno(EBADF);
		retval = EOF;
	}
#endif

#ifndef NDEBUG
	if ((stream != NULL) && (stream != (FILE *) &_stdio_openlist)) {
		__STDIO_STREAM_VALIDATE(stream); /* debugging only */
	}
#endif

	return retval;

#else  /* __STDIO_BUFFERS --------------------------------------- */

#ifndef NDEBUG
	if ((stream != NULL)
#ifdef __STDIO_HAS_OPENLIST
		&& (stream != (FILE *) &_stdio_openlist)
#endif
		) {
		__STDIO_STREAM_VALIDATE(stream); /* debugging only */
	}
#endif

#if 0
	if (stream && (stream->__modeflags & (__MASK_READING|__FLAG_READONLY))) {
		/* ANSI/ISO says behavior in this case is undefined but also says you
		 * shouldn't flush a stream you were reading from.  As usual, glibc
		 * caters to broken programs and simply ignores this. */
		__UNDEFINED_OR_NONPORTABLE;
		__STDIO_STREAM_SET_ERROR(stream);
		__set_errno(EBADF);
		return EOF;
	}
#endif

	return 0;
#endif /* __STDIO_BUFFERS */
}
libc_hidden_def(fflush_unlocked)

#ifndef __UCLIBC_HAS_THREADS__
libc_hidden_proto(fflush)
strong_alias(fflush_unlocked,fflush)
libc_hidden_def(fflush)
#endif

#elif defined __UCLIBC_HAS_THREADS__

libc_hidden_proto(fflush)
int fflush(register FILE *stream)
{
	int retval;
	__STDIO_AUTO_THREADLOCK_VAR;

	if (stream
#ifdef __STDIO_HAS_OPENLIST
		&& (stream != (FILE *) &_stdio_openlist)
#endif
		) {

		__STDIO_AUTO_THREADLOCK(stream);

		retval = fflush_unlocked(stream);

		__STDIO_AUTO_THREADUNLOCK(stream);
	} else {
		retval = fflush_unlocked(stream);
	}

	return retval;
}
libc_hidden_def(fflush)

#endif
