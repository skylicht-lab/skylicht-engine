#include "pch.h"
#include "CD3D11CallBridge.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "irrOS.h"
#include "CD3D11Driver.h"
#include "CD3D11Texture.h"
#include "CD3D11TextureCube.h"
#include "CD3D11TextureArray.h"
#include "CD3D11VertexDescriptor.h"

#include <d3d11.h>

namespace irr
{
namespace video
{

CD3D11CallBridge::CD3D11CallBridge(ID3D11Device* device, CD3D11Driver* driver)
	: Context(NULL), Device(device), Driver(driver), InputLayout(NULL),
	Topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED), VtxDescriptor(NULL), ShaderByteCode(NULL), ShaderByteCodeSize(0),
	samplersChanged(0), texturesChanged(0)
{
	if (Device)
	{
		Device->AddRef();
		Device->GetImmediateContext( &Context );
	}

	for (int i = 0; i < EST_COUNT; ++i)
		shaders[i] = NULL;

	ZeroMemory(CurrentTextures, sizeof(CurrentTextures[0]) * MATERIAL_MAX_TEXTURES);
	ZeroMemory(SamplerStates, sizeof(SamplerStates[0]) * MATERIAL_MAX_TEXTURES);
}

CD3D11CallBridge::~CD3D11CallBridge()
{
	// release blend states
	core::map<SD3D11_BLEND_DESC, ID3D11BlendState*>::Iterator bldIt = BlendMap.getIterator();
	while (!bldIt.atEnd())
	{
		if (bldIt->getValue()) 
			bldIt->getValue()->Release();
		bldIt++;
	}
	BlendMap.clear();

	// release rasterizer states
	core::map<SD3D11_RASTERIZER_DESC, ID3D11RasterizerState*>::Iterator rasIt = RasterizerMap.getIterator();
	while (!rasIt.atEnd())
	{
		if (rasIt->getValue()) 
			rasIt->getValue()->Release();
		rasIt++;
	}
	RasterizerMap.clear();

	// release depth stencil states
	core::map<SD3D11_DEPTH_STENCIL_DESC, ID3D11DepthStencilState*>::Iterator dsIt = DepthStencilMap.getIterator();
	while (!dsIt.atEnd())
	{
		if (dsIt->getValue()) 
			dsIt->getValue()->Release();
		dsIt++;
	}
	DepthStencilMap.clear();

	// release sampler states
	core::map<SD3D11_SAMPLER_DESC, ID3D11SamplerState*>::Iterator samIt = SamplerMap.getIterator();
	while (!samIt.atEnd())
	{
		if (samIt->getValue()) 
			samIt->getValue()->Release();
		samIt++;
	}
	SamplerMap.clear();

	// release input states
	core::map<u64, ID3D11InputLayout*>::Iterator layIt = LayoutMap.getIterator();
	while (!layIt.atEnd())
	{
		if (layIt->getValue()) 
			layIt->getValue()->Release();
		layIt++;
	}
	LayoutMap.clear();

	if(Context)
		Context->Release();

	if(Device)
		Device->Release();
}

void CD3D11CallBridge::setVertexShader(SShader* shader)
{
	if (shaders[EST_VERTEX_SHADER] != shader)
	{
		shaders[EST_VERTEX_SHADER] = shader;

		if (shader)
		{
			Context->VSSetShader((ID3D11VertexShader*)shader->shader, NULL, 0);

			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->VSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->VSSetShader(NULL, NULL, 0);
	}	

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->VSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
						views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();

					Context->VSSetShaderResources(i, 1, &views);
				}
			}
		}
	}
}

void CD3D11CallBridge::setPixelShader(SShader* shader)
{
	if (shaders[EST_PIXEL_SHADER] != shader)
	{
		shaders[EST_PIXEL_SHADER] = shader;

		// duc.phamhong: need reset sampler state when shader is changed
		samplersChanged = 0;
		texturesChanged = 0;

		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
		{
			samplersChanged = samplersChanged | (1 << i);
			texturesChanged = samplersChanged | (1 << i);
		}

		if (shader)
		{
			Context->PSSetShader((ID3D11PixelShader*)shader->shader, NULL, 0);
			
			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->PSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->PSSetShader(NULL, NULL, 0);
	}

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{			
			/*
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->PSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
					{
						E_TEXTURE_TYPE textureType = CurrentTextures[i]->getTextureType();

						if (textureType == ETT_TEXTURE_2D)
							views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();
						else if (textureType == ETT_TEXTURE_CUBE)
							views = ((CD3D11TextureCube*)CurrentTextures[i])->getShaderResourceView();
						else if (textureType == ETT_TEXTURE_ARRAY)
							views = ((CD3D11TextureArray*)CurrentTextures[i])->getShaderResourceView();
					}

					Context->PSSetShaderResources(i, 1, &views);
				}
			}			
			*/
			
			ID3D11ShaderResourceView* views[MATERIAL_MAX_TEXTURES];
			int maxViewUsed = 0;

			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (CurrentTextures[i])
				{
					E_TEXTURE_TYPE textureType = CurrentTextures[i]->getTextureType();

					if (textureType == ETT_TEXTURE_2D)
						views[i] = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();
					else if (textureType == ETT_TEXTURE_CUBE)
						views[i] = ((CD3D11TextureCube*)CurrentTextures[i])->getShaderResourceView();
					else if (textureType == ETT_TEXTURE_ARRAY)
						views[i] = ((CD3D11TextureArray*)CurrentTextures[i])->getShaderResourceView();

					maxViewUsed = i + 1;
				}
				else
					views[i] = NULL;
			}

			Context->PSSetSamplers(0, maxViewUsed, SamplerStates);
			Context->PSSetShaderResources(0, maxViewUsed, views);			
		}
	}
}

void CD3D11CallBridge::setGeometryShader(SShader* shader)
{
	if (shaders[EST_GEOMETRY_SHADER] != shader)
	{
		shaders[EST_GEOMETRY_SHADER] = shader;

		if (shader)
		{
			Context->GSSetShader((ID3D11GeometryShader*)shader->shader, NULL, 0);

			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->GSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->GSSetShader(NULL, NULL, 0);
	}

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->GSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
						views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();

					Context->GSSetShaderResources(i, 1, &views);
				}
			}
		}
	}
}

void CD3D11CallBridge::setHullShader(SShader* shader)
{
	if (shaders[EST_HULL_SHADER] != shader)
	{
		shaders[EST_HULL_SHADER] = shader;

		if (shader)
		{
			Context->HSSetShader((ID3D11HullShader*)shader->shader, NULL, 0);

			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->HSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->HSSetShader(NULL, NULL, 0);
	}

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->HSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
						views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();

					Context->HSSetShaderResources(i, 1, &views);
				}
			}
		}
	}
}

void CD3D11CallBridge::setDomainShader(SShader* shader)
{
	if (shaders[EST_DOMAIN_SHADER] != shader)
	{
		shaders[EST_DOMAIN_SHADER] = shader;

		if (shader)
		{
			Context->DSSetShader((ID3D11DomainShader*)shader->shader, NULL, 0);

			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->DSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->DSSetShader(NULL, NULL, 0);
	}

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->DSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
						views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();

					Context->DSSetShaderResources(i, 1, &views);
				}
			}
		}
	}
}

void CD3D11CallBridge::setComputeShader(SShader* shader)
{
	if (shaders[EST_COMPUTE_SHADER] != shader)
	{
		shaders[EST_COMPUTE_SHADER] = shader;		

		if (shader)
		{
			Context->CSSetShader((ID3D11ComputeShader*)shader->shader, NULL, 0);

			const u32 size = shader->bufferArray.size();

			if (size != 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(shader->bufferArray[i]->data);

				Context->CSSetConstantBuffers(0, size, &buffs[0]);
			}
		}
		else
			Context->CSSetShader(NULL, NULL, 0);
	}

	if (shader)
	{
		// only set samplers and textures if a shader is set and if samplers / textures are used, setted and changed
		u32 samplersToSet = shader->samplersUsed & samplersChanged;
		u32 texturesToSet = shader->texturesUsed & texturesChanged;

		if (samplersToSet || texturesToSet)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (samplersToSet & (1 << i))
					Context->CSSetSamplers(i, 1, &SamplerStates[i]);

				if (texturesChanged & (1 << i))
				{
					ID3D11ShaderResourceView* views = NULL;

					if (CurrentTextures[i])
						views = ((CD3D11Texture*)CurrentTextures[i])->getShaderResourceView();

					Context->CSSetShaderResources(i, 1, &views);
				}
			}
		}
	}
}

void CD3D11CallBridge::setDepthStencilState(const SD3D11_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	if(DepthStencilDesc != depthStencilDesc)
	{
		DepthStencilDesc = depthStencilDesc;

		ID3D11DepthStencilState* state = NULL;
		core::map<SD3D11_DEPTH_STENCIL_DESC, ID3D11DepthStencilState*>::Node* dsIt = DepthStencilMap.find(DepthStencilDesc);

		if(dsIt)
		{
			state = dsIt->getValue();
		}
		else	// if not found, create and insert into map
		{
			HRESULT hr;
			if(SUCCEEDED(hr = Device->CreateDepthStencilState(&DepthStencilDesc, &state)))
			{
				DepthStencilMap.insert(DepthStencilDesc, state);
			}
			else
			{
				logFormatError(hr, "Could not create depth stencil state");

				return;
			}
		}

		Context->OMSetDepthStencilState(state, 1);
	}
}

void CD3D11CallBridge::setRasterizerState(const SD3D11_RASTERIZER_DESC& rasterizerDesc)
{
	if(RasterizerDesc != rasterizerDesc)
	{
		RasterizerDesc = rasterizerDesc;

		// Rasterizer state
		ID3D11RasterizerState* state = 0;
		core::map<SD3D11_RASTERIZER_DESC, ID3D11RasterizerState*>::Node* rasIt = RasterizerMap.find(RasterizerDesc);
		if(rasIt)
		{
			state = rasIt->getValue();
		}
		else	// if not found, create and insert into map
		{
			HRESULT hr;
			if(SUCCEEDED(hr = Device->CreateRasterizerState(&RasterizerDesc, &state)))
			{
				RasterizerMap.insert(RasterizerDesc, state);
			}
			else
			{
				logFormatError(hr, "Could not create rasterizer state");

				return;
			}
		}

		Context->RSSetState(state);
	}
}

void CD3D11CallBridge::setBlendState(const SD3D11_BLEND_DESC& blendDesc)
{
	if(BlendDesc != blendDesc)
	{
		BlendDesc = blendDesc;

		ID3D11BlendState* state = 0;
		core::map<SD3D11_BLEND_DESC, ID3D11BlendState*>::Node* bldIt = BlendMap.find(BlendDesc);

		if (bldIt)
		{
			state = bldIt->getValue();
		}
		else	// if not found, create and insert into map
		{
			HRESULT hr;
			if(SUCCEEDED(hr = Device->CreateBlendState(&BlendDesc, &state)))
			{
				BlendMap.insert( BlendDesc, state );
			}
			else
			{
				logFormatError(hr, "Could not create blend state");

				return;
			}
		}

		Context->OMSetBlendState(state, 0, 0xffffffff);
	}
}


void CD3D11CallBridge::setShaderResources(SD3D11_SAMPLER_DESC samplerDesc[MATERIAL_MAX_TEXTURES], ITexture* currentTextures[MATERIAL_MAX_TEXTURES])
{
	texturesChanged = 0;
	samplersChanged = 0;

	for(u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if(SamplerDesc[i] != samplerDesc[i])
		{
			SamplerDesc[i] = samplerDesc[i];

			SamplerStates[i] = getSamplerState(i);

			samplersChanged |= (1 << i);
		}

		if(CurrentTextures[i] != currentTextures[i])
		{
			CurrentTextures[i] = currentTextures[i];

			texturesChanged |= (1 << i);
		}
	}
}

void CD3D11CallBridge::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if(Topology != topology)
	{
		Topology = topology;

		Context->IASetPrimitiveTopology(Topology);
	}
}

void CD3D11CallBridge::setInputLayout(IVertexDescriptor* vtxDescriptor, IMaterialRenderer* r)
{
	CD3D11MaterialRenderer* renderer = (CD3D11MaterialRenderer*)r;

	if(VtxDescriptor != vtxDescriptor || renderer->getShaderByteCode() != ShaderByteCode || renderer->getShaderByteCodeSize() != ShaderByteCodeSize)
	{
		VtxDescriptor = vtxDescriptor;
		ShaderByteCode = renderer->getShaderByteCode();
		ShaderByteCodeSize = renderer->getShaderByteCodeSize();

		ID3D11InputLayout* state = NULL;
		
		u64 signature = reinterpret_cast<u64>(renderer->getShaderByteCode());

		core::map<u64, ID3D11InputLayout*>::Node* layIt = LayoutMap.find(signature);
		if(layIt)
		{
			state = layIt->getValue();
		}
		else	// if not found, create and insert into layout
		{
			core::array<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc = ((CD3D11VertexDescriptor*)VtxDescriptor)->getInputLayoutDescription();

			HRESULT hr;
			if (SUCCEEDED(hr = Device->CreateInputLayout(inputLayoutDesc.pointer(), inputLayoutDesc.size(),
				renderer->getShaderByteCode(), renderer->getShaderByteCodeSize(), &state)))
			{
				LayoutMap.insert(signature, state);
			}
			else
			{
				logFormatError(hr, "Could not create input layout");

				return;
			}
		}

		Context->IASetInputLayout(state);
	}
}

ID3D11SamplerState* CD3D11CallBridge::getSamplerState(u32 idx)
{
	// Depth stencil state
	ID3D11SamplerState* state = NULL;
	core::map<SD3D11_SAMPLER_DESC, ID3D11SamplerState*>::Node* samIt = SamplerMap.find( SamplerDesc[idx] );
	if (samIt)
	{
		state = samIt->getValue();
	}
	else	// if not found, create and insert into map
	{
		HRESULT hr;
		if (SUCCEEDED(hr = Device->CreateSamplerState( &SamplerDesc[idx], &state )))
		{
			SamplerMap.insert( SamplerDesc[idx], state );
		}
		else
		{
			logFormatError(hr, "Could not create sampler state");

			return NULL;
		}
	}

	return state;
}

void CD3D11CallBridge::setViewPort( const core::rect<s32>& vp )
{
	if(ViewPort != vp)
	{
		D3D11_VIEWPORT viewPort;
		viewPort.TopLeftX = (FLOAT)vp.UpperLeftCorner.X;
		viewPort.TopLeftY = (FLOAT)vp.UpperLeftCorner.Y;
		viewPort.Width = (FLOAT)vp.getWidth();
		viewPort.Height = (FLOAT)vp.getHeight();
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		Context->RSSetViewports( 1, &viewPort );
	}
}

void CD3D11CallBridge::setScissor(const core::rect<s32>& vp)
{
	if (ScissorRect != vp)
	{
		D3D11_RECT rect;
		rect.left = (LONG)vp.UpperLeftCorner.X;
		rect.top = (LONG)vp.UpperLeftCorner.Y;
		rect.bottom = (LONG)vp.LowerRightCorner.Y;
		rect.right = (LONG)vp.LowerRightCorner.X;

		Context->RSSetScissorRects(1, &rect);
	}
}

void CD3D11CallBridge::resetState()
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		SamplerStates[i] = NULL;
		CurrentTextures[i] = NULL;
	}
}

}
}

#endif