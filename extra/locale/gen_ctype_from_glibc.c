/*
 * Generator locale ctype tables
 * You must have already setuped locale for worked libc (libc5 or glibc)
 *
 * This programm scan /usr/share/locale directories and write
 * ./LOCALE/LC_CTYPE files for system with uclibc
 *
 * Written by Vladimir Oleynik <vodz@usa.net> 2001
 * Base on ideas Nickolay Saukh  <nms@ussr.EU.net>
 *
 */

#include <locale.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "../../misc/ctype/ctype.h"


#define DEFAULT_LOCALE_DIR      "/usr/share/locale/"

#define DEF_OUT_NAME            "LC_CTYPE"

#define CURRENT_SUPPORT_MAX (LOCALE_BUF_SIZE/2)


unsigned char x2type[CURRENT_SUPPORT_MAX];

unsigned char x2trans[CURRENT_SUPPORT_MAX];



int
write_out (outname)
     unsigned char *outname;
{
  FILE *ofp = fopen (outname, "w");

  if (ofp == NULL) {
    fprintf (stderr, "Can`t write `%s\n", outname);
    return 1;
  }

  fwrite (x2type, sizeof (x2type), 1, ofp);
  fwrite (x2trans, sizeof (x2trans), 1, ofp);
  fclose (ofp);
  return 0;
}


typedef struct bbits_ {
    int bbits;
    char *bb_name;
  } bbits_t;


bbits_t basic_bits[] =
{
  {ISprint , "ISprint" },
  {ISupper , "ISupper" },
  {ISlower , "ISlower" },
  {IScntrl , "IScntrl" },
  {ISspace , "ISspace" },
  {ISpunct , "ISpunct" },
  {ISalpha , "ISalpha" },
  {ISxdigit, "ISxdigit"},
  {0, NULL}
};


void
ctab_out (char *oun)
{
  int i;
  char *outname;
  FILE *fout;

  outname = alloca(strlen(oun)+strlen("ctype_.c")+1);
  if(outname==0) {
	perror("");
	exit(1);
	}
  strcpy(outname, "ctype_");
  strcat(outname, oun);
  strcat(outname, ".c");

  fout = fopen (outname, "w");

  if (fout == NULL)
    {
      perror ("");
      return;
    }

  fprintf (fout, "const unsigned char _uc_ctype_b_C[LOCALE_BUF_SIZE] = {\n");

  for (i = 0; i < CURRENT_SUPPORT_MAX; i++)
    {
      if(i)
	fprintf (fout, ",\n");
      fprintf (fout, "\t/* 0x%02x, %d, 0%o */\t", i, i, i);
      if (x2type[i])
	{
	  int dirty = 0;
	  bbits_t *tb = basic_bits;

	  while (tb->bbits)
	    {
	      if (x2type[i] & tb->bbits)
		{
		  if (dirty)
		    fputs ("|", fout);
		  fputs (tb->bb_name, fout);
		  dirty = 1;
		}
	      tb++;
	    }
	}
      else
	fputs ("0", fout);
    }

  fputs (",\n\n", fout);

  fprintf (fout, "/* _uc_ctype_trans_C */\n\n");

  for (i = 0; i < CURRENT_SUPPORT_MAX; i++)
    {
      if(i)
	fprintf (fout, ",\n");
      fprintf (fout, "\t/* 0x%02x, %d, 0%o */\t0x%02x", i, i, i, x2trans[i]);
    }
  fputs ("\n};\n", fout);

  (void) fclose (fout);
}


int
main (int argc, char *argv[])
{
  int i,l;
  char *outname = DEF_OUT_NAME;
  char *search_dir = DEFAULT_LOCALE_DIR;
  char *full_path = 0;
  DIR *dir;
  struct dirent *next;
  char *t;
  int  err=0;
  char *ln;
  int  generate_c_code = 1;

  while ((i = getopt (argc, argv, "d:o:c")) != EOF) {
      switch (i) {
	case 'o':
	  outname = optarg;
	  break;
	case 'd':
	  search_dir = optarg;
	  break;
	case 'c':
	  generate_c_code = 0;
	  break;
	default:
	  optind = i = -1;
	  break;
	}
	if(i<0)
	  break;
    }

  if (argc > optind) {
      fprintf (stderr,
"Usage: %s [-d search_dir] [-o output_name] [-c]\n\
Defaults:\n\
  search_dir  : " DEFAULT_LOCALE_DIR "\n\
  output_name : " DEF_OUT_NAME "\n\
  -c          : no generate c-code for other locale exept C-locale.\n"
					, argv[0]);
      return 3;
  }

  l = strlen(search_dir);
  if(l == 0) {
	search_dir = "./";
	l = 2;
  } else {
	if(search_dir[l-1]!='/') {

		t = malloc(l+2);
		if(t==0) {
			fprintf (stderr, "Can`t get %d bytes memory\n", l+2);
			return 4;
		}
		search_dir = strcat(strcpy(t, search_dir), "/");
		l++;
	}
  }

  dir = opendir(search_dir);
  if (!dir) {
      fprintf (stderr, "Can`t open directory `%s' load all locales\n", search_dir);
      return 2;
  }

  while ((next = readdir(dir)) != NULL) {

      struct stat st;
      if(strcmp(next->d_name, ".")==0)
	ln = "C";
      else if(strcmp(next->d_name, "..")==0)
	continue;
      else {
	ln = next->d_name;
	full_path = realloc(full_path, l+strlen(ln)+1);
	strcat(strcpy(full_path, search_dir), ln);
	if (lstat(full_path, &st) < 0)
		continue;
	if(S_ISDIR(st.st_mode)==0)
		continue;
	}
      t = setlocale(LC_CTYPE, ln);
      printf("setlocale(LC_CTYPE, %s) returned %s\n", ln, t);
      if(t==0)
		continue;
      if(mkdir(ln, 0755)) {
		fprintf(stderr, "Can`t create directory `%s'\n", ln);
		continue;
      }
      if(chdir(ln)) {
		fprintf(stderr, "Can`t change directory to `%s'\n", ln);
		continue;
      }

      for (i = 0; i < CURRENT_SUPPORT_MAX; i++) {

	if(isprint(i))
		x2type[i] |= ISprint;
	if(isupper(i))
		x2type[i] |= ISupper;
	if(islower(i))
		x2type[i] |= ISlower;
	if(isspace(i))
		x2type[i] |= ISspace;
	if(isalpha(i))
		x2type[i] |= ISalpha;
	if(iscntrl(i))
		x2type[i] |= IScntrl;
	if(ispunct(i))
		x2type[i] |= ISpunct;
	if(isxdigit(i))
		x2type[i] |= ISxdigit;
	x2trans[i] = i;
	if(toupper(x2trans[i]) != x2trans[i])
		x2trans[i] = toupper(x2trans[i]);
	else if(tolower(x2trans[i]) != x2trans[i])
		x2trans[i] = tolower(x2trans[i]);
      }
      err += write_out(outname);
      if(chdir("..")) {
	fprintf(stderr, "Can`t change directory to `..'\n");
	return 1;
      }
      if(strcmp(ln, "C")==0 || generate_c_code!=0)
	ctab_out(ln);
      for (i = 0; i < CURRENT_SUPPORT_MAX; i++)
	x2type[i] = x2trans[i] = 0;
  }
  return err ? 1 : 0;
}
