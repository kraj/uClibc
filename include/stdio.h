
#ifndef __STDIO_H
#define __STDIO_H

#include <features.h>
#include <stdarg.h>
#include <sys/types.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define _IOFBF		0x00	/* full buffering */
#define _IOLBF		0x01	/* line buffering */
#define _IONBF		0x02	/* no buffering */
#define __MODE_BUF	0x03	/* Modal buffering dependent on isatty */

#define __MODE_FREEBUF	0x04	/* Buffer allocated with malloc, can free */
#define __MODE_FREEFIL	0x08	/* FILE allocated with malloc, can free */

#define __MODE_READ	0x10	/* Opened in read only */
#define __MODE_WRITE	0x20	/* Opened in write only */
#define __MODE_RDWR	0x30	/* Opened in read/write */

#define __MODE_READING	0x40	/* Buffer has pending read data */
#define __MODE_WRITING	0x80	/* Buffer has pending write data */

#define __MODE_EOF	0x100	/* EOF status */
#define __MODE_ERR	0x200	/* Error status */
#define __MODE_UNGOT	0x400	/* Buffer has been polluted by ungetc */

#define __MODE_IOTRAN	0

/* when you add or change fields here, be sure to change the initialization
 * in stdio_init and fopen */
struct __stdio_file {
  unsigned char *bufpos;   /* the next byte to write to or read from */
  unsigned char *bufread;  /* the end of data returned by last read() */
  unsigned char *bufwrite; /* highest address writable by macro */
  unsigned char *bufstart; /* the start of the buffer */
  unsigned char *bufend;   /* the end of the buffer; ie the byte after the last
                              malloc()ed byte */

  int fd; /* the file descriptor associated with the stream */
  int mode;

  char unbuf[8];	   /* The buffer for 'unbuffered' streams */

  struct __stdio_file * next;
};

#define EOF	(-1)
#ifndef NULL
#define NULL	(0)
#endif

typedef struct __stdio_file FILE;

#define BUFSIZ	(500) /*(508) should get us a fully used kmalloc bucket */

extern FILE stdin[1];
extern FILE stdout[1];
extern FILE stderr[1];


#define stdio_pending(fp) ((fp)->bufread>(fp)->bufpos)

/* Read chunks of generic data from STREAM.  */
extern size_t fread __P ((void *__restrict __ptr, size_t __size,
			  size_t __n, FILE *__restrict __stream));
/* Write chunks of generic data to STREAM.  */
extern size_t fwrite __P ((__const void *__restrict __ptr, size_t __size,
			   size_t __n, FILE *__restrict __s));


#define putc(c, stream)	\
    (((stream)->bufpos >= (stream)->bufwrite) ? fputc((c), (stream))	\
                          : (unsigned char) (*(stream)->bufpos++ = (c))	)

#define getc(stream)	\
  (((stream)->bufpos >= (stream)->bufread) ? fgetc(stream):		\
    (*(stream)->bufpos++))

#define putchar(c) putc((c), stdout)  
#define getchar() getc(stdin)

#define ferror(fp)	(((fp)->mode&__MODE_ERR) != 0)
#define feof(fp)   	(((fp)->mode&__MODE_EOF) != 0)
#define clearerr(fp)	((fp)->mode &= ~(__MODE_EOF|__MODE_ERR),0)
#define fileno(fp)	((fp)->fd)

/* These two call malloc */
#define setlinebuf(__fp)             setvbuf((__fp), (char*)0, _IOLBF, 0)
extern int setvbuf __P((FILE*, char*, int, size_t));

/* These don't */
#define setbuf(__fp, __buf) setbuffer((__fp), (__buf), BUFSIZ)
extern void setbuffer __P((FILE*, char*, int));

/* Read a character from STREAM.  */
extern int fgetc __P ((FILE *__stream));
extern int getc __P ((FILE *__stream));
/* Push a character back onto the input buffer of STREAM.  */
extern int ungetc __P ((int __c, FILE *__stream));
/* Read a character from stdin.  */
extern int getchar __P ((void));

/* Write a character to STREAM.  */
extern int fputc __P ((int __c, FILE *__stream));
extern int putc __P ((int __c, FILE *__stream));
/* Write a character to stdout.  */
extern int putchar __P ((int __c));

/* Close STREAM.  */
extern int fclose __P ((FILE *__stream));
/* Flush STREAM, or all streams if STREAM is NULL.  */
extern int fflush __P ((FILE *__stream));

/* Get a newline-terminated string from stdin, removing the newline.
   DO NOT USE THIS FUNCTION!!  There is no limit on how much it will read.  */
extern char *gets __P ((char *__s));
/* Get a newline-terminated string of finite length from STREAM.  */
extern char *fgets __P ((char *__restrict __s, int __n,
			 FILE *__restrict __stream));


extern FILE *__fopen __P((__const char *__restrict __filename, int __fd, 
	    FILE *__restrict __stream, __const char *__restrict __modes));

/* Open a file and create a new stream for it.  */
#define fopen(__file, __mode)         __fopen((__file), -1, (FILE*)0, (__mode))
/* Open a file, replacing an existing stream with it. */
#define freopen(__file, __mode, __fp) __fopen((__file), -1, (__fp), (__mode))
/* Create a new stream that refers to an existing system file descriptor.  */
#define fdopen(__file, __mode)  __fopen((char*)0, (__file), (FILE*)0, (__mode))


/* Seek to a certain position on STREAM.  */
extern int fseek __P ((FILE *__stream, long int __off, int __whence));
/* Return the current position of STREAM.  */
extern long int ftell __P ((FILE *__stream));
/* Rewind to the beginning of STREAM.  */
extern void rewind __P ((FILE *__stream));


/* Write a string, followed by a newline, to stdout.  */
extern int puts __P ((__const char *__s));
/* Write a string to STREAM.  */
extern int fputs __P ((__const char *__restrict __s,
		       FILE *__restrict __stream));


/* Write formatted output to stdout.  */
extern int printf __P ((__const char *__restrict __format, ...));
/* Write formatted output to STREAM.  */
extern int fprintf __P ((FILE *__restrict __stream,
			 __const char *__restrict __format, ...));
/* Write formatted output to S.  */
extern int sprintf __P ((char *__restrict __s,
			 __const char *__restrict __format, ...));

/* Write formatted output to stdout from argument list ARG.  */
extern int vprintf __P ((__const char *__restrict __format,
			 va_list __arg));
/* Write formatted output to S from argument list ARG.  */
extern int vfprintf __P ((FILE *__restrict __s,
			  __const char *__restrict __format,
			  va_list __arg));
/* Write formatted output to S from argument list ARG.  */
extern int vsprintf __P ((char *__restrict __s,
			  __const char *__restrict __format,
			  va_list __arg));



/* Read formatted input from stdin.  */
extern int scanf __P ((__const char *__restrict __format, ...));
/* Read formatted input from S.  */
extern int sscanf __P ((__const char *__restrict __s,
			__const char *__restrict __format, ...));
/* Read formatted input from STREAM.  */
extern int fscanf __P ((FILE *__restrict __stream,
			__const char *__restrict __format, ...));
/* Read formatted input from stdin into argument list ARG.  */
extern int vscanf __P ((__const char *__restrict __format, va_list __arg))
     __attribute__ ((__format__ (__scanf__, 1, 0)));
/* Read formatted input from S into argument list ARG.  */
extern int vsscanf __P ((__const char *__restrict __s,
			 __const char *__restrict __format,
			 va_list __arg))
     __attribute__ ((__format__ (__scanf__, 2, 0)));
/* Read formatted input from S into argument list ARG.  */
extern int vfscanf __P ((FILE *__restrict __s,
			 __const char *__restrict __format,
			 va_list __arg))
     __attribute__ ((__format__ (__scanf__, 2, 0)));


/* Print a message describing the meaning of the value of errno.  */
extern void perror __P ((__const char *__s));

/* Like `getdelim', but reads up to a newline.  */
extern int getline __P ((char **__restrict __lineptr,
				 size_t *__restrict __n,
				 FILE *__restrict __stream));


#endif /* __STDIO_H */
