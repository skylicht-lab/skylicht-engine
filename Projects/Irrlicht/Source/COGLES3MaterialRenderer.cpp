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
#ifdef _IRR_COMPILE_WITH_OGLES3_

#include "COGLES3MaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "irrOS.h"
#include "COGLES3Driver.h"

namespace irr
{
	namespace video
	{

		//! Constructor
		COGLES3MaterialRenderer::COGLES3MaterialRenderer(video::COGLES3Driver* driver,
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
			setDebugName("COGLES3MaterialRenderer");
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
		COGLES3MaterialRenderer::COGLES3MaterialRenderer(COGLES3Driver* driver,
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
		COGLES3MaterialRenderer::~COGLES3MaterialRenderer()
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
				glGetAttachedShaders(Program, 8, &count, shaders);
				// avoid bugs in some drivers, which return larger numbers
				// use int variable to avoid compiler problems with template
				int mincount = core::min_((int)count, 8);
				for (int i = 0; i < mincount; ++i)
					glDeleteShader(shaders[i]);
				glDeleteProgram(Program);
				Program = 0;
			}

			UniformInfo.clear();
		}


		void COGLES3MaterialRenderer::init(s32& outMaterialTypeNr,
			const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			const c8* geometryShaderProgram,
			scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
			u32 verticesOut)
		{
			outMaterialTypeNr = -1;

			if (!createProgram())
				return;

			if (vertexShaderProgram)
				if (!createShader(GL_VERTEX_SHADER, vertexShaderProgram))
					return;

			if (pixelShaderProgram)
				if (!createShader(GL_FRAGMENT_SHADER, pixelShaderProgram))
					return;

#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_program4) || defined(GL_NV_geometry_shader4)
			if (geometryShaderProgram && Driver->queryFeature(EVDF_GEOMETRY_SHADER))
			{
				if (!createShader(GL_GEOMETRY_SHADER_EXT, geometryShaderProgram))
					return;
#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_shader4)
				if (Program) // Geometry shaders are supported only in OGL2.x+ drivers.
				{
					glProgramParameteri(Program, GL_GEOMETRY_INPUT_TYPE_EXT, Driver->primitiveTypeToGL(inType));
					glProgramParameteri(Program, GL_GEOMETRY_OUTPUT_TYPE_EXT, Driver->primitiveTypeToGL(outType));
					if (verticesOut == 0)
						glProgramParameteri(Program, GL_GEOMETRY_VERTICES_OUT_EXT, Driver->MaxGeometryVerticesOut);
					else
						glProgramParameteri(Program, GL_GEOMETRY_VERTICES_OUT_EXT, core::min_(verticesOut, Driver->MaxGeometryVerticesOut));
				}
#elif defined(GL_NV_geometry_program4)
				if (verticesOut == 0)
					glProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, Driver->MaxGeometryVerticesOut);
				else
					glProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, core::min_(verticesOut, Driver->MaxGeometryVerticesOut));
#endif
			}
#endif

			if (!linkProgram())
				return;

			// register myself as new material
			outMaterialTypeNr = Driver->addMaterialRenderer(this);
		}


		bool COGLES3MaterialRenderer::OnRender(IMaterialRendererServices* service, bool updateConstant)
		{
			// call callback to set shader constants
			if (CallBack && Program)
				CallBack->OnSetConstants(this, UserData, updateConstant);

			return true;
		}


		void COGLES3MaterialRenderer::OnSetMaterial(const video::SMaterial& material,
			const video::SMaterial& lastMaterial,
			bool resetAllRenderstates,
			video::IMaterialRendererServices* services)
		{
			COGLES3CallBridge* bridgeCalls = Driver->getBridgeCalls();

			if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
			{
				if (Program)
					glUseProgram(Program);
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
				E_BLEND_FACTOR srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact;
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


		void COGLES3MaterialRenderer::OnUnsetMaterial()
		{
			Driver->setActiveGLSLProgram(0);

			if (Program)
				glUseProgram(0);
		}


		//! Returns if the material is transparent.
		bool COGLES3MaterialRenderer::isTransparent() const
		{
			return (Alpha || Blending || FixedBlending);
		}


		bool COGLES3MaterialRenderer::createProgram()
		{
			Program = glCreateProgram();
			return true;
		}


		bool COGLES3MaterialRenderer::createShader(GLenum shaderType, const char* shader)
		{
			const char* code[32];
			int num = 0;
			code[num++] = "#version 300 es\n";
			code[num++] = "#define GLES3\n";
			code[num++] = shader;

			GLuint shaderHandle = glCreateShader(shaderType);
			glShaderSource(shaderHandle, 3, code, NULL);
			glCompileShader(shaderHandle);

			GLint status = 0;

			glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

			if (status != GL_TRUE)
			{
                if (GL_VERTEX_SHADER == shaderType)
                    os::Printer::log("GLSL GL_VERTEX_SHADER shader failed to compile", ELL_ERROR);
                else if (GL_FRAGMENT_SHADER == shaderType)
                    os::Printer::log("GLSL GL_FRAGMENT_SHADER shader failed to compile", ELL_ERROR);
                else
                    os::Printer::log("GLSL shader failed to compile", ELL_ERROR);
                
				// check error message and log it
				GLint maxLength = 0;
				GLint length;

				glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength)
				{
					GLchar *infoLog = new GLchar[maxLength];
					glGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
					os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
					delete[] infoLog;
				}

				return false;
			}

			glAttachShader(Program, shaderHandle);

			return true;
		}


		bool COGLES3MaterialRenderer::linkProgram()
		{
			glLinkProgram(Program);

			GLint status = 0;

			glGetProgramiv(Program, GL_LINK_STATUS, &status);

			if (!status)
			{
				os::Printer::log("GLSL shader program failed to link", ELL_ERROR);
				// check error message and log it
				GLint maxLength = 0;
				GLsizei length;

				glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength)
				{
					GLchar *infoLog = new GLchar[maxLength];
					glGetProgramInfoLog(Program, maxLength, &length, infoLog);
					os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
					delete[] infoLog;
				}

				return false;
			}

			// get uniforms information

			GLint num = 0;
			glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num);

			if (num == 0)
			{
				// no uniforms
				return true;
			}

			GLint maxlen = 0;
			glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

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

			for (GLint i = 0; i < num; ++i)
			{
				SUniformInfo ui;
				memset(buf, 0, maxlen);

				GLint size;
				glGetActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar*>(buf));

				core::stringc name = "";

				// array support.
				for (u32 i = 0; buf[i] != '\0' && buf[i] != '['; ++i)
					name += buf[i];

				ui.name = name;
				ui.location = glGetUniformLocation(Program, name.c_str());

				UniformInfo.push_back(ui);
			}

			delete[] buf;
			return true;
		}


		void COGLES3MaterialRenderer::setBasicRenderStates(const SMaterial& material,
			const SMaterial& lastMaterial,
			bool resetAllRenderstates)
		{
			// forward
			Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
		}

		s32 COGLES3MaterialRenderer::getVertexShaderConstantID(const c8* name)
		{
			return getPixelShaderConstantID(name);
		}

		s32 COGLES3MaterialRenderer::getPixelShaderConstantID(const c8* name)
		{
			for (u32 i = 0; i < UniformInfo.size(); ++i)
			{
				if (UniformInfo[i].name == name)
					return i;
			}

			return -1;
		}

		bool COGLES3MaterialRenderer::setVertexShaderConstant(s32 index, const f32* floats, int count)
		{
			return setPixelShaderConstant(index, floats, count);
		}

		bool COGLES3MaterialRenderer::setVertexShaderConstant(s32 index, const s32* ints, int count)
		{
			return setPixelShaderConstant(index, ints, count);
		}

		bool COGLES3MaterialRenderer::setPixelShaderConstant(s32 index, const f32* floats, int count)
		{
			if (index < 0 || UniformInfo[index].location < 0)
				return false;

			bool status = true;

			switch (UniformInfo[index].type)
			{
			case GL_FLOAT:
				glUniform1fv(UniformInfo[index].location, count, floats);
				break;
			case GL_FLOAT_VEC2:
				glUniform2fv(UniformInfo[index].location, count / 2, floats);
				break;
			case GL_FLOAT_VEC3:
				glUniform3fv(UniformInfo[index].location, count / 3, floats);
				break;
			case GL_FLOAT_VEC4:
				glUniform4fv(UniformInfo[index].location, count / 4, floats);
				break;
			case GL_FLOAT_MAT2:
				glUniformMatrix2fv(UniformInfo[index].location, count / 4, false, floats);
				break;
			case GL_FLOAT_MAT3:
				glUniformMatrix3fv(UniformInfo[index].location, count / 9, false, floats);
				break;
			case GL_FLOAT_MAT4:
				glUniformMatrix4fv(UniformInfo[index].location, count / 16, false, floats);
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_2D_SHADOW:
			case GL_SAMPLER_2D_ARRAY:
			{
				if (floats)
				{
					const GLint id = static_cast<const GLint>(*floats);
					glUniform1i(UniformInfo[index].location, id);
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
		}

		bool COGLES3MaterialRenderer::setPixelShaderConstant(s32 index, const s32* ints, int count)
		{
			if (index < 0 || UniformInfo[index].location < 0)
				return false;

			bool status = true;

			switch (UniformInfo[index].type)
			{
			case GL_INT:
			case GL_BOOL:
				glUniform1iv(UniformInfo[index].location, count, reinterpret_cast<const GLint*>(ints));
				break;
			case GL_INT_VEC2:
			case GL_BOOL_VEC2:
				glUniform2iv(UniformInfo[index].location, count / 2, reinterpret_cast<const GLint*>(ints));
				break;
			case GL_INT_VEC3:
			case GL_BOOL_VEC3:
				glUniform3iv(UniformInfo[index].location, count / 3, reinterpret_cast<const GLint*>(ints));
				break;
			case GL_INT_VEC4:
			case GL_BOOL_VEC4:
				glUniform4iv(UniformInfo[index].location, count / 4, reinterpret_cast<const GLint*>(ints));
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_2D_SHADOW:
				glUniform1iv(UniformInfo[index].location, 1, reinterpret_cast<const GLint*>(ints));
				break;
			default:
				status = false;
				break;
			}
			return status;
		}

		//! get shader id
		s32 COGLES3MaterialRenderer::getShaderVariableID(const c8* name, E_SHADER_TYPE shaderType)
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
		void COGLES3MaterialRenderer::setShaderVariable(s32 id, const f32 *value, int count, E_SHADER_TYPE shaderType)
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

		IVideoDriver* COGLES3MaterialRenderer::getVideoDriver()
		{
			return Driver;
		}

	} // end namespace video
} // end namespace irr


#endif

