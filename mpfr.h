/* mpfr.h -- Include file for mpfr.

Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
Contributed by the Arenaire and Cacao projects, INRIA.

This file is part of the GNU MPFR Library.

The GNU MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFR Library; see the file COPYING.LIB.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#ifndef __MPFR_H
#define __MPFR_H

/* Define MPFR version number */
#define MPFR_VERSION_MAJOR 3
#define MPFR_VERSION_MINOR 0
#define MPFR_VERSION_PATCHLEVEL 0
#define MPFR_VERSION_STRING "3.0.0-dev"

/* Macros dealing with MPFR VERSION */
#define MPFR_VERSION_NUM(a,b,c) (((a) << 16L) | ((b) << 8) | (c))
#define MPFR_VERSION \
MPFR_VERSION_NUM(MPFR_VERSION_MAJOR,MPFR_VERSION_MINOR,MPFR_VERSION_PATCHLEVEL)

/* Check if GMP is included, and try to include it (Works with local GMP) */
#ifndef __GMP_H__
# include <gmp.h>
#endif

/* Check if stdio.h is included or if the user wants FILE */
#if defined (_GMP_H_HAVE_FILE) || defined (MPFR_USE_FILE)
# define _MPFR_H_HAVE_FILE 1
#endif

#if defined (_GMP_H_HAVE_VA_LIST)
# define _MPFR_H_HAVE_VA_LIST 1
#endif

/* Check if <stdint.h> / <inttypes.h> is included or if the user
   explicitly wants intmax_t. Note: with C++ implementations, the
   test of the C99 macros will work only if the user has defined
   __STDC_LIMIT_MACROS or __STDC_CONSTANT_MACROS before <stdint.h>
   has been included (see ISO C99 standard); that's why _STDINT_H
   (defined by the glibc) and _STDINT_H_ (defined under Mac OS X)
   are tested too, but under other OS, MPFR_USE_INTMAX_T may need
   to be defined. */
#if ((defined (INTMAX_C) || defined (INTMAX_MAX)) && \
     (defined (UINTMAX_C) || defined (UINTMAX_MAX))) || \
  defined (MPFR_USE_INTMAX_T) || defined (_STDINT_H) || defined (_STDINT_H_)
# define _MPFR_H_HAVE_INTMAX_T 1
#endif

/* Definition of rounding modes (DON'T USE MPFR_RNDNA!).
   Warning! Changing the contents of this enum should be seen as an
   interface change since the old and the new types are not compatible
   (the integer type compatible with the enumerated type can even change,
   see ISO C99, 6.7.2.2#4), and in Makefile.am, AGE should be set to 0.

   MPFR_RNDU must appear just before MPFR_RNDD (see
   MPFR_IS_RNDUTEST_OR_RNDDNOTTEST in mpfr-impl.h).

   If you change the order of the rounding modes, please update the routines
   in texceptions.c which assume 0=RNDN, 1=RNDZ, 2=RNDU, 3=RNDD, 4=RNDA.
*/
typedef enum {
  MPFR_RNDN=0,  /* round to nearest, with ties to even */
  MPFR_RNDZ,    /* round toward zero */
  MPFR_RNDU,    /* round toward +Inf */
  MPFR_RNDD,    /* round toward -Inf */
  MPFR_RNDA,    /* round away from zero */
  MPFR_RND_MAX, /* gives number of supported rounding modes, those after are
                  only supported by some functions */
  MPFR_RNDNA=-1 /* round to nearest, with ties away from zero (mpfr_round) */
} mpfr_rnd_t;

/* kept for compatibility with MPFR 2.4.x and before */
#define GMP_RNDN MPFR_RNDN
#define GMP_RNDZ MPFR_RNDZ
#define GMP_RNDU MPFR_RNDU
#define GMP_RNDD MPFR_RNDD

/* Define precision : 1 (short), 2 (int) or 3 (long) (DON'T USE IT!)*/
#ifndef _MPFR_PREC_FORMAT
# if __GMP_MP_SIZE_T_INT == 1
#  define _MPFR_PREC_FORMAT 2
# else
#  define _MPFR_PREC_FORMAT 3
# endif
#endif

#if   _MPFR_PREC_FORMAT == 1
typedef unsigned short mpfr_prec_t;
#elif _MPFR_PREC_FORMAT == 2
typedef unsigned int   mpfr_prec_t;
#elif _MPFR_PREC_FORMAT == 3
typedef unsigned long  mpfr_prec_t;
#else
# error "Invalid MPFR Prec format"
#endif

/* Definition of precision limits */
#define MPFR_PREC_MIN 2
#define MPFR_PREC_MAX ((mpfr_prec_t)((mpfr_prec_t)(~(mpfr_prec_t)0)>>1))

/* Definition of sign */
typedef int          mpfr_sign_t;

/* Definition of the standard exponent limits */
#define MPFR_EMAX_DEFAULT ((mp_exp_t) (((unsigned long) 1 << 30) - 1))
#define MPFR_EMIN_DEFAULT (-(MPFR_EMAX_DEFAULT))

/* Definition of the main structure */
typedef struct {
  mpfr_prec_t  _mpfr_prec;
  mpfr_sign_t  _mpfr_sign;
  mp_exp_t     _mpfr_exp;
  mp_limb_t   *_mpfr_d;
} __mpfr_struct;

/* Compatibility with previous types of MPFR */
#ifndef mp_rnd_t
# define mp_rnd_t  mpfr_rnd_t
#endif
#ifndef mp_prec_t
# define mp_prec_t mpfr_prec_t
#endif

/*
   The represented number is
      _sign*(_d[k-1]/B+_d[k-2]/B^2+...+_d[0]/B^k)*2^_exp
   where k=ceil(_mp_prec/BITS_PER_MP_LIMB) and B=2^BITS_PER_MP_LIMB.

   For the msb (most significant bit) normalized representation, we must have
      _d[k-1]>=B/2, unless the number is singular.

   We must also have the last k*BITS_PER_MP_LIMB-_prec bits set to zero.
*/

typedef __mpfr_struct mpfr_t[1];
typedef __mpfr_struct *mpfr_ptr;
typedef __gmp_const __mpfr_struct *mpfr_srcptr;

/* For those who need a direct and fast access to the sign field.
   However it is not in the API, thus use it at your own risk: it might
   not be supported, or change name, in further versions!
   Unfortunately, it must be defined here (instead of MPFR's internal
   header file mpfr-impl.h) because it is used by some macros below.
*/
#define MPFR_SIGN(x) ((x)->_mpfr_sign)

/* Stack interface */
typedef enum {
  MPFR_NAN_KIND = 0,
  MPFR_INF_KIND = 1, MPFR_ZERO_KIND = 2, MPFR_REGULAR_KIND = 3
} mpfr_kind_t;

/* GMP defines:
    + size_t:                Standard size_t
    + __GMP_ATTRIBUTE_PURE   Attribute for math functions.
    + __GMP_NOTHROW          For C++: can't throw .
    + __GMP_EXTERN_INLINE    Attribute for inline function.
    * __gmp_const            const (Supports for K&R compiler only for mpfr.h).
    + __GMP_DECLSPEC_EXPORT  compiling to go into a DLL
    + __GMP_DECLSPEC_IMPORT  compiling to go into a application
*/
/* Extra MPFR defines */
#define __MPFR_SENTINEL_ATTR
#if defined (__GNUC__)
# if __GNUC__ >= 4
#  undef __MPFR_SENTINEL_ATTR
#  define __MPFR_SENTINEL_ATTR __attribute__ ((sentinel))
# endif
#endif

/* Prototypes: Support of K&R compiler */
#if defined (__GMP_PROTO)
# define _MPFR_PROTO __GMP_PROTO
#elif defined (__STDC__) || defined (__cplusplus)
# define _MPFR_PROTO(x) x
#else
# define _MPFR_PROTO(x) ()
#endif
/* Support for WINDOWS Dll:
   Check if we are inside a MPFR build, and if so export the functions.
   Otherwise does the same thing as GMP */
#if defined(__MPFR_WITHIN_MPFR) && __GMP_LIBGMP_DLL
# define __MPFR_DECLSPEC __GMP_DECLSPEC_EXPORT
#else
# define __MPFR_DECLSPEC __GMP_DECLSPEC
#endif

/* Note: some functions need that a specific system header is included before
   mpfr.h to be defined. If the user forgets to include the header, the mpfr
   function prototype in the user object file is not correct. In order to
   raise a linker error in that case, we change their internal name in the
   GNU mpfr library (prefixed by __gmpfr instead of mpfr)*/
#if defined (__cplusplus)
extern "C" {
#endif

__MPFR_DECLSPEC __gmp_const char * mpfr_get_version _MPFR_PROTO ((void));
__MPFR_DECLSPEC __gmp_const char * mpfr_get_patches _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_buildopt_tls_p     _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_buildopt_decimal_p _MPFR_PROTO ((void));

__MPFR_DECLSPEC mp_exp_t mpfr_get_emin     _MPFR_PROTO ((void));
__MPFR_DECLSPEC int      mpfr_set_emin     _MPFR_PROTO ((mp_exp_t));
__MPFR_DECLSPEC mp_exp_t mpfr_get_emin_min _MPFR_PROTO ((void));
__MPFR_DECLSPEC mp_exp_t mpfr_get_emin_max _MPFR_PROTO ((void));
__MPFR_DECLSPEC mp_exp_t mpfr_get_emax     _MPFR_PROTO ((void));
__MPFR_DECLSPEC int      mpfr_set_emax     _MPFR_PROTO ((mp_exp_t));
__MPFR_DECLSPEC mp_exp_t mpfr_get_emax_min _MPFR_PROTO ((void));
__MPFR_DECLSPEC mp_exp_t mpfr_get_emax_max _MPFR_PROTO ((void));

__MPFR_DECLSPEC void mpfr_set_default_rounding_mode _MPFR_PROTO((mpfr_rnd_t));
__MPFR_DECLSPEC mp_rnd_t mpfr_get_default_rounding_mode _MPFR_PROTO((void));
__MPFR_DECLSPEC __gmp_const char *
   mpfr_print_rnd_mode _MPFR_PROTO((mpfr_rnd_t));

__MPFR_DECLSPEC void mpfr_clear_flags _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_clear_underflow _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_clear_overflow _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_clear_nanflag _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_clear_inexflag _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_clear_erangeflag _MPFR_PROTO ((void));

__MPFR_DECLSPEC void mpfr_set_underflow _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_set_overflow _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_set_nanflag _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_set_inexflag _MPFR_PROTO ((void));
__MPFR_DECLSPEC void mpfr_set_erangeflag _MPFR_PROTO ((void));

__MPFR_DECLSPEC int mpfr_underflow_p _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_overflow_p _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_nanflag_p _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_inexflag_p _MPFR_PROTO ((void));
__MPFR_DECLSPEC int mpfr_erangeflag_p _MPFR_PROTO ((void));

__MPFR_DECLSPEC int
  mpfr_check_range _MPFR_PROTO ((mpfr_ptr, int, mpfr_rnd_t));

__MPFR_DECLSPEC void mpfr_init2 _MPFR_PROTO ((mpfr_ptr, mpfr_prec_t));
__MPFR_DECLSPEC void mpfr_init _MPFR_PROTO ((mpfr_ptr));
__MPFR_DECLSPEC void mpfr_clear _MPFR_PROTO ((mpfr_ptr));

__MPFR_DECLSPEC void
  mpfr_inits2 _MPFR_PROTO ((mp_prec_t, mpfr_ptr, ...)) __MPFR_SENTINEL_ATTR;
__MPFR_DECLSPEC void
  mpfr_inits _MPFR_PROTO ((mpfr_ptr, ...)) __MPFR_SENTINEL_ATTR;
__MPFR_DECLSPEC void
  mpfr_clears _MPFR_PROTO ((mpfr_ptr, ...)) __MPFR_SENTINEL_ATTR;

__MPFR_DECLSPEC int
  mpfr_prec_round _MPFR_PROTO ((mpfr_ptr, mpfr_prec_t, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_can_round _MPFR_PROTO ((mpfr_srcptr, mp_exp_t, mpfr_rnd_t, mpfr_rnd_t,
                               mpfr_prec_t));
__MPFR_DECLSPEC mpfr_prec_t mpfr_min_prec _MPFR_PROTO ((mpfr_srcptr));

__MPFR_DECLSPEC mp_exp_t mpfr_get_exp _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_set_exp _MPFR_PROTO ((mpfr_ptr, mp_exp_t));
__MPFR_DECLSPEC mp_prec_t mpfr_get_prec _MPFR_PROTO((mpfr_srcptr));
__MPFR_DECLSPEC void mpfr_set_prec _MPFR_PROTO((mpfr_ptr, mpfr_prec_t));
__MPFR_DECLSPEC void mpfr_set_prec_raw _MPFR_PROTO((mpfr_ptr, mpfr_prec_t));
__MPFR_DECLSPEC void mpfr_set_default_prec _MPFR_PROTO((mpfr_prec_t));
__MPFR_DECLSPEC mp_prec_t mpfr_get_default_prec _MPFR_PROTO((void));

__MPFR_DECLSPEC int mpfr_set_d _MPFR_PROTO ((mpfr_ptr, double, mpfr_rnd_t));
#ifdef MPFR_WANT_DECIMAL_FLOATS
__MPFR_DECLSPEC int mpfr_set_decimal64 _MPFR_PROTO ((mpfr_ptr, _Decimal64,
                                                     mp_rnd_t));
#endif
__MPFR_DECLSPEC int
  mpfr_set_ld _MPFR_PROTO ((mpfr_ptr, long double, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_z _MPFR_PROTO ((mpfr_ptr, mpz_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC void mpfr_set_nan _MPFR_PROTO ((mpfr_ptr));
__MPFR_DECLSPEC void mpfr_set_inf _MPFR_PROTO ((mpfr_ptr, int));
__MPFR_DECLSPEC int
  mpfr_set_f _MPFR_PROTO ((mpfr_ptr, mpf_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_get_f _MPFR_PROTO ((mpf_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_set_si _MPFR_PROTO ((mpfr_ptr, long, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_ui _MPFR_PROTO ((mpfr_ptr, unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_si_2exp _MPFR_PROTO ((mpfr_ptr, long, mp_exp_t, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_ui_2exp _MPFR_PROTO ((mpfr_ptr,unsigned long,mp_exp_t,mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_q _MPFR_PROTO ((mpfr_ptr, mpq_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_str _MPFR_PROTO ((mpfr_ptr, __gmp_const char *, int, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_init_set_str _MPFR_PROTO ((mpfr_ptr, __gmp_const char *, int,
                                  mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set4 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t, int));
__MPFR_DECLSPEC int
  mpfr_abs _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_neg _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_signbit _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC int
  mpfr_setsign _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, int, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_copysign _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t));

#ifdef _MPFR_H_HAVE_INTMAX_T
#define mpfr_set_sj __gmpfr_set_sj
#define mpfr_set_sj_2exp __gmpfr_set_sj_2exp
#define mpfr_set_uj __gmpfr_set_uj
#define mpfr_set_uj_2exp __gmpfr_set_uj_2exp
#define mpfr_get_sj __gmpfr_mpfr_get_sj
#define mpfr_get_uj __gmpfr_mpfr_get_uj
__MPFR_DECLSPEC int mpfr_set_sj _MPFR_PROTO ((mpfr_t, intmax_t, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_sj_2exp _MPFR_PROTO ((mpfr_t, intmax_t, intmax_t, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_set_uj _MPFR_PROTO ((mpfr_t, uintmax_t, mpfr_rnd_t));
__MPFR_DECLSPEC int
  mpfr_set_uj_2exp _MPFR_PROTO ((mpfr_t, uintmax_t, intmax_t, mpfr_rnd_t));
__MPFR_DECLSPEC intmax_t mpfr_get_sj _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC uintmax_t mpfr_get_uj _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
#endif

__MPFR_DECLSPEC mp_exp_t mpfr_get_z_exp _MPFR_PROTO ((mpz_ptr, mpfr_srcptr));
__MPFR_DECLSPEC double mpfr_get_d _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
#ifdef MPFR_WANT_DECIMAL_FLOATS
__MPFR_DECLSPEC _Decimal64 mpfr_get_decimal64 _MPFR_PROTO ((mpfr_srcptr,
                                                           mp_rnd_t));
#endif
__MPFR_DECLSPEC long double mpfr_get_ld _MPFR_PROTO ((mpfr_srcptr,
                                                      mpfr_rnd_t));
__MPFR_DECLSPEC double mpfr_get_d1 _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC double mpfr_get_d_2exp _MPFR_PROTO ((long*, mpfr_srcptr,
                                                     mpfr_rnd_t));
__MPFR_DECLSPEC long double mpfr_get_ld_2exp _MPFR_PROTO ((long*, mpfr_srcptr,
                                                           mpfr_rnd_t));
__MPFR_DECLSPEC long mpfr_get_si _MPFR_PROTO ((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC unsigned long mpfr_get_ui _MPFR_PROTO ((mpfr_srcptr,
                                                        mpfr_rnd_t));
__MPFR_DECLSPEC char*mpfr_get_str _MPFR_PROTO ((char*, mp_exp_t*, int, size_t,
                                                mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_get_z _MPFR_PROTO ((mpz_ptr z, mpfr_srcptr f,
                                             mpfr_rnd_t));

__MPFR_DECLSPEC void mpfr_free_str _MPFR_PROTO ((char *));

__MPFR_DECLSPEC int mpfr_urandomb _MPFR_PROTO ((mpfr_ptr, gmp_randstate_t));

__MPFR_DECLSPEC void mpfr_nextabove _MPFR_PROTO ((mpfr_ptr));
__MPFR_DECLSPEC void mpfr_nextbelow _MPFR_PROTO ((mpfr_ptr));
__MPFR_DECLSPEC void mpfr_nexttoward _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr));

#ifdef _MPFR_H_HAVE_FILE
#define mpfr_inp_str __gmpfr_inp_str
#define mpfr_out_str __gmpfr_out_str
__MPFR_DECLSPEC size_t mpfr_inp_str _MPFR_PROTO ((mpfr_ptr, FILE*, int,
                                                  mpfr_rnd_t));
__MPFR_DECLSPEC size_t mpfr_out_str _MPFR_PROTO ((FILE*, int, size_t,
                                                  mpfr_srcptr, mpfr_rnd_t));
#define mpfr_fprintf __gmpfr_fprintf
__MPFR_DECLSPEC int mpfr_fprintf _MPFR_PROTO ((FILE*, __gmp_const char*,
                                               ...));
#endif
__MPFR_DECLSPEC int mpfr_printf _MPFR_PROTO ((__gmp_const char*, ...));
__MPFR_DECLSPEC int mpfr_asprintf _MPFR_PROTO ((char**, __gmp_const char*,
                                                ...));
__MPFR_DECLSPEC int mpfr_sprintf _MPFR_PROTO ((char*, __gmp_const char*,
                                               ...));
__MPFR_DECLSPEC int mpfr_snprintf _MPFR_PROTO ((char*, size_t,
                                                __gmp_const char*, ...));

#ifdef _MPFR_H_HAVE_VA_LIST
#ifdef _MPFR_H_HAVE_FILE
#define mpfr_vfprintf __gmpfr_vfprintf
__MPFR_DECLSPEC int mpfr_vfprintf _MPFR_PROTO ((FILE*, __gmp_const char*,
                                                va_list));
#endif /* _MPFR_H_HAVE_FILE */
#define mpfr_vprintf __gmpfr_vprintf
#define mpfr_vasprintf __gmpfr_vasprintf
#define mpfr_vsprintf __gmpfr_vsprintf
#define mpfr_vsnprintf __gmpfr_vsnprintf
__MPFR_DECLSPEC int mpfr_vprintf _MPFR_PROTO ((__gmp_const char*, va_list));
__MPFR_DECLSPEC int mpfr_vasprintf _MPFR_PROTO ((char**, __gmp_const char*,
                                                 va_list));
__MPFR_DECLSPEC int mpfr_vsprintf _MPFR_PROTO ((char*, __gmp_const char*,
                                               va_list));
__MPFR_DECLSPEC int mpfr_vsnprintf _MPFR_PROTO ((char*, size_t,
                                                __gmp_const char*, va_list));
#endif /* _MPFR_H_HAVE_VA_LIST */

__MPFR_DECLSPEC int mpfr_pow _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_pow_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_pow_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              unsigned long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_ui_pow_ui _MPFR_PROTO ((mpfr_ptr, unsigned long int,
                                             unsigned long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_ui_pow _MPFR_PROTO ((mpfr_ptr, unsigned long int,
                                              mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_pow_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpz_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_sqrt _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                            mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sqrt_ui _MPFR_PROTO ((mpfr_ptr, unsigned long,
                                               mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_rec_sqrt _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_add _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_add_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_ui_sub _MPFR_PROTO ((mpfr_ptr, unsigned long,
                                              mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_ui_div _MPFR_PROTO ((mpfr_ptr, unsigned long,
                                              mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_add_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_si_sub _MPFR_PROTO ((mpfr_ptr, long int,
                                              mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              long int, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_si_div _MPFR_PROTO ((mpfr_ptr, long int,
                                              mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_add_d _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              double, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub_d _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              double, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_d_sub _MPFR_PROTO ((mpfr_ptr, double,
                                              mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul_d _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              double, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_d _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                              double, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_d_div _MPFR_PROTO ((mpfr_ptr, double,
                                              mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_sqr _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_const_pi _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_const_log2 _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_const_euler _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_const_catalan _MPFR_PROTO ((mpfr_ptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_agm _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_log _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_log2 _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_log10 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_log1p _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_exp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_exp2 _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_exp10 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_expm1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_eint _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_li2 _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_cmp  _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_cmp3 _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr, int));
__MPFR_DECLSPEC int mpfr_cmp_d _MPFR_PROTO ((mpfr_srcptr, double));
__MPFR_DECLSPEC int mpfr_cmp_ld _MPFR_PROTO ((mpfr_srcptr, long double));
__MPFR_DECLSPEC int mpfr_cmpabs _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_cmp_ui _MPFR_PROTO ((mpfr_srcptr, unsigned long));
__MPFR_DECLSPEC int mpfr_cmp_si _MPFR_PROTO ((mpfr_srcptr, long));
__MPFR_DECLSPEC int mpfr_cmp_ui_2exp _MPFR_PROTO ((mpfr_srcptr, unsigned long,
                                                   mp_exp_t));
__MPFR_DECLSPEC int mpfr_cmp_si_2exp _MPFR_PROTO ((mpfr_srcptr, long,
                                                   mp_exp_t));
__MPFR_DECLSPEC void mpfr_reldiff _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_eq _MPFR_PROTO((mpfr_srcptr, mpfr_srcptr,
                                         unsigned long));
__MPFR_DECLSPEC int mpfr_sgn _MPFR_PROTO ((mpfr_srcptr));

__MPFR_DECLSPEC int mpfr_mul_2exp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_2exp _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul_2ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                               unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_2ui _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                               unsigned long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_mul_2si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                               long, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_2si _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                               long, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_rint _MPFR_PROTO((mpfr_ptr,mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_round _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_trunc _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_ceil _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_floor _MPFR_PROTO((mpfr_ptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_rint_round _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                  mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_rint_trunc _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                  mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_rint_ceil _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                 mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_rint_floor _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                  mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_frac _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_modf _MPFR_PROTO ((mpfr_ptr, mpfr_ptr, mpfr_srcptr,
                                                  mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_remquo _MPFR_PROTO ((mpfr_ptr, long*, mpfr_srcptr,
                                              mpfr_srcptr, mp_rnd_t));
__MPFR_DECLSPEC int mpfr_remainder _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                 mpfr_srcptr, mp_rnd_t));
__MPFR_DECLSPEC int mpfr_fmod _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                                 mpfr_srcptr, mp_rnd_t));

__MPFR_DECLSPEC int mpfr_fits_ulong_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_slong_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_uint_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_sint_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_ushort_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_sshort_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_uintmax_p _MPFR_PROTO((mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fits_intmax_p _MPFR_PROTO((mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC void mpfr_extract _MPFR_PROTO ((mpz_ptr, mpfr_srcptr,
                                                unsigned int));
__MPFR_DECLSPEC void mpfr_swap _MPFR_PROTO ((mpfr_ptr, mpfr_ptr));
__MPFR_DECLSPEC void mpfr_dump _MPFR_PROTO ((mpfr_srcptr));

__MPFR_DECLSPEC int mpfr_nan_p _MPFR_PROTO((mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_inf_p _MPFR_PROTO((mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_number_p _MPFR_PROTO((mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_integer_p _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_zero_p _MPFR_PROTO ((mpfr_srcptr));

__MPFR_DECLSPEC int mpfr_greater_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_greaterequal_p _MPFR_PROTO ((mpfr_srcptr,
                                                      mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_less_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_lessequal_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_lessgreater_p _MPFR_PROTO((mpfr_srcptr,mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_equal_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));
__MPFR_DECLSPEC int mpfr_unordered_p _MPFR_PROTO ((mpfr_srcptr, mpfr_srcptr));

__MPFR_DECLSPEC int mpfr_atanh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_acosh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_asinh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cosh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sinh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_tanh _MPFR_PROTO((mpfr_ptr,mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sinh_cosh _MPFR_PROTO ((mpfr_ptr, mpfr_ptr,
                                               mpfr_srcptr, mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_sech _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_csch _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_coth _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_acos _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_asin _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_atan _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sin _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sin_cos _MPFR_PROTO ((mpfr_ptr, mpfr_ptr,
                                               mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cos _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_tan _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_atan2 _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_srcptr,
                                             mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sec _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_csc _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cot _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_hypot _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_erf _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_erfc _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cbrt _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_root _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,unsigned long,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_gamma _MPFR_PROTO((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_lngamma _MPFR_PROTO((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_lgamma _MPFR_PROTO((mpfr_ptr,int*,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_zeta _MPFR_PROTO ((mpfr_ptr,mpfr_srcptr,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_zeta_ui _MPFR_PROTO ((mpfr_ptr,unsigned long,mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fac_ui _MPFR_PROTO ((mpfr_ptr, unsigned long int,
                                              mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_j0 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_j1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_jn _MPFR_PROTO ((mpfr_ptr, long, mpfr_srcptr,
                                          mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_y0 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_y1 _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_yn _MPFR_PROTO ((mpfr_ptr, long, mpfr_srcptr,
                                          mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_min _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_max _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_dim _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_rnd_t));

__MPFR_DECLSPEC int mpfr_mul_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpz_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpz_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_add_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpz_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub_z _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpz_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cmp_z _MPFR_PROTO ((mpfr_srcptr, mpz_srcptr));

__MPFR_DECLSPEC int mpfr_mul_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpq_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_div_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpq_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_add_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpq_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sub_q _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr,
                                             mpq_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_cmp_q _MPFR_PROTO ((mpfr_srcptr, mpq_srcptr));

__MPFR_DECLSPEC int mpfr_cmp_f _MPFR_PROTO ((mpfr_srcptr, mpf_srcptr));

__MPFR_DECLSPEC int mpfr_fma _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_fms _MPFR_PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr,
                                           mpfr_srcptr, mpfr_rnd_t));
__MPFR_DECLSPEC int mpfr_sum _MPFR_PROTO ((mpfr_ptr, mpfr_ptr *__gmp_const,
                                           unsigned long, mpfr_rnd_t));

__MPFR_DECLSPEC void mpfr_free_cache _MPFR_PROTO ((void));

__MPFR_DECLSPEC int  mpfr_subnormalize _MPFR_PROTO ((mpfr_ptr, int,
                                                     mp_rnd_t));

__MPFR_DECLSPEC int  mpfr_strtofr _MPFR_PROTO ((mpfr_ptr, __gmp_const char *,
                                                char **, int, mpfr_rnd_t));

__MPFR_DECLSPEC size_t mpfr_custom_get_size   _MPFR_PROTO ((mp_prec_t));
__MPFR_DECLSPEC void   mpfr_custom_init       _MPFR_PROTO ((void *, mp_prec_t));
__MPFR_DECLSPEC void * mpfr_custom_get_mantissa _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC mp_exp_t mpfr_custom_get_exp  _MPFR_PROTO ((mpfr_srcptr));
__MPFR_DECLSPEC void   mpfr_custom_move       _MPFR_PROTO ((mpfr_ptr, void *));
__MPFR_DECLSPEC void   mpfr_custom_init_set   _MPFR_PROTO ((mpfr_ptr, int,
                                               mp_exp_t, mp_prec_t, void *));
__MPFR_DECLSPEC int    mpfr_custom_get_kind   _MPFR_PROTO ((mpfr_srcptr));

#if defined (__cplusplus)
}
#endif

/* DON'T USE THIS! (For MPFR-public macros only, see below.)
   The mpfr_sgn macro uses the fact that __MPFR_EXP_NAN and __MPFR_EXP_ZERO
   are the smallest values. */
#if __GMP_MP_SIZE_T_INT
#define __MPFR_EXP_NAN  ((mp_exp_t)((~((~(unsigned int)0)>>1))+2))
#define __MPFR_EXP_ZERO ((mp_exp_t)((~((~(unsigned int)0)>>1))+1))
#define __MPFR_EXP_INF  ((mp_exp_t)((~((~(unsigned int)0)>>1))+3))
#else
#define __MPFR_EXP_NAN  ((mp_exp_t)((~((~(unsigned long)0)>>1))+2))
#define __MPFR_EXP_ZERO ((mp_exp_t)((~((~(unsigned long)0)>>1))+1))
#define __MPFR_EXP_INF  ((mp_exp_t)((~((~(unsigned long)0)>>1))+3))
#endif

/* Define MPFR_USE_EXTENSION to avoid "gcc -pedantic" warnings. */
#ifndef MPFR_EXTENSION
# if defined(MPFR_USE_EXTENSION)
#  define MPFR_EXTENSION __extension__
# else
#  define MPFR_EXTENSION
# endif
#endif

/* Warning! This macro doesn't work with K&R C (e.g., compare the "gcc -E"
   output with and without -traditional) and shouldn't be used internally.
   For public use only, but see the MPFR manual. */
#define MPFR_DECL_INIT(_x, _p)                                        \
  MPFR_EXTENSION mp_limb_t __gmpfr_local_tab_##_x[((_p)-1)/GMP_NUMB_BITS+1]; \
  MPFR_EXTENSION mpfr_t _x = {{(_p),1,__MPFR_EXP_NAN,__gmpfr_local_tab_##_x}}

/* Fast access macros to replace function interface.
   If the USER don't want to use the macro interface, let him make happy
   even if it produces faster and smaller code. */
#ifndef MPFR_USE_NO_MACRO

/* Inlining theses functions is both faster and smaller */
#define mpfr_nan_p(_x)    ((_x)->_mpfr_exp == __MPFR_EXP_NAN)
#define mpfr_inf_p(_x)    ((_x)->_mpfr_exp == __MPFR_EXP_INF)
#define mpfr_zero_p(_x)   ((_x)->_mpfr_exp == __MPFR_EXP_ZERO)
#define mpfr_sgn(_x)                                          \
  ((_x)->_mpfr_exp < __MPFR_EXP_INF ?                         \
   (mpfr_nan_p (_x) ? mpfr_set_erangeflag () : (void) 0), 0 : \
   MPFR_SIGN (_x))

/* Prevent them from using as lvalues */
#define mpfr_get_prec(_x) ((_x)->_mpfr_prec + 0)
#define mpfr_get_exp(_x)  ((_x)->_mpfr_exp + 0)

#define mpfr_round(a,b) mpfr_rint((a), (b), MPFR_RNDNA)
#define mpfr_trunc(a,b) mpfr_rint((a), (b), MPFR_RNDZ)
#define mpfr_ceil(a,b)  mpfr_rint((a), (b), MPFR_RNDU)
#define mpfr_floor(a,b) mpfr_rint((a), (b), MPFR_RNDD)

#define mpfr_cmp_ui(b,i) mpfr_cmp_ui_2exp((b),(i),0)
#define mpfr_cmp_si(b,i) mpfr_cmp_si_2exp((b),(i),0)
#define mpfr_set(a,b,r)  mpfr_set4(a,b,r,MPFR_SIGN(b))
#define mpfr_abs(a,b,r)  mpfr_set4(a,b,r,1)
#define mpfr_copysign(a,b,c,r) mpfr_set4(a,b,r,MPFR_SIGN(c))
#define mpfr_setsign(a,b,s,r) mpfr_set4(a,b,r,(s) ? -1 : 1)
#define mpfr_signbit(x)  (MPFR_SIGN(x) < 0)
#define mpfr_cmp(b, c)   mpfr_cmp3(b, c, 1)
#define mpfr_mul_2exp(y,x,n,r) mpfr_mul_2ui((y),(x),(n),(r))
#define mpfr_div_2exp(y,x,n,r) mpfr_div_2ui((y),(x),(n),(r))


/* When using GCC, optimize certain common comparisons and affectations.
   + Remove ICC since it defines __GNUC__ but produces a
     huge number of warnings if you use this code.
     VL: I couldn't reproduce a single warning when enabling these macros
     with icc 10.1 20080212 on Itanium. But with this version, __ICC isn't
     defined (__INTEL_COMPILER is, though), so that these macros are enabled
     anyway. Checking with other ICC versions is needed. Possibly detect
     whether warnings are produced or not with a configure test.
   + Remove C++ too, since it complains too much. */
#if defined (__GNUC__) && !defined(__ICC) && !defined(__cplusplus)
#if (__GNUC__ >= 2)
#undef mpfr_cmp_ui
/* We use the fact that mpfr_sgn on NaN sets the erange flag and returns 0. */
#define mpfr_cmp_ui(_f,_u)                 \
 (__builtin_constant_p (_u) && (_u) == 0 ? \
   mpfr_sgn (_f) :                         \
   mpfr_cmp_ui_2exp ((_f),(_u),0))
#undef mpfr_cmp_si
#define mpfr_cmp_si(_f,_s)                 \
 (__builtin_constant_p (_s) && (_s) >= 0 ? \
   mpfr_cmp_ui ((_f), (_s)) :              \
   mpfr_cmp_si_2exp ((_f), (_s), 0))
#if __GNUC__ > 2 || __GNUC_MINOR__ >= 95
#undef mpfr_set_ui
#define mpfr_set_ui(_f,_u,_r)              \
 (__builtin_constant_p (_u) && (_u) == 0 ? \
   __extension__ ({                        \
     mpfr_ptr _p = (_f);                   \
     _p->_mpfr_sign = 1;                   \
     _p->_mpfr_exp = __MPFR_EXP_ZERO;      \
     (void) (_r); 0; }) :                  \
   mpfr_set_ui_2exp ((_f), (_u), 0, (_r)))
#endif
#undef mpfr_set_si
#define mpfr_set_si(_f,_s,_r)              \
 (__builtin_constant_p (_s) && (_s) >= 0 ? \
   mpfr_set_ui ((_f), (_s), (_r)) :        \
   mpfr_set_si_2exp ((_f), (_s), 0, (_r)))
#endif
#endif

/* Macro version of mpfr_stack interface for fast access */
#define mpfr_custom_get_size(p) ((size_t)                            \
       (((p)+GMP_NUMB_BITS-1)/GMP_NUMB_BITS*sizeof (mp_limb_t)))
#define mpfr_custom_init(m,p) do {} while (0)
#define mpfr_custom_get_mantissa(x) ((void*)((x)->_mpfr_d))
#define mpfr_custom_get_exp(x) ((x)->_mpfr_exp)
#define mpfr_custom_move(x,m) do { ((x)->_mpfr_d = (mp_limb_t*)(m)); } while (0)
#define mpfr_custom_init_set(x,k,e,p,m) do {                    \
  mpfr_ptr _x = (x);                                           \
  mp_exp_t _e;                                                 \
  mpfr_kind_t _t;                                              \
  int _s, _k;                                                  \
  _k = (k);                                                    \
  if (_k >= 0)  {                                              \
    _t = (mpfr_kind_t) _k;                                     \
    _s = 1;                                                    \
  } else {                                                     \
    _t = (mpfr_kind_t) -k;                                     \
    _s = -1;                                                   \
  }                                                            \
  _e = _t == MPFR_REGULAR_KIND ? (e) :                         \
    _t == MPFR_NAN_KIND ? __MPFR_EXP_NAN :                     \
    _t == MPFR_INF_KIND ? __MPFR_EXP_INF : __MPFR_EXP_ZERO;    \
  _x->_mpfr_prec = (p);                                        \
  _x->_mpfr_sign = _s;                                         \
  _x->_mpfr_exp  = _e;                                         \
  _x->_mpfr_d    = (mp_limb_t*) (m);                           \
 } while (0)
#define mpfr_custom_get_kind(x)                                              \
  ( (x)->_mpfr_exp >  __MPFR_EXP_INF ? (int)MPFR_REGULAR_KIND*MPFR_SIGN (x) \
  : (x)->_mpfr_exp == __MPFR_EXP_INF ? (int)MPFR_INF_KIND*MPFR_SIGN (x)     \
  : (x)->_mpfr_exp == __MPFR_EXP_NAN ? (int)MPFR_NAN_KIND                   \
  : (int) MPFR_ZERO_KIND * MPFR_SIGN (x) )


#endif /* MPFR_USE_NO_MACRO */

/* Theses are defined to be macros */
#define mpfr_init_set_si(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_si((x), (i), (rnd)) )
#define mpfr_init_set_ui(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_ui((x), (i), (rnd)) )
#define mpfr_init_set_d(x, d, rnd) \
 ( mpfr_init(x), mpfr_set_d((x), (d), (rnd)) )
#define mpfr_init_set_ld(x, d, rnd) \
 ( mpfr_init(x), mpfr_set_ld((x), (d), (rnd)) )
#define mpfr_init_set_z(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_z((x), (i), (rnd)) )
#define mpfr_init_set_q(x, i, rnd) \
 ( mpfr_init(x), mpfr_set_q((x), (i), (rnd)) )
#define mpfr_init_set(x, y, rnd) \
 ( mpfr_init(x), mpfr_set((x), (y), (rnd)) )
#define mpfr_init_set_f(x, y, rnd) \
 ( mpfr_init(x), mpfr_set_f((x), (y), (rnd)) )

/* Compatibility layer -- obsolete functions and macros */
#define mpfr_cmp_abs mpfr_cmpabs
#define mpfr_round_prec(x,r,p) mpfr_prec_round(x,p,r)
#define __gmp_default_rounding_mode (mpfr_get_default_rounding_mode())
#define __mpfr_emin (mpfr_get_emin())
#define __mpfr_emax (mpfr_get_emax())
#define __mpfr_default_fp_bit_precision (mpfr_get_default_fp_bit_precision())
#define MPFR_EMIN_MIN mpfr_get_emin_min()
#define MPFR_EMIN_MAX mpfr_get_emin_max()
#define MPFR_EMAX_MIN mpfr_get_emax_min()
#define MPFR_EMAX_MAX mpfr_get_emax_max()
#define mpfr_version (mpfr_get_version())
#ifndef mpz_set_fr
# define mpz_set_fr mpfr_get_z
#endif
#define mpfr_add_one_ulp(x,r) \
 (mpfr_sgn (x) > 0 ? mpfr_nextabove (x) : mpfr_nextbelow (x))
#define mpfr_sub_one_ulp(x,r) \
 (mpfr_sgn (x) > 0 ? mpfr_nextbelow (x) : mpfr_nextabove (x))

#endif /* __MPFR_H*/
