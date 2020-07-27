// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#ifndef __C_DIRECTX11_GPUCOMPUTE_H_INCLUDED__
#define __C_DIRECTX11_GPUCOMPUTE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define NUM_PARAMS_SUPPORT 4

#include "IGPUCompute.h"

namespace irr
{
	namespace video
	{
		class CD3D11Driver;

		class CD3D11GPUCompute : public IGPUCompute
		{
		protected:
			ID3D11Device* Device;
			ID3D11DeviceContext* Context;

			ID3D11ComputeShader *ComputeShader;
			ID3D10Blob* ShaderBuffer;

			ITexture *TextureSlot[NUM_PARAMS_SUPPORT];
			IRWBuffer *BufferSlot[NUM_PARAMS_SUPPORT];

			core::array<SShaderBuffer*> BufferArray;
			core::array<SShaderVariable*> VariableArray;
			SShaderVariable** VariableArrayPtr;

		public:
			CD3D11GPUCompute(CD3D11Driver *driver);

			virtual ~CD3D11GPUCompute();

			bool compile(const c8* computeShaderProgram,
				const c8* computeShaderEntryPointName = "main",
				E_COMPUTE_SHADER_TYPE csCompileTarget = ECST_CS_5_0);
			
			bool setVariable(s32 id, const f32* floats, int count);

			s32 getVariableID(const c8* name);

			virtual void setTexture(int slot, ITexture *texture);

			virtual void setBuffer(int slot, IRWBuffer *buffer);

			virtual void dispatch(int threadGroupX, int threadGroupY, int threadGroupZ);

		protected:
			
			bool initConstant();

			SShaderBuffer* createConstantBuffer(D3D11_SHADER_BUFFER_DESC& bufferDesc);

			bool uploadVariableToGPU();
		};
	}
}

#endif
#endif
#endif