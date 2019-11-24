// Copyright (C) 2012-2016 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_VERTEX_DESCRIPTOR_H_INCLUDED__
#define __I_VERTEX_DESCRIPTOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrArray.h"
#include "irrString.h"

namespace irr
{
namespace video
{
	// Remember declare Semantic Name (CD3D11VertexDescriptor)
	// getSemanticName
	enum E_VERTEX_ATTRIBUTE_SEMANTIC
	{
		EVAS_POSITION = 0,
		EVAS_NORMAL,
		EVAS_COLOR,
		EVAS_TEXCOORD0,
		EVAS_TEXCOORD1,
		EVAS_TEXCOORD2,
		EVAS_TEXCOORD3,
		EVAS_TEXCOORD4,
		EVAS_TEXCOORD5,
		EVAS_TEXCOORD6,
		EVAS_TEXCOORD7,
		EVAS_TANGENT,
		EVAS_BINORMAL,
		EVAS_BLEND_WEIGHTS,
		EVAS_BLEND_INDICES,
		EVAS_LIGHTPROBE,
		EVAS_TANGENTW,
		EVAS_CUSTOM,

		EVAS_COUNT
	};

	enum E_VERTEX_ATTRIBUTE_TYPE
	{
		EVAT_BYTE = 0,
		EVAT_UBYTE,
		EVAT_SHORT,
		EVAT_USHORT,
		EVAT_INT,
		EVAT_UINT,
		EVAT_FLOAT,
		EVAT_DOUBLE
	};

	enum E_INSTANCE_DATA_STEP_RATE
	{
		EIDSR_PER_VERTEX,
		EIDSR_PER_INSTANCE
	};

	class IVertexAttribute
	{
	public:
		IVertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID) :
			Name(name), ElementCount(elementCount), Semantic(semantic), Type(type), Offset(offset), BufferID(bufferID)
		{
			switch (Type)
			{
			case EVAT_BYTE:
			case EVAT_UBYTE:
				TypeSize = sizeof(u8);
				break;
			case EVAT_SHORT:
			case EVAT_USHORT:
				TypeSize = sizeof(u16);
				break;
			case EVAT_INT:
			case EVAT_UINT:
				TypeSize = sizeof(u32);
				break;
			case EVAT_FLOAT:
				TypeSize = sizeof(f32);
				break;
			case EVAT_DOUBLE:
				TypeSize = sizeof(f64);
				break;
			}
		}

		virtual ~IVertexAttribute()
		{
		}

		const core::stringc& getName() const
		{
			return Name;
		}

		u32 getElementCount() const
		{
			return ElementCount;
		}

		E_VERTEX_ATTRIBUTE_SEMANTIC getSemantic() const
		{
			return Semantic;
		}

		E_VERTEX_ATTRIBUTE_TYPE getType() const
		{
			return Type;
		}

		u32 getTypeSize() const
		{
			return TypeSize;
		}

		u32 getOffset() const
		{
			return Offset;
		}

		u32 getBufferID() const
		{
			return BufferID;
		}

	protected:
		core::stringc Name;

		u32 ElementCount;

		E_VERTEX_ATTRIBUTE_SEMANTIC Semantic;

		E_VERTEX_ATTRIBUTE_TYPE Type;

		u32 TypeSize;

		u32 Offset;

		u32 BufferID;
	};

	class IVertexDescriptor : public virtual IReferenceCounted
	{
	public:
		IVertexDescriptor(const core::stringc& name, u32 id) : ID(id), Name(name)
		{
#ifdef _DEBUG
			setDebugName("IVertexDescriptor");
#endif

			for (u32 i = 0; i < EVAS_COUNT; ++i)
				AttributeSemanticIndex[i] = -1;
		}

		virtual ~IVertexDescriptor()
		{
		}

		u32 getID() const
		{
			return ID;
		}

		const core::stringc& getName() const
		{
			return Name;
		}

		u32 getVertexSize(u32 bufferID) const
		{
			_IRR_DEBUG_BREAK_IF(bufferID >= VertexSize.size())

			return VertexSize[bufferID];
		}

		E_INSTANCE_DATA_STEP_RATE getInstanceDataStepRate(u32 bufferID) const
		{
			_IRR_DEBUG_BREAK_IF(bufferID >= InstanceDataStepRate.size())

			return InstanceDataStepRate[bufferID];
		}

		void setInstanceDataStepRate(E_INSTANCE_DATA_STEP_RATE rate, u32 bufferID)
		{
			_IRR_DEBUG_BREAK_IF(bufferID >= InstanceDataStepRate.size())

			InstanceDataStepRate[bufferID] = rate;
		}

		virtual IVertexAttribute* addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID) = 0;

		virtual void clearAttribute() = 0;

		IVertexAttribute* getAttribute(u32 id) const
		{
			_IRR_DEBUG_BREAK_IF(id >= AttributePointer.size())

			return AttributePointer[id];
		}

		IVertexAttribute* getAttributeByName(const core::stringc& name) const
		{
			for (u32 i = 0; i < AttributePointer.size(); ++i)
				if (name == AttributePointer[i]->getName())
					return AttributePointer[i];

			return 0;
		}

		IVertexAttribute* getAttributeBySemantic(E_VERTEX_ATTRIBUTE_SEMANTIC semantic) const
		{
			IVertexAttribute* attribute = 0;

			s32 ID = AttributeSemanticIndex[(u32)semantic];

			if (ID >= 0)
				attribute = AttributePointer[ID];

			return attribute;
		}

		u32 getAttributeCount() const
		{
			return AttributePointer.size();
		}

	protected:
		u32 ID;

		core::stringc Name;

		core::array<u32> VertexSize;
		core::array<E_INSTANCE_DATA_STEP_RATE> InstanceDataStepRate;

		s32 AttributeSemanticIndex[(u32)EVAS_COUNT];

		core::array<IVertexAttribute*> AttributePointer;
	};
}
}

#endif
