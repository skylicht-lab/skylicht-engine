/*
	mpg123_msvc: MPEG Audio Decoder library wrapper header for MS VC++ 2005

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	initially written by Patrick Dehne and Thomas Orgis.
*/
#ifndef MPG123_MSVC_H
#define MPG123_MSVC_H

#if defined(_MSC_VER)
#include <tchar.h>
#endif

#include <stdlib.h>
#include <sys/types.h>

#if defined(_MSC_VER)
typedef long ssize_t;
#endif

#include <stdint.h>

#ifndef PRIiMAX
#define PRIiMAX "I64i"
#endif

#if defined(_MSC_VER)
typedef __int64 intmax_t;
#endif

// ftell returns long, _ftelli64 returns __int64
// off_t is long, not __int64, use ftell
#define ftello ftell

#define MPG123_NO_CONFIGURE
#include "mpg123.h.in" /* Yes, .h.in; we include the configure template! */

#endif
