#define __strcspn __strpbrk
.global strpbrk
.set strpbrk,__strpbrk
.hidden __strpbrk
#include "strcspn.S"
