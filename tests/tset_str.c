#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include <time.h>

int
main(int argc, char **argv)
{
  mpfr_t x; unsigned long k, bd, nc; char *str, *str2; 

  srandom(time(NULL)); 
  
  if (argc > 1) { nc = atoi(argv[1]); } else { nc = 53; }
  if (nc < 24) { nc = 24; }

  bd = random()&8;
  
  str2 = str = (char *) malloc (nc*sizeof(char)); 

  if (bd) 
    {
      for(k = 1; k <= bd; k++) 
	{ *(str2++) = (random() & 1) + '0'; }
    }
  else { *(str2++) = '0'; }

  *(str2++) = '.'; 

  for(k = 1; k < nc - 17 - bd; k++)
    {
      *(str2++) = '0' + (random() & 1); 
    }

  *(str2++) = 'e'; 
  sprintf(str2, "%d", random() - (1 << 30)); 

  printf("%s\n", str); 
  mpfr_init2(x, nc + 10); 
  mpfr_set_str_raw(x, str); 
  mpfr_print_raw(x); printf("\n"); 

  mpfr_clear(x); free(str); 
  return 0; 
}
