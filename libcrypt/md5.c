/*
 * MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 *
 * $FreeBSD: src/lib/libmd/md5c.c,v 1.9.2.1 1999/08/29 14:57:12 peter Exp $
 *
 * This code is the same as the code published by RSA Inc.  It has been
 * edited for clarity and style only.
 *
 * ----------------------------------------------------------------------------
 * The md5_crypt() function was taken from freeBSD's libcrypt and contains 
 * this license: 
 *    "THE BEER-WARE LICENSE" (Revision 42):
 *     <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 *     can do whatever you want with this stuff. If we meet some day, and you think
 *     this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 *
 * $FreeBSD: src/lib/libcrypt/crypt.c,v 1.7.2.1 1999/08/29 14:56:33 peter Exp $
 *
 * ----------------------------------------------------------------------------
 * On April 19th, 2001 md5_crypt() was modified to make it reentrant 
 * by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 *
 * June 28, 2001             Manuel Novoa III
 *
 * "Un-inlined" code using loops and static const tables in order to
 * reduce generated code size (on i386 from approx 4k to approx 2.5k).
 *
 * WARNING!!! Changed PADDING array from a staticly allocated object to
 *            a dynamicly generated one.  Although it was declared static
 *            and not static const, it doesn't appear that it ever changes.
 */

#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <crypt.h>
#include <sys/cdefs.h>
	
/* MD5 context. */
typedef struct MD5Context {
  u_int32_t state[4];	/* state (ABCD) */
  u_int32_t count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
} MD5_CTX;

void   MD5Init (MD5_CTX *);
void   MD5Update (MD5_CTX *, const unsigned char *, unsigned int);
void   MD5Pad (MD5_CTX *);
void   MD5Final (unsigned char [16], MD5_CTX *);
char * MD5End(MD5_CTX *, char *);
char * MD5File(const char *, char *);
char * MD5Data(const unsigned char *, unsigned int, char *);
char * md5_crypt_r( const char *pw, const char *salt, struct crypt_data * data);


char	*md5_magic = "$1$";	/* * This string is magic for this algorithm.  Having 
				   it this way, we can get better later on */
static const unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


static void MD5Transform __P((u_int32_t [4], const unsigned char [64]));

#ifdef KERNEL
#define memset(x,y,z)	bzero(x,z);
#define memcpy(x,y,z)	bcopy(y, x, z)
#endif

#ifdef i386
#define Encode memcpy
#define Decode memcpy
#else /* i386 */

/*
 * Encodes input (u_int32_t) into output (unsigned char). Assumes len is
 * a multiple of 4.
 */

static void
Encode (output, input, len)
	unsigned char *output;
	u_int32_t *input;
	unsigned int len;
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/*
 * Decodes input (unsigned char) into output (u_int32_t). Assumes len is
 * a multiple of 4.
 */

static void
Decode (output, input, len)
	u_int32_t *output;
	const unsigned char *input;
	unsigned int len;
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((u_int32_t)input[j]) | (((u_int32_t)input[j+1]) << 8) |
		    (((u_int32_t)input[j+2]) << 16) | (((u_int32_t)input[j+3]) << 24);
}
#endif /* i386 */

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */

void MD5Init (MD5_CTX *context)
{
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants.  */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* 
 * MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */

void MD5Update ( MD5_CTX *context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((u_int32_t)inputLen << 3))
	    < ((u_int32_t)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((u_int32_t)inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		memcpy((void *)&context->buffer[index], (const void *)input,
		    partLen);
		MD5Transform (context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform (context->state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	memcpy ((void *)&context->buffer[index], (const void *)&input[i],
	    inputLen-i);
}

/*
 * MD5 padding. Adds padding followed by original length.
 */

#define STATIC_PADDING 0
#if STATIC_PADDING
static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

void MD5Pad ( MD5_CTX *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

#if !STATIC_PADDING
	unsigned char PADDING[64];

	for (index = 0 ; index < sizeof(PADDING) ; index++) {
		PADDING[index] = 0;
	}
	PADDING[0] = 0x80;
#endif

	/* Save number of bits */
	Encode (bits, context->count, 8);

	/* Pad out to 56 mod 64. */
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update (context, PADDING, padLen);

	/* Append length (before padding) */
	MD5Update (context, bits, 8);
}

/*
 * MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */

void MD5Final ( unsigned char digest[16], MD5_CTX *context)
{
	/* Do padding. */
	MD5Pad (context);

	/* Store state in digest */
	Encode (digest, context->state, 16);

	/* Zeroize sensitive information. */
	memset ((void *)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block. */

static void
MD5Transform (state, block)
	u_int32_t state[4];
	const unsigned char block[64];
{
	u_int32_t a, b, c, d, x[16], temp;
	int i;

	static const int S1[] = { 7, 12, 17, 22 };
	static const u_int32_t C1[] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
	};

	static const int p2[] = {
		1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12
	};
	static const int S2[] = { 5, 9, 14, 20 };
	static const u_int32_t C2[] = {
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x2441453,  0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
	};
	
	static const int p3[] = {
		5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2
	};
	static const int S3[] = { 4, 11, 16, 23 };
	static const u_int32_t C3[] = {
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
	};

	static const int p4[] = {
		0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9
	};
	static const int S4[] = { 6, 10, 15, 21 };
	static const u_int32_t C4[] = {
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

	Decode (x, block, 64);

	a = state[0]; b = state[1]; c = state[2]; d = state[3]; 

	/* Round 1 */
	for ( i = 0 ; i < 16 ; i++ ) {
		FF (a, b, c, d, x[i], S1[i&3], C1[i]);
		temp = d; d= c; c = b; b = a; a = temp;
	}

	/* Round 2 */
	for ( i = 0 ; i < 16 ; i++ ) {
		GG (a, b, c, d, x[p2[i]], S2[i&3], C2[i]);
		temp = d; d= c; c = b; b = a; a = temp;
	}
	/* Round 3 */
	for ( i = 0 ; i < 16 ; i++ ) {
		HH (a, b, c, d, x[p3[i]], S3[i&3], C3[i]);
		temp = d; d= c; c = b; b = a; a = temp;
	}

	/* Round 4 */
	for ( i = 0 ; i < 16 ; i++ ) {
		II (a, b, c, d, x[p4[i]], S4[i&3], C4[i]);
		temp = d; d= c; c = b; b = a; a = temp;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	memset ((void *)x, 0, sizeof (x));
}


static void to64( char *s, unsigned long v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v&0x3f];
		v >>= 6;
	}
}

/*
 * UNIX password
 *
 * Use MD5 for what it is best at...
 */

char * md5_crypt_r( const char *pw, const char *salt, struct crypt_data * data)
{
	char *p = data->p;
	const char *sp = data->sp;
	const char *ep = data->ep;
	char *passwd = *data->KS;
	unsigned char	final[17];	/* final[16] exists only to aid in looping */
	int sl,pl,i,md5_magic_len,pw_len;
	MD5_CTX	ctx,ctx1;
	unsigned long l;

	/* Refine the Salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	md5_magic_len = strlen(md5_magic);
	if(!strncmp(sp,md5_magic,md5_magic_len))
		sp += md5_magic_len;

	/* It stops at the first '$', max 8 chars */
	for(ep=sp;*ep && *ep != '$' && ep < (sp+8);ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	MD5Init(&ctx);

	/* The password first, since that is what is most unknown */
	pw_len = strlen(pw);
	MD5Update(&ctx,pw,pw_len);

	/* Then our magic string */
	MD5Update(&ctx,md5_magic,md5_magic_len);

	/* Then the raw salt */
	MD5Update(&ctx,sp,sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	MD5Init(&ctx1);
	MD5Update(&ctx1,pw,pw_len);
	MD5Update(&ctx1,sp,sl);
	MD5Update(&ctx1,pw,pw_len);
	MD5Final(final,&ctx1);
	for(pl = pw_len; pl > 0; pl -= 16)
		MD5Update(&ctx,final,pl>16 ? 16 : pl);

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	/* Then something really weird... */
	for (i = pw_len; i ; i >>= 1) {
		MD5Update(&ctx, ((i&1) ? final : (const unsigned char *) pw), 1);
	}

	/* Now make the output string */
	strcpy(passwd,md5_magic);
	strncat(passwd,sp,sl);
	strcat(passwd,"$");

	MD5Final(final,&ctx);

	/*
	 * and now, just to make sure things don't run too fast
	 * On a 60 Mhz Pentium this takes 34 msec, so you would
	 * need 30 seconds to build a 1000 entry dictionary...
	 */
	for(i=0;i<1000;i++) {
		MD5Init(&ctx1);
		if(i & 1)
			MD5Update(&ctx1,pw,pw_len);
		else
			MD5Update(&ctx1,final,16);

		if(i % 3)
			MD5Update(&ctx1,sp,sl);

		if(i % 7)
			MD5Update(&ctx1,pw,pw_len);

		if(i & 1)
			MD5Update(&ctx1,final,16);
		else
			MD5Update(&ctx1,pw,pw_len);
		MD5Final(final,&ctx1);
	}

	p = passwd + strlen(passwd);

	final[16] = final[5];
	for ( i=0 ; i < 5 ; i++ ) {
		l = (final[i]<<16) | (final[i+6]<<8) | final[i+12];
		to64(p,l,4); p += 4;
	}
	l = final[11];
	to64(p,l,2); p += 2;
	*p = '\0';

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	return passwd;
}

