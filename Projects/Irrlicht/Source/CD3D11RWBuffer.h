// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#ifndef __C_DIRECTX11_RWBUFFER_H_INCLUDED__
#define __C_DIRECTX11_RWBUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "IRWBuffer.h"

namespace irr
{
	namespace video
	{
		class CD3D11Driver;

		class CD3D11RWBuffer : public IRWBuffer
		{
		public:
			CD3D11RWBuffer(CD3D11Driver *driver, ECOLOR_FORMAT format, u32 numElements);

			~CD3D11RWBuffer();

		protected:

			CD3D11Driver *Driver;

			ID3D11Device* Device;
			ID3D11DeviceContext* Context;

			ID3D11Buffer* Buffer;
			ID3D11ShaderResourceView* SRView;
			ID3D11UnorderedAccessView* UAView;

			DXGI_FORMAT D3DFormat;
		};
}
}

#endif
#endif
#endif