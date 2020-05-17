// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CD3D11Driver.h"
#include "CD3D11Texture.h"
#include "irrOS.h"

#include "CImage.h"
#include "CColorConverter.h"

namespace irr
{
namespace video
{

//! rendertarget constructor
CD3D11Texture::CD3D11Texture(CD3D11Driver* driver, const core::dimension2d<u32>& size,
						   const io::path& name, const ECOLOR_FORMAT format, u32 arraySlices,
						   u32 sampleCount, u32 sampleQuality)
						   : ITexture(name), Texture(0), TextureBuffer(0), 
						   Device(0), Context(0), Driver(driver),
						   RTView(0), SRView(0),
						   TextureDimension(D3D11_RESOURCE_DIMENSION_TEXTURE2D), 
						   MipLevelLocked(0), NumberOfMipLevels(0), ArraySliceLocked(0), NumberOfArraySlices(arraySlices),
						   SampleCount(sampleCount), SampleQuality(sampleQuality), 
						   LastMapDirection((D3D11_MAP)0), DepthSurface(0),
						   HardwareMipMaps(false),
						   IsCompressed(false)

{
#ifdef _DEBUG
	setDebugName("CD3D11Texture");
#endif

	DriverType = EDT_DIRECT3D11;
	OriginalSize = size;
	Size = size;
	IsRenderTarget = true;

	Device = driver->getExposedVideoData().D3D11.D3DDev11;
	if (Device)
	{
		Device->AddRef();
		Device->GetImmediateContext( &Context );
	}

	createRenderTarget(format);
}


//! constructor
CD3D11Texture::CD3D11Texture(IImage* image, CD3D11Driver* driver,
			   u32 flags, const io::path& name, u32 arraySlices, void* mipmapData)
			   : ITexture(name), Texture(0), TextureBuffer(0),
			   Device(0), Context(0), Driver(driver),   
			   RTView(0), SRView(0), 
			   TextureDimension(D3D11_RESOURCE_DIMENSION_TEXTURE2D),
			   LastMapDirection((D3D11_MAP)0), DepthSurface(0), MipLevelLocked(0), NumberOfMipLevels(0), 
			   ArraySliceLocked(0), NumberOfArraySlices(arraySlices), SampleCount(1), SampleQuality(0),
			   HardwareMipMaps(false),
			   IsCompressed(false)
{
#ifdef _DEBUG
	setDebugName("CD3D11Texture");
#endif

	DriverType = EDT_DIRECT3D11;
	HasMipMaps = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);

	Device = driver->getExposedVideoData().D3D11.D3DDev11;
	if (Device)
	{
		Device->AddRef();
		Device->GetImmediateContext( &Context );
	}

	if (image)
	{
		IsCompressed = image->isCompressed();

		if (createTexture(flags, image))
		{
			if (IsCompressed == false)
			{
				if (copyTexture(image))
					regenerateMipMapLevels(mipmapData);
			}
			else
			{
				// compress texture
				// we copy data on createTexture function
			}
		}
		else
			os::Printer::log("Could not create Direct3D11 Texture.", ELL_WARNING);
	}
}


//! destructor
CD3D11Texture::~CD3D11Texture()
{	
	if (DepthSurface)
	{
		if (DepthSurface->drop())
			Driver->removeDepthSurface(DepthSurface);
	}

	if(RTView)
		RTView->Release();

	if(SRView)
		SRView->Release();

	if(Texture)
		Texture->Release();

	if(TextureBuffer)
		TextureBuffer->Release();

	if(Context)
		Context->Release();

	if(Device)
		Device->Release();
}

//! return texture resource
ID3D11Resource* CD3D11Texture::getTextureResource() const
{
	return Texture;
}

//! return render target view
ID3D11RenderTargetView* CD3D11Texture::getRenderTargetView() const
{
	return RTView;
}

//! return shader resource view
ID3D11ShaderResourceView* CD3D11Texture::getShaderResourceView() const
{
	// Emulate "auto" mipmap generation
	/*
	if( IsRenderTarget && SRView )
		Context->GenerateMips( SRView );
	*/
	return SRView;
}

//! lock function
void* CD3D11Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	bool ronly;
	if (mode == ETLM_READ_ONLY)
		ronly = true;
	else
		ronly = false;
	return lock(ronly,mipmapLevel,0);

}

void* CD3D11Texture::lock(bool readOnly, u32 mipmapLevel, u32 arraySlice)
{
	if(!Texture)
		return 0;

	if (TextureBuffer == NULL)
	{
		if (!createTextureBuffer())
			return 0;
	}


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
	if ( (IsRenderTarget == true) && (LastMapDirection & D3D11_MAP_READ) )
	{
		Context->CopyResource( TextureBuffer, Texture );
	}

	// Map texture buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	hr = Context->Map( TextureBuffer,
								D3D11CalcSubresource(MipLevelLocked,		// mip level to lock
													 ArraySliceLocked,		// array slice (only 1 slice for now)
													 NumberOfMipLevels), 	// number of mip levels
								LastMapDirection, 							// direction to map
								0,
								&mappedData );								// mapped result
	
	if(FAILED(hr))
	{
		logFormatError(hr, "Could not map texture buffer");

		return NULL;
	}

	Pitch = mappedData.RowPitch;

	return mappedData.pData;
}

//! unlock function
void CD3D11Texture::unlock()
{
	if (!Texture)
		return;

	// unlock texture buffer
	Context->Unmap( TextureBuffer, D3D11CalcSubresource(MipLevelLocked, ArraySliceLocked, NumberOfMipLevels) );

	// copy texture buffer to main texture ONLY if buffer was write
	if (LastMapDirection & D3D11_MAP_WRITE)
	{
		Context->CopyResource( Texture, TextureBuffer );
	}
}

u32 CD3D11Texture::getNumberOfArraySlices() const
{
	return NumberOfArraySlices;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CD3D11Texture::regenerateMipMapLevels(void* mipmapData)
{
	if( SRView && HardwareMipMaps )
		Context->GenerateMips( SRView );
}

void CD3D11Texture::createRenderTarget(const ECOLOR_FORMAT format)
{
	HRESULT hr = S_OK;

	// are texture size restrictions there ?
	if(!Driver->queryFeature(EVDF_TEXTURE_NPOT))
	{
		if (Size != OriginalSize)
			os::Printer::log("RenderTarget size has to be a power of two", ELL_INFORMATION);
	}

	Size = Size.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT), !Driver->queryFeature(EVDF_TEXTURE_NSQUARE), true, Driver->getMaxTextureSize().Width);

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

	switch (ColorFormat)
	{
	case ECF_A8R8G8B8:
	case ECF_A1R5G5B5:
	case ECF_A16B16G16R16F:
	case ECF_A32B32G32R32F:
		HasAlpha = true;
		break;
	default:
		break;
	}

	// Check hardware support for automatic mipmap support
	if (Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		UINT support = 0;
		Device->CheckFormatSupport(d3dformat, &support);

		if (support && D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)
			HardwareMipMaps = true;
	}

	// creating texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
	desc.ArraySize = NumberOfArraySlices;
	desc.CPUAccessFlags = 0;
	desc.Format = d3dformat;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = SampleCount;
	desc.SampleDesc.Quality = SampleQuality;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	// test if Direct3D support automatic mip map generation
	if (Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS) && 
		Driver->querySupportForColorFormat(d3dformat, D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
	{
		desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		desc.MipLevels = 0;
	}
	else
	{
		//desc.MiscFlags = 0;
		desc.MipLevels = 1;
	}

	// If array size == 6, force cube texture
	if (desc.ArraySize == 6)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	// If multisampled, mip levels shall be 1
	if (desc.SampleDesc.Count > 1)
	{
		desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;
		desc.MipLevels = 1;
	}
	
	// Texture size
	desc.Width = Size.Width;
	desc.Height = Size.Height;

	// create texture
	hr = Device->CreateTexture2D( &desc, NULL, &Texture );
	if (FAILED(hr))
	{
		logFormatError(hr, "Could not create render target texture");

		return;
	}

	// Get texture description to update some fields
	Texture->GetDesc( &desc );
	NumberOfMipLevels = desc.MipLevels;
	Size.Width = desc.Width;
	Size.Height = desc.Height;

	// create views
	createViews();
}

//! creates the hardware texture
bool CD3D11Texture::createTexture(u32 flags, IImage* image)
{
	HRESULT hr = S_OK;
	OriginalSize = image->getDimension();

	core::dimension2d<u32> optSize = OriginalSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT),
															  !Driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
															  true,
															  Driver->getMaxTextureSize().Width);

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Color format for DX 10 driver shall be different that for DX 9
	// - B5G5R5A1 family is deprecated in DXGI, and doesn't exists in DX 10
	// - Irrlicht color format follows DX 9 (alpha first), and DX 10 is alpha last
	switch(getTextureFormatFromFlags(flags))
	{
		case ETCF_ALWAYS_16_BIT:
		case ETCF_ALWAYS_32_BIT:
		{		
			if (image->isCompressed() == true)
			{
				switch(image->getColorFormat())
				{
					case ECF_DXT1:
						format = DXGI_FORMAT_BC1_UNORM;
						break;
					case ECF_DXT2:
						format = DXGI_FORMAT_BC2_UNORM;
						break;
					case ECF_DXT3:
						format = DXGI_FORMAT_BC2_UNORM;
						break;
					case ECF_DXT4:
						format = DXGI_FORMAT_BC3_UNORM;
						break;
					case ECF_DXT5:
						format = DXGI_FORMAT_BC3_UNORM;
						break;
					default:
						format = DXGI_FORMAT_R8G8B8A8_UNORM;
						break;
				}
			}
			else
			{
				format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			break;
		}
		case ETCF_OPTIMIZED_FOR_QUALITY:
		{
			switch(image->getColorFormat())
			{
			case ECF_R16F:
				format = DXGI_FORMAT_R16_FLOAT;
				break;

			case ECF_R32F:
				format = DXGI_FORMAT_R32_FLOAT;
				break;

			case ECF_G16R16F:
				format = DXGI_FORMAT_R16G16_FLOAT;
				break;

			case ECF_G32R32F:
				format = DXGI_FORMAT_R32G32_FLOAT;
				break;

			case ECF_A16B16G16R16F:
				format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;

			case ECF_A32B32G32R32F:
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case ECF_DXT1:
				format = DXGI_FORMAT_BC1_UNORM;
				break;
			case ECF_DXT2:
				format = DXGI_FORMAT_BC2_UNORM;
				break;
			case ECF_DXT3:
				format = DXGI_FORMAT_BC2_UNORM;
				break;
			case ECF_DXT4:
				format = DXGI_FORMAT_BC3_UNORM;
				break;
			case ECF_DXT5:
				format = DXGI_FORMAT_BC3_UNORM;
				break;
			case ECF_A1R5G5B5:
			case ECF_R5G6B5:
			case ECF_R8G8B8:
			case ECF_A8R8G8B8:
			default:
				format = DXGI_FORMAT_R8G8B8A8_UNORM; 
				break;
			}
		}
		break;
		case ETCF_OPTIMIZED_FOR_SPEED:
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		default:
			break;
	}

	// Check hardware support for automatic mipmap support
	if(HasMipMaps && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		UINT support = 0;
		Device->CheckFormatSupport(format, &support);

		if( support && D3D11_FORMAT_SUPPORT_MIP_AUTOGEN )
			HardwareMipMaps = true;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
	desc.ArraySize = NumberOfArraySlices;
	desc.CPUAccessFlags = 0;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = SampleCount;
	desc.SampleDesc.Quality = SampleQuality;
	desc.Width = optSize.Width;
	desc.Height = optSize.Height;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	if (IsCompressed)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;		
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}
	else
	{

		// test if Direct3D support automatic mip map generation
		// AND creation flag is true
		if (HasMipMaps && Driver->querySupportForColorFormat(format, D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
			desc.MipLevels = 0;
		}
		else
		{
			desc.MipLevels = 1;		// Set only one mip level if do not support auto mip generation
		}

		// If array size == 6, force cube texture
		if (desc.ArraySize == 6)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		}

		// If multisampled, mip levels shall be 1
		if (desc.SampleDesc.Count > 1)
		{
			desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;
			desc.MipLevels = 1;
		}
	}

	// init data
	D3D11_SUBRESOURCE_DATA *initData = NULL;

	if (IsCompressed)
	{
		int mipCount = 1;

		// we calc num mipmap count
		if (image->hasMipMaps() == true)
		{
			int width = optSize.Width;
			int height = optSize.Height;
			int m = width < height ? width : height;

			while (m > 1)
			{
				mipCount++;
				m = m / 2;
			}
		}		

		initData = new D3D11_SUBRESOURCE_DATA[mipCount];

		// update mipmap level
		desc.MipLevels = mipCount;

		// get mipmap data
		int w = optSize.Width;
		int h = optSize.Height;
		int numBytes = 0;
		int rowBytes = 0;

		unsigned char *pSrcBits = (unsigned char*)image->lock();

		for (int i = 0; i < mipCount; i++ )
        {  
			getSurfaceInfo(w, h, format, &numBytes, &rowBytes, nullptr);

			initData[i].pSysMem = (const void*)pSrcBits;
            initData[i].SysMemPitch = static_cast<UINT>( rowBytes );
            initData[i].SysMemSlicePitch = static_cast<UINT>( numBytes );

			pSrcBits += numBytes;

            w = w >> 1;
            h = h >> 1;            
        }

		image->unlock();
	}

	// create texture
	hr = Device->CreateTexture2D( &desc, initData, &Texture );
	if (FAILED(hr))
	{
		logFormatError(hr, "Could not create texture");

		return false;
	}

	// get color format
	ColorFormat = Driver->getColorFormatFromD3DFormat(format);

	if (IsCompressed)
		ColorFormat = image->getColorFormat();

	setPitch(format);

	// Get texture description to update number of mipmaps
	Texture->GetDesc( &desc );

	NumberOfMipLevels = desc.MipLevels;
	Size.Width = desc.Width;
	Size.Height = desc.Height;

	// delete init data
	if (initData)
		delete[]initData;

	// create views to bound texture to pipeline
	return createViews();
}

//! copies the image to the texture
bool CD3D11Texture::copyTexture(IImage* image)
{
	if (IsCompressed)
	{
		unsigned char *source = (unsigned char*)image->lock();

		void* ptr = lock();
		if (ptr)
		{
			u32 compressSize = IImage::getCompressedImageSize(image->getColorFormat(), Size.Width, Size.Height);
			memcpy(ptr, source, compressSize);
		}

		unlock();

		image->unlock();
	}
	else
	{
		void* ptr = lock();
		if (ptr)
			image->copyToScaling(ptr, Size.Width, Size.Height, ColorFormat, Pitch, true);
		unlock();
	}

	return true;
}

void CD3D11Texture::setPitch(DXGI_FORMAT d3dformat)
{
	Pitch = Driver->getBitsPerPixel(d3dformat) * Size.Width;
}

bool CD3D11Texture::createTextureBuffer()
{
	if (!Texture)
	{
		os::Printer::log("Error creating texture buffer: main texture is null", ELL_ERROR);
		return false;
	}
	
	D3D11_TEXTURE2D_DESC desc;
	Texture->GetDesc( &desc );

	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Device->CreateTexture2D( &desc, NULL, &TextureBuffer );
	if (FAILED(hr))
	{
		logFormatError(hr, "Could not create texture buffer");

		return false;
	}

	// sync main texture contents with texture buffer
	Context->CopyResource( TextureBuffer, Texture );

	return true;
}

bool CD3D11Texture::createViews()
{
	if (!Texture)
		return false;

	HRESULT hr = S_OK;
	DXGI_FORMAT format = Driver->getD3DFormatFromColorFormat( ColorFormat );

	// create render target view only if needed
	if(IsRenderTarget)
	{
		if(RTView)
			RTView->Release();

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		::ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
		rtvDesc.Format = format;

		// check if texture is array and/or multisampled
		if (SampleCount > 1 && NumberOfArraySlices > 1)		// multisampled array
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtvDesc.Texture2DMSArray.ArraySize = NumberOfArraySlices;
			rtvDesc.Texture2DMSArray.FirstArraySlice = 0;
		}
		else if (SampleCount > 1)	// only multisampled
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else if (NumberOfArraySlices > 1)	// only array
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.ArraySize = NumberOfArraySlices;
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}
		else	// simple texture
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
		}

		hr = Device->CreateRenderTargetView( Texture, &rtvDesc, &RTView );
		if (FAILED(hr))
		{
			logFormatError(hr, "Could not create render target view");

			return false;
		}
	}

	// create shader resource view
	if(SRView)
		SRView->Release();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	::ZeroMemory( &srvDesc, sizeof( srvDesc ) );
	srvDesc.Format = format;

	// check if texture is array and/or multisampled
	if (SampleCount > 1 && NumberOfArraySlices > 1)		// multisampled array
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		srvDesc.Texture2DMSArray.ArraySize = NumberOfArraySlices;
	}
	else if (SampleCount > 1)	// only multisampled
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	else if (NumberOfArraySlices > 1)	// only array
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = NumberOfArraySlices;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = NumberOfMipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
	}
	else	// simple texture
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = NumberOfMipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;	
	}
		
	hr = Device->CreateShaderResourceView( Texture, &srvDesc, &SRView );
	if (FAILED(hr))
	{
		logFormatError(hr, "Could not create shader resource view");

		return false;
	}

	return true;
}

int CD3D11Texture::bitsPerPixel(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_YUY2:
		return 32;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		return 24;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 16;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_NV11:
		return 12;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

#if defined(_XBOX_ONE) && defined(_TITLE)

	case DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
	case DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
		return 32;

#if MONOLITHIC
	case DXGI_FORMAT_D16_UNORM_S8_UINT:
	case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
		return 24;
#endif

#endif // _XBOX_ONE && _TITLE

	default:
		return 0;
	}
}

void CD3D11Texture::getSurfaceInfo(int width, int height, DXGI_FORMAT fmt, int* outNumBytes, int* outRowBytes, int* outNumRows)
{
	int numBytes = 0;
	int rowBytes = 0;
	int numRows = 0;

	bool bc = false;
	bool packed = false;
	bool planar = false;
	int bpe = 0;

	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bc = true;
		bpe = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		bc = true;
		bpe = 16;
		break;

	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_YUY2:
		packed = true;
		bpe = 4;
		break;

	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		packed = true;
		bpe = 8;
		break;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
		planar = true;
		bpe = 2;
		break;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		planar = true;
		bpe = 4;
		break;

#if defined(_XBOX_ONE) && defined(_TITLE) && MONOLITHIC

	case DXGI_FORMAT_D16_UNORM_S8_UINT:
	case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
		planar = true;
		bpe = 4;
		break;

#endif
	}

	if (bc)
	{
		size_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = core::max_<int>(1, (width + 3) / 4);
		}
		size_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = core::max_<int>(1, (height + 3) / 4);
		}
		rowBytes = numBlocksWide * bpe;
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (packed)
	{
		rowBytes = ((width + 1) >> 1) * bpe;
		numRows = height;
		numBytes = rowBytes * height;
	}
	else if (fmt == DXGI_FORMAT_NV11)
	{
		rowBytes = ((width + 3) >> 2) * 4;
		numRows = height * 2; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		numBytes = rowBytes * numRows;
	}
	else if (planar)
	{
		rowBytes = ((width + 1) >> 1) * bpe;
		numBytes = (rowBytes * height) + ((rowBytes * height + 1) >> 1);
		numRows = height + ((height + 1) >> 1);
	}
	else
	{
		size_t bpp = bitsPerPixel(fmt);
		rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
		numBytes = rowBytes * height;
	}

	if (outNumBytes)
	{
		*outNumBytes = numBytes;
	}
	if (outRowBytes)
	{
		*outRowBytes = rowBytes;
	}
	if (outNumRows)
	{
		*outNumRows = numRows;
	}
}

}
}

#endif