/* Operations on HOST_WIDE_INT.
   Copyright (C) 1987-2013 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "diagnostic-core.h"

#if GCC_VERSION < 3004

/* The functions clz_hwi, ctz_hwi, ffs_hwi, floor_log2, ceil_log2,
   and exact_log2 are defined as inline functions in hwint.h
   if GCC_VERSION >= 3004.
   The definitions here are used for older versions of GCC and
   non-GCC bootstrap compilers.  */

/* Given X, an unsigned number, return the largest int Y such that 2**Y <= X.
   If X is 0, return -1.  */

int
floor_log2 (unsigned HOST_WIDE_INT x)
{
  int t = 0;

  if (x == 0)
    return -1;

  if (HOST_BITS_PER_WIDE_INT > 64)
    if (x >= (unsigned HOST_WIDE_INT) 1 << (t + 64))
      t += 64;
  if (HOST_BITS_PER_WIDE_INT > 32)
    if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 32))
      t += 32;
  if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 16))
    t += 16;
  if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 8))
    t += 8;
  if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 4))
    t += 4;
  if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 2))
    t += 2;
  if (x >= ((unsigned HOST_WIDE_INT) 1) << (t + 1))
    t += 1;

  return t;
}

/* Given X, an unsigned number, return the largest Y such that 2**Y >= X.  */

int
ceil_log2 (unsigned HOST_WIDE_INT x)
{
  return floor_log2 (x - 1) + 1;
}

/* Return the logarithm of X, base 2, considering X unsigned,
   if X is a power of 2.  Otherwise, returns -1.  */

int
exact_log2 (unsigned HOST_WIDE_INT x)
{
  if (x != (x & -x))
    return -1;
  return floor_log2 (x);
}

/* Given X, an unsigned number, return the number of least significant bits
   that are zero.  When X == 0, the result is the word size.  */

int
ctz_hwi (unsigned HOST_WIDE_INT x)
{
  return x ? floor_log2 (x & -x) : HOST_BITS_PER_WIDE_INT;
}

/* Similarly for most significant bits.  */

int
clz_hwi (unsigned HOST_WIDE_INT x)
{
  return HOST_BITS_PER_WIDE_INT - 1 - floor_log2 (x);
}

/* Similar to ctz_hwi, except that the least significant bit is numbered
   starting from 1, and X == 0 yields 0.  */

int
ffs_hwi (unsigned HOST_WIDE_INT x)
{
  return 1 + floor_log2 (x & -x);
}

/* Return the number of set bits in X.  */

int
popcount_hwi (unsigned HOST_WIDE_INT x)
{
  int i, ret = 0;
  size_t bits = sizeof (x) * CHAR_BIT;

  for (i = 0; i < bits; i += 1)
    {
      ret += x & 1;
      x >>= 1;
    }

  return ret;
}

#endif /* GCC_VERSION < 3004 */

/* Compute the absolute value of X.  */

HOST_WIDE_INT
abs_hwi (HOST_WIDE_INT x)
{
  gcc_checking_assert (x != HOST_WIDE_INT_MIN);
  return x >= 0 ? x : -x;
}

/* Compute the absolute value of X as an unsigned type.  */

unsigned HOST_WIDE_INT
absu_hwi (HOST_WIDE_INT x)
{
  return x >= 0 ? (unsigned HOST_WIDE_INT)x : -(unsigned HOST_WIDE_INT)x;
}

/* Compute the greatest common divisor of two numbers A and B using
   Euclid's algorithm.  */

HOST_WIDE_INT
gcd (HOST_WIDE_INT a, HOST_WIDE_INT b)
{
  HOST_WIDE_INT x, y, z;

  x = abs_hwi (a);
  y = abs_hwi (b);

  while (x > 0)
    {
      z = y % x;
      y = x;
      x = z;
    }

  return y;
}

/* For X and Y positive integers, return X multiplied by Y and check
   that the result does not overflow.  */

HOST_WIDE_INT
pos_mul_hwi (HOST_WIDE_INT x, HOST_WIDE_INT y)
{
  if (x != 0)
    gcc_checking_assert ((HOST_WIDE_INT_MAX) / x >= y);

  return x * y;
}

/* Return X multiplied by Y and check that the result does not
   overflow.  */

HOST_WIDE_INT
mul_hwi (HOST_WIDE_INT x, HOST_WIDE_INT y)
{
  gcc_checking_assert (x != HOST_WIDE_INT_MIN
		       && y != HOST_WIDE_INT_MIN);

  if (x >= 0)
    {
      if (y >= 0)
	return pos_mul_hwi (x, y);

      return -pos_mul_hwi (x, -y);
    }

  if (y >= 0)
    return -pos_mul_hwi (-x, y);

  return pos_mul_hwi (-x, -y);
}

/* Compute the least common multiple of two numbers A and B .  */

HOST_WIDE_INT
least_common_multiple (HOST_WIDE_INT a, HOST_WIDE_INT b)
{
  return mul_hwi (abs_hwi (a) / gcd (a, b), abs_hwi (b));
}

#ifdef ENABLE_CHECKING
/* Sign extend SRC starting from PREC.  */

HOST_WIDE_INT
sext_hwi (HOST_WIDE_INT src, unsigned int prec)
{
  gcc_checking_assert (prec <= HOST_BITS_PER_WIDE_INT);

  if (prec == HOST_BITS_PER_WIDE_INT)
    return src;
  else
    {
      int shift = HOST_BITS_PER_WIDE_INT - prec;
      return (src << shift) >> shift;
    }
}

/* Zero extend SRC starting from PREC.  */

unsigned HOST_WIDE_INT
zext_hwi (unsigned HOST_WIDE_INT src, unsigned int prec)
{
  gcc_checking_assert (prec <= HOST_BITS_PER_WIDE_INT);

  if (prec == HOST_BITS_PER_WIDE_INT)
    return src;
  else
    return src & (((HOST_WIDE_INT)1 << prec) - 1);
}

#endif
