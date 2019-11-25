// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_MESH_MANIPULATOR_H_INCLUDED__
#define __C_MESH_MANIPULATOR_H_INCLUDED__

#include "IMeshManipulator.h"

namespace irr
{
namespace scene
{

class CMeshManipulator : public IMeshManipulator
{
public:
	virtual void flipSurfaces(IMesh* mesh) const _IRR_OVERRIDE_;

	virtual void setVertexColorAlpha(IMeshBuffer* meshBuffer, s32 alpha) const _IRR_OVERRIDE_;

	virtual void setVertexColors(IMeshBuffer* meshBuffer, video::SColor color) const _IRR_OVERRIDE_;

	virtual void scale(IMeshBuffer* meshBuffer, const core::vector3df& factor) const _IRR_OVERRIDE_;

	virtual void scaleTCoords(IMeshBuffer* meshBuffer, const core::vector2df& factor, u32 level = 0) const _IRR_OVERRIDE_;

	virtual void transform(IMeshBuffer* meshBuffer, const core::matrix4& mat) const _IRR_OVERRIDE_;

	virtual void recalculateNormals(IMeshBuffer* meshBuffer, bool smooth, bool angleWeighted) const _IRR_OVERRIDE_;

	virtual void recalculateTangents(IMeshBuffer* meshBuffer, bool recalculateNormals, bool smooth, bool angleWeighted) const _IRR_OVERRIDE_;

	virtual void makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolution = 0.001f) const _IRR_OVERRIDE_;

	virtual void makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset) const _IRR_OVERRIDE_;

	virtual bool copyIndices(IIndexBuffer* srcBuffer, IIndexBuffer* dstBuffer) const _IRR_OVERRIDE_;

	virtual bool copyVertices(IVertexBuffer* srcBuffer, u32 srcDescriptionBufferID, video::IVertexDescriptor* srcDescriptor,
		IVertexBuffer* dstBuffer, u32 dstDescriptionBufferID, video::IVertexDescriptor* dstDescriptor, bool copyCustomAttribute) const _IRR_OVERRIDE_;

	virtual bool createTangents(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, bool copyCustomAttribute,
		bool recalculateNormals, bool smooth, bool angleWeighted) _IRR_OVERRIDE_;

	virtual bool createUniquePrimitives(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer) const _IRR_OVERRIDE_;

	virtual bool createWelded(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, f32 tolerance = core::ROUNDING_ERROR_f32,
		bool check4Component = true, bool check3Component = true, bool check2Component = true, bool check1Component = true) const _IRR_OVERRIDE_;

	virtual s32 getPolyCount(IMesh* mesh) const _IRR_OVERRIDE_;

	virtual IMesh* createForsythOptimizedMesh(const IMesh* mesh) const _IRR_OVERRIDE_;

protected:
	virtual void setVertexColor(IMeshBuffer* meshBuffer, video::SColor color, bool onlyAlpha) const;
};

} // end namespace scene
} // end namespace irr


#endif
