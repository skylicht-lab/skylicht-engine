// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// This file was originally written by William Finlayson.  I (Nikolaus
// Gebhardt) did some minor modifications and changes to it and integrated it
// into Irrlicht. Thanks a lot to William for his work on this and that he gave
// me his permission to add it into Irrlicht using the zlib license.

// After Irrlicht 0.12, Michael Zoech did some improvements to this renderer, I
// merged this into Irrlicht 0.14, thanks to him for his work.

#include "pch.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLSLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "irrOS.h"
#include "COpenGLDriver.h"

namespace irr
{
namespace video
{


//! Constructor
COpenGLSLMaterialRenderer::COpenGLSLMaterialRenderer(video::COpenGLDriver* driver,
		s32& outMaterialTypeNr, const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderProgram,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		const c8* geometryShaderProgram,
		const c8* geometryShaderEntryPointName,
		E_GEOMETRY_SHADER_TYPE gsCompileTarget,
		scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
		u32 verticesOut,
		IShaderConstantSetCallBack* callback,
		E_MATERIAL_TYPE baseMaterial,
		s32 userData)
	: Driver(driver), CallBack(callback), Alpha(false), Blending(false), FixedBlending(false), AlphaTest(false), Program(0), UserData(userData)
{
	#ifdef _DEBUG
	setDebugName("COpenGLSLMaterialRenderer");
	#endif

	switch (baseMaterial)
	{
	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		Alpha = true;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
		FixedBlending = true;
		break;	
	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		AlphaTest = true;
		break;
	default:
		break;
	}

	if (CallBack)
		CallBack->grab();

	if (!Driver->queryFeature(EVDF_ARB_GLSL))
		return;

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram, geometryShaderProgram);
}


//! constructor only for use by derived classes who want to
//! create a fall back material for example.
COpenGLSLMaterialRenderer::COpenGLSLMaterialRenderer(COpenGLDriver* driver,
					IShaderConstantSetCallBack* callback,
					E_MATERIAL_TYPE baseMaterial, s32 userData)
: Driver(driver), CallBack(callback), Alpha(false), Blending(false), FixedBlending(false), AlphaTest(false), Program(0), UserData(userData)
{
	switch (baseMaterial)
	{
	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		Alpha = true;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
		FixedBlending = true;
		break;	
	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		AlphaTest = true;
		break;
	default:
		break;
	}

	if (CallBack)
		CallBack->grab();
}


//! Destructor
COpenGLSLMaterialRenderer::~COpenGLSLMaterialRenderer()
{
	if (Program)
	{
		if (Driver->getActiveGLSLProgram() == Program)
			Driver->setActiveGLSLProgram(0);
	}

	if (CallBack)
		CallBack->drop();

	if (Program)
	{
		GLuint shaders[8];
		GLint count;
		Driver->extGlGetAttachedShaders(Program, 8, &count, shaders);
		// avoid bugs in some drivers, which return larger numbers
		// use int variable to avoid compiler problems with template
		int mincount=core::min_((int)count,8);
		for (int i=0; i<mincount; ++i)
			Driver->extGlDeleteShader(shaders[i]);
		Driver->extGlDeleteProgram(Program);
		Program = 0;
	}

	UniformInfo.clear();
}


void COpenGLSLMaterialRenderer::init(s32& outMaterialTypeNr,
		const c8* vertexShaderProgram,
		const c8* pixelShaderProgram,
		const c8* geometryShaderProgram,
		scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
		u32 verticesOut)
{
	outMaterialTypeNr = -1;

	if (!createProgram())
		return;

#if defined(GL_ARB_vertex_shader) && defined (GL_ARB_fragment_shader)
	if (vertexShaderProgram)
		if (!createShader(GL_VERTEX_SHADER_ARB, vertexShaderProgram))
			return;

	if (pixelShaderProgram)
		if (!createShader(GL_FRAGMENT_SHADER_ARB, pixelShaderProgram))
			return;
#endif

#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_program4) || defined(GL_NV_geometry_shader4)
	if (geometryShaderProgram && Driver->queryFeature(EVDF_GEOMETRY_SHADER))
	{
		if (!createShader(GL_GEOMETRY_SHADER_EXT, geometryShaderProgram))
			return;
#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_shader4)
		if (Program) // Geometry shaders are supported only in OGL2.x+ drivers.
		{
			Driver->extGlProgramParameteri(Program, GL_GEOMETRY_INPUT_TYPE_EXT, Driver->primitiveTypeToGL(inType));
			Driver->extGlProgramParameteri(Program, GL_GEOMETRY_OUTPUT_TYPE_EXT, Driver->primitiveTypeToGL(outType));
			if (verticesOut==0)
				Driver->extGlProgramParameteri(Program, GL_GEOMETRY_VERTICES_OUT_EXT, Driver->MaxGeometryVerticesOut);
			else
				Driver->extGlProgramParameteri(Program, GL_GEOMETRY_VERTICES_OUT_EXT, core::min_(verticesOut, Driver->MaxGeometryVerticesOut));
		}
#elif defined(GL_NV_geometry_program4)
		if (verticesOut==0)
			Driver->extGlProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, Driver->MaxGeometryVerticesOut);
		else
			Driver->extGlProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, core::min_(verticesOut, Driver->MaxGeometryVerticesOut));
#endif
	}
#endif

	if (!linkProgram())
		return;

	// register myself as new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}


bool COpenGLSLMaterialRenderer::OnRender(IMaterialRendererServices* service, bool updateConstant)
{
	// call callback to set shader constants
	if (CallBack && Program)
		CallBack->OnSetConstants(this, UserData, updateConstant);

	return true;
}


void COpenGLSLMaterialRenderer::OnSetMaterial(const video::SMaterial& material,
				const video::SMaterial& lastMaterial,
				bool resetAllRenderstates,
				video::IMaterialRendererServices* services)
{
	COpenGLCallBridge* bridgeCalls = Driver->getBridgeCalls();

	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
		if (Program)
			Driver->extGlUseProgram(Program);		
	}

	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	if (Alpha)
	{
		bridgeCalls->setBlend(true);
		bridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (FixedBlending)
	{
		bridgeCalls->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		bridgeCalls->setBlend(true);
	}
	else if (Blending)
	{
		E_BLEND_FACTOR srcRGBFact,dstRGBFact,srcAlphaFact,dstAlphaFact;
		E_MODULATE_FUNC modulate;
		u32 alphaSource;
		unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);

		if (Driver->queryFeature(EVDF_BLEND_SEPARATE))
        {
            bridgeCalls->setBlendFuncSeparate(Driver->getGLBlend(srcRGBFact), Driver->getGLBlend(dstRGBFact),
                Driver->getGLBlend(srcAlphaFact), Driver->getGLBlend(dstAlphaFact));
        }
        else
        {
            bridgeCalls->setBlendFunc(Driver->getGLBlend(srcRGBFact), Driver->getGLBlend(dstRGBFact));
        }

		bridgeCalls->setBlend(true);
	}
	else if (AlphaTest)
	{
		bridgeCalls->setAlphaTest(true);
		bridgeCalls->setAlphaFunc(GL_GREATER, 0.5f);
	}
	else
	{
		bridgeCalls->setBlend(false);
		bridgeCalls->setAlphaTest(false);
	}

	if (CallBack)
		CallBack->OnSetMaterial(material);

	if (Program)
		Driver->setActiveGLSLProgram(Program);	
	else
		Driver->setActiveGLSLProgram(0);
}


void COpenGLSLMaterialRenderer::OnUnsetMaterial()
{
	Driver->setActiveGLSLProgram(0);
	
	if (Program)
		Driver->extGlUseProgram(0);
}


//! Returns if the material is transparent.
bool COpenGLSLMaterialRenderer::isTransparent() const
{
	return (Alpha || Blending || FixedBlending);
}


bool COpenGLSLMaterialRenderer::createProgram()
{	
	Program = Driver->extGlCreateProgram();	
	return true;
}


bool COpenGLSLMaterialRenderer::createShader(GLenum shaderType, const char* shader)
{
	const char* code[32];
	int num = 0;
	code[num++] = "#version 400\n";
	code[num++] = 
		"#ifndef lowp\n#define lowp\n#endif\n"
		"#ifndef mediump\n#define mediump\n#endif\n"
		"#ifndef highp\n#define highp\n#endif\n"
		"#ifndef precision\n#define precision\n#endif\n"
		"#define OPENGL\n";
	code[num++] = shader;

	GLuint shaderHandle = Driver->extGlCreateShader(shaderType);
	Driver->extGlShaderSource(shaderHandle, 3, code, NULL);
	Driver->extGlCompileShader(shaderHandle);

	GLint status = 0;

#ifdef GL_VERSION_2_0
	Driver->extGlGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
#endif

	if (status != GL_TRUE)
	{
		if (GL_VERTEX_SHADER == shaderType)
			os::Printer::log("GLSL GL_VERTEX_SHADER shader failed to compile", ELL_ERROR);
		else if (GL_FRAGMENT_SHADER == shaderType)
			os::Printer::log("GLSL GL_FRAGMENT_SHADER shader failed to compile", ELL_ERROR);
		else
			os::Printer::log("GLSL shader failed to compile", ELL_ERROR);

		// check error message and log it
		GLint maxLength=0;
		GLint length;
#ifdef GL_VERSION_2_0
		Driver->extGlGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH,
				&maxLength);
#endif
		if (maxLength)
		{
			GLchar *infoLog = new GLchar[maxLength];
			Driver->extGlGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
			os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
			delete [] infoLog;
		}

		return false;
	}

	Driver->extGlAttachShader(Program, shaderHandle);
	
	return true;
}


bool COpenGLSLMaterialRenderer::linkProgram()
{
	Driver->extGlLinkProgram(Program);

	GLint status = 0;

#ifdef GL_VERSION_2_0
	Driver->extGlGetProgramiv(Program, GL_LINK_STATUS, &status);
#endif

	if (!status)
	{
		os::Printer::log("GLSL shader program failed to link", ELL_ERROR);
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;
#ifdef GL_VERSION_2_0
		Driver->extGlGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);
#endif
		if (maxLength)
		{
			GLchar *infoLog = new GLchar[maxLength];
			Driver->extGlGetProgramInfoLog(Program, maxLength, &length, infoLog);
			os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
			delete [] infoLog;
		}

		return false;
	}

	// get uniforms information

	GLint num = 0;
#ifdef GL_VERSION_2_0
	Driver->extGlGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num);
#endif

	if (num == 0)
	{
		// no uniforms
		return true;
	}

	GLint maxlen = 0;
#ifdef GL_VERSION_2_0
	Driver->extGlGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
#endif

	if (maxlen == 0)
	{
		os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
		return false;
	}

	// seems that some implementations use an extra null terminator
	++maxlen;
	c8 *buf = new c8[maxlen];

	UniformInfo.clear();
	UniformInfo.reallocate(num);

	for (GLint i=0; i < num; ++i)
	{
		SUniformInfo ui;
		memset(buf, 0, maxlen);

		GLint size;
		Driver->extGlGetActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar*>(buf));

        core::stringc name = "";

		// array support.
		for (u32 i = 0; buf[i] != '\0' && buf[i] != '['; ++i)
            name += buf[i];

		ui.name = name;
		ui.location = Driver->extGlGetUniformLocation(Program, name.c_str());

		UniformInfo.push_back(ui);
	}

	delete [] buf;
	return true;
}


void COpenGLSLMaterialRenderer::setBasicRenderStates(const SMaterial& material,
						const SMaterial& lastMaterial,
						bool resetAllRenderstates)
{
	// forward
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

s32 COpenGLSLMaterialRenderer::getVertexShaderConstantID(const c8* name)
{
	return getPixelShaderConstantID(name);
}

s32 COpenGLSLMaterialRenderer::getPixelShaderConstantID(const c8* name)
{
	for (u32 i = 0; i < UniformInfo.size(); ++i)
	{
		if (UniformInfo[i].name == name)
			return i;
	}

	return -1;
}

bool COpenGLSLMaterialRenderer::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	return setPixelShaderConstant(index, floats, count);
}

bool COpenGLSLMaterialRenderer::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
#ifdef GL_ARB_shader_objects
	if(index < 0 || UniformInfo[index].location < 0)
		return false;

	bool status = true;

	switch (UniformInfo[index].type)
	{
		case GL_FLOAT:
			Driver->extGlUniform1fv(UniformInfo[index].location, count, floats);
			break;
		case GL_FLOAT_VEC2:
			Driver->extGlUniform2fv(UniformInfo[index].location, count/2, floats);
			break;
		case GL_FLOAT_VEC3:
			Driver->extGlUniform3fv(UniformInfo[index].location, count/3, floats);
			break;
		case GL_FLOAT_VEC4:
			Driver->extGlUniform4fv(UniformInfo[index].location, count/4, floats);
			break;
		case GL_FLOAT_MAT2:
			Driver->extGlUniformMatrix2fv(UniformInfo[index].location, count/4, false, floats);
			break;
		case GL_FLOAT_MAT3:
			Driver->extGlUniformMatrix3fv(UniformInfo[index].location, count/9, false, floats);
			break;
		case GL_FLOAT_MAT4:
			Driver->extGlUniformMatrix4fv(UniformInfo[index].location, count/16, false, floats);
			break;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_ARRAY:
			{
				if(floats)
				{
					const GLint id = static_cast<const GLint>(*floats);
					Driver->extGlUniform1iv(UniformInfo[index].location, 1, &id);
				}
				else
					status = false;
			}
			break;
		default:
			status = false;
			break;
	}
	return status;
#else
	return false;
#endif
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
#ifdef GL_ARB_shader_objects
	if(index < 0 || UniformInfo[index].location < 0)
		return false;

	bool status = true;

	switch (UniformInfo[index].type)
	{
		case GL_INT:
		case GL_BOOL:
			Driver->extGlUniform1iv(UniformInfo[index].location, count, reinterpret_cast<const GLint*>(ints));
			break;
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
			Driver->extGlUniform2iv(UniformInfo[index].location, count/2, reinterpret_cast<const GLint*>(ints));
			break;
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
			Driver->extGlUniform3iv(UniformInfo[index].location, count/3, reinterpret_cast<const GLint*>(ints));
			break;
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
			Driver->extGlUniform4iv(UniformInfo[index].location, count/4, reinterpret_cast<const GLint*>(ints));
			break;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
			Driver->extGlUniform1iv(UniformInfo[index].location, 1, reinterpret_cast<const GLint*>(ints));
			break;
		default:
			status = false;
			break;
	}
	return status;
#else
	return false;
#endif
}

//! get shader id
s32 COpenGLSLMaterialRenderer::getShaderVariableID(const c8* name, E_SHADER_TYPE shaderType)
{
	if (shaderType == video::EST_VERTEX_SHADER)
	{
		return getPixelShaderConstantID(name);
	}
	else if (shaderType == video::EST_PIXEL_SHADER)
	{
		return getPixelShaderConstantID(name);
	}

	return -1;
}

//! set shader value
void COpenGLSLMaterialRenderer::setShaderVariable(s32 id, const f32 *value, int count, E_SHADER_TYPE shaderType)
{
	if (shaderType == video::EST_VERTEX_SHADER)
	{
		setVertexShaderConstant(id, value, count);
	}
	else if (shaderType == video::EST_PIXEL_SHADER)
	{
		setPixelShaderConstant(id, value, count);
	}
}

IVideoDriver* COpenGLSLMaterialRenderer::getVideoDriver()
{
	return Driver;
}

} // end namespace video
} // end namespace irr


#endif

