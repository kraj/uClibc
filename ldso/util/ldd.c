/*
 * ldd - print shared library dependencies
 *
 * usage: ldd [-vVdr] prog ...
 *        -v: print ldd version
 *        -V: print ld.so version
 *	  -d: Perform relocations and report any missing functions. (ELF only).
 *	  -r: Perform relocations for both data objects and functions, and
 *	      report any missing objects (ELF only).
 *        prog ...: programs to check
 *
 * Copyright 1993-2000, David Engel
 *
 * This program may be used for any purpose as long as this
 * copyright notice is kept.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <elf.h>
#include "../d-link/linuxelf.h"
#include "../config.h"
#include "readsoname.h"

struct exec
{
  unsigned long a_info;		/* Use macros N_MAGIC, etc for access */
  unsigned a_text;		/* length of text, in bytes */
  unsigned a_data;		/* length of data, in bytes */
  unsigned a_bss;		/* length of uninitialized data area for file, in bytes */
  unsigned a_syms;		/* length of symbol table data in file, in bytes */
  unsigned a_entry;		/* start address */
  unsigned a_trsize;		/* length of relocation info for text, in bytes */
  unsigned a_drsize;		/* length of relocation info for data, in bytes */
};

#if !defined (N_MAGIC)
#define N_MAGIC(exec) ((exec).a_info & 0xffff)
#endif
/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413
/* This indicates a demand-paged executable with the header in the text. 
   The first page is unmapped to help trap NULL pointer references */
#define QMAGIC 0314
/* Code indicating core file.  */
#define CMAGIC 0421



extern int uselib(const char *library);

#ifdef __GNUC__
void warn(char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
void error(char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#endif

char *prog = NULL;

void warn(char *fmt, ...)
{
    va_list ap;

    fflush(stdout);    /* don't mix output and error messages */
    fprintf(stderr, "%s: warning: ", prog);

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    return;
}

void error(char *fmt, ...)
{
    va_list ap;

    fflush(stdout);    /* don't mix output and error messages */
    fprintf(stderr, "%s: ", prog);

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    exit(1);
}

void *xmalloc(size_t size)
{
    void *ptr;
    if ((ptr = malloc(size)) == NULL)
	error("out of memory");
    return ptr;
}

char *xstrdup(char *str)
{
    char *ptr;
    if ((ptr = strdup(str)) == NULL)
	error("out of memory");
    return ptr;
}

/* see if prog is a binary file */
int is_bin(char *argv0, char *prog)
{
    int res = 0;
    FILE *file;
    struct exec exec;
    char *cp;
    int libtype;

    /* must be able to open it for reading */
    if ((file = fopen(prog, "rb")) == NULL)
	fprintf(stderr, "%s: can't open %s (%s)\n", argv0, prog,
		strerror(errno));
    else
    {
	/* then see if it's Z, Q or OMAGIC */
	if (fread(&exec, sizeof exec, 1, file) < 1)
	    fprintf(stderr, "%s: can't read header from %s\n", argv0, prog);
	else if (N_MAGIC(exec) != ZMAGIC && N_MAGIC(exec) != QMAGIC &&
                 N_MAGIC(exec) != OMAGIC)
	{
	    elfhdr elf_hdr;
	    
 	    rewind(file);
	    fread(&elf_hdr, sizeof elf_hdr, 1, file);
	    if (elf_hdr.e_ident[0] != 0x7f ||
		strncmp(elf_hdr.e_ident+1, "ELF", 3) != 0)
		fprintf(stderr, "%s: %s is not a.out or ELF\n", argv0, prog);
	    else
	    {
		elf_phdr phdr;
		int i;

		/* Check its an exectuable, library or other */
		switch (elf_hdr.e_type)
		{
		  case ET_EXEC:
		    res = 3;
		    /* then determine if it is dynamic ELF */
		    for (i=0; i<elf_hdr.e_phnum; i++)
		    {
		        fread(&phdr, sizeof phdr, 1, file);
			if (phdr.p_type == PT_DYNAMIC)
			{
			    res = 2;
			    break;
			}
		    }
		    break;
		  case ET_DYN:
		    if ((cp = readsoname(prog, file, LIB_ANY, &libtype, 
			 elf_hdr.e_ident[EI_CLASS])) != NULL)
		        free(cp);
		    if (libtype == LIB_ELF_LIBC5)
		        res = 5;
		    else
		        res = 4;
		    break;
		  default:
		    res = 0;
		    break;
		}
	    }
	}
	else
	    res = 1; /* looks good */

	fclose(file);
    }
    return res;
}

int main(int argc, char **argv, char **envp)
{
    int i;
    int vprinted = 0;
    int resolve = 0;
    int bind = 0;
    int bintype;
    char *ld_preload;
    int status = 0;

    /* this must be volatile to work with -O, GCC bug? */
    volatile loadptr loader = (loadptr)LDSO_ADDR;
  
    prog = argv[0];

    while ((i = getopt(argc, argv, "drvV")) != EOF)
	switch (i)
	{
	case 'v':
	    /* print our version number */
	    printf("%s: version %s\n", argv[0], VERSION);
	    vprinted = 1;
	    break;
	case 'd':
	    bind = 1;
	    break;
	case 'r':
	    resolve = 1;
	    break;
	case 'V':
	    /* print the version number of ld.so */
	    if (uselib(LDSO_IMAGE))
	    {
		fprintf(stderr, "%s: can't load dynamic linker %s (%s)\n",
			argv[0], LDSO_IMAGE, strerror(errno));
		exit(1);
	    }
	    loader(FUNC_VERS, NULL);
	    vprinted = 1;
	    break;
	}

    /* must specify programs if -v or -V not used */
    if (optind >= argc && !vprinted)
    {
	printf("usage: %s [-vVdr] prog ...\n", argv[0]);
	exit(0);
    }

    /* setup the environment for ELF binaries */
    putenv("LD_TRACE_LOADED_OBJECTS=1");
    if (resolve || bind)
        putenv("LD_BIND_NOW=1");
    if (resolve)
        putenv("LD_WARN=1");
    ld_preload = getenv("LD_PRELOAD");

    /* print the dependencies for each program */
    for (i = optind; i < argc; i++)
    {
	pid_t pid;
	char buff[1024];

	/* make sure it's a binary file */
	if (!(bintype = is_bin(argv[0], argv[i])))
	{
	    status = 1;
	    continue;
	}

	/* print the program name if doing more than one */
	if (optind < argc-1)
	{
	    printf("%s:\n", argv[i]);
	    fflush(stdout);
	}

	/* no need to fork/exec for static ELF program */
	if (bintype == 3)
	{
	    printf("\tstatically linked (ELF)\n");
	    continue;
	}

	/* now fork and exec prog with argc = 0 */
	if ((pid = fork()) < 0)
	{
	    fprintf(stderr, "%s: can't fork (%s)\n", argv[0], strerror(errno));
	    exit(1);
	}
	else if (pid == 0)
	{
	    switch (bintype)
	    {
	      case 1: /* a.out */
	        /* save the name in the enviroment, ld.so may need it */
	        snprintf(buff, sizeof buff, "%s=%s", LDD_ARGV0, argv[i]);
		putenv(buff);
		execl(argv[i], NULL);
		break;
	      case 2: /* ELF program */
		execl(argv[i], argv[i], NULL);
		break;
	      case 4: /* ELF libc6 library */
		/* try to use /lib/ld-linux.so.2 first */
#if !defined(__mc68000__)
		execl("/lib/ld-linux.so.2", "/lib/ld-linux.so.2", 
		      "--list", argv[i], NULL);
#else
		execl("/lib/ld.so.1", "/lib/ld.so.1", 
		      "--list", argv[i], NULL);
#endif
		/* fall through */
	      case 5: /* ELF libc5 library */
	        /* if that fails, add library to LD_PRELOAD and 
		   then execute lddstub */
		if (ld_preload && *ld_preload)
		    snprintf(buff, sizeof buff, "LD_PRELOAD=%s:%s%s", 
			     ld_preload, *argv[i] == '/' ? "" : "./", argv[i]);
		else
		    snprintf(buff, sizeof buff, "LD_PRELOAD=%s%s", 
			     *argv[i] == '/' ? "" : "./", argv[i]);
		putenv(buff);
		execl(LDDSTUB, argv[i], NULL);
		break;
	      default:
		fprintf(stderr, "%s: internal error, bintype = %d\n",
			argv[0], bintype);
		exit(1);
	    }
	    fprintf(stderr, "%s: can't execute %s (%s)\n", argv[0], argv[i],
		    strerror(errno));
	    exit(1);
	}
	else
	{
	    /* then wait for it to complete */
	    int status;
	    if (waitpid(pid, &status, 0) != pid)
	    {
	        fprintf(stderr, "%s: error waiting for %s (%s)\n", argv[0], 
			argv[i], strerror(errno));
	    }
	    else if (WIFSIGNALED(status))
	    {
	        fprintf(stderr, "%s: %s exited with signal %d\n", argv[0], 
			argv[i], WTERMSIG(status));
	    }
	}
    }

    exit(status);
}
