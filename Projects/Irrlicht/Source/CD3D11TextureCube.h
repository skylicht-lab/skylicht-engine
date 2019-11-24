
#ifndef __C_DIRECTX11_TEXTURECUBE_H_INCLUDED__
#define __C_DIRECTX11_TEXTURECUBE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "ITexture.h"
#include "IImage.h"

namespace irr
{
namespace video
{

class CD3D11Driver;

class CD3D11TextureCube : public ITexture
{	
private:
	friend class CD3D11Driver;

	ECOLOR_FORMAT ColorFormat;
	s32 Pitch;

	bool HardwareMipMaps;

	ID3D11Texture2D* TextureBuffer;		// staging texture used for lock/unlock
	u32 MipLevelLocked;
	u32 ArraySliceLocked;
	D3D11_MAP LastMapDirection;

	SDepthSurface11* DepthSurface;

	int NumberOfMipLevels;
public:
	
	//! rendertarget constructor
	CD3D11TextureCube(CD3D11Driver* driver, const core::dimension2d<u32>& size, const io::path& name,
		const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! rendertarget constructor
	CD3D11TextureCube(CD3D11Driver* driver, const io::path& name,
		IImage* posXImage, IImage* negXImage, 
		IImage* posYImage, IImage* negYImage, 
		IImage* posZImage, IImage* negZImage);

	//! destructor
	virtual ~CD3D11TextureCube();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0);
	virtual void* lock(bool readOnly , u32 mipmapLevel=0, u32 arraySlice = 0);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture.
	virtual const core::dimension2d<u32>& getOriginalSize() const;

	//! Returns (=size) of the texture.
	virtual const core::dimension2d<u32>& getSize() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;

	virtual u32 getPitch() const;

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels(void* mipmapData=0);

	//! returns if it is a render target
	virtual bool isFrameBufferObject() const;

public:

	ID3D11RenderTargetView *getTargetView(int face)
	{
		return RTView[face];
	}

	ID3D11ShaderResourceView *getShaderResourceView()
	{
		return SRView;
	}
private:
	friend class CD3D11Driver;

	ID3D11Device* Device;
	ID3D11DeviceContext* Context;

	ID3D11Texture2D* Texture;	

	ID3D11RenderTargetView* RTView[6];
	ID3D11ShaderResourceView* SRView;

	CD3D11Driver* Driver;
private:

	void createCubeTexture(IImage* posXImage, IImage* negXImage, 
		IImage* posYImage, IImage* negYImage, 
		IImage* posZImage, IImage* negZImage);

	//! creates hardware render target
	void createRenderTarget(const ECOLOR_FORMAT format);

	//! create views to bound texture to pipeline
	bool createViews();

	//! set Pitch based on the d3d format
	void setPitch(DXGI_FORMAT d3dformat);

	bool createTextureBuffer();
};

}
}

#endif
#endif
#endif