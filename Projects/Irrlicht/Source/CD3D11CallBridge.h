#ifndef __C_DIRECTX11_CALLBRIDGE_H_INCLUDED__
#define __C_DIRECTX11_CALLBRIDGE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CD3D11MaterialRenderer.h"
#include "irrMap.h"

struct ID3D11DeviceContext;
struct ID3D11Device;

namespace irr
{
namespace video
{
struct SD3D11_BLEND_DESC : public D3D11_BLEND_DESC
{
	SD3D11_BLEND_DESC()
	{
		reset();
	}

	inline bool operator==(const SD3D11_BLEND_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC) ) == 0;
	}

	inline bool operator!=(const SD3D11_BLEND_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC) ) != 0;
	}

	inline bool operator<(const SD3D11_BLEND_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC) ) < 0;
	}

	inline void reset()
	{
		AlphaToCoverageEnable = false;
		IndependentBlendEnable = false;
		RenderTarget[0].BlendEnable = false;
		RenderTarget[0].SrcBlend	= D3D11_BLEND_ONE;
		RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;		
		RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		memcpy( &RenderTarget[1], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[2], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[3], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[4], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[5], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[6], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		memcpy( &RenderTarget[7], &RenderTarget[0], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
	}
};

// Rasterizer
struct SD3D11_RASTERIZER_DESC : public D3D11_RASTERIZER_DESC
{
	SD3D11_RASTERIZER_DESC()
	{
		reset();
	}

	inline bool operator==(const SD3D11_RASTERIZER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC) ) == 0;
	}

	inline bool operator!=(const SD3D11_RASTERIZER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC) ) != 0;
	}

	inline bool operator<(const SD3D11_RASTERIZER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC) ) < 0;
	}

	inline void reset()
	{
		FillMode = D3D11_FILL_SOLID;
		CullMode = D3D11_CULL_BACK;
		FrontCounterClockwise = false;
		DepthBias = 0;
		DepthBiasClamp = 0;
		SlopeScaledDepthBias = 0;
		DepthClipEnable = true;
		ScissorEnable = false;
		MultisampleEnable = false;
		AntialiasedLineEnable = false;
	}
};

// Depth stencil
struct SD3D11_DEPTH_STENCIL_DESC : public D3D11_DEPTH_STENCIL_DESC
{
	SD3D11_DEPTH_STENCIL_DESC()
	{
		reset();
	}

	inline bool operator==(const SD3D11_DEPTH_STENCIL_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC) ) == 0;
	}

	inline bool operator!=(const SD3D11_DEPTH_STENCIL_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC) ) != 0;
	}

	inline bool operator<(const SD3D11_DEPTH_STENCIL_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC) ) < 0;
	}

	inline void reset()
	{
		DepthEnable = true;
		DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthFunc = D3D11_COMPARISON_LESS;
		StencilEnable = false;
		StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}
};

// Samplers
struct SD3D11_SAMPLER_DESC : public D3D11_SAMPLER_DESC
{
	SD3D11_SAMPLER_DESC()
	{
		reset();
	}

	inline bool operator==(const SD3D11_SAMPLER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC) ) == 0;
	}

	inline bool operator!=(const SD3D11_SAMPLER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC) ) != 0;
	}

	inline bool operator<(const SD3D11_SAMPLER_DESC& other) const
	{
		return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC) ) < 0;
	}

	inline void reset()
	{
		Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		MipLODBias = 0;
		MaxAnisotropy = 16;
		ComparisonFunc = D3D11_COMPARISON_NEVER;
		MinLOD = 0.0f;
		MaxLOD = D3D11_FLOAT32_MAX;
	}
};	

class CD3D11VertexDeclaration;
class CD3D11Driver;
class IVertexDescriptor;

//! This bridge between Irlicht pseudo DX calls and true DX calls.
class CD3D11CallBridge
{
public:
	CD3D11CallBridge(ID3D11Device* device, CD3D11Driver* driver);
	~CD3D11CallBridge();

	void setVertexShader(SShader* shader);
	void setPixelShader(SShader* shader);
	void setGeometryShader(SShader* shader);
	void setHullShader(SShader* shader);
	void setDomainShader(SShader* shader);
	void setComputeShader(SShader* shader);

	void setBlendState(const SD3D11_BLEND_DESC& BlendDesc);
	void setDepthStencilState(const SD3D11_DEPTH_STENCIL_DESC& depthStencilDesc);
	void setRasterizerState(const SD3D11_RASTERIZER_DESC& rasterizerDesc);

	void setShaderResources(SD3D11_SAMPLER_DESC SamplerDesc[MATERIAL_MAX_TEXTURES], ITexture* shaderViews[MATERIAL_MAX_TEXTURES]);
	void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY top);
	void setInputLayout(IVertexDescriptor* vtxDescriptor, IMaterialRenderer* r);

	ID3D11SamplerState* getSamplerState(u32 idx);

	void setViewPort( const core::rect<s32>& vp );

private:
	ID3D11DeviceContext* Context;
	ID3D11Device* Device;
	CD3D11Driver* Driver;

	SShader* shaders[EST_COUNT];

	u32 samplersChanged;
	u32 texturesChanged;

	SD3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	core::map<SD3D11_DEPTH_STENCIL_DESC, ID3D11DepthStencilState*> DepthStencilMap;

	SD3D11_BLEND_DESC BlendDesc;
	core::map<SD3D11_BLEND_DESC, ID3D11BlendState*> BlendMap;

	ITexture* CurrentTextures[MATERIAL_MAX_TEXTURES];
	ID3D11SamplerState* SamplerStates[MATERIAL_MAX_TEXTURES];
	SD3D11_SAMPLER_DESC SamplerDesc[MATERIAL_MAX_TEXTURES];
	core::map<SD3D11_SAMPLER_DESC, ID3D11SamplerState*> SamplerMap;

	SD3D11_RASTERIZER_DESC RasterizerDesc;
	core::map<SD3D11_RASTERIZER_DESC, ID3D11RasterizerState*> RasterizerMap;

	ID3D11InputLayout* InputLayout;

	D3D11_PRIMITIVE_TOPOLOGY Topology;

	IVertexDescriptor* VtxDescriptor;
	core::map<u64, ID3D11InputLayout*> LayoutMap;
	void* ShaderByteCode;
	u32 ShaderByteCodeSize;

	core::rect<s32> ViewPort;
};

}
}

#endif
#endif
#endif