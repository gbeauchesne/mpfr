/* mpfr_atanh -- Inverse Hyperbolic Tangente of Unsigned Integer Number

Copyright 2001, 2002, 2003 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

 /* The computation of acosh is done by

    atanh= 1/2*ln(x+1)-1/2*ln(1-x)
 */

int
mpfr_atanh (mpfr_ptr y, mpfr_srcptr xt , mp_rnd_t rnd_mode) 
{
  int inexact =0;
  mpfr_t x;
  mp_prec_t Nx=MPFR_PREC(xt);   /* Precision of input variable */

  /* Special cases */
  if (MPFR_UNLIKELY( MPFR_IS_SINGULAR(xt) ))
    {
      if (MPFR_IS_NAN(xt)) 
	{  
	  MPFR_SET_NAN(y);
	  MPFR_RET_NAN;
	}
      else if (MPFR_IS_INF(xt))
	{ 
	  MPFR_SET_INF(y);
	  MPFR_SET_SAME_SIGN(y, xt);
	  MPFR_RET(0);
	}
      else if (MPFR_IS_ZERO(xt))
	{
	  MPFR_SET_ZERO(y);   /* atanh(0) = 0 */
	  MPFR_SET_SAME_SIGN(y,xt);
	  MPFR_RET(0);
	}
      else
	MPFR_ASSERTN(1);
    }
  /* Useless due to final mpfr_set
     MPFR_CLEAR_FLAGS(y);*/
  
  mpfr_init2(x,Nx);
  mpfr_abs(x, xt, GMP_RNDN); 

  /* General case */
  {
    /* Declaration of the intermediary variable */
    mpfr_t t, te,ti;       
    
    /* Declaration of the size variable */
    mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */
    
    mp_prec_t Nt;   /* Precision of the intermediary variable */
    long int err;  /* Precision of error */
                
    /* compute the precision of intermediary variable */
    Nt=MAX(Nx,Ny);
    /* the optimal number of bits : see algorithms.ps */
    Nt=Nt+4+__gmpfr_ceil_log2(Nt);

    /* initialise of intermediary	variable */
    mpfr_init(t);             
    mpfr_init(te);             
    mpfr_init(ti);                    

    /* First computation of cosh */
    do
      {
        /* reactualisation of the precision */
        mpfr_set_prec(t,Nt);             
        mpfr_set_prec(te,Nt);             
        mpfr_set_prec(ti,Nt);             

        /* compute atanh */
        mpfr_ui_sub(te,1,x,GMP_RNDU);   /* (1-xt)*/
        mpfr_add_ui(ti,x,1,GMP_RNDD);   /* (xt+1)*/
        mpfr_div(te,ti,te,GMP_RNDN);    /* (1+xt)/(1-xt)*/
        mpfr_log(te,te,GMP_RNDN);       /* ln((1+xt)/(1-xt))*/
        mpfr_div_2ui(t,te,1,GMP_RNDN);  /* (1/2)*ln((1+xt)/(1-xt))*/

        /* error estimate see- algorithms.ps*/
        /* err=Nt-__gmpfr_ceil_log2(1+5*pow(2,1-MPFR_EXP(t)));*/
        err = Nt - (MAX (4 - MPFR_GET_EXP (t), 0) + 1);

        /* actualisation of the precision */
        Nt += 10;

      }
    while ((err < 0) || (!mpfr_can_round (t, err, GMP_RNDN, GMP_RNDZ,
                                          Ny + (rnd_mode == GMP_RNDN))
                         || MPFR_IS_ZERO(t)));

    if (MPFR_IS_NEG(xt))
      MPFR_CHANGE_SIGN(t);

    inexact = mpfr_set (y, t, rnd_mode);

    mpfr_clear(t);
    mpfr_clear(ti);
    mpfr_clear(te);
  }
  mpfr_clear(x);
  return inexact;
}

