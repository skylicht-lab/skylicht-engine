#include "pch.h"
#include "CD3D11VertexDescriptor.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "irrOS.h"

#include <d3d11.h>

namespace irr
{
namespace video
{

CD3D11VertexDescriptor::CD3D11VertexDescriptor(ID3D11Device* device, const core::stringc& name, u32 id)
	: CVertexDescriptor(name, id), Device(device)
{
#ifdef _DEBUG
	setDebugName("CD3D11VertexDescriptor");
#endif

	if (Device)
	{
		Device->AddRef();
	}

	clear();
}

CD3D11VertexDescriptor::~CD3D11VertexDescriptor()
{
	clear();

	if (Device)
		Device->Release();
}

IVertexAttribute* CD3D11VertexDescriptor::addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID)
{
	IVertexAttribute* attribute = CVertexDescriptor::addAttribute(name, elementCount, semantic, type, bufferID);

	if (attribute != 0)
		clear();

	return attribute;
}

void CD3D11VertexDescriptor::clearAttribute()
{
	CVertexDescriptor::clearAttribute();

	clear();
}

core::array<D3D11_INPUT_ELEMENT_DESC>& CD3D11VertexDescriptor::getInputLayoutDescription()
{
	if (InputLayoutDesc.empty())
		rebuild();

	return InputLayoutDesc;
}

void CD3D11VertexDescriptor::rebuild()
{
	// if don't exists, create layout
	D3D11_INPUT_ELEMENT_DESC desc;

	const u32 size = Attribute.size();

	for(u32 i = 0; i < size; ++i)
	{
		desc.SemanticName = getSemanticName(Attribute[i].getSemantic());

		u32 index = 0;
		switch (Attribute[i].getSemantic())
		{
		case EVAS_TEXCOORD0:
		case EVAS_TEXCOORD1:
		case EVAS_TEXCOORD2:
		case EVAS_TEXCOORD3:
		case EVAS_TEXCOORD4:
		case EVAS_TEXCOORD5:
		case EVAS_TEXCOORD6:
		case EVAS_TEXCOORD7:
			index = SemanticIndex[EVAS_TEXCOORD0];
			break;
		default:
			index = SemanticIndex[Attribute[i].getSemantic()];
		}

		desc.SemanticIndex = index;
		desc.Format = getFormat(Attribute[i].getType(), Attribute[i].getElementCount());
		desc.InputSlot = Attribute[i].getBufferID();
		desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		if (getInstanceDataStepRate(Attribute[i].getBufferID()) == 0 )
		{
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
		}
		else
		{
			desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			desc.InstanceDataStepRate = getInstanceDataStepRate(Attribute[i].getBufferID());
		}

		InputLayoutDesc.push_back(desc);

		switch (Attribute[i].getSemantic())
		{
		case EVAS_TEXCOORD0:
		case EVAS_TEXCOORD1:
		case EVAS_TEXCOORD2:
		case EVAS_TEXCOORD3:
		case EVAS_TEXCOORD4:
		case EVAS_TEXCOORD5:
		case EVAS_TEXCOORD6:
		case EVAS_TEXCOORD7:
			++SemanticIndex[EVAS_TEXCOORD0];
			break;
		default:
			++SemanticIndex[Attribute[i].getSemantic()];
		}

	}
}

const c8* CD3D11VertexDescriptor::getSemanticName(E_VERTEX_ATTRIBUTE_SEMANTIC semantic) const
{
	switch (semantic)
	{
	case EVAS_POSITION:
		return "POSITION";
	case EVAS_NORMAL:
		return "NORMAL";
	case EVAS_COLOR:
		return "COLOR";
	case EVAS_TEXCOORD0:
	case EVAS_TEXCOORD1:
	case EVAS_TEXCOORD2:
	case EVAS_TEXCOORD3:
	case EVAS_TEXCOORD4:
	case EVAS_TEXCOORD5:
	case EVAS_TEXCOORD6:
	case EVAS_TEXCOORD7:
		return "TEXCOORD";
	case EVAS_TANGENT:
		return "TANGENT";
	case EVAS_BINORMAL:
		return "BINORMAL";
	case EVAS_BLEND_WEIGHTS:
		return "BLENDWEIGHT";
	case EVAS_BLEND_INDICES:
		return "BLENDINDICES";
	case EVAS_LIGHTPROBE:
		return "LIGHTPROBE";
	case EVAS_TANGENTW:
		return "TANGENTW";
	case EVAS_LIGHTMAP:
		return "LIGHTMAP";
	case EVAS_CUSTOM:
		return "CUSTOM";
	default:
		return "POSITION";
	}
}

DXGI_FORMAT CD3D11VertexDescriptor::getFormat(E_VERTEX_ATTRIBUTE_TYPE type, u32 count) const
{
	switch (type)
	{
	case EVAT_BYTE:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R8_SNORM;
		case 2:
			return DXGI_FORMAT_R8G8_SNORM;
		case 4:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}	
	case EVAT_UBYTE:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R8_UNORM;
		case 2:
			return DXGI_FORMAT_R8G8_UNORM;
		case 4:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}	
	case EVAT_SHORT:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R16_SINT;
		case 2:
			return DXGI_FORMAT_R16G16_SINT;
		case 4:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	case EVAT_USHORT:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R16_UINT;
		case 2:
			return DXGI_FORMAT_R16G16_UINT;
		case 4:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	case EVAT_INT:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R32_SINT;
		case 2:
			return DXGI_FORMAT_R32G32_SINT;
		case 3:
			return DXGI_FORMAT_R32G32B32_SINT;
		case 4:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	case EVAT_UINT:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R32_UINT;
		case 2:
			return DXGI_FORMAT_R32G32_UINT;
		case 3:
			return DXGI_FORMAT_R32G32B32_UINT;
		case 4:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	case EVAT_DOUBLE:
	case EVAT_FLOAT:
		switch (count)
		{
		case 1:
			return DXGI_FORMAT_R32_FLOAT;
		case 2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case 3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case 4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

void CD3D11VertexDescriptor::clear()
{
	InputLayoutDesc.clear();

	for(u32 i = 0; i < EVAS_COUNT; ++i)
		SemanticIndex[i] = 0;
}

}
}
#endif