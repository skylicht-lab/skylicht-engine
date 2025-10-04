// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "CFPSCounter.h"
#include "irrMath.h"

namespace irr
{
namespace video
{


CFPSCounter::CFPSCounter()
	:FPS(60), DrawCall(0), Primitive(0), StartTime(0), FramesCounted(0), 
	TextureChangedCount(0), TextureChangedCounted(0),
	PrimitivesCounted(0), TextureChangedCountAverage(0), PrimitiveAverage(0), PrimitiveTotal(0)
{

}


//! returns current fps
s32 CFPSCounter::getFPS() const
{
	return FPS;
}


//! returns current primitive count
u32 CFPSCounter::getPrimitive() const
{
	return Primitive;
}

u32 CFPSCounter::getTextureChangeCount() const
{
	return TextureChangedCount;
}

//! returns average primitive count of last period
u32 CFPSCounter::getPrimitiveAverage() const
{
	return PrimitiveAverage;
}

u32 CFPSCounter::getTextureChangeAverage() const
{
	return TextureChangedCountAverage;
}

//! returns accumulated primitive count since start
u32 CFPSCounter::getPrimitiveTotal() const
{
	return PrimitiveTotal;
}

u32 CFPSCounter::getDrawCallAverage() const
{
	return DrawCallAverage;
}

u32 CFPSCounter::getDrawCall() const
{
	return DrawCall;
}

//! to be called every frame
void CFPSCounter::registerFrame(u32 now, u32 primitivesDrawn, u32 textureChangeCount, u32 drawCall)
{
	++FramesCounted;
	PrimitiveTotal += primitivesDrawn;
	PrimitivesCounted += primitivesDrawn;
	Primitive = primitivesDrawn;
	
	TextureChangedCounted += textureChangeCount;
	TextureChangedCount = textureChangeCount;

	DrawCallTotal += drawCall;
	DrawCall = drawCall;

	const u32 milliseconds = now - StartTime;

	if (milliseconds >= 1500 )
	{
		const f32 invMilli = core::reciprocal ( (f32) milliseconds );
		
		FPS = core::ceil32 ( ( 1000 * FramesCounted ) * invMilli );
		PrimitiveAverage = core::ceil32 ( ( 1000 * PrimitivesCounted ) * invMilli );
		TextureChangedCountAverage = core::ceil32 ( ( 1000 * TextureChangedCounted ) * invMilli );
		DrawCallAverage = core::ceil32((1000 * DrawCallTotal) * invMilli);

		FramesCounted = 0;
		PrimitivesCounted = 0;
		DrawCallTotal = 0;
		StartTime = now;
	}
}


} // end namespace video
} // end namespace irr

