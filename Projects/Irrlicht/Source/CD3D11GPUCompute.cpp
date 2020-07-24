// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#include "pch.h"
#include "IrrCompileConfig.h"
#include "CD3D11Driver.h"
#include "CD3D11Texture.h"
#include "CD3D11RWBuffer.h"
#include "CD3D11GPUCompute.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "irrOS.h"

#include "d3dcompiler.h"

namespace irr
{
	namespace video
	{
		CD3D11GPUCompute::CD3D11GPUCompute(CD3D11Driver *driver) :
			ComputeShader(NULL),
			ShaderBuffer(NULL)
		{
			DriverType = EDT_DIRECT3D11;

			Device = driver->getExposedVideoData().D3D11.D3DDev11;
			if (Device)
			{
				Device->AddRef();
				Device->GetImmediateContext(&Context);
			}

			for (int i = 0; i < NUM_PARAMS_SUPPORT; i++)
			{
				TextureSlot[i] = NULL;
				BufferSlot[i] = NULL;
			}
		}

		CD3D11GPUCompute::~CD3D11GPUCompute()
		{
			Device->Release();
			Context->Release();

			if (ComputeShader != NULL)
				ComputeShader->Release();

			if (ShaderBuffer != NULL)
				ShaderBuffer->Release();
		}

		bool CD3D11GPUCompute::compile(const c8* computeShaderProgram,
			const c8* computeShaderEntryPointName,
			E_COMPUTE_SHADER_TYPE csCompileTarget)
		{
			ID3D10Blob* errorMsgs = 0;

			ID3DInclude* includer = NULL;

			UINT flags = 0;

#if !defined(WINDOWS_STORE)
			if (csCompileTarget >= ECST_CS_5_0)
				flags |= D3D10_SHADER_ENABLE_STRICTNESS;
			else
			{
				flags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
				csCompileTarget = ECST_CS_4_0;
			}

#ifdef _DEBUG
			// These values allow use of PIX and shader debuggers
			flags |= D3D10_SHADER_DEBUG;
			flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#else
			// These flags allow maximum performance
			flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

			flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

			// last macro has to be NULL
			core::array<D3D_SHADER_MACRO> macroArray;

			D3D_SHADER_MACRO macro;
			macro.Definition = NULL;
			macro.Name = NULL;

			macroArray.push_back(macro);

			HRESULT hr = D3DCompile(
				computeShaderProgram,
				strlen(computeShaderProgram),
				"",
				&macroArray[0],
				includer,
				computeShaderEntryPointName,
				COMPUTE_SHADER_TYPE_NAMES[csCompileTarget],
				flags, 0,
				&ShaderBuffer,
				&errorMsgs);

			if (FAILED(hr))
			{
				core::stringc errorMsg = "Could not compile shader";

				if (errorMsgs)
				{
					errorMsg += ": ";
					errorMsg += static_cast<const char*>(errorMsgs->GetBufferPointer());

					errorMsgs->Release();
				}

				logFormatError(hr, errorMsg);

				return false;
			}
#ifdef _DEBUG
			else if (errorMsgs)
			{
				core::stringc errorMsg = "Shader compilation warning: ";
				errorMsg += static_cast<const char*>(errorMsgs->GetBufferPointer());

				errorMsgs->Release();
				errorMsgs = NULL;

				os::Printer::log(errorMsg.c_str(), ELL_WARNING);
			}
#endif

			if (errorMsgs)
				errorMsgs->Release();

			if (!ShaderBuffer)
				return false;

			hr = Device->CreateComputeShader(
				ShaderBuffer->GetBufferPointer(),
				ShaderBuffer->GetBufferSize(),
				NULL,
				&ComputeShader);

			if (FAILED(hr))
			{
				core::stringc errorMsg = "Could not create computeshader";
				logFormatError(hr, errorMsg);
				return false;
			}

			return true;
		}

		void CD3D11GPUCompute::setTexture(int slot, ITexture *texture)
		{
			TextureSlot[slot] = texture;
		}

		void CD3D11GPUCompute::setBuffer(int slot, IRWBuffer *buffer)
		{
			BufferSlot[slot] = buffer;
		}

		void CD3D11GPUCompute::dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			Context->CSSetShader(ComputeShader, NULL, 0);

			for (int i = 0; i < NUM_PARAMS_SUPPORT; i++)
			{
				// Texture resource view
				ID3D11ShaderResourceView* views = NULL;
				if (TextureSlot[i])
					views = ((CD3D11Texture*)TextureSlot[i])->getShaderResourceView();
				Context->CSSetShaderResources(i, 1, &views);

				// Buffer unorderred access view
				ID3D11UnorderedAccessView* unorderedAccessView;
				if (BufferSlot[i])
					unorderedAccessView = ((CD3D11RWBuffer*)BufferSlot[i])->getUnorderedAccessView();
				Context->CSSetUnorderedAccessViews(i, 1, &unorderedAccessView, NULL);
			}

			// do gpu compute
			Context->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}
	}
}

#endif