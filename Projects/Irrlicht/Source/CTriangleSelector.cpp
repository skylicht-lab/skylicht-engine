// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "CTriangleSelector.h"
#include "ISceneNode.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{

//! constructor
CTriangleSelector::CTriangleSelector(ISceneNode* node)
: SceneNode(node), LastMeshFrame(0)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	BoundingBox.reset(0.f, 0.f, 0.f);
}


//! constructor
CTriangleSelector::CTriangleSelector(const core::aabbox3d<f32>& box, ISceneNode* node)
: SceneNode(node), LastMeshFrame(0)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	BoundingBox=box;
	// TODO
}


//! constructor
CTriangleSelector::CTriangleSelector(const IMesh* mesh, ISceneNode* node)
: SceneNode(node), LastMeshFrame(0)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	createFromMesh(mesh);
}

void CTriangleSelector::createFromMesh(const IMesh* mesh)
{
	const u32 cnt = mesh->getMeshBufferCount();
	u32 totalFaceCount = 0;
	for (u32 j=0; j<cnt; ++j)
		totalFaceCount += mesh->getMeshBuffer(j)->getIndexBuffer()->getIndexCount();
	totalFaceCount /= 3;
	Triangles.set_used(totalFaceCount);

	updateFromMesh(mesh);
}


void CTriangleSelector::updateFromMesh(const IMesh* mesh) const
{
	if (!mesh)
		return;

	u32 meshBuffers = mesh->getMeshBufferCount();
	u32 triangleCount = 0;

	BoundingBox.reset(0.f, 0.f, 0.f);
	for (u32 i = 0; i < meshBuffers; ++i)
	{
		IMeshBuffer* buf = mesh->getMeshBuffer(i);
		u32 idxCnt = buf->getIndexBuffer()->getIndexCount();
		
		video::IVertexAttribute* attribute = buf->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_POSITION);

		if(!attribute)
			continue;

		u8* offset = static_cast<u8*>(buf->getVertexBuffer()->getVertices());
		offset += attribute->getOffset();

		for (u32 index = 0; index < idxCnt; index += 3)
		{
			core::vector3df* position0 = (core::vector3df*)(offset + buf->getVertexBuffer()->getVertexSize() * buf->getIndexBuffer()->getIndex(index+0));
			core::vector3df* position1 = (core::vector3df*)(offset + buf->getVertexBuffer()->getVertexSize() * buf->getIndexBuffer()->getIndex(index+1));
			core::vector3df* position2 = (core::vector3df*)(offset + buf->getVertexBuffer()->getVertexSize() * buf->getIndexBuffer()->getIndex(index+2));

			core::triangle3df& tri = Triangles[triangleCount++];
			tri.pointA = *position0;
			tri.pointB = *position1;
			tri.pointC = *position2;
			BoundingBox.addInternalPoint(tri.pointA);
			BoundingBox.addInternalPoint(tri.pointB);
			BoundingBox.addInternalPoint(tri.pointC);
		}
	}
}


void CTriangleSelector::update(void) const
{
	
}


//! Gets all triangles.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::matrix4* transform) const
{
	// Update my triangles if necessary
	update();

	u32 cnt = Triangles.size();
	if (cnt > (u32)arraySize)
		cnt = (u32)arraySize;

	core::matrix4 mat;
	if (transform)
		mat = *transform;
	if (SceneNode)
		mat = SceneNode->getAbsoluteTransformation();

	for (u32 i=0; i<cnt; ++i)
	{
		mat.transformVect( triangles[i].pointA, Triangles[i].pointA );
		mat.transformVect( triangles[i].pointB, Triangles[i].pointB );
		mat.transformVect( triangles[i].pointC, Triangles[i].pointC );
	}

	outTriangleCount = cnt;
}


//! Gets all triangles which lie within a specific bounding box.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::aabbox3d<f32>& box,
					const core::matrix4* transform) const
{
	// Update my triangles if necessary
	update();

	core::matrix4 mat(core::matrix4::EM4CONST_NOTHING);
	core::aabbox3df tBox(box);

	if (SceneNode)
	{
		SceneNode->getAbsoluteTransformation().getInverse(mat);
		mat.transformBoxEx(tBox);
	}
	if (transform)
		mat = *transform;
	else
		mat.makeIdentity();
	if (SceneNode)
		mat *= SceneNode->getAbsoluteTransformation();

	outTriangleCount = 0;

	if (!tBox.intersectsWithBox(BoundingBox))
		return;

	s32 triangleCount = 0;
	const u32 cnt = Triangles.size();
	for (u32 i=0; i<cnt; ++i)
	{
		// This isn't an accurate test, but it's fast, and the
		// API contract doesn't guarantee complete accuracy.
		if (Triangles[i].isTotalOutsideBox(tBox))
		   continue;

		triangles[triangleCount] = Triangles[i];
		mat.transformVect(triangles[triangleCount].pointA);
		mat.transformVect(triangles[triangleCount].pointB);
		mat.transformVect(triangles[triangleCount].pointC);

		++triangleCount;

		if (triangleCount == arraySize)
			break;
	}

	outTriangleCount = triangleCount;
}


//! Gets all triangles which have or may have contact with a 3d line.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::line3d<f32>& line,
					const core::matrix4* transform) const
{
	// Update my triangles if necessary
	update();

	core::aabbox3d<f32> box(line.start);
	box.addInternalPoint(line.end);

	// TODO: Could be optimized for line a little bit more.
	getTriangles(triangles, arraySize, outTriangleCount,
				box, transform);
}


//! Returns amount of all available triangles in this selector
s32 CTriangleSelector::getTriangleCount() const
{
	return Triangles.size();
}


/* Get the number of TriangleSelectors that are part of this one.
Only useful for MetaTriangleSelector others return 1
*/
u32 CTriangleSelector::getSelectorCount() const
{
	return 1;
}


/* Get the TriangleSelector based on index based on getSelectorCount.
Only useful for MetaTriangleSelector others return 'this' or 0
*/
ITriangleSelector* CTriangleSelector::getSelector(u32 index)
{
	if (index)
		return 0;
	else
		return this;
}


/* Get the TriangleSelector based on index based on getSelectorCount.
Only useful for MetaTriangleSelector others return 'this' or 0
*/
const ITriangleSelector* CTriangleSelector::getSelector(u32 index) const
{
	if (index)
		return 0;
	else
		return this;
}


} // end namespace scene
} // end namespace irr
