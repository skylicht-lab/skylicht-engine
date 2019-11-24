// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CMeshManipulator.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "irrOS.h"
#include "irrMap.h"

namespace irr
{
namespace scene
{

void CMeshManipulator::flipSurfaces(scene::IMesh* mesh) const
{
	if (!mesh)
		return;

	const u32 bcount = mesh->getMeshBufferCount();

	for (u32 b=0; b<bcount; ++b)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		IIndexBuffer* ib = buffer->getIndexBuffer();

		const u32 idxcnt = ib->getIndexCount();

		if (ib->getType() == video::EIT_16BIT)
		{
			u16* idx = reinterpret_cast<u16*>(ib->getIndices());
			for (u32 i=0; i<idxcnt; i+=3)
			{
				const u16 tmp = idx[i+1];
				idx[i+1] = idx[i+2];
				idx[i+2] = tmp;
			}
		}
		else
		{
			u32* idx = reinterpret_cast<u32*>(ib->getIndices());
			for (u32 i=0; i<idxcnt; i+=3)
			{
				const u32 tmp = idx[i+1];
				idx[i+1] = idx[i+2];
				idx[i+2] = tmp;
			}
		}
	}
}

void CMeshManipulator::setVertexColor(IMeshBuffer* meshBuffer, video::SColor color, bool onlyAlpha) const
{
	if (!meshBuffer)
		return;

	video::IVertexAttribute* attribute = meshBuffer->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_COLOR);

	if (!attribute)
		return;

	u32 bufferID = attribute->getBufferID();

	IVertexBuffer* vb = meshBuffer->getVertexBuffer(bufferID);

	u8* offset = static_cast<u8*>(vb->getVertices());
	offset += attribute->getOffset();

	for (u32 i = 0; i < vb->getVertexCount(); ++i)
	{
		video::SColor* vcolor = (video::SColor*)offset;

		if(onlyAlpha)
			(*vcolor).setAlpha(color.getAlpha());
		else
			*vcolor = color;

		offset += vb->getVertexSize();
	}
}

void CMeshManipulator::setVertexColorAlpha(IMeshBuffer* meshBuffer, s32 alpha) const
{
	setVertexColor(meshBuffer, video::SColor(alpha, 255, 255, 255), true);
}

void CMeshManipulator::setVertexColors(IMeshBuffer* meshBuffer, video::SColor color) const
{
	setVertexColor(meshBuffer, color, false);
}

void CMeshManipulator::scale(IMeshBuffer* meshBuffer, const core::vector3df& factor) const
{
	if (!meshBuffer)
		return;

	video::IVertexAttribute* attribute = meshBuffer->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_POSITION);

	if (!attribute)
		return;

	u32 bufferID = attribute->getBufferID();

	IVertexBuffer* vb = meshBuffer->getVertexBuffer(bufferID);

	u8* offset = static_cast<u8*>(vb->getVertices());
	offset += attribute->getOffset();

	for (u32 i = 0; i < vb->getVertexCount(); ++i)
	{
		core::vector3df* position = (core::vector3df*)offset;

		*position *= factor;

		offset += vb->getVertexSize();
	}

	meshBuffer->recalculateBoundingBox();
}

void CMeshManipulator::scaleTCoords(IMeshBuffer* meshBuffer, const core::vector2df& factor, u32 level) const
{
	if (!meshBuffer)
		return;

	if (level > 7)
		level = 7;

	for (u32 j = 0; j < level; ++j)
	{
		video::E_VERTEX_ATTRIBUTE_SEMANTIC semantic = video::EVAS_TEXCOORD0;
		s32 value = (s32)semantic;
		value += j;
		semantic = (video::E_VERTEX_ATTRIBUTE_SEMANTIC)value;

		video::IVertexAttribute* attribute = meshBuffer->getVertexDescriptor()->getAttributeBySemantic(semantic);

		if (!attribute)
			continue;

		u32 bufferID = attribute->getBufferID();

		IVertexBuffer* vb = meshBuffer->getVertexBuffer(bufferID);

		u8* offset = static_cast<u8*>(vb->getVertices());
		offset += attribute->getOffset();

		for (u32 i = 0; i < vb->getVertexCount(); ++i)
		{
			core::vector2df* texCoord = (core::vector2df*)offset;

			*texCoord *= factor;

			offset += vb->getVertexSize();
		}
	}
}

void CMeshManipulator::transform(IMeshBuffer* meshBuffer, const core::matrix4& matrix) const
{
	if (!meshBuffer)
		return;

	video::IVertexAttribute* attribute = meshBuffer->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_POSITION);

	if (!attribute)
		return;

	u32 bufferID = attribute->getBufferID();

	IVertexBuffer* vb = meshBuffer->getVertexBuffer(bufferID);

	u8* offset = static_cast<u8*>(vb->getVertices());
	offset += attribute->getOffset();

	for (u32 i = 0; i < vb->getVertexCount(); ++i)
	{
		core::vector3df* position = (core::vector3df*)offset;

		matrix.transformVect(*position);

		offset += vb->getVertexSize();
	}

	meshBuffer->recalculateBoundingBox();
}

static inline core::vector3df getAngleWeight(const core::vector3df& v1,
		const core::vector3df& v2,
		const core::vector3df& v3)
{
	// Calculate this triangle's weight for each of its three vertices
	// start by calculating the lengths of its sides
	const f32 a = v2.getDistanceFromSQ(v3);
	const f32 asqrt = sqrtf(a);
	const f32 b = v1.getDistanceFromSQ(v3);
	const f32 bsqrt = sqrtf(b);
	const f32 c = v1.getDistanceFromSQ(v2);
	const f32 csqrt = sqrtf(c);

	// use them to find the angle at each vertex
	return core::vector3df(
		acosf((b + c - a) / (2.f * bsqrt * csqrt)),
		acosf((-b + c + a) / (2.f * asqrt * csqrt)),
		acosf((b - c + a) / (2.f * bsqrt * asqrt)));
}

void CMeshManipulator::recalculateNormals(IMeshBuffer* meshBuffer, bool smooth, bool angleWeighted) const
{
	if (!meshBuffer || !meshBuffer->isVertexBufferCompatible())
		return;

	// Check Descriptor format for required 2 components.

	int Found = 0;

	video::IVertexDescriptor* vd = meshBuffer->getVertexDescriptor();

	u32 positionBufferID = 0;
	u32 positionOffset = 0;

	u32 normalBufferID = 0;
	u32 normalOffset = 0;

	for (u32 i = 0; i < vd->getAttributeCount(); ++i)
	{
		video::IVertexAttribute * attribute = vd->getAttribute(i);

		switch (attribute->getSemantic())
		{
		case video::EVAS_POSITION:
			positionBufferID = attribute->getBufferID();
			positionOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_NORMAL:
			normalBufferID = attribute->getBufferID();
			normalOffset = attribute->getOffset();
			++Found;
			break;
		default:
			break;
		}
	}

	if (Found != 2)
		return;

	// Recalculate normals.

	core::vector3df* position0 = 0;
	core::vector3df* position1 = 0;
	core::vector3df* position2 = 0;
	core::vector3df* normal0 = 0;
	core::vector3df* normal1 = 0;
	core::vector3df* normal2 = 0;

	IIndexBuffer* ib = meshBuffer->getIndexBuffer();
	IVertexBuffer* vbP = meshBuffer->getVertexBuffer(positionBufferID);
	IVertexBuffer* vbN = meshBuffer->getVertexBuffer(normalBufferID);

	const u32 vertexSizeP = vbP->getVertexSize();
	const u32 vertexSizeN = vbN->getVertexSize();

	u8* positionData = static_cast<u8*>(vbP->getVertices());
	positionData += positionOffset;

	u8* normalData = static_cast<u8*>(vbN->getVertices());
	normalData += normalOffset;

	for (u32 i = 0; i < ib->getIndexCount(); i+=3)
	{
		position0 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 0));
		position1 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 1));
		position2 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 2));

		normal0 = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i + 0));
		normal1 = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i + 1));
		normal2 = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i + 2));

		core::vector3df normal = core::plane3d<f32>(*position0, *position1, *position2).Normal;

		core::vector3df weight(1.0f, 1.0f, 1.0f);

		if (angleWeighted)
			weight = getAngleWeight(*position0, *position1, *position2);

		*normal0 = normal * weight.X;
		*normal1 = normal * weight.Y;
		*normal2 = normal * weight.Z;
	}

	if (smooth)
	{
		for (u32 i = 0; i < vbN->getVertexCount(); ++i)
		{
			normal0 = (core::vector3df*)(normalData + vertexSizeN * i);
			(*normal0).normalize();
		}
	}
}

void calculateTangents(
	core::vector3df& normal,
	core::vector3df& tangent,
	core::vector3df& binormal,
	const core::vector3df& vt1, const core::vector3df& vt2, const core::vector3df& vt3,
	const core::vector2df& tc1, const core::vector2df& tc2, const core::vector2df& tc3)
{
	// choose one of them:
	//#define USE_NVIDIA_GLH_VERSION // use version used by nvidia in glh headers
	#define USE_IRR_VERSION

#ifdef USE_IRR_VERSION

	core::vector3df v1 = vt1 - vt2;
	core::vector3df v2 = vt3 - vt1;
	normal = v2.crossProduct(v1);
	normal.normalize();

	// binormal

	f32 deltaX1 = tc1.X - tc2.X;
	f32 deltaX2 = tc3.X - tc1.X;
	binormal = (v1 * deltaX2) - (v2 * deltaX1);
	binormal.normalize();

	// tangent

	f32 deltaY1 = tc1.Y - tc2.Y;
	f32 deltaY2 = tc3.Y - tc1.Y;
	tangent = (v1 * deltaY2) - (v2 * deltaY1);
	tangent.normalize();

	// adjust

	core::vector3df txb = tangent.crossProduct(binormal);
	if (txb.dotProduct(normal) < 0.0f)
	{
		tangent *= -1.0f;
		binormal *= -1.0f;
	}

#endif // USE_IRR_VERSION

#ifdef USE_NVIDIA_GLH_VERSION

	tangent.set(0,0,0);
	binormal.set(0,0,0);

	core::vector3df v1(vt2.X - vt1.X, tc2.X - tc1.X, tc2.Y - tc1.Y);
	core::vector3df v2(vt3.X - vt1.X, tc3.X - tc1.X, tc3.Y - tc1.Y);

	core::vector3df txb = v1.crossProduct(v2);
	if ( !core::iszero ( txb.X ) )
	{
		tangent.X  = -txb.Y / txb.X;
		binormal.X = -txb.Z / txb.X;
	}

	v1.X = vt2.Y - vt1.Y;
	v2.X = vt3.Y - vt1.Y;
	txb = v1.crossProduct(v2);

	if ( !core::iszero ( txb.X ) )
	{
		tangent.Y  = -txb.Y / txb.X;
		binormal.Y = -txb.Z / txb.X;
	}

	v1.X = vt2.Z - vt1.Z;
	v2.X = vt3.Z - vt1.Z;
	txb = v1.crossProduct(v2);

	if ( !core::iszero ( txb.X ) )
	{
		tangent.Z  = -txb.Y / txb.X;
		binormal.Z = -txb.Z / txb.X;
	}

	tangent.normalize();
	binormal.normalize();

	normal = tangent.crossProduct(binormal);
	normal.normalize();

	binormal = tangent.crossProduct(normal);
	binormal.normalize();

	core::plane3d<f32> pl(vt1, vt2, vt3);

	if(normal.dotProduct(pl.Normal) < 0.0f )
		normal *= -1.0f;

#endif // USE_NVIDIA_GLH_VERSION
}

void CMeshManipulator::recalculateTangents(IMeshBuffer* meshBuffer, bool recalculateNormals, bool smooth, bool angleWeighted) const
{
	if (!meshBuffer || !meshBuffer->isVertexBufferCompatible())
		return;

	// Check Descriptor format for required 5 components.

	int Found = 0;

	video::IVertexDescriptor* vd = meshBuffer->getVertexDescriptor();

	u32 positionBufferID = 0;
	u32 positionOffset = 0;

	u32 normalBufferID = 0;
	u32 normalOffset = 0;

	u32 texCoordBufferID = 0;
	u32 texCoordOffset = 0;

	u32 tangentBufferID = 0;
	u32 tangentOffset = 0;

	u32 binormalBufferID = 0;
	u32 binormalOffset = 0;

	for (u32 i = 0; i < vd->getAttributeCount(); ++i)
	{
		video::IVertexAttribute * attribute = vd->getAttribute(i);

		switch (attribute->getSemantic())
		{
		case video::EVAS_POSITION:
			positionBufferID = attribute->getBufferID();
			positionOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_NORMAL:
			normalBufferID = attribute->getBufferID();
			normalOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_TEXCOORD0:
			texCoordBufferID = attribute->getBufferID();
			texCoordOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_TANGENT:
			tangentBufferID = attribute->getBufferID();
			tangentOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_BINORMAL:
			binormalBufferID = attribute->getBufferID();
			binormalOffset = attribute->getOffset();
			++Found;
			break;
		default:
			break;
		}
	}

	if (Found != 5)
		return;

	// Recalculate tangents.

	core::vector3df* normal[3] = { 0, 0, 0 };
	core::vector3df* tangent[3] = { 0, 0, 0 };
	core::vector3df* binormal[3] = { 0, 0, 0 };
	core::vector3df* position[3] = { 0, 0, 0 };
	core::vector2df* texCoord[3] = { 0, 0, 0 };

	core::vector3df tmpNormal[3] = {
		core::vector3df(0.f, 0.f, 0.f),
		core::vector3df(0.f, 0.f, 0.f),
		core::vector3df(0.f, 0.f, 0.f)
	};

	IIndexBuffer* ib = meshBuffer->getIndexBuffer();
	IVertexBuffer* vbP = meshBuffer->getVertexBuffer(positionBufferID);
	IVertexBuffer* vbN = meshBuffer->getVertexBuffer(normalBufferID);
	IVertexBuffer* vbC = meshBuffer->getVertexBuffer(texCoordBufferID);
	IVertexBuffer* vbT = meshBuffer->getVertexBuffer(tangentBufferID);
	IVertexBuffer* vbB = meshBuffer->getVertexBuffer(binormalBufferID);

	const u32 vertexSizeP = vbP->getVertexSize();
	const u32 vertexSizeN = vbN->getVertexSize();
	const u32 vertexSizeC = vbC->getVertexSize();
	const u32 vertexSizeT = vbT->getVertexSize();
	const u32 vertexSizeB = vbB->getVertexSize();

	u8* positionData = static_cast<u8*>(vbP->getVertices());
	positionData += positionOffset;

	u8* normalData = static_cast<u8*>(vbN->getVertices());
	normalData += normalOffset;

	u8* texCoordData = static_cast<u8*>(vbC->getVertices());
	texCoordData += texCoordOffset;

	u8* tangentData = static_cast<u8*>(vbT->getVertices());
	tangentData += tangentOffset;

	u8* binormalData = static_cast<u8*>(vbB->getVertices());
	binormalData += binormalOffset;

	for (u32 i = 0; i < ib->getIndexCount(); i += 3)
	{
		normal[0] = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i));
		normal[1] = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i + 1));
		normal[2] = (core::vector3df*)(normalData + vertexSizeN * ib->getIndex(i + 2));
		tangent[0] = (core::vector3df*)(tangentData + vertexSizeT * ib->getIndex(i));
		tangent[1] = (core::vector3df*)(tangentData + vertexSizeT * ib->getIndex(i + 1));
		tangent[2] = (core::vector3df*)(tangentData + vertexSizeT * ib->getIndex(i + 2));
		binormal[0] = (core::vector3df*)(binormalData + vertexSizeB * ib->getIndex(i));
		binormal[1] = (core::vector3df*)(binormalData + vertexSizeB * ib->getIndex(i + 1));
		binormal[2] = (core::vector3df*)(binormalData + vertexSizeB * ib->getIndex(i + 2));
		position[0] = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i));
		position[1] = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 1));
		position[2] = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 2));
		texCoord[0] = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i));
		texCoord[1] = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + 1));
		texCoord[2] = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + 2));

		calculateTangents(tmpNormal[0], *(tangent[0]), *(binormal[0]),
			*(position[0]), *(position[1]), *(position[2]),
			*(texCoord[0]), *(texCoord[1]), *(texCoord[2]));

		calculateTangents(tmpNormal[1], *(tangent[1]), *(binormal[1]),
			*(position[1]), *(position[2]), *(position[0]),
			*(texCoord[1]), *(texCoord[2]), *(texCoord[0]));

		calculateTangents(tmpNormal[2], *(tangent[2]), *(binormal[2]),
			*(position[2]), *(position[0]), *(position[1]),
			*(texCoord[2]), *(texCoord[0]), *(texCoord[1]));

		if (smooth)
		{
			core::vector3df weight(1.f, 1.f, 1.f);

			if (angleWeighted)
				weight = getAngleWeight(*(position[0]), *(position[1]), *(position[2]));

			if (recalculateNormals)
			{
				tmpNormal[0] *= weight.X;
				*(normal[0]) = tmpNormal[0].normalize();
				tmpNormal[1] *= weight.Y;
				*(normal[1]) = tmpNormal[1].normalize();
				tmpNormal[2] *= weight.Z;
				*(normal[2]) = tmpNormal[2].normalize();
			}

			*(tangent[0]) *= weight.X;
			(*(tangent[0])).normalize();
			*(tangent[1]) *= weight.Y;
			(*(tangent[1])).normalize();
			*(tangent[2]) *= weight.Z;
			(*(tangent[2])).normalize();

			*(binormal[0]) *= weight.X;
			(*(binormal[0])).normalize();
			*(binormal[1]) *= weight.Y;
			(*(binormal[1])).normalize();
			*(binormal[2]) *= weight.Z;
			(*(binormal[2])).normalize();
		}
		else if (recalculateNormals)
		{
			*(normal[0]) = tmpNormal[0];
			*(normal[1]) = tmpNormal[1];
			*(normal[2]) = tmpNormal[2];
		}
	}
}

void CMeshManipulator::makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolution) const
{
	if (!meshBuffer || !meshBuffer->isVertexBufferCompatible())
		return;

	// Check Descriptor format for required 2 components.

	int Found = 0;

	video::IVertexDescriptor* vd = meshBuffer->getVertexDescriptor();

	u32 positionBufferID = 0;
	u32 positionOffset = 0;

	u32 texCoordBufferID = 0;
	u32 texCoordOffset = 0;

	for (u32 i = 0; i < vd->getAttributeCount(); ++i)
	{
		video::IVertexAttribute * attribute = vd->getAttribute(i);

		switch (attribute->getSemantic())
		{
		case video::EVAS_POSITION:
			positionBufferID = attribute->getBufferID();
			positionOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_TEXCOORD0:
			texCoordBufferID = attribute->getBufferID();
			texCoordOffset = attribute->getOffset();
			++Found;
			break;
		default:
			break;
		}
	}

	if(Found != 2)
		return;

	// Make mapping.

	core::vector3df* position0 = 0;
	core::vector3df* position1 = 0;
	core::vector3df* position2 = 0;
	core::vector2df* texCoord = 0;

	IIndexBuffer* ib = meshBuffer->getIndexBuffer();
	IVertexBuffer* vbP = meshBuffer->getVertexBuffer(positionBufferID);
	IVertexBuffer* vbC = meshBuffer->getVertexBuffer(texCoordBufferID);

	const u32 vertexSizeP = vbP->getVertexSize();
	const u32 vertexSizeC = vbC->getVertexSize();

	u8* positionData = static_cast<u8*>(vbP->getVertices());
	positionData += positionOffset;

	u8* texCoordData = static_cast<u8*>(vbC->getVertices());
	texCoordData += texCoordOffset;

	for (u32 i = 0; i < ib->getIndexCount(); i+=3)
	{
		position0 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 0));
		position1 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 1));
		position2 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + 2));

		core::plane3df plane(*position0, *position1, *position2);
		plane.Normal.X = fabsf(plane.Normal.X);
		plane.Normal.Y = fabsf(plane.Normal.Y);
		plane.Normal.Z = fabsf(plane.Normal.Z);

		// Calculate planar mapping worldspace coordinates.

		if (plane.Normal.X > plane.Normal.Y && plane.Normal.X > plane.Normal.Z)
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position0 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = (*position0).Y * resolution;
				(*texCoord).Y = (*position0).Z * resolution;
			}
		}
		else if (plane.Normal.Y > plane.Normal.X && plane.Normal.Y > plane.Normal.Z)
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position0 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = (*position0).X * resolution;
				(*texCoord).Y = (*position0).Z * resolution;
			}
		}
		else
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position0 = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = (*position0).X * resolution;
				(*texCoord).Y = (*position0).Y * resolution;
			}
		}
	}
}

void CMeshManipulator::makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset) const
{
	if (!meshBuffer || !meshBuffer->isVertexBufferCompatible())
		return;

	// Check Descriptor format for required 2 components.

	int Found = 0;

	video::IVertexDescriptor* vd = meshBuffer->getVertexDescriptor();

	u32 positionBufferID = 0;
	u32 positionOffset = 0;

	u32 texCoordBufferID = 0;
	u32 texCoordOffset = 0;

	for (u32 i = 0; i < vd->getAttributeCount(); ++i)
	{
		video::IVertexAttribute * attribute = vd->getAttribute(i);

		switch (attribute->getSemantic())
		{
		case video::EVAS_POSITION:
			positionBufferID = attribute->getBufferID();
			positionOffset = attribute->getOffset();
			++Found;
			break;
		case video::EVAS_TEXCOORD0:
			texCoordBufferID = attribute->getBufferID();
			texCoordOffset = attribute->getOffset();
			++Found;
			break;
		default:
			break;
		}
	}

	if(Found != 2)
		return;

	// Make mapping.

	core::vector3df* position = 0;
	core::vector2df* texCoord = 0;

	IIndexBuffer* ib = meshBuffer->getIndexBuffer();
	IVertexBuffer* vbP = meshBuffer->getVertexBuffer(positionBufferID);
	IVertexBuffer* vbC = meshBuffer->getVertexBuffer(texCoordBufferID);

	const u32 vertexSizeP = vbP->getVertexSize();
	const u32 vertexSizeC = vbC->getVertexSize();

	u8* positionData = static_cast<u8*>(vbP->getVertices());
	positionData += positionOffset;

	u8* texCoordData = static_cast<u8*>(vbC->getVertices());
	texCoordData += texCoordOffset;

	for (u32 i = 0; i < ib->getIndexCount(); i+=3)
	{
		// Calculate planar mapping worldspace coordinates.

		if (axis == 0)
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = 0.5f + ((*position).Z + offset.Z) * resolutionS;
				(*texCoord).Y = 0.5f - ((*position).Y + offset.Y) * resolutionT;
			}
		}
		else if (axis == 1)
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = 0.5f + ((*position).X + offset.X) * resolutionS;
				(*texCoord).Y = 1.0f - ((*position).Z + offset.Z) * resolutionT;
			}
		}
		else if (axis == 2)
		{
			for (u32 j = 0; j < 3; ++j)
			{
				position = (core::vector3df*)(positionData + vertexSizeP * ib->getIndex(i + j));
				texCoord = (core::vector2df*)(texCoordData + vertexSizeC * ib->getIndex(i + j));

				(*texCoord).X = 0.5f + ((*position).X + offset.X) * resolutionS;
				(*texCoord).Y = 0.5f - ((*position).Y + offset.Y) * resolutionT;
			}
		}
	}
}

bool CMeshManipulator::copyIndices(IIndexBuffer* srcBuffer, IIndexBuffer* dstBuffer) const
{
	if (!srcBuffer || !dstBuffer)
		return false;

	const u32 indexCount = srcBuffer->getIndexCount();

	dstBuffer->reallocate(indexCount);
	dstBuffer->set_used(0);

	for (u32 i = 0; i < indexCount; ++i)
		dstBuffer->addIndex(srcBuffer->getIndex(i));

	return true;
}

bool CMeshManipulator::copyVertices(IVertexBuffer* srcBuffer, u32 srcDescriptionBufferID, video::IVertexDescriptor* srcDescriptor,
	IVertexBuffer* dstBuffer, u32 dstDescriptionBufferID, video::IVertexDescriptor* dstDescriptor, bool copyCustomAttribute) const
{
	if (!srcBuffer || !dstBuffer || !srcDescriptor || !dstDescriptor || srcBuffer->getVertexCount() == 0)
		return false;

	bool FillBuffer = false;

	for (u32 i = 0; i < dstDescriptor->getAttributeCount(); ++i)
	{
		video::IVertexAttribute* attributeSrc = 0;
		video::IVertexAttribute* attributeDst = dstDescriptor->getAttribute(i);

		if (dstDescriptionBufferID != attributeDst->getBufferID())
			continue;

		video::E_VERTEX_ATTRIBUTE_SEMANTIC semantic = attributeDst->getSemantic();

		if (semantic != video::EVAS_CUSTOM)
			attributeSrc = srcDescriptor->getAttributeBySemantic(semantic);
		else if (copyCustomAttribute)
		{
			core::stringc name = attributeDst->getName();

			for (u32 j = 0; j < srcDescriptor->getAttributeCount(); ++j)
			{
				if (name == srcDescriptor->getAttribute(j)->getName())
				{
					attributeSrc = srcDescriptor->getAttribute(j);
					break;
				}
			}
		}

		if (!attributeSrc || srcDescriptionBufferID != attributeSrc->getBufferID() ||
			attributeDst->getElementCount() != attributeSrc->getElementCount() ||
			attributeDst->getType() != attributeSrc->getType())
		{
			attributeSrc = 0;
		}

		if (attributeSrc)
		{
			if (!FillBuffer)
			{
				dstBuffer->fill(srcBuffer->getVertexCount());
				FillBuffer = true;
			}

			const u32 elementSize = attributeSrc->getTypeSize() * attributeSrc->getElementCount();

			const u32 vertexSizeSrc = srcBuffer->getVertexSize();
			const u32 vertexSizeDst = dstBuffer->getVertexSize();

			u8* VerticesSrc = static_cast<u8*>(srcBuffer->getVertices());
			VerticesSrc += attributeSrc->getOffset();

			u8* VerticesDst = static_cast<u8*>(dstBuffer->getVertices());
			VerticesDst += attributeDst->getOffset();

			for (u32 j = 0; j < srcBuffer->getVertexCount(); ++j)
			{
				memcpy(VerticesDst, VerticesSrc, elementSize);

				VerticesSrc += vertexSizeSrc;
				VerticesDst += vertexSizeDst;
			}
		}
	}

	return true;
}

bool CMeshManipulator::createTangents(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, bool copyCustomAttribute,
	bool recalculateNormals, bool smooth, bool angleWeighted)
{
	if (!srcBuffer || !srcBuffer->isVertexBufferCompatible() || !dstBuffer || !dstBuffer->isVertexBufferCompatible())
		return false;

	// Check Descriptor format for required 5 components.

	int Found = 0;

	video::IVertexDescriptor* vd = dstBuffer->getVertexDescriptor();

	for (u32 i = 0; i < vd->getAttributeCount(); ++i)
	{
		video::IVertexAttribute * attribute = vd->getAttribute(i);

		switch (attribute->getSemantic())
		{
		case video::EVAS_POSITION:
			++Found;
			break;
		case video::EVAS_NORMAL:
			++Found;
			break;
		case video::EVAS_TEXCOORD0:
			++Found;
			break;
		case video::EVAS_TANGENT:
			++Found;
			break;
		case video::EVAS_BINORMAL:
			++Found;
			break;
		default:
			break;
		}
	}

	if (Found != 5)
		return false;

	copyIndices(srcBuffer->getIndexBuffer(), dstBuffer->getIndexBuffer());

	for (u32 i = 0; i < srcBuffer->getVertexBufferCount(); ++i)
	{
		if (i >= dstBuffer->getVertexBufferCount())
			break;

		copyVertices(srcBuffer->getVertexBuffer(i), i, srcBuffer->getVertexDescriptor(), dstBuffer->getVertexBuffer(i), i, dstBuffer->getVertexDescriptor(), copyCustomAttribute);
	}

	// Calculate tangents.

	recalculateTangents(dstBuffer, recalculateNormals, smooth, angleWeighted);

	return true;
}

bool CMeshManipulator::createUniquePrimitives(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer) const
{
	if (!srcBuffer || !srcBuffer->isVertexBufferCompatible() || !dstBuffer || !dstBuffer->isVertexBufferCompatible())
		return false;

	IIndexBuffer* srcIndexBuffer = srcBuffer->getIndexBuffer();
	IIndexBuffer* dstIndexBuffer = dstBuffer->getIndexBuffer();

	copyIndices(srcIndexBuffer, dstIndexBuffer);

	const u32 indexCount = dstIndexBuffer->getIndexCount();

	for (u32 i = 0; i < srcBuffer->getVertexBufferCount(); ++i)
	{
		if (i >= dstBuffer->getVertexBufferCount())
			break;

		IVertexBuffer* srcVertexBuffer = srcBuffer->getVertexBuffer(i);

		IVertexBuffer* dstVertexBuffer = dstBuffer->getVertexBuffer(i);
		dstVertexBuffer->reallocate(indexCount);

		for (u32 i = 0; i < indexCount; i += 3)
		{
			dstVertexBuffer->addVertex(srcVertexBuffer->getVertex(dstIndexBuffer->getIndex(i + 0)));
			dstVertexBuffer->addVertex(srcVertexBuffer->getVertex(dstIndexBuffer->getIndex(i + 1)));
			dstVertexBuffer->addVertex(srcVertexBuffer->getVertex(dstIndexBuffer->getIndex(i + 2)));

			dstIndexBuffer->setIndex(i + 0, i + 0);
			dstIndexBuffer->setIndex(i + 1, i + 1);
			dstIndexBuffer->setIndex(i + 2, i + 2);
		}
	}

	dstBuffer->recalculateBoundingBox();

	return true;
}

// TO-DO: support mesh buffers with more than 1 vertex buffer
bool CMeshManipulator::createWelded(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, f32 tolerance,
	bool check4Component, bool check3Component, bool check2Component, bool check1Component) const
{
	if (!srcBuffer || !srcBuffer->isVertexBufferCompatible() || !dstBuffer || !dstBuffer->isVertexBufferCompatible() ||
		srcBuffer->getVertexBufferCount() > 1 || dstBuffer->getVertexBufferCount() > 1)
		return false;

	IIndexBuffer* srcIndexBuffer = srcBuffer->getIndexBuffer();
	const u32 srcIndexCount = srcIndexBuffer->getIndexCount();

	IIndexBuffer* dstIndexBuffer = dstBuffer->getIndexBuffer();
	dstIndexBuffer->set_used(srcIndexCount);

	IVertexBuffer* srcVertexBuffer = srcBuffer->getVertexBuffer(0);
	const u32 srcVertexCount = srcVertexBuffer->getVertexCount();

	IVertexBuffer* dstVertexBuffer = dstBuffer->getVertexBuffer();
	dstVertexBuffer->reallocate(srcVertexCount);

	core::array<u32> Redirects;
	Redirects.set_used(srcVertexCount);

	u8* Vertices = static_cast<u8*>(srcVertexBuffer->getVertices());

	// Create indices.

	bool checkComponents[4] =
	{
		check1Component,
		check2Component,
		check3Component,
		check4Component
	};

	video::IVertexDescriptor* vd = srcBuffer->getVertexDescriptor();

	const u32 vertexSize = srcVertexBuffer->getVertexSize();

	for (u32 i = 0; i < srcVertexCount; ++i)
	{
		bool found = false;

		for (u32 j = 0; j < i; ++j)
		{
			bool Equal = true;
			bool Compare = false;

			for (u32 l = 0; l < vd->getAttributeCount() && Equal; ++l)
			{
				video::IVertexAttribute* attribute = vd->getAttribute(l);

				u32 ElementCount = attribute->getElementCount();

				if (ElementCount > 4)
					continue;

				const u32 AttributeOffset = attribute->getOffset();

				switch (attribute->getType())
				{
				case video::EVAT_BYTE:
					{
						s8* valueA = (s8*)(Vertices + AttributeOffset + vertexSize * i);
						s8* valueB = (s8*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((s32)(valueA[k]), (s32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_UBYTE:
					{
						u8* valueA = (u8*)(Vertices + AttributeOffset + vertexSize * i);
						u8* valueB = (u8*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((u32)(valueA[k]), (u32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_SHORT:
					{
						s16* valueA = (s16*)(Vertices + AttributeOffset + vertexSize * i);
						s16* valueB = (s16*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((s32)(valueA[k]), (s32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_USHORT:
					{
						u16* valueA = (u16*)(Vertices + AttributeOffset + vertexSize * i);
						u16* valueB = (u16*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((u32)(valueA[k]), (u32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_INT:
					{
						s32* valueA = (s32*)(Vertices + AttributeOffset + vertexSize * i);
						s32* valueB = (s32*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((s32)(valueA[k]), (s32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_UINT:
					{
						u32* valueA = (u32*)(Vertices + AttributeOffset + vertexSize * i);
						u32* valueB = (u32*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((u32)(valueA[k]), (u32)(valueB[k]), (s32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_FLOAT:
					{
						f32* valueA = (f32*)(Vertices + AttributeOffset + vertexSize * i);
						f32* valueB = (f32*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((f32)(valueA[k]), (f32)(valueB[k]), (f32)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				case video::EVAT_DOUBLE:
					{
						f64* valueA = (f64*)(Vertices + AttributeOffset + vertexSize * i);
						f64* valueB = (f64*)(Vertices + AttributeOffset + vertexSize * j);

						if (checkComponents[ElementCount - 1])
						{
							for (u32 k = 0; k < ElementCount; ++k)
							{
								if (!core::equals((f64)(valueA[k]), (f64)(valueB[k]), (f64)tolerance))
									Equal = false;
							}

							Compare = true;
						}
					}
					break;
				default:
					break;
				}
			}

			if (Equal && Compare)
			{
				Redirects[i] = Redirects[j];
				found = true;
				break;
			}
		}

		if (!found)
		{
			Redirects[i] = srcVertexCount;
			dstVertexBuffer->addVertex(Vertices + vertexSize * i);
		}
	}

	for (u32 i = 0; i < srcIndexCount; ++i)
		dstIndexBuffer->setIndex(i, Redirects[srcIndexBuffer->getIndex(i)]);

	dstBuffer->recalculateBoundingBox();

	return true;
}

s32 CMeshManipulator::getPolyCount(IMesh* mesh) const
{
	if (!mesh)
		return 0;

	s32 trianglecount = 0;
	const u32 bcount = mesh->getMeshBufferCount();

	for (u32 g=0; g<bcount; ++g)
		trianglecount += mesh->getMeshBuffer(g)->getIndexBuffer()->getIndexCount() / 3;

	return trianglecount;
}

namespace
{

struct vcache
{
	core::array<u32> tris;
	float score;
	s16 cachepos;
	u16 NumActiveTris;
};

struct tcache
{
	u16 ind[3];
	float score;
	bool drawn;
};

const u16 cachesize = 32;

float FindVertexScore(vcache *v)
{
	const float CacheDecayPower = 1.5f;
	const float LastTriScore = 0.75f;
	const float ValenceBoostScale = 2.0f;
	const float ValenceBoostPower = 0.5f;
	const float MaxSizeVertexCache = 32.0f;

	if (v->NumActiveTris == 0)
	{
		// No tri needs this vertex!
		return -1.0f;
	}

	float Score = 0.0f;
	int CachePosition = v->cachepos;
	if (CachePosition < 0)
	{
		// Vertex is not in FIFO cache - no score.
	}
	else
	{
		if (CachePosition < 3)
		{
			// This vertex was used in the last triangle,
			// so it has a fixed score.
			Score = LastTriScore;
		}
		else
		{
			// Points for being high in the cache.
			const float Scaler = 1.0f / (MaxSizeVertexCache - 3);
			Score = 1.0f - (CachePosition - 3) * Scaler;
			Score = powf(Score, CacheDecayPower);
		}
	}

	// Bonus points for having a low number of tris still to
	// use the vert, so we get rid of lone verts quickly.
	float ValenceBoost = powf(v->NumActiveTris,
				-ValenceBoostPower);
	Score += ValenceBoostScale * ValenceBoost;

	return Score;
}

/*
	A specialized LRU cache for the Forsyth algorithm.
*/

class f_lru
{

public:
	f_lru(vcache *v, tcache *t): vc(v), tc(t)
	{
		for (u16 i = 0; i < cachesize; i++)
		{
			cache[i] = -1;
		}
	}

	// Adds this vertex index and returns the highest-scoring triangle index
	u32 add(u16 vert, bool updatetris = false)
	{
		bool found = false;

		// Mark existing pos as empty
		for (u16 i = 0; i < cachesize; i++)
		{
			if (cache[i] == vert)
			{
				// Move everything down
				for (u16 j = i; j; j--)
				{
					cache[j] = cache[j - 1];
				}

				found = true;
				break;
			}
		}

		if (!found)
		{
			if (cache[cachesize-1] != -1)
				vc[cache[cachesize-1]].cachepos = -1;

			// Move everything down
			for (u16 i = cachesize - 1; i; i--)
			{
				cache[i] = cache[i - 1];
			}
		}

		cache[0] = vert;

		u32 highest = 0;
		float hiscore = 0;

		if (updatetris)
		{
			// Update cache positions
			for (u16 i = 0; i < cachesize; i++)
			{
				if (cache[i] == -1)
					break;

				vc[cache[i]].cachepos = i;
				vc[cache[i]].score = FindVertexScore(&vc[cache[i]]);
			}

			// Update triangle scores
			for (u16 i = 0; i < cachesize; i++)
			{
				if (cache[i] == -1)
					break;

				const u16 trisize = vc[cache[i]].tris.size();
				for (u16 t = 0; t < trisize; t++)
				{
					tcache *tri = &tc[vc[cache[i]].tris[t]];

					tri->score =
						vc[tri->ind[0]].score +
						vc[tri->ind[1]].score +
						vc[tri->ind[2]].score;

					if (tri->score > hiscore)
					{
						hiscore = tri->score;
						highest = vc[cache[i]].tris[t];
					}
				}
			}
		}

		return highest;
	}

private:
	s32 cache[cachesize];
	vcache *vc;
	tcache *tc;
};

} // end anonymous namespace

/**
Vertex cache optimization according to the Forsyth paper:
http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html

The function is thread-safe (read: you can optimize several meshes in different threads)

\param mesh Source mesh for the operation.  */
IMesh* CMeshManipulator::createForsythOptimizedMesh(const IMesh* mesh) const
{
	if (!mesh)
		return 0;

	const u32 mbcount = mesh->getMeshBufferCount();

	// TO-DO: support mesh buffers with more than 1 vertex buffer
	for (u32 b = 0; b < mbcount; ++b)
		if (mesh->getMeshBuffer(b)->getVertexBufferCount() > 1)
			return 0;

	SMesh *newmesh = new SMesh();
	newmesh->BoundingBox = mesh->getBoundingBox();

	for (u32 b = 0; b < mbcount; ++b)
	{
		const IMeshBuffer *mb = mesh->getMeshBuffer(b);

		if (mb->getIndexBuffer()->getType() != video::EIT_16BIT)
		{
			os::Printer::log("Cannot optimize a mesh with 32bit indices", ELL_ERROR);
			newmesh->drop();
			return 0;
		}

		const u32 icount = mb->getIndexBuffer()->getIndexCount();
		const u32 tcount = icount / 3;
		const u32 vcount = mb->getVertexBuffer(0)->getVertexCount();
		const u16 *ind = (u16*)mb->getIndexBuffer()->getIndices();

		vcache *vc = new vcache[vcount];
		tcache *tc = new tcache[tcount];

		f_lru lru(vc, tc);

		// init
		for (u16 i = 0; i < vcount; i++)
		{
			vc[i].score = 0;
			vc[i].cachepos = -1;
			vc[i].NumActiveTris = 0;
		}

		// First pass: count how many times a vert is used
		for (u32 i = 0; i < icount; i += 3)
		{
			vc[ind[i]].NumActiveTris++;
			vc[ind[i + 1]].NumActiveTris++;
			vc[ind[i + 2]].NumActiveTris++;

			const u32 tri_ind = i/3;
			tc[tri_ind].ind[0] = ind[i];
			tc[tri_ind].ind[1] = ind[i + 1];
			tc[tri_ind].ind[2] = ind[i + 2];
		}

		// Second pass: list of each triangle
		for (u32 i = 0; i < tcount; i++)
		{
			vc[tc[i].ind[0]].tris.push_back(i);
			vc[tc[i].ind[1]].tris.push_back(i);
			vc[tc[i].ind[2]].tris.push_back(i);

			tc[i].drawn = false;
		}

		// Give initial scores
		for (u16 i = 0; i < vcount; i++)
		{
			vc[i].score = FindVertexScore(&vc[i]);
		}
		for (u32 i = 0; i < tcount; i++)
		{
			tc[i].score =
					vc[tc[i].ind[0]].score +
					vc[tc[i].ind[1]].score +
					vc[tc[i].ind[2]].score;
		}

		switch(mb->getVertexBuffer(0)->getVertexSize())
		{
		case sizeof(video::S3DVertex):
			{
				video::S3DVertex *v = (video::S3DVertex *) mb->getVertexBuffer(0)->getVertices();

				CMeshBuffer<video::S3DVertex> *buf = new CMeshBuffer<video::S3DVertex>(mb->getVertexDescriptor(), video::EIT_16BIT);
				buf->getMaterial() = mb->getMaterial();

				buf->getVertexBuffer(0)->reallocate(vcount);
				buf->getIndexBuffer()->reallocate(icount);

				core::map<const video::S3DVertex, const u16> sind; // search index for fast operation
				typedef core::map<const video::S3DVertex, const u16>::Node snode;

				// Main algorithm
				u32 highest = 0;
				u32 drawcalls = 0;
				for (;;)
				{
					if (tc[highest].drawn)
					{
						bool found = false;
						float hiscore = 0;
						for (u32 t = 0; t < tcount; t++)
						{
							if (!tc[t].drawn)
							{
								if (tc[t].score > hiscore)
								{
									highest = t;
									hiscore = tc[t].score;
									found = true;
								}
							}
						}
						if (!found)
							break;
					}

					// Output the best triangle
					u16 newind = buf->getVertexBuffer(0)->getVertexCount();

					snode *s = sind.find(v[tc[highest].ind[0]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[0]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[0]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[1]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[1]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[1]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[2]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[2]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[2]], newind);
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					vc[tc[highest].ind[0]].NumActiveTris--;
					vc[tc[highest].ind[1]].NumActiveTris--;
					vc[tc[highest].ind[2]].NumActiveTris--;

					tc[highest].drawn = true;

					for (u16 j = 0; j < 3; j++)
					{
						vcache *vert = &vc[tc[highest].ind[j]];
						for (u16 t = 0; t < vert->tris.size(); t++)
						{
							if (highest == vert->tris[t])
							{
								vert->tris.erase(t);
								break;
							}
						}
					}

					lru.add(tc[highest].ind[0]);
					lru.add(tc[highest].ind[1]);
					highest = lru.add(tc[highest].ind[2], true);
					drawcalls++;
				}

				buf->getBoundingBox() = mb->getBoundingBox();
				newmesh->addMeshBuffer(buf);
				buf->drop();
			}
			break;
		case sizeof(video::S3DVertex2TCoords):
			{
				video::S3DVertex2TCoords *v = (video::S3DVertex2TCoords *) mb->getVertexBuffer(0)->getVertices();

				CMeshBuffer<video::S3DVertex2TCoords> *buf = new CMeshBuffer<video::S3DVertex2TCoords>(mb->getVertexDescriptor(), video::EIT_16BIT);
				buf->getMaterial() = mb->getMaterial();

				buf->getVertexBuffer(0)->reallocate(vcount);
				buf->getIndexBuffer()->reallocate(icount);

				core::map<const video::S3DVertex2TCoords, const u16> sind; // search index for fast operation
				typedef core::map<const video::S3DVertex2TCoords, const u16>::Node snode;

				// Main algorithm
				u32 highest = 0;
				u32 drawcalls = 0;
				for (;;)
				{
					if (tc[highest].drawn)
					{
						bool found = false;
						float hiscore = 0;
						for (u32 t = 0; t < tcount; t++)
						{
							if (!tc[t].drawn)
							{
								if (tc[t].score > hiscore)
								{
									highest = t;
									hiscore = tc[t].score;
									found = true;
								}
							}
						}
						if (!found)
							break;
					}

					// Output the best triangle
					u16 newind = buf->getVertexBuffer(0)->getVertexCount();

					snode *s = sind.find(v[tc[highest].ind[0]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[0]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[0]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[1]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[1]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[1]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[2]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[2]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[2]], newind);
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					vc[tc[highest].ind[0]].NumActiveTris--;
					vc[tc[highest].ind[1]].NumActiveTris--;
					vc[tc[highest].ind[2]].NumActiveTris--;

					tc[highest].drawn = true;

					for (u16 j = 0; j < 3; j++)
					{
						vcache *vert = &vc[tc[highest].ind[j]];
						for (u16 t = 0; t < vert->tris.size(); t++)
						{
							if (highest == vert->tris[t])
							{
								vert->tris.erase(t);
								break;
							}
						}
					}

					lru.add(tc[highest].ind[0]);
					lru.add(tc[highest].ind[1]);
					highest = lru.add(tc[highest].ind[2]);
					drawcalls++;
				}

				buf->getBoundingBox() = mb->getBoundingBox();
				newmesh->addMeshBuffer(buf);
				buf->drop();

			}
			break;
		case sizeof(video::S3DVertexTangents):
			{
				video::S3DVertexTangents *v = (video::S3DVertexTangents *) mb->getVertexBuffer(0)->getVertices();

				CMeshBuffer<video::S3DVertexTangents> *buf = new CMeshBuffer<video::S3DVertexTangents>(mb->getVertexDescriptor(), video::EIT_16BIT);
				buf->getMaterial() = mb->getMaterial();

				buf->getVertexBuffer(0)->reallocate(vcount);
				buf->getIndexBuffer()->reallocate(icount);

				core::map<const video::S3DVertexTangents, const u16> sind; // search index for fast operation
				typedef core::map<const video::S3DVertexTangents, const u16>::Node snode;

				// Main algorithm
				u32 highest = 0;
				u32 drawcalls = 0;
				for (;;)
				{
					if (tc[highest].drawn)
					{
						bool found = false;
						float hiscore = 0;
						for (u32 t = 0; t < tcount; t++)
						{
							if (!tc[t].drawn)
							{
								if (tc[t].score > hiscore)
								{
									highest = t;
									hiscore = tc[t].score;
									found = true;
								}
							}
						}
						if (!found)
							break;
					}

					// Output the best triangle
					u16 newind = buf->getVertexBuffer(0)->getVertexCount();

					snode *s = sind.find(v[tc[highest].ind[0]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[0]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[0]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[1]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[1]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[1]], newind);
						newind++;
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					s = sind.find(v[tc[highest].ind[2]]);

					if (!s)
					{
						buf->getVertexBuffer(0)->addVertex(&v[tc[highest].ind[2]]);
						buf->getIndexBuffer()->addIndex(newind);
						sind.insert(v[tc[highest].ind[2]], newind);
					}
					else
					{
						buf->getIndexBuffer()->addIndex(s->getValue());
					}

					vc[tc[highest].ind[0]].NumActiveTris--;
					vc[tc[highest].ind[1]].NumActiveTris--;
					vc[tc[highest].ind[2]].NumActiveTris--;

					tc[highest].drawn = true;

					for (u16 j = 0; j < 3; j++)
					{
						vcache *vert = &vc[tc[highest].ind[j]];
						for (u16 t = 0; t < vert->tris.size(); t++)
						{
							if (highest == vert->tris[t])
							{
								vert->tris.erase(t);
								break;
							}
						}
					}

					lru.add(tc[highest].ind[0]);
					lru.add(tc[highest].ind[1]);
					highest = lru.add(tc[highest].ind[2]);
					drawcalls++;
				}

				buf->getBoundingBox() = mb->getBoundingBox();
				newmesh->addMeshBuffer(buf);
				buf->drop();
			}
			break;
		}

		delete [] vc;
		delete [] tc;

	} // for each meshbuffer

	return newmesh;
}

} // end namespace scene
} // end namespace irr

