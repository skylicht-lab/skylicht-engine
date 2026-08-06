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
			: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial), Program(0), UserData(userData)
		{
#ifdef _DEBUG
			setDebugName("COGLES3MaterialRenderer");
#endif

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
			: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial), Program(0), UserData(userData)
		{
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
			UniformBlockInfo.clear();
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

			if (BaseMaterial == EMT_TRANSPARENT_VERTEX_ALPHA ||
				BaseMaterial == EMT_TRANSPARENT_ALPHA_CHANNEL)
			{
				bridgeCalls->setBlend(true);
				bridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else if (BaseMaterial == EMT_TRANSPARENT_MULTIPLY_COLOR)
			{
				bridgeCalls->setBlend(true);
				bridgeCalls->setBlendFunc(GL_ZERO, GL_SRC_COLOR);
			}
			else if (BaseMaterial == EMT_TRANSPARENT_SCREEN_COLOR)
			{
				bridgeCalls->setBlend(true);
				bridgeCalls->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			}
			else if (BaseMaterial == EMT_TRANSPARENT_ADD_COLOR)
			{
				bridgeCalls->setBlend(true);
				bridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
			else if (BaseMaterial == EMT_TRANSPARENT_ALPHA_CHANNEL || BaseMaterial == EMT_TRANSPARENT_VERTEX_ALPHA)
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
			else if (BaseMaterial == EMT_TRANSPARENT_ALPHA_CHANNEL_REF)
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
			if (BaseMaterial == EMT_TRANSPARENT_ALPHA_CHANNEL ||
				BaseMaterial == EMT_TRANSPARENT_ADD_COLOR ||
				BaseMaterial == EMT_TRANSPARENT_VERTEX_ALPHA ||
				BaseMaterial == EMT_TRANSPARENT_MULTIPLY_COLOR ||
				BaseMaterial == EMT_TRANSPARENT_SCREEN_COLOR)
			{
				return true;
			}
			return false;
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

			GLint maxlen = 0;
			glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

			if (num > 0 && maxlen == 0)
			{
				os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
				return false;
			}

			UniformInfo.clear();
			UniformInfo.reallocate(num);

			if (num > 0)
			{
				// seems that some implementations use an extra null terminator
				++maxlen;
				c8 *buf = new c8[maxlen];

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
			}

			GLint numUniformBlocks = 0;
			glGetProgramiv(Program, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);

			UniformBlockInfo.clear();
			UniformBlockInfo.reallocate(numUniformBlocks);

			if (numUniformBlocks > 0)
			{
				GLint maxBlockNameLength = 0;
				glGetProgramiv(Program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxBlockNameLength);

				if (maxBlockNameLength == 0)
				{
					os::Printer::log("GLSL: failed to retrieve uniform block information", ELL_ERROR);
					return false;
				}

				c8 *buf = new c8[maxBlockNameLength];

				for (GLint i = 0; i < numUniformBlocks; ++i)
				{
					SUniformBlockInfo ubi;
					memset(buf, 0, maxBlockNameLength);

					glGetActiveUniformBlockName(Program, i, maxBlockNameLength, 0, reinterpret_cast<GLchar*>(buf));

					ubi.name = buf;
					ubi.index = (GLuint)i;

					UniformBlockInfo.push_back(ubi);
				}

				delete[] buf;
			}

			return true;
		}


		void COGLES3MaterialRenderer::setBasicRenderStates(const SMaterial& material,
			const SMaterial& lastMaterial,
			bool resetAllRenderstates)
		{
			// forward
			Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
		}

		//! get shader id
		s32 COGLES3MaterialRenderer::getShaderVariableID(const c8* name, E_SHADER_TYPE shaderType)
		{
			for (u32 i = 0; i < UniformInfo.size(); ++i)
			{
				if (UniformInfo[i].name == name)
					return i;
			}

			for (u32 i = 0; i < UniformBlockInfo.size(); ++i)
			{
				if (UniformBlockInfo[i].name == name)
					return (s32)UniformBlockInfo[i].index;
			}

			return -1;
		}

		//! set shader value
		void COGLES3MaterialRenderer::setShaderVariable(s32 id, const f32 *value, int count, E_SHADER_TYPE shaderType)
		{
			if (id < 0 || id >= (s32)UniformInfo.size() || UniformInfo[id].location < 0)
				return;

			switch (UniformInfo[id].type)
			{
			case GL_FLOAT:
				glUniform1fv(UniformInfo[id].location, count, value);
				break;
			case GL_FLOAT_VEC2:
				glUniform2fv(UniformInfo[id].location, count / 2, value);
				break;
			case GL_FLOAT_VEC3:
				glUniform3fv(UniformInfo[id].location, count / 3, value);
				break;
			case GL_FLOAT_VEC4:
				glUniform4fv(UniformInfo[id].location, count / 4, value);
				break;
			case GL_FLOAT_MAT2:
				glUniformMatrix2fv(UniformInfo[id].location, count / 4, false, value);
				break;
			case GL_FLOAT_MAT3:
				glUniformMatrix3fv(UniformInfo[id].location, count / 9, false, value);
				break;
			case GL_FLOAT_MAT4:
				glUniformMatrix4fv(UniformInfo[id].location, count / 16, false, value);
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_2D_SHADOW:
			case GL_SAMPLER_2D_ARRAY:
				if (value)
				{
					const GLint sampler = static_cast<const GLint>(*value);
					glUniform1i(UniformInfo[id].location, sampler);
				}
				break;
			default:
				break;
			}
		}

		void COGLES3MaterialRenderer::setShaderUBO(s32 id, const IHardwareBuffer* buffer, E_SHADER_TYPE shaderType)
		{
			if (id < 0 || !buffer || buffer->getDriverType() != EDT_OPENGLES || buffer->getType() != EHBT_CONSTANTS)
				return;

			const COGLES3HardwareBuffer* glBuffer = static_cast<const COGLES3HardwareBuffer*>(buffer);
			GLuint bufferID = glBuffer->getBufferID();
			if (!bufferID || !Program)
				return;

			glUniformBlockBinding(Program, (GLuint)id, (GLuint)id);
			glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)id, bufferID);
		}

		IVideoDriver* COGLES3MaterialRenderer::getVideoDriver()
		{
			return Driver;
		}

	} // end namespace video
} // end namespace irr


#endif

