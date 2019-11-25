
#ifndef __C_DIRECTX11_TEXTURE_H_INCLUDED__
#define __C_DIRECTX11_TEXTURE_H_INCLUDED__

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
// forward declaration for RTT depth buffer handling
struct SDepthSurface11;

class CD3D11Texture : public ITexture
{
public:

	//! constructor
	CD3D11Texture(IImage* image, CD3D11Driver* driver,
			u32 flags, const io::path& name, u32 arraySlices = 1, void* mipmapData=0);

	//! rendertarget constructor
	CD3D11Texture(CD3D11Driver* driver, const core::dimension2d<u32>& size, const io::path& name,
		const ECOLOR_FORMAT format = ECF_UNKNOWN, u32 arraySlices = 1, 
		u32 sampleCount = 1, u32 sampleQuality = 0 );

	//! destructor
	virtual ~CD3D11Texture();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0);

	virtual void* lock(bool readOnly , u32 mipmapLevel=0, u32 arraySlice = 0);

	//! unlock function
	virtual void unlock();

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels(void* mipmapData = 0);

	virtual u32 getNumberOfArraySlices() const;

public:
	//! return texture resource
	ID3D11Resource* getTextureResource() const;

	//! return render target view
	ID3D11RenderTargetView* getRenderTargetView() const;

	//! return shader resource view
	ID3D11ShaderResourceView* getShaderResourceView() const;

private:
	friend class CD3D11Driver;

	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	ID3D11Texture2D* Texture;
	ID3D11RenderTargetView* RTView;
	ID3D11ShaderResourceView* SRView;
	D3D11_RESOURCE_DIMENSION TextureDimension;
	D3D11_MAP LastMapDirection;
	
	CD3D11Driver* Driver;
	SDepthSurface11* DepthSurface;
	u32 NumberOfMipLevels;
	u32 NumberOfArraySlices;
	u32 SampleCount;
	u32 SampleQuality;

	ID3D11Texture2D* TextureBuffer;		// staging texture used for lock/unlock
	u32 MipLevelLocked;
	u32 ArraySliceLocked;

	bool IsCompressed;
	bool HardwareMipMaps;

	void getSurfaceInfo(int width, int height, DXGI_FORMAT fmt, int* outNumBytes, int* outRowBytes, int* outNumRows);

	int bitsPerPixel(DXGI_FORMAT fmt);

	//! creates hardware render target
	void createRenderTarget(const ECOLOR_FORMAT format);

	//! creates the hardware texture
	bool createTexture(u32 flags, IImage * image);

	//! copies the image to the texture
	bool copyTexture(IImage* image);

	//! set Pitch based on the d3d format
	void setPitch(DXGI_FORMAT d3dformat);

	//! create texture buffer needed for lock/unlock
	bool createTextureBuffer();

	//! create views to bound texture to pipeline
	bool createViews();
};

}
}

#endif
#endif
#endif