/* stdlib.h  <ndf@linux.mit.edu> */
#include <features.h>
#include <sys/types.h>

#ifndef __STDLIB_H
#define __STDLIB_H

/* Don't overwrite user definitions of NULL */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* For program termination */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/* Call all functions registered with `atexit' and `on_exit',
 * in the reverse of the order in which they were registered
 * perform stdio cleanup, and terminate program execution with STATUS.  */
extern void exit __P ((int __status)) __attribute__ ((__noreturn__));
/* Register a function to be called when `exit' is called.  */
extern int atexit __P ((void (*__func) (void)));
/* Abort execution and generate a core-dump.  */
extern void abort __P ((void)) __attribute__ ((__noreturn__));

extern void * malloc __P ((size_t));
extern void * calloc __P ((size_t, size_t));
extern void free __P ((void *));
extern void * realloc __P ((void *, size_t));

#if defined __USE_GNU || defined __USE_BSD || defined __USE_MISC
# include <alloca.h>
#endif /* Use GNU, BSD, or misc.  */

#ifdef DEBUG_MALLOC

extern void * malloc_dbg __P ((size_t, char* func, char* file, int line));
extern void * calloc_dbg __P ((size_t, size_t, char* func, char* file, int line));
extern void free_dbg __P ((void *, char* func, char* file, int line));
extern void * realloc_dbg __P ((void *, size_t, char* func, char* file, int line));

#define malloc(x) malloc_dbg((x),__FUNCTION__,__FILE__,__LINE__)
#define calloc(x,y) calloc_dbg((x),(y),__FUNCTION__,__FILE__,__LINE__)
#define free(x) free_dbg((x),__FUNCTION__,__FILE__,__LINE__)
#define realloc(x) realloc((x),__FUNCTION__,__FILE__,__LINE__)

#endif

extern int rand __P ((void));
extern void srand __P ((unsigned int seed));

extern long strtol __P ((const char * nptr, char ** endptr, int base));
extern unsigned long strtoul __P ((const char * nptr,
				   char ** endptr, int base));
#ifndef __HAS_NO_FLOATS__
extern float strtod __P ((const char * nptr, char ** endptr));
#endif

extern char *getenv __P ((__const char *__name));

extern int putenv __P ((__const char *__string));

extern int setenv __P ((__const char *__name, __const char *__value,
                        int __replace));
extern void unsetenv __P ((__const char *__name));

extern int system __P ((__const char *__command));

extern char * gcvt __P ((float number, size_t ndigit, char * buf));

#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED
/* Return the canonical absolute name of file NAME.  The last file name
 * component need not exist, and may be a symlink to a nonexistent file.
 * If RESOLVED is null, the result is malloc'd; otherwise, if the canonical
 * name is PATH_MAX chars or more, returns null with `errno' set to
 * ENAMETOOLONG; if the name fits in fewer than PATH_MAX chars, returns the
 * name in RESOLVED.  */
extern char *realpath __P ((__const char *__restrict __name,
	    char *__restrict __resolved));
#endif


/* Shorthand for type of comparison functions.  */
typedef int (*__compar_fn_t) __P ((__const __ptr_t, __const __ptr_t));
typedef __compar_fn_t comparison_fn_t;
/* Sort NMEMB elements of BASE, of SIZE bytes each,
   using COMPAR to perform the comparisons.  */
extern void qsort __P ((__ptr_t __base, size_t __nmemb, size_t __size,
			  __compar_fn_t __compar));


#define atof(x) strtod((x),(char**)0)
#define atoi(x) (int)strtol((x),(char**)0,10)
#define atol(x) strtol((x),(char**)0,10)

/* Returned by `div'.  */
typedef struct
  {
    int quot;			/* Quotient.  */
    int rem;			/* Remainder.  */
  } div_t;

/* Returned by `ldiv'.  */
typedef struct
  {
    long int quot;		/* Quotient.  */
    long int rem;		/* Remainder.  */
  } ldiv_t;



#endif /* __STDLIB_H */
