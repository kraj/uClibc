/*
 * Generator collate table from glibc special for Uclibc.
 * Author Vladimir Oleynik. vodz@usa.net (c) 2001
 *
 * Require setuped work non-C LC_COLLATE
 * This programm created ./LOCALE/LC_COLLATE file for Uclibc
 * setlocale() and strcoll().
 * Without argument this programm used setlocale(LC_COLLATE, "") -
 * equivalent result setlocale(LC_COLLATE, getenv("LC_XXX"))
 *
 * Also, this programm have russian koi8 collate for test
 * working Uclibc ;-)
 *
 */

#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>   /* mkdir() */
#include <errno.h>


/* For strong test russian locale LC_COLLATE="ru_RU.KOI8-R" */
static const unsigned char koi8_weights[256] = {
  0,   99,  100,  101,  102,  103,  104,  105,
106,    2,    5,    3,    6,    4,  107,  108,
109,  110,  111,  112,  113,  114,  115,  116,
117,  118,  119,  120,  121,  122,  123,  124,
  1,   12,   21,   34,   30,   35,   33,   20,
 22,   23,   31,   36,    9,    8,   15,   14,
127,  128,  129,  131,  132,  133,  134,  135,
136,  137,   11,   10,   38,   40,   42,   13,
 29,  138,  140,  142,  144,  146,  148,  150,
152,  154,  156,  158,  160,  162,  164,  166,
168,  170,  172,  174,  176,  178,  180,  182,
184,  186,  188,   24,   32,   25,   17,    7,
 16,  139,  141,  143,  145,  147,  149,  151,
153,  155,  157,  159,  161,  163,  165,  167,
169,  171,  173,  175,  177,  179,  181,  183,
185,  187,  189,   26,   43,   27,   18,  125,
 50,   52,   54,   58,   62,   66,   70,   74,
 78,   82,   86,   90,   91,   92,   93,   94,
 95,   96,   97,   48,   98,   45,   46,   47,
 39,   41,  126,   49,   44,  130,   19,   37,
 51,   53,   55,  203,   56,   57,   59,   60,
 61,   63,   64,   65,   67,   68,   69,   71,
 72,   73,   75,  202,   76,   77,   79,   80,
 81,   83,   84,   85,   87,   88,   89,   28,
253,  191,  193,  237,  199,  201,  233,  197,
235,  209,  211,  213,  215,  217,  219,  221,
223,  255,  225,  227,  229,  231,  205,  195,
249,  247,  207,  241,  251,  243,  239,  245,
252,  190,  192,  236,  198,  200,  232,  196,
234,  208,  210,  212,  214,  216,  218,  220,
222,  254,  224,  226,  228,  230,  204,  194,
248,  246,  206,  240,  250,  242,  238,  244
};

int gen_weights(const char *collate)
{
	int weights[256];
	int i,j;
	char probe_str1[2];
	char probe_str2[2];
	char print_buf[16];
	int  retcode = 0;
	unsigned char out_weights[256];
	FILE *out;

	memset(weights, 0, sizeof(weights));
	probe_str1[1]=probe_str2[1]=0;

	for(i=0; i<256; i++) {
		probe_str1[0] = i;
		for(j=0; j<256; j++) {
			probe_str2[0] = j;
			if(strcoll(probe_str1, probe_str2)>0) {
				weights[i]++;
				if(i==j) {
					fprintf(stderr, "\
\nWarning! c1=%d == c2, but strcoll returned greater zero\n", i);
				retcode++;
				}
			}
		}
	}
	for(i=0; i<256; ) {
		if(isprint(i))
			sprintf(print_buf, " '%c'", i);
		 else {
			if(i=='\0')
				strcpy(print_buf, "'\\0'");
			else if(i=='\a')
				strcpy(print_buf, "'\\a'");
			else if(i=='\b')
				strcpy(print_buf, "'\\b'");
			else if(i=='\f')
				strcpy(print_buf, "'\\f'");
			else if(i=='\r')
				strcpy(print_buf, "'\\r'");
			else if(i=='\t')
				strcpy(print_buf, "'\\t'");
			else sprintf(print_buf, " x%02X", i);
			}
		printf("weights[%s] = %3d ", print_buf, weights[i]);
		i++;
		if( (i%4) == 0)
			printf("\n");
		}

	for(i=0; i<256; i++) {
		if(weights[i]<0 || weights[i]>=256) {
			fprintf(stderr, "Hmm, weights[%d]=%d\n", i, weights[i]);
			retcode++;
		}
		for(j=0; j<256; j++) {
			if(i==j)
				continue;
			if(weights[i]==weights[j]) {
				fprintf(stderr, "\
Warning! c1=%d c2=%d and strcoll returned equivalent weight\n", i, j);
			retcode++;
			}
		}
	}
	if(retcode)
		return 1;

	if(strcasecmp(collate, "ru_RU.KOI8-R")==0 ||
				strcmp(collate, "ru_RU")==0 ||
					strcmp(collate, "koi8-r")==0) {
		for(i=0; i<256; i++)
			if(weights[i]!=koi8_weights[i]) {
				fprintf(stderr, "\
Error koi8-r collate compare, glibc weights[%d]=%d but current generation %d\n",
					i, koi8_weights[i], weights[i]);
				retcode++;
			}
		if(retcode)
			return 5;
	}
	for(i=0; i<256; i++)
		out_weights[i] = weights[i];
	out = fopen("LC_COLLATE", "w");
	if(out == NULL) {
		fprintf(stderr, "Can`t create ./%s/LC_COLLATE file\n", collate);
		return 10;
	}
	if(fwrite(out_weights, 1, 256, out)!=256) {
		fprintf(stderr, "IO error in process write ./%s/LC_COLLATE file\n", collate);
		return 11;
	}
	return 0;
}

int main(int argc, char **argv)
{
	char *locale;
	char *slr;
	char *collate;

	if(argc<1 || argc>2) {
		fprintf(stderr, "Usage: %s [locale]\n", argv[0]);
	}
	locale = argc==1 ? "" : argv[1];

	collate = setlocale(LC_COLLATE, locale);
	fprintf(stderr, "setlocale(LC_COLLATE, \"%s\") returned %s\n", locale, collate);
	if(collate==0) {
		fprintf(stderr, "Can`t set LC_COLLATE\n");
		return 2;
		}
	if(strcmp(collate, "C")==0) {
		fprintf(stderr, "\
LC_COLLATE=\"C\" is trivial and not interesting for this programm\n");
		return 3;
	}
	slr = setlocale(LC_CTYPE, locale);
	fprintf(stderr, "setlocale(LC_CTYPE, \"%s\") returned %s\n", locale, slr);
	if(slr==0) {
		slr = setlocale(LC_CTYPE, "POSIX");
		if(slr==0) {
			fprintf(stderr, "Hmm, can`t set setlocale(LC_CTYPE, \"POSIX\")\n");
			return 4;
		}
	}
	if(mkdir(collate, 0755)!=0 && errno!=EEXIST) {
		fprintf(stderr, "Can`t make directory %s\n", collate);
		return 6;
	}
	if(chdir(collate)) {
		fprintf(stderr, "Hmm, can`t change directory to %s\n", collate);
		return 7;
	}
	if(gen_weights(collate)) {
		if(chdir("..")) {
			fprintf(stderr, "Hmm, can`t change to current directory\n");
			return 7;
		}
		rmdir(collate);
		return 1;
	}
	return 0;
}
