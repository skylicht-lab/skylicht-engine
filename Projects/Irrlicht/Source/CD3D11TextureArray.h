#ifndef __C_DIRECTX1_TEXTURE_ARRAY_
#define __C_DIRECTX1_TEXTURE_ARRAY_

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "ITexture.h"
#include "IImage.h"

#define MAX_TEXTURE_ARRAY_RT_SIZE 32

namespace irr
{
	namespace video
	{
		class CD3D11Driver;
		// forward declaration for RTT depth buffer handling
		struct SDepthSurface11;

		class CD3D11TextureArray : public ITexture
		{
		private:			
			friend class CD3D11Driver;
			ECOLOR_FORMAT ColorFormat;

			ID3D11Device* Device;
			ID3D11DeviceContext* Context;
			ID3D11Texture2D* Texture;
			ID3D11RenderTargetView* RTView[MAX_TEXTURE_ARRAY_RT_SIZE];
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
			bool HardwareMipMaps;

		public:

			//! constructor
			CD3D11TextureArray(IImage** image, u32 arraySlices, CD3D11Driver* driver, u32 flags, const io::path& name);

			//! fbo
			CD3D11TextureArray(const core::dimension2d<u32>& size, u32 arraySlices, CD3D11Driver* driver, const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

			//! destructor
			virtual ~CD3D11TextureArray();

			//! lock function
			virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0);

			//! lock function			
			virtual void* lock(bool readOnly, u32 mipmapLevel = 0, u32 arraySlice = 0);

			//! unlock function
			virtual void unlock();

			//! Regenerates the mip map levels of the texture. Useful after locking and
			//! modifying the texture
			virtual void regenerateMipMapLevels(void* mipmapData = 0);

			//! creates the hardware texture
			bool createTexture(u32 flags, IImage **image);

			//! return shader resource view
			ID3D11ShaderResourceView* getShaderResourceView() const;

			//! target view
			ID3D11RenderTargetView* getTargetView(int id)
			{
				return RTView[id];
			}
		private:

			//! creates hardware render target
			void createRenderTarget(const ECOLOR_FORMAT format);

			bool createTextureBuffer();

			void getSurfaceInfo(int width, int height, DXGI_FORMAT fmt, int* outNumBytes, int* outRowBytes, int* outNumRows);

			int bitsPerPixel(DXGI_FORMAT fmt);

			bool createViews();
		};
	}
}

#endif

#endif

#endif
