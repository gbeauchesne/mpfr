/* Test file for the various mpfr_random fonctions.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void test_random(unsigned long nbtests, unsigned long prec)
{
  mpfr_t x; 
  int *tab, size_tab, k; 
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2(x, prec); 

  size_tab = (nbtests < 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  for (k = 0; k < nbtests; k++) {
    mpfr_random(x); 
    d = mpfr_get_d(x); av += d; var += d*d; 
    tab[(int)(size_tab * d)]++;     
  }
  
  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double)nbtests / size_tab; 
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for random. Each integer should be close to %d.\n", 
	 (int)th); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 

  printf("\n"); 

  return;
}

void test_random2(unsigned long nbtests, unsigned long prec)
{
  mpfr_t x; 
  int *tab, size_tab, k; 
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2(x, prec); 

  size_tab = (nbtests < 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  for (k = 0; k < nbtests; k++) {
    mpfr_random2(x, ABSSIZE(x), 0); 
    d = mpfr_get_d(x); av += d; var += d*d; 
    tab[(int)(size_tab * d)]++;     
  }
  
  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double)nbtests / size_tab; 
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for random2.\n"); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 


  return;
}

void test_urandomb(unsigned long nbtests, unsigned long prec)
{
  mpfr_t x; 
  int *tab, size_tab, k; 
  gmp_randstate_t state; 
  double d, av = 0, var = 0, chi2 = 0, th; 

  mpfr_init2(x, prec); 

  size_tab = (nbtests < 1000 ? nbtests / 50 : 20); 
  tab = (int *) malloc (size_tab * sizeof(int)); 
  for (k = 0; k < size_tab; ++k) tab[k] = 0; 

  gmp_randinit(state, GMP_RAND_ALG_LC, 128); 

  for (k = 0; k < nbtests; k++) {
    mpfr_urandomb(x, state); 
    d = mpfr_get_d(x); av += d; var += d*d; 
    tab[(int)(size_tab * d)]++;     
  }
  
  av /= nbtests; 
  var = (var /nbtests) - av*av; 

  th = (double)nbtests / size_tab; 
  printf("Average = %.5f\nVariance = %.5f\n", av, var); 
  printf("Repartition for urandomb. Each integer should be close to %d.\n", 
	 (int)th); 

  for (k = 0; k < size_tab; k++) {
    chi2 += (tab[k] - th) * (tab[k] - th) / th; 
    printf("%d ", tab[k]); 
    if (((k+1) & 7) == 0) printf("\n"); 
  }

  printf("\nChi2 statistics value (with %d degrees of freedom) : %.5f\n\n", 
	 size_tab - 1, chi2); 

  return;
}

int main(int argc, char **argv)
{
  unsigned long nbtests, prec; 

  if (argc == 1) { nbtests = 10000; } else nbtests = atoi(argv[1]);
  if (argc <= 2) { prec = 1000; } else prec = atoi(argv[2]); 

  test_random(nbtests, prec); 
  test_random2(nbtests, prec); 
  test_urandomb(nbtests, prec);
  
  return 0;
}

