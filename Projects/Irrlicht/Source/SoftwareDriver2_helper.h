// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

/*
	History:
	- changed behavior for log2 textures ( replaced multiplies by shift )
*/

#ifndef __S_VIDEO_2_SOFTWARE_HELPER_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_HELPER_H_INCLUDED__

#include "irrMath.h"
#include "SMaterial.h"



namespace irr
{
	// supporting different packed pixel needs many defines...
	typedef u32	tVideoSample;

	#define	MASK_A	0xFF000000
	#define	MASK_R	0x00FF0000
	#define	MASK_G	0x0000FF00
	#define	MASK_B	0x000000FF

	#define	SHIFT_A	24
	#define	SHIFT_R	16
	#define	SHIFT_G	8
	#define	SHIFT_B	0

	#define	COLOR_MAX					0xFF
	#define	COLOR_MAX_LOG2				8
	#define	COLOR_BRIGHT_WHITE			0xFFFFFFFF

	#define VIDEO_SAMPLE_GRANULARITY	2	


	// ----------------------- Generic ----------------------------------

	//! a more useful memset for pixel
	// (standard memset only works with 8-bit values)
	inline void memset32(void * dest, const u32 value, u32 bytesize)
	{
		u32 * d = (u32*) dest;

		u32 i;

		// loops unrolled to reduce the number of increments by factor ~8.
		i = bytesize >> (2 + 3);
		while (i)
		{
			d[0] = value;
			d[1] = value;
			d[2] = value;
			d[3] = value;

			d[4] = value;
			d[5] = value;
			d[6] = value;
			d[7] = value;

			d += 8;
			i -= 1;
		}

		i = (bytesize >> 2 ) & 7;
		while (i)
		{
			d[0] = value;
			d += 1;
			i -= 1;
		}
	}

	//! a more useful memset for pixel
	// (standard memset only works with 8-bit values)
	inline void memset16(void * dest, const u16 value, u32 bytesize)
	{
		u16 * d = (u16*) dest;

		u32 i;

		// loops unrolled to reduce the number of increments by factor ~8.
		i = bytesize >> (1 + 3);
		while (i)
		{
			d[0] = value;
			d[1] = value;
			d[2] = value;
			d[3] = value;

			d[4] = value;
			d[5] = value;
			d[6] = value;
			d[7] = value;

			d += 8;
			--i;
		}

		i = (bytesize >> 1 ) & 7;
		while (i)
		{
			d[0] = value;
			++d;
			--i;
		}
	}

	/*
		use biased loop counter
		--> 0 byte copy is forbidden
	*/
	REALINLINE void memcpy32_small ( void * dest, const void *source, u32 bytesize )
	{
		u32 c = bytesize >> 2;

		do
		{
			((u32*) dest ) [ c-1 ] = ((u32*) source) [ c-1 ];
		} while ( --c );

	}



	// integer log2 of a float ieee 754. TODO: non ieee floating point
	static inline s32 s32_log2_f32( f32 f)
	{
		u32 x = IR ( f );
		return ((x & 0x7F800000) >> 23) - 127;
	}

	static inline s32 s32_log2_s32(u32 x)
	{
		return s32_log2_f32( (f32) x);
	}

	static inline s32 s32_abs(s32 x)
	{
		s32 b = x >> 31;
		return (x ^ b ) - b;
	}


	//! conditional set based on mask and arithmetic shift
	REALINLINE u32 if_mask_a_else_b ( const u32 mask, const u32 a, const u32 b )
	{
		return ( mask & ( a ^ b ) ) ^ b;
	}

	// ------------------ Video---------------------------------------
	/*!
		Pixel = dest * ( 1 - alpha ) + source * alpha
		alpha [0;256]
	*/
	REALINLINE u32 PixelBlend32 ( const u32 c2, const u32 c1, u32 alpha )
	{
		u32 srcRB = c1 & 0x00FF00FF;
		u32 srcXG = c1 & 0x0000FF00;

		u32 dstRB = c2 & 0x00FF00FF;
		u32 dstXG = c2 & 0x0000FF00;


		u32 rb = srcRB - dstRB;
		u32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 8;
		xg >>= 8;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x00FF00FF;
		xg &= 0x0000FF00;

		return rb | xg;
	}

	/*!
		Pixel = dest * ( 1 - alpha ) + source * alpha
		alpha [0;32]
	*/
	inline u16 PixelBlend16 ( const u16 c2, const u32 c1, const u16 alpha )
	{
		const u16 srcRB = c1 & 0x7C1F;
		const u16 srcXG = c1 & 0x03E0;

		const u16 dstRB = c2 & 0x7C1F;
		const u16 dstXG = c2 & 0x03E0;

		u32 rb = srcRB - dstRB;
		u32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 5;
		xg >>= 5;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x7C1F;
		xg &= 0x03E0;

		return (u16)(rb | xg);
	}

	/*
		Pixel = c0 * (c1/31). c0 Alpha retain
	*/
	inline u16 PixelMul16 ( const u16 c0, const u16 c1)
	{
		return (u16)((( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 ) |
				(( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 ) |
				(( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5 ) |
				(c0 & 0x8000));
	}

	/*
		Pixel = c0 * (c1/31).
	*/
	inline u16 PixelMul16_2 ( u16 c0, u16 c1)
	{
		return	(u16)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
				( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
				( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
				( c0 & c1 & 0x8000));
	}

	/*
		Pixel = c0 * (c1/255). c0 Alpha Retain
	*/
	REALINLINE u32 PixelMul32 ( const u32 c0, const u32 c1)
	{
		return	(c0 & 0xFF000000) |
				(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
				(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
				(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
	}

	/*
		Pixel = c0 * (c1/255).
	*/
	REALINLINE u32 PixelMul32_2 ( const u32 c0, const u32 c1)
	{
		return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
				(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
				(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
				(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
	}

	/*
		Pixel = clamp ( c0 + c1, 0, 255 )
	*/
	REALINLINE u32 PixelAdd32 ( const u32 c2, const u32 c1)
	{
		u32 sum = ( c2 & 0x00FFFFFF )  + ( c1 & 0x00FFFFFF );
		u32 low_bits = ( c2 ^ c1 ) & 0x00010101;
		s32 carries  = ( sum - low_bits ) & 0x01010100;
		u32 modulo = sum - carries;
		u32 clamp = carries - ( carries >> 8 );
		return modulo | clamp;
	}

	#if 0

	// 1 - Bit Alpha Blending
	inline u16 PixelBlend16 ( const u16 destination, const u16 source )
	{
	   if((source & 0x8000) == 0x8000)
		  return source; // The source is visible, so use it.
	   else
		  return destination; // The source is transparent, so use the destination.
	}

	// 1 - Bit Alpha Blending 16Bit SIMD
	inline u32 PixelBlend16_simd ( const u32 destination, const u32 source )
	{
		switch(source & 0x80008000)
		{
			case 0x80008000: // Both source pixels are visible
				return source;

			case 0x80000000: // Only the first source pixel is visible
				return (source & 0xFFFF0000) | (destination & 0x0000FFFF);

			case 0x00008000: // Only the second source pixel is visible.
				return (destination & 0xFFFF0000) | (source & 0x0000FFFF);

			default: // Neither source pixel is visible.
				return destination;
		}
	}
	#else

	// 1 - Bit Alpha Blending
	inline u16 PixelBlend16 ( const u16 c2, const u16 c1 )
	{
		u16 mask = ((c1 & 0x8000) >> 15 ) + 0x7fff;
		return (c2 & mask ) | ( c1 & ~mask );
	}

	// 1 - Bit Alpha Blending 16Bit SIMD
	inline u32 PixelBlend16_simd ( const u32 c2, const u32 c1 )
	{
		u32 mask = ((c1 & 0x80008000) >> 15 ) + 0x7fff7fff;
		return (c2 & mask ) | ( c1 & ~mask );
	}

	#endif

	/*!
		Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
	*/
	inline u32 PixelBlend32 ( const u32 c2, const u32 c1 )
	{
		// alpha test
		u32 alpha = c1 & 0xFF000000;

		if ( 0 == alpha )
			return c2;

		if ( 0xFF000000 == alpha )
		{
			return c1;
		}

		alpha >>= 24;

		// add highbit alpha, if ( alpha > 127 ) alpha += 1;
		alpha += ( alpha >> 7);

		u32 srcRB = c1 & 0x00FF00FF;
		u32 srcXG = c1 & 0x0000FF00;

		u32 dstRB = c2 & 0x00FF00FF;
		u32 dstXG = c2 & 0x0000FF00;


		u32 rb = srcRB - dstRB;
		u32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 8;
		xg >>= 8;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x00FF00FF;
		xg &= 0x0000FF00;

		return (c1 & 0xFF000000) | rb | xg;
	}

	// some 2D Defines
	struct AbsRectangle
	{
		s32 x0;
		s32 y0;
		s32 x1;
		s32 y1;
	};

	//! 2D Intersection test
	inline bool intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
	{
		dest.x0 = core::s32_max( a.x0, b.x0 );
		dest.y0 = core::s32_max( a.y0, b.y0 );
		dest.x1 = core::s32_min( a.x1, b.x1 );
		dest.y1 = core::s32_min( a.y1, b.y1 );
		return dest.x0 < dest.x1 && dest.y0 < dest.y1;
	}

	// some 1D defines
	struct sIntervall
	{
		s32 start;
		s32 end;
	};

	// returning intersection width
	inline s32 intervall_intersect_test( const sIntervall& a, const sIntervall& b)
	{
		return core::s32_min( a.end, b.end ) - core::s32_max( a.start, b.start );
	}


} // end namespace irr

#endif

