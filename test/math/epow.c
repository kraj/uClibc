/*						epow.c	*/
/*  power function: z = x**y */
/*  by Stephen L. Moshier. */


#include "ehead.h"
#define MAXPOS ((long) (((unsigned long) ~(0L)) >> 1))
#define MAXNEG (-MAXPOS)
/* #define MAXNEG (-MAXPOS - 1L) */

extern int rndprc;
void epowi();
static void epowr();


/* Run-time determination of largest integers */

int powinited = 0;
unsigned short maxposint[NE], maxnegint[NE];

void initpow()
{
long li;

li = MAXPOS;
ltoe( &li, maxposint );
li = MAXNEG;
ltoe( &li, maxnegint );
powinited = 1;
}




void epow( x, y, z )
unsigned short *x, *y, *z;
{
unsigned short w[NE];
int rndsav;
long li;

if( powinited == 0 )
	initpow();

/* Check for integer power. */

efloor( y, w );
if( (ecmp(y,w) == 0)
   && (ecmp(maxposint,w) >= 0)
   && (ecmp(w,maxnegint) >= 0) )
	{
	eifrac( y, &li, w );
	epowi( x, y, z );
	return;
	}
epowr( x, y, z );
}




/* y is integer valued. */

void epowi( x, y, z )
unsigned short x[], y[], z[];
{
unsigned short w[NE];
long li, lx;
unsigned long lu;
int rndsav;
unsigned short signx;
/* unsigned short signy; */

if( powinited == 0 )
	initpow();

rndsav = rndprc;

if( (ecmp(y,maxposint) > 0) || (ecmp(maxnegint,y) > 0) )
	{
	epowr( x, y, z );
	return;
	}

eifrac( y, &li, w );
if( li < 0 )
	lx = -li;
else
	lx = li;

/*
if( (x[NE-1] & (unsigned short )0x7fff) == 0 )
*/

if( ecmp( x, ezero) == 0 )
	{
	if( li == 0 )
		{
		emov( eone, z );
		return;
		}
	else if( li < 0 )
		{
		einfin( z );
		return;
		}
	else
		{
		eclear( z );
		return;
		}
	}

if( li == 0L )
	{
	emov( eone, z );
	return;
	}

emov( x, w );
signx = w[NE-1] & (unsigned short )0x8000;
w[NE-1] &= (unsigned short )0x7fff;

/* Overflow detection */
/*
lx = li * (w[NE-1] - 0x3fff);
if( lx > 16385L )
	{
	einfin( z );
	mtherr( "epowi", OVERFLOW );
	goto done;
	}
if( lx < -16450L )
	{
	eclear( z );
	return;
	}
*/
rndprc = NBITS;

if( li < 0 )
	{
	lu = (unsigned int )( -li );
/*	signy = 0xffff;*/
	ediv( w, eone, w );
	}
else
	{
	lu = (unsigned int )li;
/*	signy = 0;*/
	}

/* First bit of the power */
if( lu & 1 )
	{
	emov( w, z );
	}	
else
	{
	emov( eone, z );
	signx = 0;
	}


lu >>= 1;
while( lu != 0L )
	{
	emul( w, w, w );	/* arg to the 2-to-the-kth power */
	if( lu & 1L )	/* if that bit is set, then include in product */
		emul( w, z, z );
	lu >>= 1;
	}


done:

if( signx )
	eneg( z ); /* odd power of negative number */

/*
if( signy )
  	{
  	if( ecmp( z, ezero ) != 0 )
 		{
		ediv( z, eone, z );
		}
	else
		{
		einfin( z );
		printf( "epowi OVERFLOW\n" );
		}
	}
*/
rndprc = rndsav;
emul( eone, z, z );
}



/* z = exp( y * log(x) ) */

static void epowr( x, y, z )
unsigned short *x, *y, *z;
{
unsigned short w[NE];
int rndsav;

rndsav = rndprc;
rndprc = NBITS;
elog( x, w );
emul( y, w, w );
eexp( w, z );
rndprc = rndsav;
emul( eone, z, z );
}
