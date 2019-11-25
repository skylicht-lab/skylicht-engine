// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_D3D11_MATERIAL_RENDERER_H_INCLUDED__
#define __C_D3D11_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CD3D11MaterialRenderer.h"
#include "IGPUProgrammingServices.h"

#include "IMaterialRenderer.h"

#include <d3d11.h>

typedef struct _D3D11_SHADER_BUFFER_DESC D3D11_SHADER_BUFFER_DESC;

namespace irr
{
namespace io
{
class IFileSystem;
}

namespace video
{

class IVideoDriver;
class IShaderConstantSetCallBack;
class IMaterialRenderer;
class CD3D11VertexDeclaration;
class CD3D11CallBridge;

struct SShaderBuffer 
{
	SShaderBuffer()
		: data(NULL), name(""), size(-1), cData(NULL)
	{
	}

	~SShaderBuffer()
	{
		if(cData)
			free(cData);
	}

	void AddRef()
	{
		if(data)
			data->AddRef();
	};

	void Release()
	{
		if(data)
			if(!data->Release())
			{
				delete this;
			}
	};

	// name of the buffer
	core::stringc name;

	// size in bytes
	s32 size;

	// data on the gpu
	ID3D11Buffer* data;

	// whole data that will be pushed to the gpu
	void* cData;
};

struct SShaderVariable 
{
	SShaderVariable()
		: offset(-1), name(""), buffer(NULL), size(-1), baseType(-1), classType(-1)
	{
	}

	SShaderBuffer* buffer;

	// name of the var
	core::stringc name;

	// offset in the buffer
	s32 offset;

	// eg sizeof(float)
	s32 size;

	// eg float, int etc
	s32 baseType;

	// eg matrix, vector etc
	s32 classType;
};

struct SShader
{
	SShader()
		: shader(NULL)
	{
		samplersUsed = 0;
		texturesUsed = 0;
		variableArrayPtr = NULL;
	}

	~SShader()
	{
		const u32 size = variableArray.size();

		for(u32 i = 0; i < size; ++i)
		{
			delete variableArray[i];
		}

		variableArray.clear();
		bufferArray.clear();
	}

	void AddRef()
	{
		const u32 size = bufferArray.size();

		for(u32 i = 0; i < size; ++i)
			bufferArray[i]->AddRef();

		if(shader)
			shader->AddRef();
	}

	void Release()
	{
		const u32 size = bufferArray.size();

		for(u32 i = 0; i < size; ++i)
		{
			bufferArray[i]->Release();
		}

		if(shader)
			if(!shader->Release())
			{
				delete this;
			}
	}

	// constant buffers in the shader
	core::array<SShaderBuffer*> bufferArray;
	
	// 
	u32 samplersUsed;
	u32 texturesUsed;

	// all vars in all buffers
	// all vars are in one array for quicker access (no searching is needed)
	core::array<SShaderVariable*> variableArray;
	SShaderVariable** variableArrayPtr;

	IUnknown* shader;
};

//! Class for using vertex and pixel shaders via HLSL with D3D11
class CD3D11MaterialRenderer : public IMaterialRenderer
{
public:

	//! Public constructor
	CD3D11MaterialRenderer(ID3D11Device* device, video::IVideoDriver* driver, CD3D11CallBridge* bridgeCalls, s32& outMaterialTypeNr,
		const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName, E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName, E_PIXEL_SHADER_TYPE psCompileTarget,
		const c8* geometryShaderProgram, const c8* geometryShaderEntryPointName, E_GEOMETRY_SHADER_TYPE gsCompileTarget,
		scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType, u32 verticesOut, E_VERTEX_TYPE vertexTypeOut,				// Only for DirectX 11
		IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData, io::IFileSystem* fileSystem);

	//! Destructor
	virtual ~CD3D11MaterialRenderer();

	//! isTransparent
	virtual bool isTransparent() const;

	//! uploadVariableToGPU
	bool uploadVariableToGPU(E_SHADER_TYPE type);

	//! sets a variable in the shader.
	//! \param id: Id of the variable
	//! \param floats: Pointer to array of floats
	//! \param count: Amount of floats in array.
	//! \param type: Shader type.
	virtual bool setVariable(s32 id, const f32* floats, int count, E_SHADER_TYPE type);

	virtual bool setVariable(s32 id, const s32* ints, int count, E_SHADER_TYPE type);

	virtual s32 getVariableID(const c8* name,E_SHADER_TYPE type);

	virtual s32 getConstantBufferID(const c8* name,E_SHADER_TYPE type);

	//! sets a constant buffer in the shader (only use this if you are know what you are doing).
	//! Hint: http://blog.signalsondisplay.com/?p=244
	//! \param id: Id of the constant buffer
	//! \param data: Pointer to a structure that represents the buffer
	//! \param type: Shader type.
	virtual bool setConstantBuffer(s32 id, const void* data, E_SHADER_TYPE type);

	virtual bool OnRender(IMaterialRendererServices* service, bool updateConstant);

	virtual void OnSetMaterial(const video::SMaterial& material, const video::SMaterial& lastMaterial, bool resetAllRenderstates, video::IMaterialRendererServices* services);
	
	virtual void OnUnsetMaterial();

	//! get shader signature
	virtual void* getShaderByteCode() const;

	//! get shader signature size
	virtual u32 getShaderByteCodeSize() const;

	//! get shader id
	virtual s32 getShaderVariableID(const c8* name, E_SHADER_TYPE shaderType);

	//! set shader value
	virtual void setShaderVariable(s32 id, const f32 *value, int count, E_SHADER_TYPE shaderType);

protected:
	CD3D11MaterialRenderer(ID3D11Device* device, video::IVideoDriver* driver, CD3D11CallBridge* bridgeCalls, IShaderConstantSetCallBack* callback, IMaterialRenderer* baseRenderer, io::IFileSystem* fileSystem = 0, s32 userData = 0);

	bool init(const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName, E_VERTEX_SHADER_TYPE vsCompileTarget,
			  const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName, E_PIXEL_SHADER_TYPE psCompileTarget,
			  const c8* geometryShaderProgram = NULL, const c8* geometryShaderEntryPointName = NULL, E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_COUNT,
			  const c8* hullShaderProgram = NULL, const c8* hullShaderEntryPointName = NULL, E_HULL_SHADER_TYPE hsCompileTarget = EHST_COUNT,
			  const c8* domainShaderProgram = NULL, const c8* domainShaderEntryPointName = NULL, E_DOMAIN_SHADER_TYPE dsCompileTarget = EDST_COUNT,
			  const c8* computeShaderProgram = NULL, const c8* computeShaderEntryPointName = NULL, E_COMPUTE_SHADER_TYPE csCompileTarget = ECST_COUNT);

	bool createShader(const char* code, const char* entryPointName, const char* targetName, UINT flags, E_SHADER_TYPE type);

	SShaderBuffer* createConstantBuffer(D3D11_SHADER_BUFFER_DESC& bufferDesc);

	void createResources(ID3D10Blob* program, E_SHADER_TYPE type);

	void printVariables(E_SHADER_TYPE type);
	void printBuffers(E_SHADER_TYPE type);

	void addMacros(core::array<D3D_SHADER_MACRO>& macroArray);

	bool compileShader(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude,
		LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode);
	
	SShaderVariable* getVariable( SShader* sh, s32 id );
	SShaderBuffer* getBuffer( E_SHADER_TYPE type, s32 id );

	// DX 11 objects
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;

	IShaderConstantSetCallBack* CallBack;
	// Irrlicht objects
	IVideoDriver* Driver;
	E_MATERIAL_TYPE BaseMaterial;

	SMaterial CurrentMaterial;

	s32 UserData;

	ID3D10Blob* buffer;

	ID3DInclude* includer;

	SShader* shaders[EST_COUNT];

	CD3D11CallBridge* BridgeCalls;
};

} // end namespace video
} // end namespace irr

#endif
#endif
#endif