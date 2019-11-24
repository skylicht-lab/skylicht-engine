// Copyright (C) 2012-2016 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VERTEX_DESCRIPTOR_H_INCLUDED__
#define __C_VERTEX_DESCRIPTOR_H_INCLUDED__

#include "IVertexDescriptor.h"

namespace irr
{
namespace video
{
	class CVertexAttribute : public IVertexAttribute
	{
	public:
		CVertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID);

		virtual void setOffset(u32 offset);
	};

	class CVertexDescriptor : public IVertexDescriptor
	{
	public:
		CVertexDescriptor(const core::stringc& name, u32 id);

		virtual void setID(u32 id);

		virtual IVertexAttribute* addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID) _IRR_OVERRIDE_;

		virtual void clearAttribute() _IRR_OVERRIDE_;

	protected:
		core::array<CVertexAttribute> Attribute;
	};

}
}

#endif
