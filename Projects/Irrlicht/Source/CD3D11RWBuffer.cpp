// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#include "pch.h"
#include "IrrCompileConfig.h"
#include "CD3D11Driver.h"
#include "CD3D11RWBuffer.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "irrOS.h"

namespace irr
{
	namespace video
	{
		CD3D11RWBuffer::CD3D11RWBuffer(CD3D11Driver *driver, ECOLOR_FORMAT format, u32 numElements) :
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
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			Device->CreateBuffer(&bufferDesc, NULL, &Buffer);

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
	}
}

#endif