#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <locale.h>
#include <langinfo.h>
#include <nl_types.h>
#include <stdint.h>

#define __CTYPE_HAS_8_BIT_LOCALES
#include "c8tables.h"


#define CATEGORIES 6

/* must agree with ordering of gen_mmap! */
static const unsigned char *lc_names[] = {
	"LC_CTYPE",
	"LC_NUMERIC",
	"LC_MONETARY",
	"LC_TIME",
	"LC_COLLATE",
	"LC_MESSAGES",
#if CATEGORIES == 12
	"LC_PAPER",
	"LC_NAME",
	"LC_ADDRESS",
	"LC_TELEPHONE",
	"LC_MEASUREMENT",
	"LC_IDENTIFICATION",
#elif CATEGORIES != 6
#error unsupported CATEGORIES value!
#endif
};



typedef struct {
	char *glibc_name;
	char name[5];
	char dot_cs;				/* 0 if no codeset specified */
	char cs;
	unsigned char idx_name;
	unsigned char lc_time_row;
	unsigned char lc_numeric_row;
	unsigned char lc_monetary_row;
	unsigned char lc_messages_row;
#if CATEGORIES != 6
#error unsupported CATEGORIES value
#endif
} locale_entry;

static void read_at_mappings(void);
static void read_enable_disable(void);
static void read_locale_list(void);

static int find_codeset_num(const char *cs);
static int find_at_string_num(const char *as);
static int le_cmp(const void *, const void *);
static void dump_table8(const char *name, const char *tbl, int len);
static void dump_table8c(const char *name, const char *tbl, int len);
static void dump_table16(const char *name, const int *tbl, int len);

static void do_lc_time(void);
static void do_lc_numeric(void);
static void do_lc_monetary(void);

static void do_lc_messages(void);


static FILE *fp;
static FILE *ofp;
static char line_buf[80];
static char at_mappings[256];
static char at_mapto[256];
static char at_strings[1024];
static char *at_strings_end;
static locale_entry locales[700];
static char glibc_locale_names[60000];

static int num_locales;

static int default_utf8;
static int default_8bit;

static int total_size;
static int null_count;

static void do_locale_names(void)
{
	/* "C" locale name is handled specially by the setlocale code. */
	int uniq = 0;
	int i;

	if (num_locales <= 1) {
/*  		printf("error - only C locale?\n"); */
/*  		exit(EXIT_FAILURE); */
		fprintf(ofp, "static const unsigned char __locales[%d];\n", (3 + CATEGORIES));
		fprintf(ofp, "static const unsigned char __locale_names5[5];\n");
	} else {
		fprintf(ofp, "#define CATEGORIES\t\t\t%d\n", CATEGORIES);
		fprintf(ofp, "#define WIDTH_LOCALES\t\t\t%d\n", 3+CATEGORIES);
		fprintf(ofp, "#define NUM_LOCALES\t\t\t%d\n", num_locales);
		fprintf(ofp, "static const unsigned char __locales[%d] = {\n",
				(num_locales) * (3 + CATEGORIES));
		for (i=0 ; i < num_locales ; i++) {
			if (memcmp(locales[i].name, locales[i-1].name, 5) != 0) {
				locales[i].idx_name = uniq;
				++uniq;
			} else {
				locales[i].idx_name = uniq - 1;
			}
			fprintf(ofp, "\t%#4x, ", (int)((unsigned char) locales[i].idx_name));
			fprintf(ofp, "\t%#4x, ", (int)((unsigned char) locales[i].dot_cs));
			fprintf(ofp, "\t%#4x, ", (int)((unsigned char) locales[i].cs));
			/* lc_ctype would store translit flags and turkish up/low flag. */
			fprintf(ofp, "%#4x, ", 0); /* place holder for lc_ctype */
			fprintf(ofp, "%#4x, ", (int)((unsigned char) locales[i].lc_numeric_row));
			fprintf(ofp, "%#4x, ", (int)((unsigned char) locales[i].lc_monetary_row));
			fprintf(ofp, "%#4x, ", (int)((unsigned char) locales[i].lc_time_row));
			fprintf(ofp, "%#4x, ", 0); /* place holder for lc_collate */
			fprintf(ofp, "%#4x, ", (int)((unsigned char) locales[i].lc_messages_row));
			fprintf(ofp, "\t/* %s */\n", locales[i].glibc_name);
		}
		fprintf(ofp, "};\n\n");

		fprintf(ofp, "#define NUM_LOCALE_NAMES\t\t%d\n", uniq );
		fprintf(ofp, "static const unsigned char __locale_names5[%d] = \n\t", uniq * 5);
		uniq = 0;
		for (i=1 ; i < num_locales ; i++) {
			if (memcmp(locales[i].name, locales[i-1].name, 5) != 0) {
				fprintf(ofp, "\"%5.5s\" ", locales[i].name);
				++uniq;
				if ((uniq % 8) == 0) {
					fprintf(ofp, "\n\t");
				}
			}
		}
		fprintf(ofp,";\n\n");

		if (at_strings_end > at_strings) {
			int i, j;
			char *p;
			i = 0;
			p = at_strings;
			while (*p) {
				++i;
				p += 1 + (unsigned char) *p;
			}
			/* len, char, string\0 */
			fprintf(ofp, "#define LOCALE_AT_MODIFIERS_LENGTH\t\t%d\n",
					i + (at_strings_end - at_strings));
			fprintf(ofp, "static const unsigned char __locale_at_modifiers[%d] = {",
					i + (at_strings_end - at_strings));
			i = 0;
			p = at_strings;
			while (*p) {
				fprintf(ofp, "\n\t%4d, '%c',",
						(unsigned char) *p, /* len of string\0 */
						at_mapto[i]);
				for (j=1 ; j < ((unsigned char) *p) ; j++) {
					fprintf(ofp, " '%c',", p[j]);
				}
				fprintf(ofp, " 0,");
				++i;
				p += 1 + (unsigned char) *p;
			}
			fprintf(ofp, "\n};\n\n");
		}

		{
			int pos[CATEGORIES];
			pos[0] = CATEGORIES;
			for (i=0 ; i < CATEGORIES ; i++) {
				fprintf(ofp, "#define __%s\t\t%d\n", lc_names[i], i);
				if (i + 1 < CATEGORIES) {
					pos[i+1] = 1 + strlen(lc_names[i]) + pos[i];
				}
			}
			if (pos[CATEGORIES-1] > 255) {
				printf("error - lc_names is too big (%d)\n", pos[CATEGORIES-1]);
				exit(EXIT_FAILURE);
			}
			fprintf(ofp, "#define __LC_ALL\t\t%d\n\n", i);

			fprintf(ofp, "#define lc_names_LEN\t\t%d\n",
					pos[CATEGORIES-1] + strlen(lc_names[CATEGORIES-1]) + 1);
			total_size += pos[CATEGORIES-1] + strlen(lc_names[CATEGORIES-1]) + 1;

			fprintf(ofp, "static unsigned const char lc_names[%d] =\n",
					pos[CATEGORIES-1] + strlen(lc_names[CATEGORIES-1]) + 1);
			fprintf(ofp, "\t\"");
			for (i=0 ; i < CATEGORIES ; i++) {
				fprintf(ofp, "\\x%02x", (unsigned char) pos[i]);
			}
			fprintf(ofp, "\"");
			for (i=0 ; i < CATEGORIES ; i++) {
				fprintf(ofp, "\n\t\"%s\\0\"", lc_names[i]);
			}
			fprintf(ofp, ";\n\n");
		}

		printf("locale data = %d  name data = %d for %d uniq\n",
			   num_locales * (3 + CATEGORIES), uniq * 5, uniq);

		total_size += num_locales * (3 + CATEGORIES) + uniq * 5;
	}

}

int main(int argc, char **argv)
{
	if ((argc != 2) || (!(fp = fopen(*++argv, "r")))) {
		printf("error: missing filename or file!\n");
		return EXIT_FAILURE;
	}

	at_strings_end = at_strings;

	read_at_mappings();
	read_enable_disable();
	read_locale_list();

	fclose(fp);

	/* handle C locale specially */
	qsort(locales+1, num_locales-1, sizeof(locale_entry), le_cmp);

#if 0
	for (i=0 ; i < num_locales ; i++) {
		printf("locale: %5.5s %2d %2d %s\n",
			   locales[i].name,
			   locales[i].cs,
			   locales[i].dot_cs,
			   locales[i].glibc_name
			   );
	}
#endif

	if (!(ofp = fopen("locale_tables.h", "w"))) {
		printf("error: can not open locale_tables.h for writing!\n");
		return EXIT_FAILURE;
	}

	do_lc_time();
	do_lc_numeric();
	do_lc_monetary();
	do_lc_messages();

	do_locale_names();

	fclose(ofp);

	printf("total data size = %d\n", total_size);
	printf("null count = %d\n", null_count);

	return EXIT_SUCCESS;
}

static char *idx[10000];
static char buf[100000];
static char *last;
static int uniq;

static int addstring(const char *s)
{
	int j;
	size_t l;

	if (!s) {
		++null_count;
		return 0;
	}

	for (j=0 ; j < uniq ; j++) {
		if (!strcmp(s, idx[j])) {
			return idx[j] - buf;
		}
	}
	if (uniq >= sizeof(idx)) {
		printf("too many uniq strings!\n");
		exit(EXIT_FAILURE);
	}
	l = strlen(s) + 1;
	if (last + l >= buf + sizeof(buf)) {
		printf("need to increase size of buf!\n");
		exit(EXIT_FAILURE);
	}

	idx[uniq] = last;
	++uniq;
	strcpy(last, s);
	last += l;
	return idx[uniq - 1] - buf;
}

static int buf16[50*256];


static int lc_time_item[50][256];
static int lc_time_count[50];
static unsigned char lc_time_uniq_50[700][50];
static int lc_time_uniq;

#define DO_NL_S(X)	lc_time_S(X, k++)

static void lc_time_S(int X, int k)
{
	int j, m;
	j = addstring(nl_langinfo(X));
	for (m=0 ; m < lc_time_count[k] ; m++) {
		if (lc_time_item[k][m] == j) {
			break;
		}
	}
	if (m == lc_time_count[k]) { /* new for this nl_item */
		if (m > 255) {
			printf("too many nl_item %d entries in lc_time\n", k);
			exit(EXIT_FAILURE);
		}
		lc_time_item[k][m] = j;
		++lc_time_count[k];
	}
/*  	printf("\\x%02x", m); */
	lc_time_uniq_50[lc_time_uniq][k] = m;
}

static void do_lc_time(void)
{
	int i, k, m;

	last = buf+1;
	uniq = 1;
	*buf = 0;
	*idx = buf;

/*  	printf("processing lc_time..."); */
	for (i=0 ; i < num_locales ; i++) {
		k = 0;

/*  		printf(" %d", i); fflush(stdout); */
		if (!setlocale(LC_ALL, locales[i].glibc_name)) {
			printf("setlocale(LC_ALL,%s) failed!\n",
				   locales[i].glibc_name);
		}

		DO_NL_S(ABDAY_1);
		DO_NL_S(ABDAY_2);
		DO_NL_S(ABDAY_3);
		DO_NL_S(ABDAY_4);
		DO_NL_S(ABDAY_5);
		DO_NL_S(ABDAY_6);
		DO_NL_S(ABDAY_7);

		DO_NL_S(DAY_1);
		DO_NL_S(DAY_2);
		DO_NL_S(DAY_3);
		DO_NL_S(DAY_4);
		DO_NL_S(DAY_5);
		DO_NL_S(DAY_6);
		DO_NL_S(DAY_7);

		DO_NL_S(ABMON_1);
		DO_NL_S(ABMON_2);
		DO_NL_S(ABMON_3);
		DO_NL_S(ABMON_4);
		DO_NL_S(ABMON_5);
		DO_NL_S(ABMON_6);
		DO_NL_S(ABMON_7);
		DO_NL_S(ABMON_8);
		DO_NL_S(ABMON_9);
		DO_NL_S(ABMON_10);
		DO_NL_S(ABMON_11);
		DO_NL_S(ABMON_12);

		DO_NL_S(MON_1);
		DO_NL_S(MON_2);
		DO_NL_S(MON_3);
		DO_NL_S(MON_4);
		DO_NL_S(MON_5);
		DO_NL_S(MON_6);
		DO_NL_S(MON_7);
		DO_NL_S(MON_8);
		DO_NL_S(MON_9);
		DO_NL_S(MON_10);
		DO_NL_S(MON_11);
		DO_NL_S(MON_12);

		DO_NL_S(AM_STR);
		DO_NL_S(PM_STR);

		DO_NL_S(D_T_FMT);
		DO_NL_S(D_FMT);
		DO_NL_S(T_FMT);
		DO_NL_S(T_FMT_AMPM);
		DO_NL_S(ERA);

		DO_NL_S(ERA_YEAR);		/* non SuSv3 */
		DO_NL_S(ERA_D_FMT);
		DO_NL_S(ALT_DIGITS);
		DO_NL_S(ERA_D_T_FMT);
		DO_NL_S(ERA_T_FMT);

		if (k > 50) {
			printf("error -- lc_time nl_item count > 50!\n");
			exit(EXIT_FAILURE);
		}

		{
			int r;
			for (r=0 ; r < lc_time_uniq ; r++) {
				if (!memcmp(lc_time_uniq_50[lc_time_uniq],
							lc_time_uniq_50[r], 50)) {
					break;
				}
			}
			if (r == lc_time_uniq) { /* new locale row */
				++lc_time_uniq;
				if (lc_time_uniq > 255) {
					printf("too many unique lc_time rows!\n");
					exit(EXIT_FAILURE);
				}
			}
			locales[i].lc_time_row = r;
		}
/*  		printf("\n"); */
	}
/*  	printf(" done\n"); */

	m = k = 0;
	for (i=0 ; i < 50 ; i++) {
		k += lc_time_count[i];
		if (m < lc_time_count[i]) {
			m = lc_time_count[i];
		}
	}
	printf("buf-size=%d  uniq=%d  item_offsets=%d  max=%d  rows=%d\n",
		   (int)(last - buf), uniq, k, m, lc_time_uniq);
/*  	printf("total = %d * 50 + 2 * (50 + %d) + %d = %d\n", */
/*  		   num_locales, k, (int)(last - buf), */
/*  		   num_locales*50 + 2*(50 + k) + (int)(last - buf)); */
	printf("total = %d + %d * 50 + 2 * (50 + %d) + %d = %d\n",
		   num_locales, lc_time_uniq, k, (int)(last - buf),
		   i = num_locales + lc_time_uniq*50 + 2*(50 + k) + (int)(last - buf));
	total_size += i;

	dump_table8c("__lc_time_data", buf, (int)(last - buf));

	for (i=0 ; i < lc_time_uniq ; i++) {
		m = locales[i].lc_time_row;
		for (k=0 ; k < 50 ; k++) {
			buf[50*i + k] = (char)((unsigned char) lc_time_uniq_50[i][k]);
		}
	}
	dump_table8("__lc_time_rows", buf, lc_time_uniq * 50);

	buf16[0] =0;
	for (i=0 ; i < 50 - 1 ; i++) {
		buf16[i+1] = buf16[i] + lc_time_count[i];
	}
	dump_table16("__lc_time_item_offsets", buf16, 50);

	m = 0;
	for (k=0 ; k < 50 ; k++) {
		for (i=0 ; i < lc_time_count[k] ; i++) {
			buf16[m] = lc_time_item[k][i];
			++m;
		}
	}
	dump_table16("__lc_time_item_idx", buf16, m);
}

static void dump_table8(const char *name, const char *tbl, int len)
{
	int i;

	fprintf(ofp, "#define %s_LEN\t\t%d\n", name, len);
	fprintf(ofp, "static const unsigned char %s[%d] = {", name, len);
	for (i=0 ; i < len ; i++) {
		if ((i % 12) == 0) {
			fprintf(ofp, "\n\t");
		}
		fprintf(ofp, "%#4x, ", (int)((unsigned char) tbl[i]));
	}
	fprintf(ofp, "\n};\n\n");
}

#define __C_isdigit(c) \
	((sizeof(c) == sizeof(char)) \
	 ? (((unsigned char)((c) - '0')) < 10) \
	 : (((unsigned int)((c) - '0')) < 10))
#define __C_isalpha(c) \
	((sizeof(c) == sizeof(char)) \
	 ? (((unsigned char)(((c) | 0x20) - 'a')) < 26) \
	 : (((unsigned int)(((c) | 0x20) - 'a')) < 26))
#define __C_isalnum(c) (__C_isalpha(c) || __C_isdigit(c))

static void dump_table8c(const char *name, const char *tbl, int len)
{
	int i;

	fprintf(ofp, "#define %s_LEN\t\t%d\n", name, len);
	fprintf(ofp, "static const unsigned char %s[%d] = {", name, len);
	for (i=0 ; i < len ; i++) {
		if ((i % 12) == 0) {
			fprintf(ofp, "\n\t");
		}
		if (__C_isalnum(tbl[i]) || (tbl[i] == ' ')) {
			fprintf(ofp, " '%c', ", (int)((unsigned char) tbl[i]));
		} else {
			fprintf(ofp, "%#4x, ", (int)((unsigned char) tbl[i]));
		}
	}
	fprintf(ofp, "\n};\n\n");
}

static void dump_table16(const char *name, const int *tbl, int len)
{
	int i;

	fprintf(ofp, "#define %s_LEN\t\t%d\n", name, len);
	fprintf(ofp, "static const uint16_t %s[%d] = {", name, len);
	for (i=0 ; i < len ; i++) {
		if ((i % 8) == 0) {
			fprintf(ofp, "\n\t");
		}
		if (tbl[i] != (uint16_t) tbl[i]) {
			printf("error - falls outside uint16 range!\n");
			exit(EXIT_FAILURE);
		}
		fprintf(ofp, "%#6x, ", tbl[i]);
	}
	fprintf(ofp, "\n};\n\n");
}

#undef DO_NL_S

static int lc_numeric_item[3][256];
static int lc_numeric_count[3];
static unsigned char lc_numeric_uniq_3[700][3];
static int lc_numeric_uniq;

#define DO_NL_S(X)	lc_numeric_S(X, k++)

static void lc_numeric_S(int X, int k)
{
	int j, m;
	j = addstring(nl_langinfo(X));
	for (m=0 ; m < lc_numeric_count[k] ; m++) {
		if (lc_numeric_item[k][m] == j) {
			break;
		}
	}
	if (m == lc_numeric_count[k]) { /* new for this nl_item */
		if (m > 255) {
			printf("too many nl_item %d entries in lc_numeric\n", k);
			exit(EXIT_FAILURE);
		}
		lc_numeric_item[k][m] = j;
		++lc_numeric_count[k];
	}
/*  	printf("\\x%02x", m); */
	lc_numeric_uniq_3[lc_numeric_uniq][k] = m;
}

static void do_lc_numeric(void)
{
	int i, k, m;

	last = buf+1;
	uniq = 1;
	*buf = 0;
	*idx = buf;

	for (i=0 ; i < num_locales ; i++) {
		k = 0;

		if (!setlocale(LC_ALL, locales[i].glibc_name)) {
			printf("setlocale(LC_ALL,%s) failed!\n",
				   locales[i].glibc_name);
		}

		DO_NL_S(RADIXCHAR);		/* DECIMAL_POINT */
		DO_NL_S(THOUSEP);		/* THOUSANDS_SEP */
		DO_NL_S(GROUPING);

		if (k > 3) {
			printf("error -- lc_numeric nl_item count > 3!\n");
			exit(EXIT_FAILURE);
		}

		{
			int r;
			for (r=0 ; r < lc_numeric_uniq ; r++) {
				if (!memcmp(lc_numeric_uniq_3[lc_numeric_uniq],
							lc_numeric_uniq_3[r], 3)) {
					break;
				}
			}
			if (r == lc_numeric_uniq) { /* new locale row */
				++lc_numeric_uniq;
				if (lc_numeric_uniq > 255) {
					printf("too many unique lc_numeric rows!\n");
					exit(EXIT_FAILURE);
				}
			}
			locales[i].lc_numeric_row = r;
		}
	}

	printf("buf-size=%d  uniq=%d  rows=%d\n",
		   (int)(last - buf), uniq, lc_numeric_uniq);
	printf("total = %d + %d * 3 + %d = %d\n",
		   num_locales, lc_numeric_uniq, (int)(last - buf),
		   i = num_locales + lc_numeric_uniq*3 + (int)(last - buf));
	total_size += i;

/*  	printf("buf-size=%d  uniq=%d\n", (int)(last - buf), uniq); */

	dump_table8c("__lc_numeric_data", buf, (int)(last - buf));


	for (i=0 ; i < lc_numeric_uniq ; i++) {
		m = locales[i].lc_numeric_row;
		for (k=0 ; k < 3 ; k++) {
			buf[3*i + k] = (char)((unsigned char) lc_numeric_uniq_3[i][k]);
		}
	}
	dump_table8("__lc_numeric_rows", buf, lc_numeric_uniq * 3);

	buf16[0] =0;
	for (i=0 ; i < 3 - 1 ; i++) {
		buf16[i+1] = buf16[i] + lc_numeric_count[i];
	}
	dump_table16("__lc_numeric_item_offsets", buf16, 3);

	m = 0;
	for (k=0 ; k < 3 ; k++) {
		for (i=0 ; i < lc_numeric_count[k] ; i++) {
			buf16[m] = lc_numeric_item[k][i];
			++m;
		}
	}
	dump_table16("__lc_numeric_item_idx", buf16, m);
}

#undef DO_NL_S

/*  #define NUM_NL_MONETARY 7 */
#define NUM_NL_MONETARY (7+14+1)

static int lc_monetary_item[NUM_NL_MONETARY][256];
static int lc_monetary_count[NUM_NL_MONETARY];
static unsigned char lc_monetary_uniq_X[700][NUM_NL_MONETARY];
static int lc_monetary_uniq;

#define DO_NL_S(X)	lc_monetary_S(X, k++)

/*  #define DO_NL_C(X)		printf("%#02x", (int)(unsigned char)(*nl_langinfo(X))); */
#define DO_NL_C(X) lc_monetary_C(X, k++)

static void lc_monetary_C(int X, int k)
{
	int j, m;
	char c_buf[2];

#warning fix the char entries for monetary... target signedness of char may be different!

	c_buf[1] = 0;
	c_buf[0] = *nl_langinfo(X);
	j = addstring(c_buf);
	for (m=0 ; m < lc_monetary_count[k] ; m++) {
		if (lc_monetary_item[k][m] == j) {
			break;
		}
	}
	if (m == lc_monetary_count[k]) { /* new for this nl_item */
		if (m > 255) {
			printf("too many nl_item %d entries in lc_monetary\n", k);
			exit(EXIT_FAILURE);
		}
		lc_monetary_item[k][m] = j;
		++lc_monetary_count[k];
	}
/*  	printf("\\x%02x", m); */
	lc_monetary_uniq_X[lc_monetary_uniq][k] = m;
}


static void lc_monetary_S(int X, int k)
{
	int j, m;
	j = addstring(nl_langinfo(X));
	for (m=0 ; m < lc_monetary_count[k] ; m++) {
		if (lc_monetary_item[k][m] == j) {
			break;
		}
	}
	if (m == lc_monetary_count[k]) { /* new for this nl_item */
		if (m > 255) {
			printf("too many nl_item %d entries in lc_monetary\n", k);
			exit(EXIT_FAILURE);
		}
		lc_monetary_item[k][m] = j;
		++lc_monetary_count[k];
	}
/*  	printf("\\x%02x", m); */
	lc_monetary_uniq_X[lc_monetary_uniq][k] = m;
}

static void do_lc_monetary(void)
{
	int i, k, m;

	last = buf+1;
	uniq = 1;
	*buf = 0;
	*idx = buf;

	for (i=0 ; i < num_locales ; i++) {
		k = 0;

		if (!setlocale(LC_ALL, locales[i].glibc_name)) {
			printf("setlocale(LC_ALL,%s) failed!\n",
				   locales[i].glibc_name);
		}


		/* non SUSv3 */
		DO_NL_S(INT_CURR_SYMBOL);
		DO_NL_S(CURRENCY_SYMBOL);
		DO_NL_S(MON_DECIMAL_POINT);
		DO_NL_S(MON_THOUSANDS_SEP);
		DO_NL_S(MON_GROUPING);
		DO_NL_S(POSITIVE_SIGN);
		DO_NL_S(NEGATIVE_SIGN);
		DO_NL_C(INT_FRAC_DIGITS);
		DO_NL_C(FRAC_DIGITS);
		DO_NL_C(P_CS_PRECEDES);
		DO_NL_C(P_SEP_BY_SPACE);
		DO_NL_C(N_CS_PRECEDES);
		DO_NL_C(N_SEP_BY_SPACE);
		DO_NL_C(P_SIGN_POSN);
		DO_NL_C(N_SIGN_POSN);
		DO_NL_C(INT_P_CS_PRECEDES);
		DO_NL_C(INT_P_SEP_BY_SPACE);
		DO_NL_C(INT_N_CS_PRECEDES);
		DO_NL_C(INT_N_SEP_BY_SPACE);
		DO_NL_C(INT_P_SIGN_POSN);
		DO_NL_C(INT_N_SIGN_POSN);

		DO_NL_S(CRNCYSTR);		/* CURRENCY_SYMBOL */

/*  		printf("\n"); */

		if (k > NUM_NL_MONETARY) {
			printf("error -- lc_monetary nl_item count > %d!\n", NUM_NL_MONETARY);
			exit(EXIT_FAILURE);
		}

		{
			int r;
			for (r=0 ; r < lc_monetary_uniq ; r++) {
				if (!memcmp(lc_monetary_uniq_X[lc_monetary_uniq],
							lc_monetary_uniq_X[r], NUM_NL_MONETARY)) {
					break;
				}
			}
			if (r == lc_monetary_uniq) { /* new locale row */
				++lc_monetary_uniq;
				if (lc_monetary_uniq > 255) {
					printf("too many unique lc_monetary rows!\n");
					exit(EXIT_FAILURE);
				}
			}
			locales[i].lc_monetary_row = r;
		}
	}

	printf("buf-size=%d  uniq=%d  rows=%d\n",
		   (int)(last - buf), uniq, lc_monetary_uniq);
	printf("total = %d + %d * %d + %d = %d\n",
		   num_locales, lc_monetary_uniq, NUM_NL_MONETARY, (int)(last - buf),
		   i = num_locales + lc_monetary_uniq*NUM_NL_MONETARY + (int)(last - buf));
	total_size += i;

	dump_table8c("__lc_monetary_data", buf, (int)(last - buf));

	for (i=0 ; i < lc_monetary_uniq ; i++) {
		m = locales[i].lc_monetary_row;
		for (k=0 ; k < NUM_NL_MONETARY ; k++) {
			buf[NUM_NL_MONETARY*i + k] = (char)((unsigned char) lc_monetary_uniq_X[i][k]);
		}
	}
	dump_table8("__lc_monetary_rows", buf, lc_monetary_uniq * NUM_NL_MONETARY);

	buf16[0] =0;
	for (i=0 ; i < NUM_NL_MONETARY - 1 ; i++) {
		buf16[i+1] = buf16[i] + lc_monetary_count[i];
	}
	dump_table16("__lc_monetary_item_offsets", buf16, NUM_NL_MONETARY);

	m = 0;
	for (k=0 ; k < NUM_NL_MONETARY ; k++) {
		for (i=0 ; i < lc_monetary_count[k] ; i++) {
			buf16[m] = lc_monetary_item[k][i];
			++m;
		}
	}
	dump_table16("__lc_monetary_item_idx", buf16, m);
}


#undef DO_NL_S

static int lc_messages_item[2][256];
static int lc_messages_count[2];
static unsigned char lc_messages_uniq_2[700][2];
static int lc_messages_uniq;

#define DO_NL_S(X)	lc_messages_S(X, k++)

static void lc_messages_S(int X, int k)
{
	int j, m;
	j = addstring(nl_langinfo(X));
	for (m=0 ; m < lc_messages_count[k] ; m++) {
		if (lc_messages_item[k][m] == j) {
			break;
		}
	}
	if (m == lc_messages_count[k]) { /* new for this nl_item */
		if (m > 255) {
			printf("too many nl_item %d entries in lc_messages\n", k);
			exit(EXIT_FAILURE);
		}
		lc_messages_item[k][m] = j;
		++lc_messages_count[k];
	}
/*  	printf("\\x%02x", m); */
	lc_messages_uniq_2[lc_messages_uniq][k] = m;
}

static void do_lc_messages(void)
{
	int i, k, m;

	last = buf+1;
	uniq = 1;
	*buf = 0;
	*idx = buf;

	for (i=0 ; i < num_locales ; i++) {
		k = 0;

		if (!setlocale(LC_ALL, locales[i].glibc_name)) {
			printf("setlocale(LC_ALL,%s) failed!\n",
				   locales[i].glibc_name);
		}

		DO_NL_S(YESEXPR);
		DO_NL_S(NOEXPR);

		if (k > 2) {
			printf("error -- lc_messages nl_item count > 2!\n");
			exit(EXIT_FAILURE);
		}

		{
			int r;
			for (r=0 ; r < lc_messages_uniq ; r++) {
				if (!memcmp(lc_messages_uniq_2[lc_messages_uniq],
							lc_messages_uniq_2[r], 2)) {
					break;
				}
			}
			if (r == lc_messages_uniq) { /* new locale row */
				++lc_messages_uniq;
				if (lc_messages_uniq > 255) {
					printf("too many unique lc_messages rows!\n");
					exit(EXIT_FAILURE);
				}
			}
			locales[i].lc_messages_row = r;
		}
	}

	printf("buf-size=%d  uniq=%d  rows=%d\n",
		   (int)(last - buf), uniq, lc_messages_uniq);
	printf("total = %d + %d * 2 + %d = %d\n",
		   num_locales, lc_messages_uniq, (int)(last - buf),
		   i = num_locales + lc_messages_uniq*2 + (int)(last - buf));
	total_size += i;

/*  	printf("buf-size=%d  uniq=%d\n", (int)(last - buf), uniq); */

	dump_table8c("__lc_messages_data", buf, (int)(last - buf));

	for (i=0 ; i < lc_messages_uniq ; i++) {
		m = locales[i].lc_messages_row;
		for (k=0 ; k < 2 ; k++) {
			buf[2*i + k] = (char)((unsigned char) lc_messages_uniq_2[i][k]);
		}
	}
	dump_table8("__lc_messages_rows", buf, lc_messages_uniq * 2);

	buf16[0] =0;
	for (i=0 ; i < 2 - 1 ; i++) {
		buf16[i+1] = buf16[i] + lc_messages_count[i];
	}
	dump_table16("__lc_messages_item_offsets", buf16, 2);

	m = 0;
	for (k=0 ; k < 2 ; k++) {
		for (i=0 ; i < lc_messages_count[k] ; i++) {
			buf16[m] = lc_messages_item[k][i];
			++m;
		}
	}
	dump_table16("__lc_messages_item_idx", buf16, m);
}





static void read_at_mappings(void)
{
	char *p;
	char *m;
	int mc = 0;

	do {
		if (!(p = strtok(line_buf, " \t\n")) || (*p == '#')) {
			if (!fgets(line_buf, sizeof(line_buf), fp)) {
				if (ferror(fp)) {
					printf("error reading file\n");
					exit(EXIT_FAILURE);
				}
				return;			/* EOF */
			}
			if ((*line_buf == '#') && (line_buf[1] == '-')) {
				break;
			}
			continue;
		}
		if (*p == '@') {
			if (p[1] == 0) {
				printf("error: missing @modifier name\n");
				exit(EXIT_FAILURE);
			}
			m = p;				/* save the modifier name */
			if (!(p = strtok(NULL, " \t\n")) || p[1] || (((unsigned char) *p) > 0x7f)) {
				printf("error: missing or illegal @modifier mapping char\n");
				exit(EXIT_FAILURE);
			}
			if (at_mappings[(int)((unsigned char) *p)]) {
				printf("error: reused @modifier mapping char\n");
				exit(EXIT_FAILURE);
			}
			at_mappings[(int)((unsigned char) *p)] = 1;
			at_mapto[mc] = *p;
			++mc;
			*at_strings_end = (char)( (unsigned char) (strlen(m)) );
			strcpy(++at_strings_end, m+1);
			at_strings_end += (unsigned char) at_strings_end[-1];

			printf("@mapping: \"%s\" to '%c'\n", m, *p);

			if (((p = strtok(NULL, " \t\n")) != NULL) && (*p != '#')) {
				printf("ignoring trailing text: %s...\n", p);
			}
			*line_buf = 0;
			continue;
		}
		break;
	} while (1);

#if 0
	{
		p = at_strings;
		
		if (!*p) {
			printf("no @ strings\n");
			return;
		}
		
		do {
			printf("%s\n", p+1);
			p += 1 + (unsigned char) *p;
		} while (*p);
	}
#endif
}

static void read_enable_disable(void)
{
	char *p;

	do {
		if (!(p = strtok(line_buf, " =\t\n")) || (*p == '#')) {
			if (!fgets(line_buf, sizeof(line_buf), fp)) {
				if (ferror(fp)) {
					printf("error reading file\n");
					exit(EXIT_FAILURE);
				}
				return;			/* EOF */
			}
			if ((*line_buf == '#') && (line_buf[1] == '-')) {
				break;
			}
			continue;
		}
		if (!strcmp(p, "UTF-8")) {
			if (!(p = strtok(NULL, " =\t\n"))
				|| ((toupper(*p) != 'Y') && (toupper(*p) != 'N'))) {
				printf("error: missing or illegal UTF-8 setting\n");
				exit(EXIT_FAILURE);
			}
			default_utf8 = (toupper(*p) == 'Y');
			printf("UTF-8 locales are %sabled\n", "dis\0en"+ (default_utf8 << 2));
		} else if (!strcmp(p, "8-BIT")) {
			if (!(p = strtok(NULL, " =\t\n"))
				|| ((toupper(*p) != 'Y') && (toupper(*p) != 'N'))) {
				printf("error: missing or illegal 8-BIT setting\n");
				exit(EXIT_FAILURE);
			}
			default_8bit = (toupper(*p) == 'Y');
			printf("8-BIT locales are %sabled\n", "dis\0en" + (default_8bit << 2));
		} else {
			break;
		}

		if (((p = strtok(NULL, " \t\n")) != NULL) && (*p != '#')) {
			printf("ignoring trailing text: %s...\n", p);
		}
		*line_buf = 0;
		continue;

	} while (1);
}

#ifdef CODESET_LIST

static int find_codeset_num(const char *cs)
{
	int r = 2;
	char *s = CODESET_LIST;

	/* 7-bit is 1, UTF-8 is 2, 8-bits are > 2 */

	if (strcmp(cs, "UTF-8") != 0) {
		++r;
		while (*s && strcmp(CODESET_LIST+ ((unsigned char) *s), cs)) {
/*  			printf("tried %s\n", CODESET_LIST + ((unsigned char) *s)); */
			++r;
			++s;
		}
		if (!*s) {
			printf("error: unsupported codeset %s\n", cs);
			exit(EXIT_FAILURE);
		}
	}
	return r;
}

#else

static int find_codeset_num(const char *cs)
{
	int r = 2;

	/* 7-bit is 1, UTF-8 is 2, 8-bits are > 2 */

	if (strcmp(cs, "UTF-8") != 0) {
		printf("error: unsupported codeset %s\n", cs);
		exit(EXIT_FAILURE);
	}
	return r;
}

#endif

static int find_at_string_num(const char *as)
{
	int i = 0;
	char *p = at_strings;

	while (*p) {
		if (!strcmp(p+1, as)) {
			return i;
		}
		++i;
		p += 1 + (unsigned char) *p;
	}

	printf("error: unmapped @string %s\n", as);
	exit(EXIT_FAILURE);
}

static void read_locale_list(void)
{
	char *p;
	char *s;
	char *ln;					/* locale name */
	char *ls;					/* locale name ll_CC */
	char *as;					/* at string */
	char *ds;					/* dot string */
	char *cs;					/* codeset */
	int i;

	typedef struct {
		char *glibc_name;
		char name[5];
		char dot_cs;				/* 0 if no codeset specified */
		char cs;
	} locale_entry;

	/* First the C locale. */
	locales[0].glibc_name = locales[0].name;
	strncpy(locales[0].name,"C",5);
	locales[0].dot_cs = 0;
	locales[0].cs = 1;			/* 7-bit encoding */
	++num_locales;

	do {
		if (!(p = strtok(line_buf, " \t\n")) || (*p == '#')) {
			if (!fgets(line_buf, sizeof(line_buf), fp)) {
				if (ferror(fp)) {
					printf("error reading file\n");
					exit(EXIT_FAILURE);
				}
				return;			/* EOF */
			}
			if ((*line_buf == '#') && (line_buf[1] == '-')) {
				break;
			}
			continue;
		}

		s = glibc_locale_names;
		for (i=0 ; i < num_locales ; i++) {
			if (!strcmp(s+1, p)) {
				break;
			}
			s += 1 + ((unsigned char) *s);
		}
		if (i < num_locales) {
			printf("ignoring dulplicate locale name: %s", p);
			*line_buf = 0;
			continue;
		}

		/* New locale, but don't increment num until codeset verified! */
		*s = (char)((unsigned char) (strlen(p) + 1));
		strcpy(s+1, p);
		locales[num_locales].glibc_name = s+1;
		ln = p;					/* save locale name */

		if (!(p = strtok(NULL, " \t\n"))) {
			printf("error: missing codeset for locale %s\n", ln);
			exit(EXIT_FAILURE);
		}
		cs = p;
		i = find_codeset_num(p);
		if ((i == 2) && !default_utf8) {
			printf("ignoring UTF-8 locale %s\n", ln);
			*line_buf = 0;
			continue;
		} else if ((i > 2) && !default_8bit) {	
			printf("ignoring 8-bit codeset locale %s\n", ln);
			*line_buf = 0;
			continue;
		}
		locales[num_locales].cs = (char)((unsigned char) i);

		if (((p = strtok(NULL, " \t\n")) != NULL) && (*p != '#')) {
			printf("ignoring trailing text: %s...\n", p);
		}

		/* Now go back to locale string for .codeset and @modifier */
		as = strtok(ln, "@");
		if (as) {
			as = strtok(NULL, "@");
		}
		ds = strtok(ln, ".");
		if (ds) {
			ds = strtok(NULL, ".");
		}
		ls = ln;

		if ((strlen(ls) != 5) || (ls[2] != '_')) {
			printf("error: illegal locale name %s\n", ls);
			exit(EXIT_FAILURE);
		}

		i = 0;					/* value for unspecified codeset */
		if (ds) {
			i = find_codeset_num(ds);
			if ((i == 2) && !default_utf8) {
				printf("ignoring UTF-8 locale %s\n", ln);
				*line_buf = 0;
				continue;
			} else if ((i > 2) && !default_8bit) {	
				printf("ignoring 8-bit codeset locale %s\n", ln);
				*line_buf = 0;
				continue;
			}
		}
		locales[num_locales].dot_cs = (char)((unsigned char) i);

		if (as) {
			i = find_at_string_num(as);
			ls[2] = at_mapto[i];
		}
		memcpy(locales[num_locales].name, ls, 5);
/*  		printf("locale: %5.5s %2d %2d %s\n", */
/*  			   locales[num_locales].name, */
/*  			   locales[num_locales].cs, */
/*  			   locales[num_locales].dot_cs, */
/*  			   locales[num_locales].glibc_name */
/*  			   ); */
		++num_locales;
		*line_buf = 0;
	} while (1);
}

static int le_cmp(const void *a, const void *b)
{
	const locale_entry *p;
	const locale_entry *q;
	int r;

	p = (const locale_entry *) a;
	q = (const locale_entry *) b;

	if (!(r = p->name[0] - q->name[0])
		&& !(r = p->name[1] - q->name[1])
		&& !(r = p->name[3] - q->name[3])
		&& !(r = p->name[4] - q->name[4])
		&& !(r = p->name[2] - q->name[2])
		&& !(r = -(p->cs - q->cs))
		) {
		r = -(p->dot_cs - q->dot_cs);
		/* Reverse the ordering of the codesets so UTF-8 comes last.
		 * Work-around (hopefully) for glibc bug affecting at least
		 * the euro currency symbol. */
	}

	return r;
}

