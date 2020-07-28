// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#include "pch.h"
#include "IrrCompileConfig.h"
#include "CD3D11Driver.h"
#include "CD3D11RWBuffer.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "irrOS.h"

namespace irr
{
	namespace video
	{
		CD3D11RWBuffer::CD3D11RWBuffer(CD3D11Driver *driver, ECOLOR_FORMAT format, u32 numElements, void *initialData) :
			IRWBuffer(format, numElements),
			Driver(driver)
		{
			DriverType = EDT_DIRECT3D11;

			D3DFormat = Driver->getD3DFormatFromColorFormat(format);
			u32 bytePerPixel = Driver->getBitsPerPixel(D3DFormat) / 8;

			Device = driver->getExposedVideoData().D3D11.D3DDev11;
			if (Device)
			{
				Device->AddRef();
				Device->GetImmediateContext(&Context);
			}

			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.ByteWidth = bytePerPixel * numElements;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;

			if (initialData == NULL)
				Device->CreateBuffer(&bufferDesc, NULL, &Buffer);
			else
			{
				// Load initial data
				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = initialData;
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;

				Device->CreateBuffer(&bufferDesc, &data, &Buffer);
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = D3DFormat;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.ElementOffset = 0;
			srvDesc.Buffer.ElementWidth = numElements;
			Device->CreateShaderResourceView(Buffer, &srvDesc, &SRView);

			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = D3DFormat;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.NumElements = numElements;
			Device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView);
		}

		CD3D11RWBuffer::~CD3D11RWBuffer()
		{
			SRView->Release();
			UAView->Release();
			Buffer->Release();

			Context->Release();
			Device->Release();
		}

		//! Lock function.
		void* CD3D11RWBuffer::lock(bool readOnly)
		{
			if (!Buffer)
				return 0;

			if (readOnly)
				LastMapDirection = D3D11_MAP_READ;
			else
				LastMapDirection = (D3D11_MAP)(D3D11_MAP_WRITE | D3D11_MAP_READ);

			// Otherwise, map this buffer
			D3D11_MAPPED_SUBRESOURCE mappedData;
			HRESULT hr = Context->Map(Buffer, 0, LastMapDirection, 0, &mappedData);
			if (FAILED(hr))
				return 0;

			return mappedData.pData;
		}

		//! Unlock function. Must be called after a lock() to the buffer.
		void CD3D11RWBuffer::unlock()
		{
			if (!Buffer)
				return;

			// Otherwise, unmap this
			Context->Unmap(Buffer, 0);
		}
	}
}

#endif