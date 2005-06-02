/* Copyright (C) 1991, 1992, 1995, 2002 Free Software Foundation, Inc.
This file was part of the GNU C Library. Modified by kb@mail.tug.org to
avoid glibc-isms. Modified by Vincent Lefevre (-> ISO C prototypes and
mpfr_ prefix).

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This file is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file COPYING.LIB.  If not, write
to the Free Software Foundation, Inc., 51 Franklin Place, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if !defined (__STDC__) || !__STDC__
/* This is a separate conditional since some stdc systems
   reject `defined (const)'.  */
#ifndef const
#define const
#endif
#endif

#include <ctype.h>

int mpfr_strcasecmp (const char *, const char *);

/* Compare strings S1 and S2, ignoring case, returning less than,
   equal to or greater than zero if S1 is lexiographically less
   than, equal to or greater than S2. */
int
mpfr_strcasecmp (const char *s1, const char *s2)
{
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2)
    return 0;

  do
    {
      c1 = tolower (*p1++);
      c2 = tolower (*p2++);
      if (c1 == '\0')
	break;
    }
  while (c1 == c2);

  return c1 - c2;
}
