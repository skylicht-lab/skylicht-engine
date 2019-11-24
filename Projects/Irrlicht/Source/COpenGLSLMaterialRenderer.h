// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#ifdef _IRR_WINDOWS_API_
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
	#include "glext.h"
#else
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_LEGACY 1
#else
	#define GL_GLEXT_PROTOTYPES 1
#endif
#if defined(_IRR_OSX_PLATFORM_)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#include "glext.h"
#endif
#endif


#include "EMaterialTypes.h"
#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"

namespace irr
{
namespace video
{

class COpenGLDriver;
class IShaderConstantSetCallBack;

//! Class for using GLSL shaders with OpenGL
//! Please note: This renderer implements its own IMaterialRendererServices
class COpenGLSLMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:

	//! Constructor
	COpenGLSLMaterialRenderer(
		COpenGLDriver* driver,
		s32& outMaterialTypeNr,
		const c8* vertexShaderProgram = 0,
		const c8* vertexShaderEntryPointName = 0,
		E_VERTEX_SHADER_TYPE vsCompileTarget = video::EVST_VS_1_1,
		const c8* pixelShaderProgram = 0,
		const c8* pixelShaderEntryPointName = 0,
		E_PIXEL_SHADER_TYPE psCompileTarget = video::EPST_PS_1_1,
		const c8* geometryShaderProgram = 0,
		const c8* geometryShaderEntryPointName = "main",
		E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
		scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
		scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
		u32 verticesOut = 0,
		IShaderConstantSetCallBack* callback = 0,
		E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
		s32 userData = 0);

	//! Destructor
	virtual ~COpenGLSLMaterialRenderer();

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services) _IRR_OVERRIDE_;

	virtual bool OnRender(IMaterialRendererServices* service, bool useCacheConstant) _IRR_OVERRIDE_;

	virtual void OnUnsetMaterial() _IRR_OVERRIDE_;

	//! Returns if the material is transparent.
	virtual bool isTransparent() const _IRR_OVERRIDE_;

	// implementations for the render services
	virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates) _IRR_OVERRIDE_;
	virtual s32 getVertexShaderConstantID(const c8* name);
	virtual s32 getPixelShaderConstantID(const c8* name);
	virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count);
	virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count);
	virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count);
	virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count);
	virtual IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

	//! get shader id
	virtual s32 getShaderVariableID(const c8* name, E_SHADER_TYPE shaderType);

	//! set shader value
	virtual void setShaderVariable(s32 id, const f32 *value, int count, E_SHADER_TYPE shaderType);

protected:

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	COpenGLSLMaterialRenderer(COpenGLDriver* driver,
					IShaderConstantSetCallBack* callback,
					E_MATERIAL_TYPE baseMaterial,
					s32 userData=0);

	void init(s32& outMaterialTypeNr,
		const c8* vertexShaderProgram,
		const c8* pixelShaderProgram,
		const c8* geometryShaderProgram,
		scene::E_PRIMITIVE_TYPE inType=scene::EPT_TRIANGLES,
		scene::E_PRIMITIVE_TYPE outType=scene::EPT_TRIANGLE_STRIP,
		u32 verticesOut=0);

	bool createProgram();
	bool createShader(GLenum shaderType, const char* shader);
	bool linkProgram();

	COpenGLDriver* Driver;
	IShaderConstantSetCallBack* CallBack;

	bool Alpha;
	bool Blending;
	bool FixedBlending;
	bool AlphaTest;

	struct SUniformInfo
	{
		core::stringc name;
		GLenum type;
		GLint location;
	};

	GLuint Program;
	core::array<SUniformInfo> UniformInfo;
	s32 UserData;
};


} // end namespace video
} // end namespace irr

#endif // compile with OpenGL
#endif // if included

