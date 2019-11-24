#ifndef __C_DIRECTX11_VERTEX_DESCRIPTOR_H_INCLUDED__
#define __C_DIRECTX11_VERTEX_DESCRIPTOR_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CVertexDescriptor.h"

#include <dxgiformat.h>
#include <wtypes.h>

struct ID3D11InputLayout;
struct ID3D11Device;
struct D3D11_INPUT_ELEMENT_DESC;

namespace irr
{
namespace video
{

class CD3D11VertexDescriptor : public CVertexDescriptor
{
public:
	CD3D11VertexDescriptor(ID3D11Device* device, const core::stringc& name, u32 id);
	virtual ~CD3D11VertexDescriptor();

	virtual IVertexAttribute* addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID) _IRR_OVERRIDE_;

	virtual void clearAttribute() _IRR_OVERRIDE_;

	virtual core::array<D3D11_INPUT_ELEMENT_DESC>& getInputLayoutDescription();

	virtual	void clear();

	void rebuild();

protected:
	//! Parse semantic
	const c8* getSemanticName(E_VERTEX_ATTRIBUTE_SEMANTIC semantic) const;

	DXGI_FORMAT getFormat(E_VERTEX_ATTRIBUTE_TYPE type, u32 count) const;

	ID3D11Device* Device;

	core::array<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;

	u32 SemanticIndex[EVAS_COUNT];
};

}
}
#endif
#endif
#endif