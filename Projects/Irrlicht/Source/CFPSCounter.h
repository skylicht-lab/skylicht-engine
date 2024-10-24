// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPSCOUNTER_H_INCLUDED__
#define __C_FPSCOUNTER_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace video
{


class CFPSCounter
{
public:
	CFPSCounter();

	//! returns current fps
	s32 getFPS() const;

	//! returns primitive count
	u32 getPrimitive() const;

	u32 getTextureChangeCount() const;

	//! returns average primitive count of last period
	u32 getPrimitiveAverage() const;

	u32 getTextureChangeAverage() const;

	//! returns accumulated primitive count since start
	u32 getPrimitiveTotal() const;

	u32 getDrawCallAverage() const;

	u32 getDrawCall() const;

	//! to be called every frame
	void registerFrame(u32 now, u32 primitive, u32 textureChangeCount, u32 drawCall);

private:

	s32 FPS;
	u32 Primitive;
	u32 TextureChangedCount;	
	u32 StartTime;

	u32 FramesCounted;
	u32 PrimitivesCounted;
	u32 PrimitiveAverage;
	u32 TextureChangedCounted;
	u32 TextureChangedCountAverage;
	u32 DrawCallTotal;
	u32 DrawCall;
	u32 DrawCallAverage;
	u32 PrimitiveTotal;
};


} // end namespace video
} // end namespace irr


#endif

