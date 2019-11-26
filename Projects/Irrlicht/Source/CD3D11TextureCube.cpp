// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Pham Hong Duc add to Skylicht Engine

#include "pch.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CD3D11Driver.h"
#include "CD3D11TextureCube.h"
#include "irrOS.h"

#include "CImage.h"
#include "CColorConverter.h"

namespace irr
{
namespace video
{

//! rendertarget constructor
CD3D11TextureCube::CD3D11TextureCube(CD3D11Driver* driver, const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format)
	: ITexture(name), ColorFormat(ECF_UNKNOWN), Driver(driver), Pitch(0), DepthSurface(NULL), Texture(NULL)
{
	TextureType = ETT_TEXTURE_CUBE;
	DriverType = EDT_DIRECT3D11;	

	OriginalSize = size;
	Size = size;

	IsRenderTarget = true;
	HasMipMaps = false;

	Device = driver->getExposedVideoData().D3D11.D3DDev11;
	if (Device)
	{
		Device->AddRef();
		Device->GetImmediateContext( &Context );
	}

	for (int i = 0; i < 6; i++)
		RTView[i] = NULL;

	SRView = NULL;

	TextureBuffer = NULL;
	MipLevelLocked = 0;
	ArraySliceLocked = 0;

	createRenderTarget(format);
}

//! rendertarget constructor
CD3D11TextureCube::CD3D11TextureCube(CD3D11Driver* driver, const io::path& name,
	IImage* posXImage, IImage* negXImage, 
	IImage* posYImage, IImage* negYImage, 
	IImage* posZImage, IImage* negZImage)
	: ITexture(name), ColorFormat(ECF_UNKNOWN), Driver(driver), Pitch(0), DepthSurface(NULL), Texture(NULL)
{
	TextureType = ETT_TEXTURE_CUBE;
	DriverType = EDT_DIRECT3D11;

	IsRenderTarget = false;
	HasMipMaps = false;

	OriginalSize.set(posXImage->getDimension().Width, posXImage->getDimension().Height);
	Size = OriginalSize;

	Device = driver->getExposedVideoData().D3D11.D3DDev11;
	if (Device)
	{
		Device->AddRef();
		Device->GetImmediateContext( &Context );
	}

	for (int i = 0; i < 6; i++)
		RTView[i] = NULL;

	SRView = NULL;
	HardwareMipMaps = false;

	TextureBuffer = NULL;
	MipLevelLocked = 0;
	ArraySliceLocked = 0;

	createCubeTexture(posXImage, negXImage, posYImage, negYImage, posZImage, negZImage);
}

//! destructor
CD3D11TextureCube::~CD3D11TextureCube()
{
	if (DepthSurface)
	{
		if (DepthSurface->drop())
			Driver->removeDepthSurface(DepthSurface);
	}

	if (SRView)
		SRView->Release();

	for (int i = 0; i < 6; i++)
	{
		if (RTView[i])
			RTView[i]->Release();
	}

	if (Texture)
		Texture->Release();

	if (TextureBuffer)
		TextureBuffer->Release();

	Context->Release();
	Device->Release();
}

//! lock function
void* CD3D11TextureCube::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	bool ronly;
	if (mode == ETLM_READ_ONLY)
		ronly = true;
	else
		ronly = false;
	return lock(ronly, mipmapLevel, 0);
}

void* CD3D11TextureCube::lock(bool readOnly , u32 mipmapLevel, u32 arraySlice)
{
	if (!Texture || !createTextureBuffer())
		return 0;

	HRESULT hr = S_OK;

	// Record mip level locked to use in unlock
	MipLevelLocked = mipmapLevel;
	ArraySliceLocked = arraySlice;	

	// set map direction
	if (readOnly)
		LastMapDirection = D3D11_MAP_READ;
	else
		LastMapDirection = (D3D11_MAP)(D3D11_MAP_READ | D3D11_MAP_WRITE);

	// if read, and this is a render target texture (i.ex.: GPU will write data to texture)
	// shall sync data from main texture to texture buffer
	if ((IsRenderTarget == true) && (LastMapDirection & D3D11_MAP_READ))
	{
		Context->CopyResource(TextureBuffer, Texture);
	}

	// Map texture buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	hr = Context->Map(TextureBuffer,
			D3D11CalcSubresource(MipLevelLocked,	// mip level to lock
			ArraySliceLocked,						// array slice (only 1 slice for now)
			NumberOfMipLevels), 					// number of mip levels
			LastMapDirection, 						// direction to map
			0,
			&mappedData);							// mapped result

	if (FAILED(hr))
	{
		os::Printer::log("Could not map texture buffer", irr::ELL_ERROR);

		return NULL;
	}

	Pitch = mappedData.RowPitch;
	return mappedData.pData;
}

//! unlock function
void CD3D11TextureCube::unlock()
{
	if (!Texture)
		return;

	// unlock texture buffer
	Context->Unmap(TextureBuffer, D3D11CalcSubresource(MipLevelLocked, ArraySliceLocked, NumberOfMipLevels));

	// copy texture buffer to main texture ONLY if buffer was write
	if (LastMapDirection && D3D11_MAP_WRITE)
	{
		Context->CopyResource(Texture, TextureBuffer);
	}

	TextureBuffer->Release();
	TextureBuffer = NULL;
}

bool CD3D11TextureCube::createTextureBuffer()
{
	if (!Texture)
	{
		os::Printer::log("Error creating texture buffer: main texture is null", ELL_ERROR);
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	Texture->GetDesc(&desc);

	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateTexture2D(&desc, NULL, &TextureBuffer);
	if (FAILED(hr))
	{
		os::Printer::log("Could not create texture buffer", irr::ELL_ERROR);

		return false;
	}

	// sync main texture contents with texture buffer
	Context->CopyResource(TextureBuffer, Texture);

	return true;
}

//! Returns original size of the texture.
const core::dimension2d<u32>& CD3D11TextureCube::getOriginalSize() const
{
	return OriginalSize;
}

//! Returns (=size) of the texture.
const core::dimension2d<u32>& CD3D11TextureCube::getSize() const
{
	return OriginalSize;
}

//! returns color format of texture
ECOLOR_FORMAT CD3D11TextureCube::getColorFormat() const
{
	return ColorFormat;
}

u32 CD3D11TextureCube::getPitch() const
{
	return Pitch;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CD3D11TextureCube::regenerateMipMapLevels(void* mipmapData)
{
	if (SRView && HardwareMipMaps)
	{
		Context->GenerateMips(SRView);
		HasMipMaps = true;
	}
}

//! returns if it is a render target
bool CD3D11TextureCube::isFrameBufferObject() const
{
	return IsRenderTarget;
}

void CD3D11TextureCube::createCubeTexture(IImage* posXImage, IImage* negXImage, 
	IImage* posYImage, IImage* negYImage, 
	IImage* posZImage, IImage* negZImage)
{
	int width = posXImage->getDimension().Width;
	int height = posXImage->getDimension().Height;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	if (Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		UINT support = 0;
		Device->CheckFormatSupport(texDesc.Format, &support);

		if (support && D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)
			HardwareMipMaps = true;
	}

	HasMipMaps = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	if (HasMipMaps && HardwareMipMaps)
	{
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		texDesc.MipLevels = 0;
	}

	D3D11_SUBRESOURCE_DATA pData[6];
	IImage* img[] = {posXImage, negXImage, posYImage, negYImage, posZImage, negZImage};

	for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
	{
		img[cubeMapFaceIndex]->swapBG();

		void *pSrcBits = img[cubeMapFaceIndex]->lock();

		// fill with red color  
		pData[cubeMapFaceIndex].pSysMem = pSrcBits;
		pData[cubeMapFaceIndex].SysMemPitch = width * 4;
		pData[cubeMapFaceIndex].SysMemSlicePitch = 0;	

		img[cubeMapFaceIndex]->unlock();
	}

	// create texture
	HRESULT hr = S_OK;
	if (HasMipMaps == false || HardwareMipMaps == false)
		hr = Device->CreateTexture2D(&texDesc, &pData[0], &Texture);
	else
		hr = Device->CreateTexture2D(&texDesc, NULL, &Texture );

	if (FAILED(hr))
	{
		os::Printer::log("Could not create render target texture", irr::ELL_ERROR);
		return;
	}

	Texture->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = texDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	NumberOfMipLevels = texDesc.MipLevels;

	// create view
	hr = Device->CreateShaderResourceView(Texture, &SMViewDesc, &SRView);

	// todo
	// we copy image data to mipmap 0 -> after that call generate mipmaps
	if (HasMipMaps == true && HardwareMipMaps == true)
	{
		// todo lock and copy image
		for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
		{
			void *data = lock(false, 0, cubeMapFaceIndex);

			int size = texDesc.Width * texDesc.Height * 4;
			memcpy(data, pData[cubeMapFaceIndex].pSysMem, size);
			unlock();
		}
	}
}

//! creates hardware render target
void CD3D11TextureCube::createRenderTarget(const ECOLOR_FORMAT format)
{
	DXGI_FORMAT d3dformat = Driver->getD3DColorFormat();

	if(ColorFormat == ECF_UNKNOWN)
	{
		// get irrlicht format from backbuffer
		// (This will get overwritten by the custom format if it is provided, else kept.)
		ColorFormat = Driver->getColorFormat();
		setPitch(d3dformat);

		// Use color format if provided.
		if(format != ECF_UNKNOWN)
		{
			ColorFormat = format;
			d3dformat = Driver->getD3DFormatFromColorFormat(format);
			setPitch(d3dformat); // This will likely set pitch to 0 for now.
		}
	}
	else
	{
		d3dformat = Driver->getD3DFormatFromColorFormat(ColorFormat);
	}

	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
    desc.ArraySize = 6;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = d3dformat;
    desc.Width = OriginalSize.Width;
	desc.Height = OriginalSize.Height;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

//#ifdef USE_RENDERTARGET_MIPMAP
	desc.MipLevels = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
//#endif

	// create texture
	hr = Device->CreateTexture2D( &desc, NULL, &Texture );
	if (FAILED(hr))
	{
		os::Printer::log("Could not create render target texture", irr::ELL_ERROR);
		return;
	}

	// Get texture description to update some fields
	Texture->GetDesc( &desc );
	OriginalSize.Width = desc.Width;
	OriginalSize.Height = desc.Height;

	NumberOfMipLevels = desc.MipLevels;

	// Create views
	createViews();
}

//! create views to bound texture to pipeline
bool CD3D11TextureCube::createViews()
{
	if (!Texture)
		return false;

	HRESULT hr = S_OK;
	DXGI_FORMAT format = Driver->getD3DFormatFromColorFormat( ColorFormat );

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.MipSlice = 0;
    rtvDesc.Texture2DArray.ArraySize = 1;

    for (int i = 0; i < 6; ++i)
    {
		if(RTView[i])
			RTView[i]->Release();

		rtvDesc.Texture2DArray.FirstArraySlice = i;
		hr = Device->CreateRenderTargetView( Texture, &rtvDesc, &RTView[i] );
		if (hr != S_OK)
			return false;
    }

	if(SRView)
		SRView->Release();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = -1;

	hr = Device->CreateShaderResourceView(Texture, &srvDesc, &SRView);
	if (hr != S_OK)
		return false;	

	return true;
}

//! set Pitch based on the d3d format
void CD3D11TextureCube::setPitch(DXGI_FORMAT d3dformat)
{
	Pitch = Driver->getBitsPerPixel(d3dformat) * OriginalSize.Width;
}

}
}

#endif