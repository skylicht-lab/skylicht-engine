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
			ShaderBuffer(NULL),
			VariableArrayPtr(NULL)
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

			initConstant();

			return true;
		}

		s32 CD3D11GPUCompute::getVariableID(const c8* name)
		{
			const u32 size = VariableArray.size();

			for (u32 i = 0; i < size; ++i)
			{
				if (VariableArray[i]->name == name)
					return i;
			}

			core::stringc s = "HLSL variable to get ID not found: '";
			s += name;
			s += "'. Available variables are:";
			os::Printer::log(s.c_str(), ELL_WARNING);

			return -1;
		}

		bool CD3D11GPUCompute::setVariable(s32 id, const f32* floats, int count)
		{
			SShaderVariable* var = VariableArrayPtr[id];

			if (!var)
				return false;

			SShaderBuffer* buff = var->buffer;

			c8* byteData = (c8*)buff->cData;
			byteData += var->offset;

			if (var->classType == D3D10_SVC_MATRIX_COLUMNS)
			{
				// transpose matrix		
				int numMatrix = count / 16;

				float *m = (float*)byteData;
				const float *v = floats;

				for (int i = 0; i < numMatrix; i++)
				{
					m[0] = v[0];
					m[1] = v[4];
					m[2] = v[8];
					m[3] = v[12];

					m[4] = v[1];
					m[5] = v[5];
					m[6] = v[9];
					m[7] = v[13];

					m[8] = v[2];
					m[9] = v[6];
					m[10] = v[10];
					m[11] = v[14];

					m[12] = v[3];
					m[13] = v[7];
					m[14] = v[11];
					m[15] = v[15];

					m += 16;
					v += 16;
				}
			}
			else
			{
				memcpy(byteData, floats, count * sizeof(f32));
			}

			return true;
		}

		//! uploadVariableToGPU
		bool CD3D11GPUCompute::uploadVariableToGPU()
		{
			for (int i = 0, n = BufferArray.size(); i < n; i++)
			{
				SShaderBuffer* buff = BufferArray[i];

				// do it later
				D3D11_MAPPED_SUBRESOURCE mappedData;

				HRESULT hr = Context->Map(buff->data, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

				if (FAILED(hr))
				{
					logFormatError(hr, "Could not map float variable in shader");
					return false;
				}

				memcpy(mappedData.pData, buff->cData, buff->size);

				Context->Unmap(buff->data, 0);
			}

			return true;
		}

		bool CD3D11GPUCompute::initConstant()
		{
			// D3DXCompile
			typedef HRESULT(WINAPI *D3DX11ReflectFunc)(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);

			static D3DX11ReflectFunc pFn = 0;
			static bool LoadFailed = false;

			if (LoadFailed)
				return false;

			ID3D11ShaderReflection* pReflector = NULL;
			HRESULT hr = D3DReflect(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);

			if (FAILED(hr))
			{
				logFormatError(hr, "Could not reflect shader");
				return false;
			}

			D3D11_SHADER_DESC shaderDesc;
			pReflector->GetDesc(&shaderDesc);

			for (u32 i = 0; i < shaderDesc.BoundResources; ++i)
			{
				D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
				pReflector->GetResourceBindingDesc(i, &resourceDesc);

				switch (resourceDesc.Type)
				{
				case D3D_SIT_CBUFFER:
				{
					ID3D11ShaderReflectionConstantBuffer* reflectionBuffer = pReflector->GetConstantBufferByName(resourceDesc.Name);

					D3D11_SHADER_BUFFER_DESC bufferDesc;
					reflectionBuffer->GetDesc(&bufferDesc);

					SShaderBuffer* sBuffer = createConstantBuffer(bufferDesc);

					if (sBuffer)
					{
						BufferArray.push_back(sBuffer);

						// add vars to shader
						for (u32 j = 0; j < bufferDesc.Variables; j++)
						{
							ID3D11ShaderReflectionVariable* var = reflectionBuffer->GetVariableByIndex(j);

							D3D11_SHADER_VARIABLE_DESC varDesc;
							var->GetDesc(&varDesc);

							D3D11_SHADER_TYPE_DESC typeDesc;
							var->GetType()->GetDesc(&typeDesc);

							SShaderVariable* sv = new SShaderVariable();
							sv->name = varDesc.Name;
							sv->buffer = sBuffer;
							sv->offset = varDesc.StartOffset;
							sv->size = varDesc.Size;
							sv->baseType = typeDesc.Type;
							sv->classType = typeDesc.Class;

							VariableArray.push_back(sv);
						}
					}

					break;
				}
				case D3D_SIT_TBUFFER:
				{
					// same as cbuffer?
					break;
				}
				case D3D_SIT_SAMPLER:
				{
					break;
				}
				case D3D_SIT_TEXTURE:
				{
					break;
				}
				}
			}

			VariableArrayPtr = VariableArray.pointer();

			pReflector->Release();

			return true;
		}

		SShaderBuffer* CD3D11GPUCompute::createConstantBuffer(D3D11_SHADER_BUFFER_DESC& bufferDesc)
		{
			SShaderBuffer* sBuffer = NULL;

			// take the same buffer from the other shader if it has the same name
			bool found = false;

			for (u32 j = 0; j < BufferArray.size(); ++j)
			{
				if (BufferArray[j]->name == bufferDesc.Name)
				{
					sBuffer = BufferArray[j];
					sBuffer->AddRef();
					found = true;
					break;
				}
			}

			// no buffer found so create a new one
			if (!sBuffer)
			{
				sBuffer = new SShaderBuffer();
				sBuffer->name = bufferDesc.Name;
				sBuffer->size = bufferDesc.Size;
			}

			if (!sBuffer->data)
			{
				D3D11_BUFFER_DESC cbDesc;
				cbDesc.ByteWidth = sBuffer->size;
				cbDesc.Usage = D3D11_USAGE_DYNAMIC;
				cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbDesc.MiscFlags = 0;
				cbDesc.StructureByteStride = 0;

				// Create the buffer.
				HRESULT hr = Device->CreateBuffer(&cbDesc, NULL, &sBuffer->data);

				if (FAILED(hr))
				{
					core::stringc error = "Could not create constant buffer \"";
					error += sBuffer->name;
					error += "\"";

					logFormatError(hr, error);

					delete sBuffer;

					return NULL;
				}

				sBuffer->cData = malloc(sBuffer->size);
			}

			return sBuffer;
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
				ID3D11UnorderedAccessView* unorderedAccessView = NULL;
				if (BufferSlot[i])
					unorderedAccessView = ((CD3D11RWBuffer*)BufferSlot[i])->getUnorderedAccessView();
				Context->CSSetUnorderedAccessViews(i, 1, &unorderedAccessView, NULL);
			}

			// update constant buffer to GPU
			uploadVariableToGPU();

			u32 size = BufferArray.size();
			if (size > 0)
			{
				core::array<ID3D11Buffer*> buffs;
				buffs.reallocate(size);

				for (u32 i = 0; i < size; ++i)
					buffs.push_back(BufferArray[i]->data);

				Context->CSSetConstantBuffers(0, size, &buffs[0]);
			}

			// do gpu compute
			Context->Dispatch(threadGroupX, threadGroupY, threadGroupZ);			
		}
	}
}

#endif