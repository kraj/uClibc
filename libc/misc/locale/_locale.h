extern const unsigned char *_uc_ctype_b;
extern const unsigned char *_uc_ctype_trans;

extern const unsigned char _uc_ctype_b_C[256+256];

#define LOCALE_BUF_SIZE (sizeof(_uc_ctype_b_C))

#define ISbit(bit)     (1 << bit)

enum
{
  ISprint  = ISbit (0),        /* 1   Printable.         */
  ISupper  = ISbit (1),        /* 2   UPPERCASE.         */
  ISlower  = ISbit (2),        /* 4   lowercase.         */
  IScntrl  = ISbit (3),        /* 8   Control character. */
  ISspace  = ISbit (4),        /* 16  Whitespace.        */
  ISpunct  = ISbit (5),        /* 32  Punctuation.       */
  ISalpha  = ISbit (6),        /* 64  Alphabetic.        */
  ISxdigit = ISbit (7),        /* 128 Hexnumeric.        */
  ISblank  = ISbit (8),        /* 256 Blank.             */
};

extern const unsigned char *_uc_collate_b;
