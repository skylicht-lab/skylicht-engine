#ifndef __C_VIDEO_DIRECTX_11_VRT_H_INCLUDED__
#define __C_VIDEO_DIRECTX_11_VRT_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "IVideoDriver.h"
#include "SIrrCreationParameters.h"
#include "irrOS.h"

#include <d3d11.h>

namespace irr
{
	namespace video
	{
		class CD3D11Driver;

		class CD3D11VideoRT : public IVideoRenderTarget
		{
		protected:
			HWND m_hwnd;
			u32 m_width;
			u32 m_height;

		public:
			DXGI_SWAP_CHAIN_DESC	Present;		

			UINT SampleCount;
			UINT SampleQuanlity;
			IDXGISwapChain* SwapChain;

			core::dimension2du ScreenSize;
			ID3D11RenderTargetView* DefaultBackBuffer;
			ID3D11DepthStencilView* DefaultDepthBuffer;

			CD3D11Driver* Driver;
			ID3D11Device* Device;
			ID3D11DeviceContext* Context;

			DXGI_FORMAT D3DColorFormat;
		public:
			CD3D11VideoRT(void *hwnd, u32 w, u32 h, 
				CD3D11Driver* driver,
				ID3D11Device* Device,
				ID3D11DeviceContext* Context,
				IDXGIFactory* DXGIFactory,
				DXGI_FORMAT D3DFormat, 
				const SIrrlichtCreationParameters& params);
			
			virtual ~CD3D11VideoRT();

			virtual u32 getWidth();

			virtual u32 getHeight();

			virtual void resize(u32 w, u32 h);
		};
	}
}

#endif

#endif