#include "pch.h"
#include "CD3D11VideoRT.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CD3D11Driver.h"

namespace irr
{
	namespace video
	{
		CD3D11VideoRT::CD3D11VideoRT(void *hwnd, u32 w, u32 h, 
			CD3D11Driver* driver,
			ID3D11Device* device,
			ID3D11DeviceContext* context,
			IDXGIFactory* DXGIFactory, 
			DXGI_FORMAT D3DFormat,
			const SIrrlichtCreationParameters& params)
			:m_width(w), m_height(h),
			SwapChain(NULL), DefaultBackBuffer(NULL), DefaultDepthBuffer(NULL),
			Device(device), Context(context), Driver(driver),
			D3DColorFormat(D3DFormat)
		{
			m_hwnd = reinterpret_cast<HWND>(hwnd);

			// Preparing for swap chain creation
			::ZeroMemory(&Present, sizeof(DXGI_SWAP_CHAIN_DESC));

			Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			Present.BufferCount = 1;
			Present.BufferDesc.Format = D3DFormat;
			Present.BufferDesc.Width = w;
			Present.BufferDesc.Height = h;
			Present.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			
			Present.SampleDesc.Count = 1;
			Present.SampleDesc.Quality = 0;
			Present.OutputWindow = (HWND)hwnd;

			if (params.Vsync)
				Present.BufferDesc.RefreshRate.Numerator = 60;
			else
				Present.BufferDesc.RefreshRate.Numerator = 0;

			Present.BufferDesc.RefreshRate.Denominator = 1;
			Present.Windowed = true;

			// check anti alias, ONLY if driver is pure hardware (just for performance reasons)
			if (params.AntiAlias > 0 && DriverType != D3D_DRIVER_TYPE_WARP)
			{
				int antiAlias = params.AntiAlias;

				if (antiAlias > 32)
					antiAlias = 32;

				UINT qualityLevels = 0;

				while (params.AntiAlias > 0)
				{
					if (SUCCEEDED(Device->CheckMultisampleQualityLevels(Present.BufferDesc.Format, antiAlias, &qualityLevels)))
					{
						Present.SampleDesc.Count = params.AntiAlias;
						Present.SampleDesc.Quality = qualityLevels - 1;
						Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
						break;
					}
					--antiAlias;
				}

				if (antiAlias == 0)
				{
					os::Printer::log("Anti aliasing disabled because hardware/driver lacks necessary caps.", ELL_WARNING);
				}
			}

			SampleCount = Present.SampleDesc.Count;
			SampleQuanlity = Present.SampleDesc.Quality;

			// Create swap chain
			IDXGIDevice1* DXGIDevice = NULL;
			Device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&DXGIDevice));

			HRESULT hr = DXGIFactory->CreateSwapChain(DXGIDevice, &Present, &SwapChain);

			if (FAILED(hr))
			{
				// Try a second time, may fail the first time due to back buffer count
				hr = DXGIFactory->CreateSwapChain(DXGIDevice, &Present, &SwapChain);
				if (FAILED(hr))
				{
					logFormatError(hr, "CD3D11VideoRT: Could not create swap chain");
					return;
				}
			}
			DXGIDevice->Release();

			// Get default render target
			ID3D11Texture2D* backBuffer = NULL;
			hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
			if (FAILED(hr))
			{
				logFormatError(hr, "CD3D11VideoRT: Could not get back buffer");
				return;
			}

			hr = Device->CreateRenderTargetView(backBuffer, NULL, &DefaultBackBuffer);
			if (FAILED(hr))
			{
				logFormatError(hr, "CD3D11VideoRT: Could not create render target view");
				return;
			}
			backBuffer->Release();
			
			ScreenSize = core::dimension2du(w, h);

			DefaultDepthBuffer = driver->createDepthStencilView(ScreenSize, SampleCount, SampleQuanlity);
		}

		CD3D11VideoRT::~CD3D11VideoRT()
		{
			if (SwapChain)
				SwapChain->Release();

			if (DefaultDepthBuffer)
				DefaultDepthBuffer->Release();

			if (DefaultBackBuffer)
				DefaultBackBuffer->Release();
		}

		u32 CD3D11VideoRT::getWidth()
		{
			return m_width;
		}

		u32 CD3D11VideoRT::getHeight()
		{
			return m_height;
		}

		void CD3D11VideoRT::resize(u32 w, u32 h)
		{
			os::Printer::log("CD3D11VideoRT resize.", ELL_INFORMATION);

			m_width = w;
			m_height = h;

			ScreenSize = core::dimension2du(w, h);

			// unbind render targets
			ID3D11RenderTargetView* views[] = { NULL };
			Context->OMSetRenderTargets(1, views, NULL);

			if (DefaultDepthBuffer)
				DefaultDepthBuffer->Release();

			if (DefaultBackBuffer)
				DefaultBackBuffer->Release();

			HRESULT hr = S_OK;
			hr = SwapChain->ResizeBuffers(1, ScreenSize.Width, ScreenSize.Height, D3DColorFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
			if (FAILED(hr))
			{
				logFormatError(hr, "CD3D11VideoRT: Could not resize back buffer");
				return;
			}

			// Get default render target
			ID3D11Texture2D* backBuffer = NULL;
			hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
			if (FAILED(hr))
			{
				logFormatError(hr, "CD3D11VideoRT: Could not get back buffer");
				return;
			}

			hr = Device->CreateRenderTargetView(backBuffer, NULL, &DefaultBackBuffer);
			if (FAILED(hr))
			{
				logFormatError(hr, "CD3D11VideoRT: Could not create render target view");
				return;
			}

			backBuffer->Release();

			// create depth buffer
			DefaultDepthBuffer = Driver->createDepthStencilView(ScreenSize, SampleCount, SampleQuanlity);
		}
	}
}
#endif