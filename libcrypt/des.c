/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2001 by Rene Müller
 *      DES based crypt() implementation, originally written for dietlibc by 
 *      Rene Müller, based on Bruce Schneier's Applied Cryptography, but 
 *      tightened up quite a bit.
 *
 * Copyright (C) 2001 by Erik Andersen 
 *      Adjusted each function to be reentrant, hacked in md5 support.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */



#include <crypt.h>
#include <string.h>
#include <unistd.h>

extern char *md5_magic;
extern char * md5_crypt_r( const char *pw, const char *salt, struct crypt_data * data);

/* Initial permutation, */
static const char IP[] = {
  57,49,41,33,25,17, 9, 1,
  59,51,43,35,27,19,11, 3,
  61,53,45,37,29,21,13, 5,
  63,55,47,39,31,23,15, 7,
  56,48,40,32,24,16, 8, 0,
  58,50,42,34,26,18,10, 2,
  60,52,44,36,28,20,12, 4,
  62,54,46,38,30,22,14, 6
};

/* Final permutation, FP = IP^(-1) */
static const char FP[] = {
  39, 7,47,15,55,23,63,31,
  38, 6,46,14,54,22,62,30,
  37, 5,45,13,53,21,61,29,
  36, 4,44,12,52,20,60,28,
  35, 3,43,11,51,19,59,27,
  34, 2,42,10,50,18,58,26,
  33, 1,41, 9,49,17,57,25,
  32, 0,40, 8,48,16,56,24
};

/* Permuted-choice 1 from the key bits to yield C and D.
 * Note that bits 8,16... are left out: They are intended for a parity check.
 */
static const char PC1_C[] = {
  56,48,40,32,24,16, 8,
   0,57,49,41,33,25,17,
   9, 1,58,50,42,34,26,
  18,10, 2,59,51,43,35
};

static const char PC1_D[] = {
  62,54,46,38,30,22,14,
   6,61,53,45,37,29,21,
  13, 5,60,52,44,36,28,
  20,12, 4,27,19,11, 3
};

/* Sequence of shifts used for the key schedule. */
static const char shifts[] = { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };

/*
 * Permuted-choice 2, to pick out the bits from the CD array that generate
 * the key schedule.
 */
static const char PC2_C[] = {
  13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,
  22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1
};

static const char PC2_D[] = {
  12, 23,  2,  8, 18, 26,  1, 11, 22, 16,  4, 19,
  15, 20, 10, 27,  5, 24, 17, 13, 21,  7,  0,  3
};

static const char e2[] = {
  32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
   8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

/* The 8 selection functions. For some reason, they give a 0-origin index,
 * unlike everything else.
 */
static const char S[8][64] = {
  {
    14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
     0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
     4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
    15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
  },

  {
    15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
     3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
     0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
    13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
  },

  {
    10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
    13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
     1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
  },

  {
     7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
    13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
    10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
     3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
  },

  {
     2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
    14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
     4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
    11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3
  },

  {
    12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
    10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
     9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
     4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
  },

  {
     4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
    13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
     1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
     6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
  },

  {
    13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
     1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
     7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
     2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
  }
};

/* P is a permutation on the selected combination of the current L and key. */
static const char P[] = {
  15, 6,19,20, 28,11,27,16,  0,14,22,25,  4,17,30, 9,
   1, 7,23,13, 31,26, 2, 8, 18,12,29, 5, 21,10, 3,24
};

/* Set up the key schedule from the key. */
void setkey_r(const char *key, struct crypt_data *data)
{
  register int i, j, k;
  int  t;
  int  s;

  /* First, generate C and D by permuting the key.  The low order bit of each
   * 8-bit char is not used, so C and D are only 28 bits apiece.
   */
  for(i=0; i < 28; i++) {
    data->C[i] = key[(int)PC1_C[i]];
    data->D[i] = key[(int)PC1_D[i]];
  }
  /* To generate Ki, rotate C and D according to schedule and pick up a
   * permutation using PC2.
   */
  for(i=0; i < 16; i++) {
    /* rotate. */
    s = shifts[i];
    for(k=0; k < s; k++) {
      t = data->C[0];
      for(j=0; j < 27; j++)
	data->C[j] = data->C[j+1];
      data->C[27] = t;
      t = data->D[0];
      for(j=0; j < 27; j++)
	data->D[j] = data->D[j+1];
      data->D[27] = t;
    }
    /* get Ki. Note C and D are concatenated. */
    for(j=0; j < 24; j++) {
      data->KS[i][j] = data->C[(int)PC2_C[j]];
      data->KS[i][j+24] = data->D[(int)PC2_D[j]];
    }
  }

  for(i=0; i < 48; i++)
    data->E[i] = e2[i];
}


/* The payoff: encrypt a block. */
void encrypt_r(char block[64], int edflag, struct crypt_data *data)
{
  int  i, ii;
  register int t, j, k;

  /* First, permute the bits in the input */
  for(j=0; j < 64; j++)
    data->L[j] = data->block[(int)IP[j]];
  /* Perform an encryption operation 16 times. */
  for(ii=0; ii < 16; ii++) {
    i = ii;
    /* Save the R array, which will be the new L. */
    for(j=0; j < 32; j++)
      data->tempL[j] = data->R[j];
    /* Expand R to 48 bits using the E selector;
     * exclusive-or with the current key bits.
     */
    for(j=0; j < 48; j++)
      data->preS[j] = data->R[data->E[j]-1] ^ data->KS[i][j];
    /* The pre-select bits are now considered in 8 groups of 6 bits each.
     * The 8 selection functions map these 6-bit quantities into 4-bit
     * quantities and the results permuted to make an f(R, K).
     * The indexing into the selection functions is peculiar;
     * it could be simplified by rewriting the tables.
     */
    for(j=0; j < 8; j++) {
      t = ((j<<1)+j)<<1;
      k = S[j][(data->preS[t]<<5)+
	 (data->preS[t+1]<<3)+
	 (data->preS[t+2]<<2)+
	 (data->preS[t+3]<<1)+
	 (data->preS[t+4]   )+
	 (data->preS[t+5]<<4)];
      t = j << 2;
      data->f[t  ] = (k>>3)&01;
      data->f[t+1] = (k>>2)&01;
      data->f[t+2] = (k>>1)&01;
      data->f[t+3] = (k   )&01;
    }
    /* The new R is L ^ f(R, K). The f here has to be permuted first, though. */
    for(j=0; j < 32; j++)
      data->R[j] = data->L[j] ^ data->f[(int)P[j]];
    /* Finally, the new L (the original R) is copied back. */
    for(j=0; j < 32; j++)
      data->L[j] = data->tempL[j];
  }
  /* The output L and R are reversed. */
  for(j=0; j < 32; j++) {
    data->L[j] ^= data->R[j];
    data->R[j] ^= data->L[j];
    data->L[j] ^= data->R[j];
  }
  /* The final output gets the inverse permutation of the very original. */
  for(j=0; j < 64; j++)
    data->block[j] = data->L[(int)FP[j]];
}

char * crypt_r(const char *pw, const char *salt, struct crypt_data *data)
{
  register int i, j, c;
  
  
  /* Check if we are supposed to be using the MD5 encryption replacement.  */
  if (strncmp (md5_magic, salt, sizeof (md5_magic) - 1) == 0)
    return md5_crypt_r(pw, salt, data);

  for(i=0; i < 66; i++)
    data->block[i] = 0;
  for(i=0; (c= *pw) && i < 64; pw++) {
    for(j=0; j < 7; j++, i++)
      data->block[i] = (c>>(6-j)) & 01;
    i++;
  }

  setkey_r(data->block, data);

  for(i=0; i < 66; i++)
    data->block[i] = 0;

  for(i=0; i < 2; i++) {
    c = *salt++;
    data->iobuf[i] = c;
    if(c > 'Z')
      c -= 6;
    if(c > '9')
      c -= 7;
    c -= '.';
    for(j=0; j < 6; j++) {
      if((c>>j) & 01) {
	int ind1 = (((i<<1)+i)<< 1) + j;
	int ind2 = ind1 + 24;
	data->E[ind1] ^= data->E[ind2];
	data->E[ind2] ^= data->E[ind1];
	data->E[ind1] ^= data->E[ind2];
      }
    }
  }

  for(i=0; i < 25; i++)
    encrypt_r(data->block, 0, data);

  for(i=0; i < 11; i++) {
    c = 0;
    for(j=0; j < 6; j++) {
      c <<= 1;
      c |= data->block[(((i<<1)+i)<<1)+j];
    }
    c += '.';
    if(c > '9')
      c += 7;
    if(c > 'Z')
      c += 6;
    data->iobuf[i+2] = c;
  }
  data->iobuf[i+2] = 0;
  if(data->iobuf[1] == 0)
    data->iobuf[1] = data->iobuf[0];
  return(data->iobuf);
}

