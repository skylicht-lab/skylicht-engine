// Copyright (C) 2012-2016 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "CVertexDescriptor.h"

namespace irr
{
namespace video
{

	CVertexAttribute::CVertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID) :
		IVertexAttribute(name, elementCount, semantic, type, offset, bufferID)
	{
	}

	void CVertexAttribute::setOffset(u32 offset)
	{
		Offset = offset;
	}

	CVertexDescriptor::CVertexDescriptor(const core::stringc& name, u32 id) :
		IVertexDescriptor(name, id)
	{
#ifdef _DEBUG
		setDebugName("CVertexDescriptor");
#endif
	}

	void CVertexDescriptor::setID(u32 id)
	{
		ID = id;
	}

	IVertexAttribute* CVertexDescriptor::addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID)
	{
		for (u32 i = 0; i < Attribute.size(); ++i)
			if (name == Attribute[i].getName() || (semantic != EVAS_CUSTOM && semantic == Attribute[i].getSemantic()))
				return &Attribute[i];

		if (elementCount < 1)
			elementCount = 1;

		if (elementCount > 4)
			elementCount = 4;

		for (u32 i = VertexSize.size(); i <= bufferID; ++i)
			VertexSize.push_back(0);

		for (u32 i = InstanceDataStepRate.size(); i <= bufferID; ++i)
			InstanceDataStepRate.push_back(EIDSR_PER_VERTEX);

		CVertexAttribute attribute(name, elementCount, semantic, type, VertexSize[bufferID], bufferID);
		Attribute.push_back(attribute);

		AttributeSemanticIndex[(u32)attribute.getSemantic()] = Attribute.size() - 1;

		VertexSize[bufferID] += attribute.getTypeSize() * attribute.getElementCount();

		// Assign data to the pointers.

		AttributePointer.push_back(0);

		for (u32 i = 0; i < AttributePointer.size(); ++i)
			AttributePointer[i] = &Attribute[i];

		return AttributePointer.getLast();
	}

	void CVertexDescriptor::clearAttribute()
	{
		AttributePointer.clear();
		VertexSize.clear();

		for(u32 i = 0; i < EVAS_COUNT; ++i)
			AttributeSemanticIndex[i] = -1;

		Attribute.clear();
	}

}
}
