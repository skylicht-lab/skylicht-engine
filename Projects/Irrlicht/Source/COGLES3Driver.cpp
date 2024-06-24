// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "COGLES3Driver.h"
#include "CNullDriver.h"

#ifdef _IRR_COMPILE_WITH_OGLES3_

#include "COGLES3Texture.h"
#include "COGLES3TextureCube.h"
#include "COGLES3TextureArray.h"
#include "COGLES3MaterialRenderer.h"
#include "irrOS.h"

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
void SDLSwapBuffers();
#endif

namespace irr
{
	namespace video
	{
		COGLES3VertexAttribute::COGLES3VertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID, u32 layerCount) :
			IVertexAttribute(name, elementCount, semantic, type, offset, bufferID)
		{
			Cache.reallocate(layerCount);

			for (u32 i = 0; i < layerCount; ++i)
				Cache.push_back(false);

			Location.reallocate(layerCount);

			for (u32 i = 0; i < layerCount; ++i)
				Location.push_back(-1);
		}

		void COGLES3VertexAttribute::setOffset(u32 offset)
		{
			Offset = offset;
		}

		void COGLES3VertexAttribute::addLocationLayer()
		{
			Cache.push_back(false);
			Location.push_back(-1);
		}

		s32 COGLES3VertexAttribute::getLocation(u32 materialType) const
		{
			s32 location = -1;

			if (materialType < Location.size())
				location = Location[materialType];

			return location;
		}

		void COGLES3VertexAttribute::setLocation(u32 location, u32 materialType)
		{
			if (materialType < Location.size())
			{
				Cache[materialType] = true;
				Location[materialType] = location;
			}
		}

		s32 COGLES3VertexAttribute::getLocationStatus(u32 materialType) const
		{
			s32 status = -1;

			if (materialType < Cache.size())
			{
				if (Cache[materialType])
					status = 1;
				else
					status = 0;
			}

			return status;
		}

		COGLES3VertexDescriptor::COGLES3VertexDescriptor(const core::stringc& name, u32 id, u32 layerCount) :
			IVertexDescriptor(name, id), LayerCount(layerCount)
		{
#ifdef _DEBUG
			setDebugName("COGLES3VertexDescriptor");
#endif
		}

		void COGLES3VertexDescriptor::setID(u32 id)
		{
			ID = id;
		}

		IVertexAttribute* COGLES3VertexDescriptor::addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID)
		{
			for (u32 i = 0; i < Attribute.size(); ++i)
				if (name == Attribute[i].getName() || (semantic != EVAS_CUSTOM && semantic == Attribute[i].getSemantic()))
				{
					return &Attribute[i];
				}

			if (elementCount < 1)
				elementCount = 1;

			if (elementCount > 4)
				elementCount = 4;

			for (u32 i = VertexSize.size(); i <= bufferID; ++i)
				VertexSize.push_back(0);

			for (u32 i = InstanceDataStepRate.size(); i <= bufferID; ++i)
				InstanceDataStepRate.push_back(EIDSR_PER_VERTEX);

			COGLES3VertexAttribute attribute(name, elementCount, semantic, type, VertexSize[bufferID], bufferID, LayerCount);
			Attribute.push_back(attribute);

			AttributeSemanticIndex[(u32)attribute.getSemantic()] = Attribute.size() - 1;

			VertexSize[bufferID] += attribute.getTypeSize() * attribute.getElementCount();

			// Assign data to the pointers.

			AttributePointer.push_back(0);

			for (u32 i = 0; i < AttributePointer.size(); ++i)
				AttributePointer[i] = &Attribute[i];

			return AttributePointer.getLast();
		}

		void COGLES3VertexDescriptor::clearAttribute()
		{
			AttributePointer.clear();
			VertexSize.clear();

			for (u32 i = 0; i < EVAS_COUNT; ++i)
				AttributeSemanticIndex[i] = -1;

			Attribute.clear();
		}

		void COGLES3VertexDescriptor::addLocationLayer()
		{
			LayerCount++;

			for (u32 i = 0; i < Attribute.size(); ++i)
				Attribute[i].addLocationLayer();
		}

		COGLES3HardwareBuffer::COGLES3HardwareBuffer(scene::IIndexBuffer* indexBuffer, COGLES3Driver* driver) :
			IHardwareBuffer(scene::EHM_NEVER, 0, 0, EHBT_NONE, EDT_OPENGL), Driver(driver), BufferID(0),
			RemoveFromArray(true), LinkedBuffer(0)
		{
#ifdef _DEBUG
			setDebugName("COGLES3HardwareBuffer");
#endif

			Type = EHBT_INDEX;

			if (indexBuffer)
			{
				if (update(indexBuffer->getHardwareMappingHint(), indexBuffer->getIndexSize()*indexBuffer->getIndexCount(), indexBuffer->getIndices()))
				{
					indexBuffer->setHardwareBuffer(this);
					LinkedBuffer = indexBuffer;
				}
			}
		}

		COGLES3HardwareBuffer::COGLES3HardwareBuffer(scene::IVertexBuffer* vertexBuffer, COGLES3Driver* driver) :
			IHardwareBuffer(scene::EHM_NEVER, 0, 0, EHBT_NONE, EDT_OPENGL), Driver(driver), BufferID(0),
			RemoveFromArray(true), LinkedBuffer(0)
		{
#ifdef _DEBUG
			setDebugName("COGLES3HardwareBuffer");
#endif

			Type = EHBT_VERTEX;

			if (vertexBuffer)
			{
				if (update(vertexBuffer->getHardwareMappingHint(), vertexBuffer->getVertexSize()*vertexBuffer->getVertexCount(), vertexBuffer->getVertices()))
				{
					vertexBuffer->setHardwareBuffer(this);
					LinkedBuffer = vertexBuffer;
				}
			}
		}

		COGLES3HardwareBuffer::~COGLES3HardwareBuffer()
		{
			if (RemoveFromArray)
			{
				for (u32 i = 0; i < Driver->HardwareBuffer.size(); ++i)
				{
					if (Driver->HardwareBuffer[i] == this)
					{
						Driver->HardwareBuffer[i] = 0;
						break;
					}
				}
			}

			if (LinkedBuffer)
			{
				switch (Type)
				{
				case EHBT_INDEX:
					((scene::IIndexBuffer*)LinkedBuffer)->setHardwareBuffer(0, true);
					break;
				case EHBT_VERTEX:
					((scene::IVertexBuffer*)LinkedBuffer)->setHardwareBuffer(0, true);
					break;
				default:
					break;
				}
			}

			if (BufferID)
				glDeleteBuffers(1, &BufferID);
		}

		bool COGLES3HardwareBuffer::update(const scene::E_HARDWARE_MAPPING mapping, const u32 size, const void* data)
		{
			u32 oldSize = Size;

			Mapping = mapping;
			Size = size;

			if (Mapping == scene::EHM_NEVER || Size == 0 || !data || !Driver)
				return false;

			GLenum target = 0;

			switch (Type)
			{
			case EHBT_INDEX:
				target = GL_ELEMENT_ARRAY_BUFFER;
				break;
			case EHBT_VERTEX:
				target = GL_ARRAY_BUFFER;
				break;
			default:
				return false;
			}

			bool createBuffer = false;

			if (!BufferID)
			{
				glGenBuffers(1, &BufferID);

				if (!BufferID)
					return false;

				createBuffer = true;
			}
			else if (oldSize < Size)
				createBuffer = true;

			glBindBuffer(target, BufferID);

			if (!createBuffer)
				glBufferSubData(target, 0, Size, data);
			else
			{
				if (Mapping == scene::EHM_STATIC)
					glBufferData(target, Size, data, GL_STATIC_DRAW);
				else if (Mapping == scene::EHM_DYNAMIC)
					glBufferData(target, Size, data, GL_DYNAMIC_DRAW);
				else // scene::EHM_STREAM
					glBufferData(target, Size, data, GL_STREAM_DRAW);
			}

			glBindBuffer(target, 0);

			RequiredUpdate = false;

			return true;
		}

		GLuint COGLES3HardwareBuffer::getBufferID() const
		{
			return BufferID;
		}

		void COGLES3HardwareBuffer::removeFromArray(bool status)
		{
			RemoveFromArray = status;
		}

		// -----------------------------------------------------------------------
		// WINDOWS CONSTRUCTOR
		// -----------------------------------------------------------------------
		//! Windows constructor and init code
		COGLES3Driver::COGLES3Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io)
			:CNullDriver(io, params.WindowSize), COGLES3ExtensionHandler(), BridgeCalls(0),
			ResetRenderStates(true),
			AntiAlias(params.AntiAlias), RenderTargetTexture(0),
			CurrentRendertargetSize(0, 0), ColorFormat(ECF_R8G8B8),
			CurrentTarget(ERT_FRAME_BUFFER), Params(params),
			ActiveGLSLProgram(0), LastVertexDescriptor(0)
		{
#ifdef _DEBUG
			setDebugName("COGLES3Driver");
#endif

			for (u32 i = 0; i < 16; ++i)
				VertexAttributeStatus[i] = false;
		}

		//! inits the open gl driver
		bool COGLES3Driver::initDriver()
		{
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
			EglWindow = (EGLNativeWindowType)Params.WindowId;
			HDc = GetDC((HWND)EglWindow);
			EglDisplay = eglGetDisplay((EGLNativeDisplayType)HDc);

			if (EglDisplay == EGL_NO_DISPLAY)
			{
				os::Printer::log("Getting OpenGL-ES3 display.");
				EglDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);
			}
			if (EglDisplay == EGL_NO_DISPLAY)
			{
				os::Printer::log("Could not get OpenGL-ES3 display.");
			}

			EGLint majorVersion, minorVersion;
			if (!eglInitialize(EglDisplay, &majorVersion, &minorVersion))
			{
				os::Printer::log("Could not initialize OpenGL-ES3 display.");
			}
			else
			{
				char text[64];
				sprintf(text, "EglDisplay initialized. Egl version %d.%d\n", majorVersion, minorVersion);
				os::Printer::log(text);
			}

			EGLint attribs[] =
			{
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, Params.WithAlphaChannel ? 1 : 0,
				EGL_BUFFER_SIZE, Params.Bits,
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				//EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
				EGL_DEPTH_SIZE, Params.ZBufferBits,
				EGL_STENCIL_SIZE, Params.Stencilbuffer,
				EGL_SAMPLE_BUFFERS, Params.AntiAlias ? 1 : 0,
				EGL_SAMPLES, Params.AntiAlias,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
				EGL_NONE, 0
			};
			EGLint contextAttrib[] =
			{
				EGL_CONTEXT_MAJOR_VERSION, 3,
				EGL_CONTEXT_MINOR_VERSION, 0,
				EGL_NONE, 0
			};

			EGLConfig config;
			EGLint num_configs;
			u32 steps = 5;
			while (!eglChooseConfig(EglDisplay, attribs, &config, 1, &num_configs) || !num_configs)
			{
				switch (steps)
				{
				case 5: // samples
					if (attribs[19] > 2)
					{
						--attribs[19];
					}
					else
					{
						attribs[17] = 0;
						attribs[19] = 0;
						--steps;
					}
					break;
				case 4: // alpha
					if (attribs[7])
					{
						attribs[7] = 0;
						if (Params.AntiAlias)
						{
							attribs[17] = 1;
							attribs[19] = Params.AntiAlias;
							steps = 5;
						}
					}
					else
						--steps;
					break;
				case 3: // stencil
					if (attribs[15])
					{
						attribs[15] = 0;
						if (Params.AntiAlias)
						{
							attribs[17] = 1;
							attribs[19] = Params.AntiAlias;
							steps = 5;
						}
					}
					else
						--steps;
					break;
				case 2: // depth size
					if (attribs[13] > 16)
					{
						attribs[13] -= 8;
					}
					else
						--steps;
					break;
				case 1: // buffer size
					if (attribs[9] > 16)
					{
						attribs[9] -= 8;
					}
					else
						--steps;
					break;
				default:
					os::Printer::log("Could not get config for OpenGL-ES2 display.");
					return false;
				}
			}
			if (Params.AntiAlias && !attribs[17])
				os::Printer::log("No multisampling.");
			if (Params.WithAlphaChannel && !attribs[7])
				os::Printer::log("No alpha.");
			if (Params.Stencilbuffer && !attribs[15])
				os::Printer::log("No stencil buffer.");
			if (Params.ZBufferBits > attribs[13])
				os::Printer::log("No full depth buffer.");
			if (Params.Bits > attribs[9])
				os::Printer::log("No full color buffer.");

			os::Printer::log("Creating EglSurface with nativeWindow...");
			EglSurface = eglCreateWindowSurface(EglDisplay, config, EglWindow, NULL);
			if (EGL_NO_SURFACE == EglSurface)
			{
				os::Printer::log("FAILED\n");
				EglSurface = eglCreateWindowSurface(EglDisplay, config, NULL, NULL);
				os::Printer::log("Creating EglSurface without nativeWindows...");
			}

			if (EGL_NO_SURFACE == EglSurface)
			{
				os::Printer::log("FAILED\n");
				os::Printer::log("Could not create surface for OpenGL-ES2 display.");
			}

#ifdef EGL_VERSION_1_2
			if (minorVersion > 1)
				eglBindAPI(EGL_OPENGL_ES_API);
#endif

			os::Printer::log("Creating EglContext...");
			EglContext = eglCreateContext(EglDisplay, config, EGL_NO_CONTEXT, contextAttrib);
			if (testEGLError())
			{
				os::Printer::log("FAILED\n");
				os::Printer::log("Could not create Context for OpenGL-ES3 display.");
			}

			eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);
			if (testEGLError())
			{
				os::Printer::log("Could not make Context current for OpenGL-ES3 display.");
			}

			genericDriverInit();

			if (testGLError())
			{
				os::Printer::log("Init OpenGLES have some problems.");
			}

			if (Params.Vsync)
				eglSwapInterval(EglDisplay, 1);
#else
			genericDriverInit();
#endif			
			return true;
		}

		//! destructor
		COGLES3Driver::~COGLES3Driver()
		{
			deleteMaterialRenders();

			CurrentTexture.clear();

			// I get a blue screen on my laptop, when I do not delete the
			// textures manually before releasing the dc. Oh how I love this.
			deleteAllTextures();
			removeAllOcclusionQueries();
			removeAllHardwareBuffers();

			delete BridgeCalls;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
			eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglDestroySurface(EglDisplay, EglSurface);
			eglDestroyContext(EglDisplay, EglContext);
			eglTerminate(EglDisplay);
#endif
		}

		// -----------------------------------------------------------------------
		// METHODS
		// -----------------------------------------------------------------------

		bool COGLES3Driver::genericDriverInit()
		{
			// init creates vertex descriptors based on the driver for built-in vertex structures.
			createVertexDescriptors();

			// Create driver.
			Name = glGetString(GL_VERSION);
			printVersion();

			// print renderer information
			const GLubyte* renderer = glGetString(GL_RENDERER);
			const GLubyte* vendor = glGetString(GL_VENDOR);
			if (renderer && vendor)
			{
				os::Printer::log(reinterpret_cast<const c8*>(renderer), reinterpret_cast<const c8*>(vendor), ELL_INFORMATION);
				VendorName = reinterpret_cast<const c8*>(vendor);
			}

			u32 i;
			CurrentTexture.clear();

			// load extensions
			initExtensions(Params.Stencilbuffer);

			if (!BridgeCalls)
				BridgeCalls = new COGLES3CallBridge(this);

			if (queryFeature(EVDF_ARB_GLSL))
			{
				char buf[32];
				const u32 maj = ShaderLanguageVersion / 100;
				snprintf(buf, 32, "%u.%u", maj, ShaderLanguageVersion - maj * 100);
				os::Printer::log("GLSL version", buf, ELL_INFORMATION);
			}
			else
				os::Printer::log("GLSL not available.", ELL_INFORMATION);

			DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
			DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
			DriverAttributes->setAttribute("MaxLights", MaxLights);
			DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
			DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
			DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
			DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
			DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
			DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
			DriverAttributes->setAttribute("MaxGeometryVerticesOut", (s32)MaxGeometryVerticesOut);
			DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
			DriverAttributes->setAttribute("Version", Version);
			DriverAttributes->setAttribute("ShaderLanguageVersion", ShaderLanguageVersion);
			DriverAttributes->setAttribute("AntiAlias", AntiAlias);

			glPixelStorei(GL_PACK_ALIGNMENT, 1);

			UserClipPlanes.reallocate(MaxUserClipPlanes);
			for (i = 0; i < MaxUserClipPlanes; ++i)
				UserClipPlanes.push_back(SUserClipPlane());

			setAmbientLight(SColorf(0.0f, 0.0f, 0.0f, 0.0f));

			glClearDepthf(1.0f);

			glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
			glDepthFunc(GL_LEQUAL);
			glFrontFace(GL_CW);

			// set the renderstates
			setRenderStates3DMode();

			// set fog mode
			setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

			// create matrix for flipping textures
			TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0, 0), core::vector2df(0, 1.0f), core::vector2df(1.0f, -1.0f));

			// We need to reset once more at the beginning of the first rendering.
			// This fixes problems with intermediate changes to the material during texture load.
			ResetRenderStates = true;

			return true;
		}

		//! presents the rendered scene on the screen, returns false if failed
		bool COGLES3Driver::endScene()
		{
			CNullDriver::endScene();

			glFlush();

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
			if (RenderTargetTexture == NULL)
			{
				eglSwapBuffers(EglDisplay, EglSurface);
				EGLint g = eglGetError();
				if (EGL_SUCCESS != g)
				{
					if (EGL_CONTEXT_LOST == g)
					{
						// o-oh, ogl-es has lost contexts...
						os::Printer::log("Context lost, please restart your app.");
					}
					else
					{
						os::Printer::log("Could not swap buffers for OpenGL-ES3 driver.");
					}

					return false;
				}
			}
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
			if (DeviceType == EIDT_SDL)
			{
				SDLSwapBuffers();
				return true;
			}
#endif

			return true;
		}


		//! clears the zbuffer and color buffer
		void COGLES3Driver::clearBuffers(bool backBuffer, bool zBuffer, bool stencilBuffer, SColor color)
		{
			GLbitfield mask = 0;
			if (backBuffer)
			{
				BridgeCalls->setColorMask(true, true, true, true);

				const f32 inv = 1.0f / 255.0f;
				glClearColor(color.getRed() * inv, color.getGreen() * inv,
					color.getBlue() * inv, color.getAlpha() * inv);

				mask |= GL_COLOR_BUFFER_BIT;
			}

			if (zBuffer)
			{
				BridgeCalls->setDepthMask(true);
				mask |= GL_DEPTH_BUFFER_BIT;
			}

			if (stencilBuffer)
				mask |= GL_STENCIL_BUFFER_BIT;

			if (mask)
				glClear(mask);
		}


		//! init call for rendering start
		bool COGLES3Driver::beginScene(bool backBuffer, bool zBuffer, SColor color,
			const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
		{
			CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

			clearBuffers(backBuffer, zBuffer, false, color);
			return true;
		}

		IHardwareBuffer* COGLES3Driver::createHardwareBuffer(scene::IIndexBuffer* indexBuffer)
		{
			if (!indexBuffer)
				return 0;

			COGLES3HardwareBuffer* hardwareBuffer = new COGLES3HardwareBuffer(indexBuffer, this);

			bool extendArray = true;

			for (u32 i = 0; i < HardwareBuffer.size(); ++i)
			{
				if (!HardwareBuffer[i])
				{
					HardwareBuffer[i] = hardwareBuffer;
					extendArray = false;
					break;
				}
			}

			if (extendArray)
				HardwareBuffer.push_back(hardwareBuffer);

			return hardwareBuffer;
		}


		IHardwareBuffer* COGLES3Driver::createHardwareBuffer(scene::IVertexBuffer* vertexBuffer)
		{
			if (!vertexBuffer)
				return 0;

			COGLES3HardwareBuffer* hardwareBuffer = new COGLES3HardwareBuffer(vertexBuffer, this);

			bool extendArray = true;

			for (u32 i = 0; i < HardwareBuffer.size(); ++i)
			{
				if (!HardwareBuffer[i])
				{
					HardwareBuffer[i] = hardwareBuffer;
					extendArray = false;
					break;
				}
			}

			if (extendArray)
				HardwareBuffer.push_back(hardwareBuffer);

			return hardwareBuffer;
		}


		void COGLES3Driver::removeAllHardwareBuffers()
		{
			for (u32 i = 0; i < HardwareBuffer.size(); ++i)
			{
				if (HardwareBuffer[i])
				{
					HardwareBuffer[i]->removeFromArray(false);
					delete HardwareBuffer[i];
				}
			}

			HardwareBuffer.clear();
		}


		//! Create occlusion query.
		/** Use node for identification and mesh for occlusion test. */
		void COGLES3Driver::addOcclusionQuery(scene::ISceneNode* node,
			const scene::IMesh* mesh)
		{
			if (!queryFeature(EVDF_OCCLUSION_QUERY))
				return;

			CNullDriver::addOcclusionQuery(node, mesh);
			const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
			if ((index != -1) && (OcclusionQueries[index].UID == 0))
				glGenQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
			testGLError();
		}


		//! Remove occlusion query.
		void COGLES3Driver::removeOcclusionQuery(scene::ISceneNode* node)
		{
			const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
			if (index != -1)
			{
				if (OcclusionQueries[index].UID != 0)
					glDeleteQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
				CNullDriver::removeOcclusionQuery(node);
			}
		}


		//! Run occlusion query. Draws mesh stored in query.
		/** If the mesh shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		void COGLES3Driver::runOcclusionQuery(scene::ISceneNode* node, bool visible)
		{
			if (!node)
				return;

			const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
			if (index != -1)
			{
				if (OcclusionQueries[index].UID)
					glBeginQuery(GL_ANY_SAMPLES_PASSED, OcclusionQueries[index].UID);
				CNullDriver::runOcclusionQuery(node, visible);
				if (OcclusionQueries[index].UID)
					glEndQuery(GL_ANY_SAMPLES_PASSED);
			}
		}


		//! Update occlusion query. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		void COGLES3Driver::updateOcclusionQuery(scene::ISceneNode* node, bool block)
		{
			const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
			if (index != -1)
			{
				// not yet started
				if (OcclusionQueries[index].Run == u32(~0))
					return;

				GLuint available = block ? GL_TRUE : GL_FALSE;

				if (!block)
					glGetQueryObjectuiv(OcclusionQueries[index].UID, GL_QUERY_RESULT_AVAILABLE, &available);

				if (available == GL_TRUE)
				{
					glGetQueryObjectuiv(OcclusionQueries[index].UID, GL_QUERY_RESULT, &available);
					if (queryFeature(EVDF_OCCLUSION_QUERY))
						OcclusionQueries[index].Result = available;
				}
			}
		}


		//! Return query result.
		/** Return value is the number of visible pixels/fragments.
		The value is a safe approximation, i.e. can be larger than the
		actual value of pixels. */
		u32 COGLES3Driver::getOcclusionQueryResult(scene::ISceneNode* node) const
		{
			const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
			if (index != -1)
				return OcclusionQueries[index].Result;
			else
				return ~0;
		}


		// small helper function to create vertex buffer object adress offsets
		static inline u8* buffer_offset(const long offset)
		{
			return ((u8*)0 + offset);
		}


		void COGLES3Driver::drawMeshBuffer(const scene::IMeshBuffer* mb)
		{
			if (!mb || !mb->isVertexBufferCompatible())
				return;

			if (!checkPrimitiveCount(mb->getPrimitiveCount()))
				return;

			CNullDriver::drawMeshBuffer(mb);

			COGLES3VertexDescriptor* descriptor = (COGLES3VertexDescriptor*)mb->getVertexDescriptor();

			scene::IIndexBuffer* indexBuffer = mb->getIndexBuffer();

			const u32 indexSize = indexBuffer->getIndexSize();
			const u32 indexCount = indexBuffer->getIndexCount();
			const GLenum indexType = (indexBuffer->getType() == EIT_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
			const scene::E_HARDWARE_MAPPING indexMapping = indexBuffer->getHardwareMappingHint();
			const void* indexData = indexBuffer->getIndices();

			const u32 primitiveCount = mb->getPrimitiveCount();
			const scene::E_PRIMITIVE_TYPE primitiveType = mb->getPrimitiveType();

			const bool hwRecommended = isHardwareBufferRecommend(mb);

			// draw everything
			setRenderStates3DMode();

			//due to missing defines in OSX headers, we have to be more specific with this check
			//#if defined(GL_ARB_vertex_array_bgra) || defined(GL_EXT_vertex_array_bgra)
#ifdef GL_BGRA
			const GLint colorSize = (FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra]) ? GL_BGRA : 4;
#else
			const GLint colorSize = 4;
#endif

			const u32 attributeCount = descriptor->getAttributeCount();

			// Enable semantics, attributes and hardware buffers.

			GLuint hwIndexBuffer = 0;
			GLuint hwVertexBuffer = 0;

			GLuint glslProgram = getActiveGLSLProgram();

			u32 instanceVertexCount = 0;
			bool perInstanceBufferPresent = false;

			// support hw instancing
			for (u32 i = 0; i < mb->getVertexBufferCount(); ++i)
			{
				if (descriptor->getInstanceDataStepRate(i) == EIDSR_PER_INSTANCE)
				{
					instanceVertexCount = mb->getVertexBuffer(i)->getVertexCount();
					if (!instanceVertexCount)
						return;
					perInstanceBufferPresent = true;
				}
			}

			for (u32 i = 0; i < attributeCount && i < 16; ++i)
			{
				COGLES3VertexAttribute* attribute = static_cast<COGLES3VertexAttribute*>(descriptor->getAttribute(i));

				const u32 attribElementCount = attribute->getElementCount();
				const E_VERTEX_ATTRIBUTE_SEMANTIC attribSemantic = attribute->getSemantic();
				const E_VERTEX_ATTRIBUTE_TYPE attribType = attribute->getType();
				const u32 attribOffset = attribute->getOffset();
				const u32 attribBufferID = attribute->getBufferID();

				scene::IVertexBuffer* vertexBuffer = mb->getVertexBuffer(attribBufferID);

				const u32 vertexCount = vertexBuffer->getVertexCount();
				const u32 vertexSize = vertexBuffer->getVertexSize();
				const scene::E_HARDWARE_MAPPING vertexMapping = vertexBuffer->getHardwareMappingHint();
				u8* vertexData = static_cast<u8*>(vertexBuffer->getVertices());

				u8* attribData = vertexData + attribOffset;

				GLenum attribTypeGL = 0;

				switch (attribType)
				{
				case EVAT_BYTE:
					attribTypeGL = GL_BYTE;
					break;
				case EVAT_UBYTE:
					attribTypeGL = GL_UNSIGNED_BYTE;
					break;
				case EVAT_SHORT:
					attribTypeGL = GL_SHORT;
					break;
				case EVAT_USHORT:
					attribTypeGL = GL_UNSIGNED_SHORT;
					break;
				case EVAT_INT:
					attribTypeGL = GL_INT;
					break;
				case EVAT_UINT:
					attribTypeGL = GL_UNSIGNED_INT;
					break;
				case EVAT_FLOAT:
					attribTypeGL = GL_FLOAT;
					break;
				case EVAT_DOUBLE:
					// error case !!!
					attribTypeGL = GL_FLOAT;
					break;
				}

				// Update VBO.				
				COGLES3HardwareBuffer* vertexBufferObject = (COGLES3HardwareBuffer*)vertexBuffer->getHardwareBuffer();

				if (vertexBufferObject)
				{
					if (vertexMapping != scene::EHM_NEVER)
					{
						if (vertexBufferObject->isRequiredUpdate())
							vertexBufferObject->update(vertexMapping, vertexSize*vertexCount, vertexData);

						hwVertexBuffer = vertexBufferObject->getBufferID();
					}
					else
					{
						vertexBuffer->setHardwareBuffer(0);
						hwVertexBuffer = 0;
					}
				}
				else if (vertexMapping != scene::EHM_NEVER && hwRecommended)
				{
					vertexBufferObject = (COGLES3HardwareBuffer*)createHardwareBuffer(vertexBuffer);
					vertexBuffer->setHardwareBuffer(vertexBufferObject);
					vertexBufferObject->drop();

					hwVertexBuffer = vertexBufferObject->getBufferID();
				}
				else
				{
					hwVertexBuffer = 0;
				}

				glBindBuffer(GL_ARRAY_BUFFER, hwVertexBuffer);

				if (glslProgram)
				{
					s32 status = attribute->getLocationStatus((u32)Material.MaterialType);
					GLint location = -1;

					if (status != -1)
					{
						if (status)
						{
							location = attribute->getLocation((u32)Material.MaterialType);
						}
						else
						{
							if (perInstanceBufferPresent == true)
								location = i;
							else
								location = glGetAttribLocation(glslProgram, attribute->getName().c_str());

							attribute->setLocation(location, (u32)Material.MaterialType);
						}
					}

					if (location != -1)
					{
						VertexAttributeStatus[location] = true;

						glEnableVertexAttribArray(location);

						if (hwVertexBuffer)
							glVertexAttribPointer(location, attribElementCount, attribTypeGL, GL_FALSE, vertexSize, buffer_offset(attribOffset));
						else
							glVertexAttribPointer(location, attribElementCount, attribTypeGL, GL_FALSE, vertexSize, attribData);

						// instancing
						if (descriptor->getInstanceDataStepRate(attribBufferID) == video::EIDSR_PER_INSTANCE)
							glVertexAttribDivisor(location, 1);
						else
							glVertexAttribDivisor(location, 0);
					}
				}

			}

			// Update IBO.
			COGLES3HardwareBuffer* indexBufferObject = (COGLES3HardwareBuffer*)indexBuffer->getHardwareBuffer();

			if (indexBufferObject)
			{
				if (indexMapping != scene::EHM_NEVER)
				{
					if (indexBufferObject->isRequiredUpdate())
						indexBufferObject->update(indexMapping, indexSize*indexCount, indexData);

					hwIndexBuffer = indexBufferObject->getBufferID();
				}
				else
				{
					indexBuffer->setHardwareBuffer(0);
					hwIndexBuffer = 0;
				}
			}
			else if (indexMapping != scene::EHM_NEVER && hwRecommended)
			{
				indexBufferObject = (COGLES3HardwareBuffer*)createHardwareBuffer(indexBuffer);
				indexBuffer->setHardwareBuffer(indexBufferObject);
				indexBufferObject->drop();

				hwIndexBuffer = indexBufferObject->getBufferID();
			}
			else
			{
				hwIndexBuffer = 0;
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hwIndexBuffer);

			// Draw				
			if (perInstanceBufferPresent)
				renderInstance(hwIndexBuffer ? 0 : indexData, indexType, primitiveCount, primitiveType, instanceVertexCount);
			else
				renderArray(hwIndexBuffer ? 0 : indexData, indexType, primitiveCount, primitiveType);

			LastVertexDescriptor = descriptor;

			// Disable hardware buffers.
			if (hwIndexBuffer)
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			if (hwVertexBuffer)
				glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void COGLES3Driver::renderInstance(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType, u32 instanceCount)
		{
			switch (primitiveType)
			{
			case scene::EPT_LINE_STRIP:
				glDrawElementsInstanced(GL_LINE_STRIP, primitiveCount + 1, indexType, indices, instanceCount);
				break;
			case scene::EPT_LINE_LOOP:
				glDrawElementsInstanced(GL_LINE_LOOP, primitiveCount, indexType, indices, instanceCount);
				break;
			case scene::EPT_LINES:
				glDrawElementsInstanced(GL_LINES, primitiveCount * 2, indexType, indices, instanceCount);
				break;
			case scene::EPT_TRIANGLE_STRIP:
				glDrawElementsInstanced(GL_TRIANGLE_STRIP, primitiveCount + 2, indexType, indices, instanceCount);
				break;
			case scene::EPT_TRIANGLE_FAN:
				glDrawElementsInstanced(GL_TRIANGLE_FAN, primitiveCount + 2, indexType, indices, instanceCount);
				break;
			case scene::EPT_TRIANGLES:
				glDrawElementsInstanced(GL_TRIANGLES, primitiveCount * 3, indexType, indices, instanceCount);
				break;
			default:
				break;
			}
		}

		void COGLES3Driver::renderArray(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType)
		{
			switch (primitiveType)
			{
			case scene::EPT_POINTS:
			case scene::EPT_POINT_SPRITES:
			{
				/*
				#ifdef GL_ARB_point_sprite
								if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
									glEnable(GL_POINT_SPRITE_ARB);
				#endif

								// prepare size and attenuation (where supported)
								GLfloat particleSize = Material.Thickness;
								//			if (AntiAlias)
								//				particleSize=core::clamp(particleSize, DimSmoothedPoint[0], DimSmoothedPoint[1]);
								//			else

								particleSize = core::clamp(particleSize, DimAliasedPoint[0], DimAliasedPoint[1]);
				#if defined(GL_VERSION_1_4) || defined(GL_ARB_point_parameters) || defined(GL_EXT_point_parameters) || defined(GL_SGIS_point_parameters)
								const float att[] = { 1.0f, 1.0f, 0.0f };
				#if defined(GL_VERSION_1_4)
								extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
								//			extGlPointParameterf(GL_POINT_SIZE_MIN,1.f);
								extGlPointParameterf(GL_POINT_SIZE_MAX, particleSize);
								extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);
				#elif defined(GL_ARB_point_parameters)
								extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION_ARB, att);
								//			extGlPointParameterf(GL_POINT_SIZE_MIN_ARB,1.f);
								extGlPointParameterf(GL_POINT_SIZE_MAX_ARB, particleSize);
								extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f);
				#elif defined(GL_EXT_point_parameters)
								extGlPointParameterfv(GL_DISTANCE_ATTENUATION_EXT, att);
								//			extGlPointParameterf(GL_POINT_SIZE_MIN_EXT,1.f);
								extGlPointParameterf(GL_POINT_SIZE_MAX_EXT, particleSize);
								extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_EXT, 1.0f);
				#elif defined(GL_SGIS_point_parameters)
								extGlPointParameterfv(GL_DISTANCE_ATTENUATION_SGIS, att);
								//			extGlPointParameterf(GL_POINT_SIZE_MIN_SGIS,1.f);
								extGlPointParameterf(GL_POINT_SIZE_MAX_SGIS, particleSize);
								extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_SGIS, 1.0f);
				#endif
				#endif
								glPointSize(particleSize);

				#ifdef GL_ARB_point_sprite
								if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
									glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE, GL_TRUE);
				#endif
								glDrawArrays(GL_POINTS, 0, primitiveCount);
				#ifdef GL_ARB_point_sprite
								if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
								{
									glDisable(GL_POINT_SPRITE_ARB);
									glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE, GL_FALSE);
								}
				#endif
				*/
			}
			break;
			case scene::EPT_LINE_STRIP:
				glDrawElements(GL_LINE_STRIP, primitiveCount + 1, indexType, indices);
				break;
			case scene::EPT_LINE_LOOP:
				glDrawElements(GL_LINE_LOOP, primitiveCount, indexType, indices);
				break;
			case scene::EPT_LINES:
				glDrawElements(GL_LINES, primitiveCount * 2, indexType, indices);
				break;
			case scene::EPT_TRIANGLE_STRIP:
				glDrawElements(GL_TRIANGLE_STRIP, primitiveCount + 2, indexType, indices);
				break;
			case scene::EPT_TRIANGLE_FAN:
				glDrawElements(GL_TRIANGLE_FAN, primitiveCount + 2, indexType, indices);
				break;
			case scene::EPT_TRIANGLES:
				glDrawElements(GL_TRIANGLES, primitiveCount * 3, indexType, indices);
				break;
			}
		}

		bool COGLES3Driver::setActiveTexture(u32 stage, const video::ITexture* texture)
		{
			if (stage >= MaxSupportedTextures)
				return false;

			if (CurrentTexture[stage] == texture)
				return true;

			CurrentTexture.set(stage, texture);

			if (!texture)
				return true;
			else if (texture->getDriverType() != EDT_OPENGLES)
			{
				CurrentTexture.set(stage, 0);
				os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				return false;
			}

			return true;
		}


		//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
		//! Returns whether disabling was successful or not.
		bool COGLES3Driver::disableTextures(u32 fromStage)
		{
			bool result = true;
			for (u32 i = fromStage; i < MaxSupportedTextures; ++i)
			{
				result &= setActiveTexture(i, 0);
				BridgeCalls->setTexture(i);
			}
			return result;
		}

		//! returns a device dependent texture from a software surface (IImage)
		video::ITexture* COGLES3Driver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
		{
			return new COGLES3Texture(surface, name, mipmapData, this);
		}


		//! Sets a material. All 3d drawing functions draw geometry now using this material.
		void COGLES3Driver::setMaterial(const SMaterial& material)
		{
			Material = material;

			for (u32 i = 0; i < MaxTextureUnits; ++i)
			{
				setActiveTexture(i, material.getTexture(i));
			}
		}


		//! prints error if an error happened.
		bool COGLES3Driver::testGLError()
		{
#ifdef _DEBUG
			GLenum g = glGetError();
			switch (g)
			{
			case GL_NO_ERROR:
				return false;
			case GL_INVALID_ENUM:
				os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
			case GL_INVALID_VALUE:
				os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
			case GL_INVALID_OPERATION:
				os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
			case GL_OUT_OF_MEMORY:
				os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				os::Printer::log("GL_INVALID_FRAMEBUFFER_OPERATION", ELL_ERROR); break;
			};
			//	_IRR_DEBUG_BREAK_IF(true);
			return true;
#else
			return false;
#endif
		}

		bool COGLES3Driver::testEGLError()
		{
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) && defined(EGL_VERSION_1_0) && defined(_DEBUG)
			EGLint g = eglGetError();
			switch (g)
			{
			case EGL_SUCCESS:
				return false;
			case EGL_NOT_INITIALIZED:
				os::Printer::log("Not Initialized", ELL_ERROR);
				break;
			case EGL_BAD_ACCESS:
				os::Printer::log("Bad Access", ELL_ERROR);
				break;
			case EGL_BAD_ALLOC:
				os::Printer::log("Bad Alloc", ELL_ERROR);
				break;
			case EGL_BAD_ATTRIBUTE:
				os::Printer::log("Bad Attribute", ELL_ERROR);
				break;
			case EGL_BAD_CONTEXT:
				os::Printer::log("Bad Context", ELL_ERROR);
				break;
			case EGL_BAD_CONFIG:
				os::Printer::log("Bad Config", ELL_ERROR);
				break;
			case EGL_BAD_CURRENT_SURFACE:
				os::Printer::log("Bad Current Surface", ELL_ERROR);
				break;
			case EGL_BAD_DISPLAY:
				os::Printer::log("Bad Display", ELL_ERROR);
				break;
			case EGL_BAD_SURFACE:
				os::Printer::log("Bad Surface", ELL_ERROR);
				break;
			case EGL_BAD_MATCH:
				os::Printer::log("Bad Match", ELL_ERROR);
				break;
			case EGL_BAD_PARAMETER:
				os::Printer::log("Bad Parameter", ELL_ERROR);
				break;
			case EGL_BAD_NATIVE_PIXMAP:
				os::Printer::log("Bad Native Pixmap", ELL_ERROR);
				break;
			case EGL_BAD_NATIVE_WINDOW:
				os::Printer::log("Bad Native Window", ELL_ERROR);
				break;
			case EGL_CONTEXT_LOST:
				os::Printer::log("Context Lost", ELL_ERROR);
				break;
			};
			return true;
#else
			return false;
#endif
		}

		//! sets the needed renderstates
		void COGLES3Driver::setRenderStates3DMode()
		{
			/*
			// Reset Texture Stages
			BridgeCalls->setBlend(false);
			BridgeCalls->setAlphaTest(false);
			BridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			ResetRenderStates = true;

#ifdef GL_EXT_clip_volume_hint
			if (FeatureAvailable[IRR_EXT_clip_volume_hint])
				glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
#endif
			*/
			bool shaderChanged = LastMaterial.MaterialType != Material.MaterialType;
			s32 numMaterialRenderers = (s32)MaterialRenderers.size();

			if (ResetRenderStates || LastMaterial != Material)
			{
				// unset old material
				if (shaderChanged &&
					LastMaterial.MaterialType < numMaterialRenderers)
					MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

				// set new material.
				if (Material.MaterialType < numMaterialRenderers)
					MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);

				LastMaterial = Material;
			}

			if (Material.MaterialType < numMaterialRenderers)
				MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, shaderChanged || RendererTransformChanged);

			RendererTransformChanged = false;
			ResetRenderStates = false;
		}


		//! Get native wrap mode value
		GLint COGLES3Driver::getTextureWrapMode(const u8 clamp)
		{
			GLint mode = GL_REPEAT;
			switch (clamp)
			{
			case ETC_REPEAT:
				mode = GL_REPEAT;
				break;
			case ETC_CLAMP:
				mode = GL_CLAMP_TO_EDGE;
				break;
			case ETC_CLAMP_TO_EDGE:
				// fallback
				mode = GL_CLAMP_TO_EDGE;
				break;
			case ETC_CLAMP_TO_BORDER:
				// fallback
				mode = GL_CLAMP_TO_EDGE;
				break;
			case ETC_MIRROR:
				mode = GL_REPEAT;
				break;
			case ETC_MIRROR_CLAMP:
				mode = GL_MIRRORED_REPEAT;
				break;
			case ETC_MIRROR_CLAMP_TO_EDGE:
				mode = GL_CLAMP_TO_EDGE;
				break;
			case ETC_MIRROR_CLAMP_TO_BORDER:
				mode = GL_CLAMP_TO_EDGE;
				break;
			}
			return mode;
		}


		//! Can be called by an IMaterialRenderer to make its work easier.
		void COGLES3Driver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial, bool resetAllRenderStates)
		{
			// ZBuffer
			switch (material.ZBuffer)
			{
			case ECFN_DISABLED:
				BridgeCalls->setDepthTest(false);
				break;
			case ECFN_LESSEQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_LEQUAL);
				break;
			case ECFN_EQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_EQUAL);
				break;
			case ECFN_LESS:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_LESS);
				break;
			case ECFN_NOTEQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_NOTEQUAL);
				break;
			case ECFN_GREATEREQUAL:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_GEQUAL);
				break;
			case ECFN_GREATER:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_GREATER);
				break;
			case ECFN_ALWAYS:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_ALWAYS);
				break;
			case ECFN_NEVER:
				BridgeCalls->setDepthTest(true);
				BridgeCalls->setDepthFunc(GL_NEVER);
				break;
			default:
				break;
			}

			// ZWrite
			if (material.ZWriteEnable && (AllowZWriteOnTransparent || (material.BlendOperation == EBO_NONE &&
				!MaterialRenderers[material.MaterialType].Renderer->isTransparent())))
			{
				BridgeCalls->setDepthMask(true);
			}
			else
			{
				BridgeCalls->setDepthMask(false);
			}

			// Back face culling
			if ((material.FrontfaceCulling) && (material.BackfaceCulling))
			{
				BridgeCalls->setCullFaceFunc(GL_FRONT_AND_BACK);
				BridgeCalls->setCullFace(true);
			}
			else if (material.BackfaceCulling)
			{
				BridgeCalls->setCullFaceFunc(GL_BACK);
				BridgeCalls->setCullFace(true);
			}
			else if (material.FrontfaceCulling)
			{
				BridgeCalls->setCullFaceFunc(GL_FRONT);
				BridgeCalls->setCullFace(true);
			}
			else
			{
				BridgeCalls->setCullFace(false);
			}

			// Color Mask
			BridgeCalls->setColorMask(
				(material.ColorMask & ECP_RED) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_GREEN) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_BLUE) ? GL_TRUE : GL_FALSE,
				(material.ColorMask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);

			/*
			// Blend Equation
			if (material.BlendOperation == EBO_NONE)
				BridgeCalls->setBlend(false);
			else
				BridgeCalls->setBlend(true);

			// Blend Factor
			if (IR(material.BlendFactor) & 0xFFFFFFFF)
			{
				E_BLEND_FACTOR srcRGBFact = EBF_ZERO;
				E_BLEND_FACTOR dstRGBFact = EBF_ZERO;
				E_BLEND_FACTOR srcAlphaFact = EBF_ZERO;
				E_BLEND_FACTOR dstAlphaFact = EBF_ZERO;
				E_MODULATE_FUNC modulo = EMFN_MODULATE_1X;
				u32 alphaSource = 0;

				unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo, alphaSource, material.BlendFactor);

				if (queryFeature(EVDF_BLEND_SEPARATE))
				{
					BridgeCalls->setBlendFuncSeparate(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact), getGLBlend(srcAlphaFact), getGLBlend(dstAlphaFact));
				}
				else
				{
					BridgeCalls->setBlendFunc(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact));
				}
			}
			*/

			// thickness
			/*
			if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
			{
				if (AntiAlias)
				{
					glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedLine[0], DimSmoothedLine[1]));
				}
				else
				{
					glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedLine[0], DimAliasedLine[1]));
				}
			}
			*/

			// Anti aliasing
			/*
			if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
			{
				if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
			*/

			// Texture parameters
			setTextureRenderStates(material, resetAllRenderStates);
		}

		#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
		#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

		//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
		void COGLES3Driver::setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates)
		{
			// Set textures to TU/TIU and apply filters to them
			for (s32 i = MaxTextureUnits - 1; i >= 0; --i)
			{
				BridgeCalls->setTexture(i);

				if (!CurrentTexture[i])
					continue;

				if (CurrentTexture[i]->getTextureType() == ETT_TEXTURE_CUBE)
				{
					if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
					{
						// enable mipmap
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					}
					else
					{
						// disable mipmap
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					}
				}
				else if (CurrentTexture[i]->getTextureType() == ETT_TEXTURE_ARRAY)
				{
					if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
					{
						if (material.TextureLayer[i].AnisotropicFilter > 1 &&
							material.TextureLayer[i].BilinearFilter == false &&
							material.TextureLayer[i].TrilinearFilter == false)
						{
							glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

							glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT,
								material.TextureLayer[i].AnisotropicFilter > 1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);
						}
						else
						{
							// enable mipmap
							glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						}
					}
					else
					{
						// Some device not support float linear 
						// GL_OES_texture_float_linear

						// disable mipmap
						glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					}

					// wrap mode
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
				}
				else
				{
					const COGLES3Texture* tmpTexture = static_cast<const COGLES3Texture*>(CurrentTexture[i]);
					if (resetAllRenderstates)
						tmpTexture->getStatesCache().IsCached = false;

					if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
						material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
							(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

						tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
						tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
					}

					if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
					{
						if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
							material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || !tmpTexture->getStatesCache().MipMapStatus)
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								material.TextureLayer[i].TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
								material.TextureLayer[i].BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
								GL_NEAREST_MIPMAP_NEAREST);

							tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
							tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
							tmpTexture->getStatesCache().MipMapStatus = true;
						}
					}
					else
					{
						if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
							material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || tmpTexture->getStatesCache().MipMapStatus)
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

							tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
							tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
							tmpTexture->getStatesCache().MipMapStatus = false;
						}
					}

					// Pham Hong Duc
					// AnisotropicFilter is defalt TrilinearFilter
					if (CurrentTexture[i] && CurrentTexture[i]->hasMipMaps() == true)
					{
						if (material.TextureLayer[i].AnisotropicFilter > 1 &&
							material.TextureLayer[i].BilinearFilter == false &&
							material.TextureLayer[i].TrilinearFilter == false)
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							tmpTexture->getStatesCache().BilinearFilter = false;
							tmpTexture->getStatesCache().TrilinearFilter = true;
							tmpTexture->getStatesCache().MipMapStatus = true;
						}
					}
					else
					{
						// Some device not support float linear 
						// GL_OES_texture_float_linear

						// default no mipmap
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						tmpTexture->getStatesCache().BilinearFilter = false;
						tmpTexture->getStatesCache().TrilinearFilter = false;
						tmpTexture->getStatesCache().MipMapStatus = false;
					}

					if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic] &&
						(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].AnisotropicFilter != tmpTexture->getStatesCache().AnisotropicFilter))
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
							material.TextureLayer[i].AnisotropicFilter > 1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);

						tmpTexture->getStatesCache().AnisotropicFilter = material.TextureLayer[i].AnisotropicFilter;
					}

					if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapU != tmpTexture->getStatesCache().WrapU)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
						tmpTexture->getStatesCache().WrapU = material.TextureLayer[i].TextureWrapU;
					}

					if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapV != tmpTexture->getStatesCache().WrapV)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
						tmpTexture->getStatesCache().WrapV = material.TextureLayer[i].TextureWrapV;
					}

					if (material.TextureLayer[i].TextureWrapU == ETC_CLAMP_TO_BORDER ||
						material.TextureLayer[i].TextureWrapV == ETC_CLAMP_TO_BORDER)
					{
						/*
						const SColorf &color = material.TextureLayer[i].BorderColor;
						GLfloat borderColor[] = { color.r, color.g, color.b, color.a };
						glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
						*/
					}

					tmpTexture->getStatesCache().IsCached = true;
				}
			}

			// be sure to leave in texture stage 0
			BridgeCalls->setActiveTexture(GL_TEXTURE0);
		}

		//! \return Returns the name of the video driver.
		const wchar_t* COGLES3Driver::getName() const
		{
			return Name.c_str();
		}

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		void COGLES3Driver::setAmbientLight(const SColorf& color)
		{

		}


		// this code was sent in by Oliver Klems, thank you! (I modified the glViewport
		// method just a bit.
		void COGLES3Driver::setViewPort(const core::rect<s32>& area)
		{
			core::rect<s32> vp = area;
			core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
			vp.clipAgainst(rendert);

			if (vp.getHeight() > 0 && vp.getWidth() > 0)
				BridgeCalls->setViewport(vp.UpperLeftCorner.X, getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(), vp.getWidth(), vp.getHeight());

			ViewPort = vp;
		}

		void COGLES3Driver::setScissor(const core::rect<s32>& area)
		{
			core::rect<s32> vp = area;
			core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
			vp.clipAgainst(rendert);

			if (vp.getHeight() > 0 && vp.getWidth() > 0)
				BridgeCalls->setScissor(vp.UpperLeftCorner.X, getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(), vp.getWidth(), vp.getHeight());

			ScissorRect = vp;
		}

		void COGLES3Driver::enableScissor(bool b)
		{
			EnableScissor = b;
			BridgeCalls->enableScissor(EnableScissor);
		}

		//! Sets the fog mode.
		void COGLES3Driver::setFog(SColor c, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog)
		{
			CNullDriver::setFog(c, fogType, start, end, density, pixelFog, rangeFog);
		}


		//! Draws a 3d line.
		void COGLES3Driver::draw3DLine(const core::vector3df& start, const core::vector3df& end, SColor color)
		{

		}


		//! Removes a texture from the texture cache and deletes it, freeing lot of memory.
		void COGLES3Driver::removeTexture(ITexture* texture)
		{
			if (!texture)
				return;

			CNullDriver::removeTexture(texture);
			// Remove this texture from CurrentTexture as well
			CurrentTexture.remove(texture);
		}


		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		void COGLES3Driver::OnResize(const core::dimension2d<u32>& size)
		{
			CNullDriver::OnResize(size);
			BridgeCalls->setViewport(0, 0, size.Width, size.Height);
			ResetRenderStates = true;
		}


		//! Adds a new material renderer to the video device.
		s32 COGLES3Driver::addMaterialRenderer(IMaterialRenderer* renderer, const char* name)
		{
			s32 id = CNullDriver::addMaterialRenderer(renderer, name);

			if (id != -1)
			{
				for (u32 i = 0; i < VertexDescriptor.size(); ++i)
				{
					((COGLES3VertexDescriptor*)VertexDescriptor[i])->addLocationLayer();
				}
			}

			return id;
		}


		//! Returns type of video driver
		E_DRIVER_TYPE COGLES3Driver::getDriverType() const
		{
			return EDT_OPENGLES;
		}


		//! returns color format
		ECOLOR_FORMAT COGLES3Driver::getColorFormat() const
		{
			return ColorFormat;
		}

		//! Adds a new material renderer to the VideoDriver, using pixel and/or
		//! vertex shaders to render geometry.
		s32 COGLES3Driver::addShaderMaterial(const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial, s32 userData)
		{
			s32 nr = -1;
			return nr;
		}


		//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
		s32 COGLES3Driver::addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName,
			E_VERTEX_SHADER_TYPE vsCompileTarget,
			const c8* pixelShaderProgram,
			const c8* pixelShaderEntryPointName,
			E_PIXEL_SHADER_TYPE psCompileTarget,
			const c8* geometryShaderProgram,
			const c8* geometryShaderEntryPointName,
			E_GEOMETRY_SHADER_TYPE gsCompileTarget,
			scene::E_PRIMITIVE_TYPE inType,
			scene::E_PRIMITIVE_TYPE outType,
			u32 verticesOut,
			IShaderConstantSetCallBack* callback,
			E_MATERIAL_TYPE baseMaterial,
			s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
		{
			s32 nr = -1;

			COGLES3MaterialRenderer* r = new COGLES3MaterialRenderer(
				this, nr,
				vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
				pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
				geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
				inType, outType, verticesOut,
				callback, baseMaterial, userData);

			r->drop();

			return nr;
		}


		//! Returns a pointer to the IVideoDriver interface. (Implementation for
		//! IMaterialRendererServices)
		IVideoDriver* COGLES3Driver::getVideoDriver()
		{
			return this;
		}


		ITexture* COGLES3Driver::addRenderTargetTexture(const core::dimension2d<u32>& size,
			const io::path& name,
			const ECOLOR_FORMAT format)
		{
			//disable mip-mapping
			bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

			video::ITexture* rtt = new COGLES3FBOTexture(size, name, this, format);
			addTexture(rtt);
			rtt->drop();

			//restore mip-mapping
			setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

			return rtt;
		}

		ITexture* COGLES3Driver::addRenderTargetTextureArray(const core::dimension2d<u32>& size, u32 arraySize,
			const io::path& name, const ECOLOR_FORMAT format)
		{			 
			// if driver supports FrameBufferObjects, use them
			video::ITexture* rtt = new COGLES3TextureArray(this, size, arraySize, name, format);
			if (rtt)
			{
				addTexture(rtt);
				rtt->drop();
			}

			return rtt;
		}

		ITexture* COGLES3Driver::addRenderTargetCubeTexture(const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format)
		{
			video::ITexture* rtt = new COGLES3TextureCube(this, size, name, format);
			addTexture(rtt);
			rtt->drop();

			return rtt;
		}

		ITexture* COGLES3Driver::getTextureCube(
			IImage *imageX1,
			IImage *imageX2,
			IImage *imageY1,
			IImage *imageY2,
			IImage *imageZ1,
			IImage *imageZ2)
		{
			return new COGLES3TextureCube(this, "TextureCube", imageX1, imageX2, imageY1, imageY2, imageZ1, imageZ2);
		}

		//! add texture array
		ITexture* COGLES3Driver::getTextureArray(IImage** images, u32 num)
		{
			return new COGLES3TextureArray(this, "TextureArray", images, num);
		}

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		u32 COGLES3Driver::getMaximalPrimitiveCount() const
		{
			return 0x7fffffff;
		}

		void COGLES3Driver::unbindRTT(ITexture *texture)
		{
			if (texture->getTextureType() == ETT_TEXTURE_2D)
				static_cast<COGLES3Texture*>(texture)->unbindRTT();
			else if (texture->getTextureType() == ETT_TEXTURE_CUBE)
				static_cast<COGLES3TextureCube*>(texture)->unbindRTT();
			else if (texture->getTextureType() == ETT_TEXTURE_ARRAY)
				static_cast<COGLES3TextureArray*>(texture)->unbindRTT();
		}

		//! set or reset render target
		bool COGLES3Driver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil)
		{
			// check for right driver type
			if (texture && texture->getDriverType() != EDT_OPENGLES)
			{
				os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				return false;
			}

			if (texture && texture->getTextureType() != ETT_TEXTURE_2D)
			{
				os::Printer::log("Fatal Error: This is not texture 2D", ELL_ERROR);
				return false;
			}

			if (CurrentTarget == ERT_MULTI_RENDER_TEXTURES)
			{
				for (u32 i = 0; i < MRTargets.size(); ++i)
				{
					if (MRTargets[i].TargetType == ERT_RENDER_TEXTURE)
					{
						for (++i; i < MRTargets.size(); ++i)
							if (MRTargets[i].TargetType == ERT_RENDER_TEXTURE)
								glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
					}
				}
				MRTargets.clear();
			}

			// check if we should set the previous RT back
			if ((RenderTargetTexture != texture) ||
				(CurrentTarget == ERT_MULTI_RENDER_TEXTURES))
			{
				BridgeCalls->setActiveTexture(GL_TEXTURE0);
				ResetRenderStates = true;

				if (RenderTargetTexture != 0)
					unbindRTT(RenderTargetTexture);

				if (texture)
				{
					// we want to set a new target. so do this.
					BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);

					COGLES3Texture *rtt = static_cast<COGLES3Texture*>(texture);
					RenderTargetTexture = rtt;

					if (rtt->isFrameBufferObject())
					{
						if (depthStencil)
						{
							static_cast<COGLES3FBOTexture*>(RenderTargetTexture)->setDepthTexture(depthStencil);
						}
						else
						{
							ITexture* renderBuffer = createDepthTexture(RenderTargetTexture, true);
							static_cast<COGLES3FBOTexture*>(RenderTargetTexture)->setDepthTexture(renderBuffer);
							renderBuffer->drop();
						}
					}

					// calls glDrawBuffer as well
					rtt->bindRTT();

					CurrentRendertargetSize = texture->getSize();
					CurrentTarget = ERT_RENDER_TEXTURE;
				}
				else
				{
					BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
					RenderTargetTexture = 0;
					CurrentRendertargetSize = core::dimension2d<u32>(0, 0);
					CurrentTarget = ERT_FRAME_BUFFER;
					
					GLenum buffers[] = { GL_BACK };
					glDrawBuffers(1, buffers);
				}

				// we need to update the matrices due to the rendersize change.
				ResetRenderStates = true;
			}

			clearBuffers(clearBackBuffer, clearZBuffer, false, color);
			return true;
		}

		bool COGLES3Driver::setRenderTargetArray(video::ITexture* texture, int arrayID, bool clearTarget, bool clearZBuffer, SColor color)
		{
			// check for right driver type
			if (texture && texture->getDriverType() != EDT_OPENGLES)
			{
				os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				return false;
			}

			if (texture && texture->getTextureType() != ETT_TEXTURE_ARRAY)
			{
				os::Printer::log("Fatal Error: Set render target is not texture cube", ELL_ERROR);
				return false;
			}

			// check if we should set the previous RT back
			BridgeCalls->setActiveTexture(GL_TEXTURE0);
			ResetRenderStates = true;

			if (RenderTargetTexture != 0 && RenderTargetTexture != texture)
			{
				unbindRTT(RenderTargetTexture);
			}

			if (texture)
			{
				// we want to set a new target. so do this.
				BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
				RenderTargetTexture = texture;


				COGLES3TextureArray *rtt = static_cast<COGLES3TextureArray*>(texture);
				RenderTargetTexture = rtt;

				if (rtt->isFrameBufferObject())
				{
					ITexture* renderBuffer = createDepthTexture(RenderTargetTexture, true);
					static_cast<COGLES3TextureArray*>(RenderTargetTexture)->setDepthTexture(renderBuffer);
					renderBuffer->drop();
				}

				// calls glDrawBuffer as well
				rtt->bindRTT(arrayID);

				CurrentRendertargetSize = texture->getSize();
				CurrentTarget = ERT_RENDER_TEXTURE;
			}
			else
			{
				BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
				RenderTargetTexture = 0;
				CurrentRendertargetSize = core::dimension2d<u32>(0, 0);

				GLenum buffers[] = { GL_BACK };
				glDrawBuffers(1, buffers);
			}

			clearBuffers(clearTarget, clearZBuffer, false, color);
			return true;
		}

		bool COGLES3Driver::setRenderTargetCube(video::ITexture* texture, E_CUBEMAP_FACE face, bool clearBackBuffer, bool clearZBuffer, SColor color)
		{
			// check for right driver type
			if (texture && texture->getDriverType() != EDT_OPENGLES)
			{
				os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				return false;
			}

			if (texture && texture->getTextureType() != ETT_TEXTURE_CUBE)
			{
				os::Printer::log("Fatal Error: Set render target is not texture cube", ELL_ERROR);
				return false;
			}

			// check if we should set the previous RT back
			BridgeCalls->setActiveTexture(GL_TEXTURE0);
			ResetRenderStates = true;

			if (RenderTargetTexture != 0 && RenderTargetTexture != texture)
			{
				unbindRTT(RenderTargetTexture);
			}

			if (texture)
			{
				// we want to set a new target. so do this.
				BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
				RenderTargetTexture = texture;

				int mapFace[] =
				{
					0,	// left
					1,	// right
					2,	// up
					3,	// down
					5,	// front
					4	// back
				};

				// calls glDrawBuffer as well
				static_cast<COGLES3TextureCube*>(RenderTargetTexture)->bindRTT(mapFace[(int)face]);
				CurrentRendertargetSize = texture->getSize();
			}
			else
			{
				BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
				RenderTargetTexture = 0;
				CurrentRendertargetSize = core::dimension2d<u32>(0, 0);

				GLenum buffers[] = { GL_BACK };
				glDrawBuffers(1, buffers);
			}

			clearBuffers(clearBackBuffer, clearZBuffer, false, color);
			return true;
		}


		//! Sets multiple render targets
		bool COGLES3Driver::setRenderTarget(const core::array<video::IRenderTarget>& targets, bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil)
		{
			// if simply disabling the MRT via array call
			if (targets.size() == 0)
				return setRenderTarget(0, clearBackBuffer, clearZBuffer, color, depthStencil);
			// if disabling old MRT, but enabling new one as well
			if ((MRTargets.size() != 0) && (targets != MRTargets))
				setRenderTarget(0, clearBackBuffer, clearZBuffer, color, depthStencil);
			// if no change, simply clear buffers
			else if (targets == MRTargets)
			{
				clearBuffers(clearBackBuffer, clearZBuffer, false, color);
				return true;
			}

			// copy to storage for correct disabling
			MRTargets = targets;

			u32 maxMultipleRTTs = core::min_(static_cast<u32>(MaxMultipleRenderTargets), targets.size());

			// determine common size
			core::dimension2du rttSize = CurrentRendertargetSize;
			if (targets[0].TargetType == ERT_RENDER_TEXTURE)
			{
				if (!targets[0].RenderTexture)
				{
					os::Printer::log("Missing render texture for MRT.", ELL_ERROR);
					return false;
				}
				rttSize = targets[0].RenderTexture->getSize();
			}

			for (u32 i = 0; i < maxMultipleRTTs; ++i)
			{
				// check for right driver type
				if (targets[i].TargetType == ERT_RENDER_TEXTURE)
				{
					if (!targets[i].RenderTexture)
					{
						maxMultipleRTTs = i;
						os::Printer::log("Missing render texture for MRT.", ELL_WARNING);
						break;
					}
					if (targets[i].RenderTexture->getDriverType() != EDT_OPENGLES)
					{
						maxMultipleRTTs = i;
						os::Printer::log("Tried to set a texture not owned by this driver.", ELL_WARNING);
						break;
					}

					// check for valid render target
					if (!targets[i].RenderTexture->isRenderTarget() || !static_cast<COGLES3Texture*>(targets[i].RenderTexture)->isFrameBufferObject())
					{
						maxMultipleRTTs = i;
						os::Printer::log("Tried to set a non FBO-RTT as render target.", ELL_WARNING);
						break;
					}

					// check for valid size
					if (rttSize != targets[i].RenderTexture->getSize())
					{
						maxMultipleRTTs = i;
						os::Printer::log("Render target texture has wrong size.", ELL_WARNING);
						break;
					}
				}
			}
			if (maxMultipleRTTs == 0)
			{
				os::Printer::log("No valid MRTs.", ELL_ERROR);
				return false;
			}

			// init FBO, if any
			for (u32 i = 0; i < maxMultipleRTTs; ++i)
			{
				if (targets[i].TargetType == ERT_RENDER_TEXTURE)
				{
					setRenderTarget(targets[i].RenderTexture, false, false, 0x0, depthStencil);
					break; // bind only first RTT
				}
			}
			// init other main buffer, if necessary
			if (targets[0].TargetType != ERT_RENDER_TEXTURE)
				setRenderTarget(targets[0].TargetType, false, false, 0x0, depthStencil);

			// attach other textures and store buffers into array
			if (maxMultipleRTTs > 1)
			{
				CurrentTarget = ERT_MULTI_RENDER_TEXTURES;
				core::array<GLenum> MRTs;
				MRTs.set_used(maxMultipleRTTs);

				for (u32 i = 0; i < maxMultipleRTTs; i++)
				{
					BridgeCalls->setColorMaskIndexed(i,
						(targets[i].ColorMask & ECP_RED) ? GL_TRUE : GL_FALSE,
						(targets[i].ColorMask & ECP_GREEN) ? GL_TRUE : GL_FALSE,
						(targets[i].ColorMask & ECP_BLUE) ? GL_TRUE : GL_FALSE,
						(targets[i].ColorMask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);

					if (targets[i].BlendOp == EBO_NONE)
						BridgeCalls->setBlendIndexed(i, false);
					else
						BridgeCalls->setBlendIndexed(i, true);


					BridgeCalls->setBlendFuncIndexed(i, getGLBlend(targets[i].BlendFuncSrc), getGLBlend(targets[i].BlendFuncDst));
					switch (targets[i].BlendOp)
					{
					case EBO_SUBTRACT:
						BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_SUBTRACT);
						break;
					case EBO_REVSUBTRACT:
						BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_REVERSE_SUBTRACT);
						break;
					case EBO_MIN:
						BridgeCalls->setBlendEquationIndexed(i, GL_MIN);
						break;
					case EBO_MAX:
						BridgeCalls->setBlendEquationIndexed(i, GL_MAX);
						break;
					case EBO_MIN_FACTOR:
					case EBO_MIN_ALPHA:
						BridgeCalls->setBlendEquationIndexed(i, GL_MIN);
						break;
					case EBO_MAX_FACTOR:
					case EBO_MAX_ALPHA:
						BridgeCalls->setBlendEquationIndexed(i, GL_MAX);
						break;
					default:
						BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_ADD);
						break;
					}

					if (targets[i].TargetType == ERT_RENDER_TEXTURE)
					{
						GLenum attachment = GL_NONE;

						// attach texture to FrameBuffer Object on Color [i]
						attachment = GL_COLOR_ATTACHMENT0 + i;
						if ((i != 0) && (targets[i].RenderTexture != RenderTargetTexture))
							glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, static_cast<COGLES3Texture*>(targets[i].RenderTexture)->getOpenGLTextureName(), 0);

						MRTs[i] = attachment;
					}
					else
					{
						MRTs[i] = GL_BACK;
					}
				}

				glDrawBuffers(maxMultipleRTTs, MRTs.const_pointer());
			}

			clearBuffers(clearBackBuffer, clearZBuffer, false, color);
			return true;
		}


		//! set or reset render target
		bool COGLES3Driver::setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget, bool clearZBuffer, SColor color)
		{
			if (target != CurrentTarget)
				setRenderTarget(0, false, false, 0x0, 0);

			if (ERT_RENDER_TEXTURE == target)
			{
				os::Printer::log("For render textures call setRenderTarget with the actual texture as first parameter.", ELL_ERROR);
				return false;
			}
			if (ERT_MULTI_RENDER_TEXTURES == target)
			{
				os::Printer::log("For multiple render textures call setRenderTarget with the texture array as first parameter.", ELL_ERROR);
				return false;
			}

			GLenum buffers[] = { GL_BACK };
			glDrawBuffers(1, buffers);

			CurrentTarget = target;
			clearBuffers(clearTarget, clearZBuffer, false, color);
			return true;
		}


		// returns the current size of the screen or rendertarget
		const core::dimension2d<u32>& COGLES3Driver::getCurrentRenderTargetSize() const
		{
			if (CurrentRendertargetSize.Width == 0)
				return ScreenSize;
			else
				return CurrentRendertargetSize;
		}


		//! Clears the ZBuffer.
		void COGLES3Driver::clearZBuffer()
		{
			clearBuffers(false, true, false, 0x0);
		}


		//! Returns an image created from the last rendered frame.
		IImage* COGLES3Driver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
		{
			return NULL;
		}


		//! get depth texture for the given render target texture
		ITexture* COGLES3Driver::createDepthTexture(ITexture* texture, bool shared)
		{
			if ((texture->getDriverType() != EDT_OPENGLES) || (!texture->isRenderTarget()))
				return 0;

			if (shared)
			{
				for (u32 i = 0; i < DepthTextures.size(); ++i)
				{
					if (DepthTextures[i]->getSize() == texture->getSize())
					{
						DepthTextures[i]->grab();
						return DepthTextures[i];
					}
				}
				DepthTextures.push_back(new COGLES3RenderBuffer(texture->getSize(), "depth1", this));
				return DepthTextures.getLast();
			}
			return (new COGLES3RenderBuffer(texture->getSize(), "depth1", this));
		}


		void COGLES3Driver::removeDepthTexture(ITexture* texture)
		{
			for (u32 i = 0; i < DepthTextures.size(); ++i)
			{
				if (texture == DepthTextures[i])
				{
					DepthTextures.erase(i);
					return;
				}
			}
		}


		IVertexDescriptor* COGLES3Driver::addVertexDescriptor(const core::stringc& pName)
		{
			for (u32 i = 0; i < VertexDescriptor.size(); ++i)
				if (pName == VertexDescriptor[i]->getName())
					return VertexDescriptor[i];

			IVertexDescriptor* vertexDescriptor = new COGLES3VertexDescriptor(pName, VertexDescriptor.size(), MaterialRenderers.size());
			VertexDescriptor.push_back(vertexDescriptor);

			return vertexDescriptor;
		}


		void COGLES3Driver::setVertexDescriptor(IVertexDescriptor* vertexDescriptor)
		{
			if (LastVertexDescriptor != vertexDescriptor)
			{
				u32 ID = 0;

				for (u32 i = 0; i < VertexDescriptor.size(); ++i)
				{
					if (vertexDescriptor == VertexDescriptor[i])
					{
						ID = i;
						LastVertexDescriptor = VertexDescriptor[ID];
						break;
					}
				}

				// TODO
			}
		}


		//! Set/unset a clipping plane.
		bool COGLES3Driver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
		{
			if (index >= MaxUserClipPlanes)
				return false;

			UserClipPlanes[index].Plane = plane;
			enableClipPlane(index, enable);
			return true;
		}


		void COGLES3Driver::uploadClipPlane(u32 index)
		{
		}


		//! Enable/disable a clipping plane.
		void COGLES3Driver::enableClipPlane(u32 index, bool enable)
		{
		}


		core::dimension2du COGLES3Driver::getMaxTextureSize() const
		{
			return core::dimension2du(MaxTextureSize, MaxTextureSize);
		}


		//! Convert E_PRIMITIVE_TYPE to OpenGL equivalent
		GLenum COGLES3Driver::primitiveTypeToGL(scene::E_PRIMITIVE_TYPE type) const
		{
			switch (type)
			{
			case scene::EPT_POINTS:
				return GL_POINTS;
			case scene::EPT_LINE_STRIP:
				return GL_LINE_STRIP;
			case scene::EPT_LINE_LOOP:
				return GL_LINE_LOOP;
			case scene::EPT_LINES:
				return GL_LINES;
			case scene::EPT_TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			case scene::EPT_TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			case scene::EPT_TRIANGLES:
				return GL_TRIANGLES;
			case scene::EPT_POINT_SPRITES:
				return GL_POINTS;
			}
			return GL_TRIANGLES;
		}


		GLenum COGLES3Driver::getGLBlend(E_BLEND_FACTOR factor) const
		{
			GLenum r = 0;
			switch (factor)
			{
			case EBF_ZERO:			r = GL_ZERO; break;
			case EBF_ONE:			r = GL_ONE; break;
			case EBF_DST_COLOR:		r = GL_DST_COLOR; break;
			case EBF_ONE_MINUS_DST_COLOR:	r = GL_ONE_MINUS_DST_COLOR; break;
			case EBF_SRC_COLOR:		r = GL_SRC_COLOR; break;
			case EBF_ONE_MINUS_SRC_COLOR:	r = GL_ONE_MINUS_SRC_COLOR; break;
			case EBF_SRC_ALPHA:		r = GL_SRC_ALPHA; break;
			case EBF_ONE_MINUS_SRC_ALPHA:	r = GL_ONE_MINUS_SRC_ALPHA; break;
			case EBF_DST_ALPHA:		r = GL_DST_ALPHA; break;
			case EBF_ONE_MINUS_DST_ALPHA:	r = GL_ONE_MINUS_DST_ALPHA; break;
			case EBF_SRC_ALPHA_SATURATE:	r = GL_SRC_ALPHA_SATURATE; break;
			}
			return r;
		}

		GLuint COGLES3Driver::getActiveGLSLProgram()
		{
			return ActiveGLSLProgram;
		}

		void COGLES3Driver::setActiveGLSLProgram(GLuint program)
		{
			ActiveGLSLProgram = program;
		}

		GLenum COGLES3Driver::getZBufferBits() const
		{
			GLenum bits = 0;
			switch (Params.ZBufferBits)
			{
			case 16:
				bits = GL_DEPTH_COMPONENT16;
				break;
			case 24:
				bits = GL_DEPTH_COMPONENT24;
				break;
			case 32:
				bits = GL_DEPTH_COMPONENT32F;
				break;
			default:
				bits = GL_DEPTH_COMPONENT;
				break;
			}
			return bits;
		}

		const SMaterial& COGLES3Driver::getCurrentMaterial() const
		{
			return Material;
		}

		COGLES3CallBridge* COGLES3Driver::getBridgeCalls() const
		{
			return BridgeCalls;
		}

		COGLES3CallBridge::COGLES3CallBridge(COGLES3Driver* driver) : Driver(driver),
			AlphaMode(GL_ALWAYS), AlphaRef(0.0f), AlphaTest(false),
			ClientStateVertex(false), ClientStateNormal(false), ClientStateColor(false),
			CullFaceMode(GL_BACK), CullFace(false),
			DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), EnableScissor(false),
			ActiveTexture(GL_TEXTURE0), ClientActiveTexture(GL_TEXTURE0), ViewportX(0), ViewportY(0), ScissorX(0), ScissorY(0)
		{
			FrameBufferCount = core::max_(static_cast<GLuint>(1), static_cast<GLuint>(Driver->MaxMultipleRenderTargets));

			BlendEquation = new GLenum[FrameBufferCount];
			BlendSourceRGB = new GLenum[FrameBufferCount];
			BlendDestinationRGB = new GLenum[FrameBufferCount];
			BlendSourceAlpha = new GLenum[FrameBufferCount];
			BlendDestinationAlpha = new GLenum[FrameBufferCount];
			Blend = new bool[FrameBufferCount];

			ColorMask = new bool[FrameBufferCount][4];

			for (u32 i = 0; i < FrameBufferCount; ++i)
			{
				BlendEquation[i] = GL_FUNC_ADD;
				BlendSourceRGB[i] = GL_ONE;
				BlendDestinationRGB[i] = GL_ZERO;
				BlendSourceAlpha[i] = GL_ONE;
				BlendDestinationAlpha[i] = GL_ZERO;

				Blend[i] = false;

				for (u32 j = 0; j < 4; ++j)
					ColorMask[i][j] = true;
			}

			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				Texture[i] = 0;
			}

			// Initial OpenGL values from specification.
			//glAlphaFunc(GL_ALWAYS, 0.0f);
			//glDisable(GL_ALPHA_TEST);

			if (Driver->queryFeature(EVDF_BLEND_OPERATIONS))
			{
				glBlendEquation(GL_FUNC_ADD);
			}

			glBlendFunc(GL_ONE, GL_ZERO);
			glDisable(GL_BLEND);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glCullFace(GL_BACK);
			glDisable(GL_CULL_FACE);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glDisable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0);

			const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
			ViewportWidth = ScreenSize.Width;
			ViewportHeight = ScreenSize.Height;
			glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);

			ScissorWidth = ViewportWidth;
			ScissorHeight = ViewportHeight;
		}

		COGLES3CallBridge::~COGLES3CallBridge()
		{
			delete[] BlendEquation;
			delete[] BlendSourceRGB;
			delete[] BlendDestinationRGB;
			delete[] BlendSourceAlpha;
			delete[] BlendDestinationAlpha;
			delete[] Blend;

			delete[] ColorMask;
		}

		void COGLES3CallBridge::setAlphaFunc(GLenum mode, GLclampf ref)
		{
			if (AlphaMode != mode || AlphaRef != ref)
			{
				// glAlphaFunc(mode, ref);

				AlphaMode = mode;
				AlphaRef = ref;
			}
		}

		void COGLES3CallBridge::setAlphaTest(bool enable)
		{
			if (AlphaTest != enable)
			{
				/*
				if (enable)
					glEnable(GL_ALPHA_TEST);
				else
					glDisable(GL_ALPHA_TEST);
				*/
				AlphaTest = enable;
			}
		}

		void COGLES3CallBridge::setBlendEquation(GLenum mode)
		{
			if (BlendEquation[0] != mode)
			{
				glBlendEquation(mode);

				for (GLuint i = 0; i < FrameBufferCount; ++i)
					BlendEquation[i] = mode;
			}
		}

		void COGLES3CallBridge::setBlendEquationIndexed(GLuint index, GLenum mode)
		{
			if (index < FrameBufferCount && BlendEquation[index] != mode)
			{
				// glBlendEquationi(index, mode);
				BlendEquation[index] = mode;
			}
		}

		void COGLES3CallBridge::setBlendFunc(GLenum source, GLenum destination)
		{
			if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
				BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination)
			{
				glBlendFunc(source, destination);

				for (GLuint i = 0; i < FrameBufferCount; ++i)
				{
					BlendSourceRGB[i] = source;
					BlendDestinationRGB[i] = destination;
					BlendSourceAlpha[i] = source;
					BlendDestinationAlpha[i] = destination;
				}
			}
		}

		void COGLES3CallBridge::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
		{
			if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
			{
				if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
					BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha)
				{
					glBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

					for (GLuint i = 0; i < FrameBufferCount; ++i)
					{
						BlendSourceRGB[i] = sourceRGB;
						BlendDestinationRGB[i] = destinationRGB;
						BlendSourceAlpha[i] = sourceAlpha;
						BlendDestinationAlpha[i] = destinationAlpha;
					}
				}
			}
			else
			{
				setBlendFunc(sourceRGB, destinationRGB);
			}
		}

		void COGLES3CallBridge::setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination)
		{
			if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
				BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
			{
				// glBlendFunci(index, source, destination);
				BlendSourceRGB[index] = source;
				BlendDestinationRGB[index] = destination;
				BlendSourceAlpha[index] = source;
				BlendDestinationAlpha[index] = destination;
			}
		}

		void COGLES3CallBridge::setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
		{
			if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
			{
				if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
					BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
				{
					// glBlendFuncSeparatei(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

					BlendSourceRGB[index] = sourceRGB;
					BlendDestinationRGB[index] = destinationRGB;
					BlendSourceAlpha[index] = sourceAlpha;
					BlendDestinationAlpha[index] = destinationAlpha;
				}
			}
			else
			{
				setBlendFuncIndexed(index, sourceRGB, destinationRGB);
			}
		}

		void COGLES3CallBridge::setBlend(bool enable)
		{
			if (Blend[0] != enable)
			{
				if (enable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);

				for (GLuint i = 0; i < FrameBufferCount; ++i)
					Blend[i] = enable;
			}
		}

		void COGLES3CallBridge::setBlendIndexed(GLuint index, bool enable)
		{
			if (index < FrameBufferCount && Blend[index] != enable)
			{
				/*
				if (enable)
					glEnablei(GL_BLEND, index);
				else
					glDisablei(GL_BLEND, index);
				*/
				Blend[index] = enable;
			}
		}

		void COGLES3CallBridge::setColorMask(bool red, bool green, bool blue, bool alpha)
		{
			if (ColorMask[0][0] != red || ColorMask[0][1] != green || ColorMask[0][2] != blue || ColorMask[0][3] != alpha)
			{
				glColorMask(red, green, blue, alpha);

				for (GLuint i = 0; i < FrameBufferCount; ++i)
				{
					ColorMask[i][0] = red;
					ColorMask[i][1] = green;
					ColorMask[i][2] = blue;
					ColorMask[i][3] = alpha;
				}
			}
		}

		void COGLES3CallBridge::setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha)
		{
			if (index < FrameBufferCount && (ColorMask[index][0] != red || ColorMask[index][1] != green || ColorMask[index][2] != blue || ColorMask[index][3] != alpha))
			{
				// glColorMaski(index, red, green, blue, alpha);
				ColorMask[index][0] = red;
				ColorMask[index][1] = green;
				ColorMask[index][2] = blue;
				ColorMask[index][3] = alpha;
			}
		}

		void COGLES3CallBridge::setClientState(bool vertex, bool normal, bool color)
		{
			if (ClientStateVertex != vertex)
			{
				ClientStateVertex = vertex;
			}

			if (ClientStateNormal != normal)
			{
				ClientStateNormal = normal;
			}

			if (ClientStateColor != color)
			{
				ClientStateColor = color;
			}
		}

		void COGLES3CallBridge::setCullFaceFunc(GLenum mode)
		{
			if (CullFaceMode != mode)
			{
				glCullFace(mode);
				CullFaceMode = mode;
			}
		}

		void COGLES3CallBridge::setCullFace(bool enable)
		{
			if (CullFace != enable)
			{
				if (enable)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
				CullFace = enable;
			}
		}

		void COGLES3CallBridge::setDepthFunc(GLenum mode)
		{
			if (DepthFunc != mode)
			{
				glDepthFunc(mode);
				DepthFunc = mode;
			}
		}

		void COGLES3CallBridge::setDepthMask(bool enable)
		{
			if (DepthMask != enable)
			{
				if (enable)
					glDepthMask(GL_TRUE);
				else
					glDepthMask(GL_FALSE);
				DepthMask = enable;
			}
		}

		void COGLES3CallBridge::setDepthTest(bool enable)
		{
			if (DepthTest != enable)
			{
				if (enable)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);
				DepthTest = enable;
			}
		}

		void COGLES3CallBridge::enableScissor(bool enable)
		{
			if (EnableScissor != enable)
			{
				if (enable)
					glEnable(GL_SCISSOR_TEST);
				else
					glDisable(GL_SCISSOR_TEST);
				EnableScissor = enable;
			}
		}

		void COGLES3CallBridge::setScissor(GLint scissorX, GLint scissorY, GLsizei scissorWidth, GLsizei scissorHeight)
		{
			if (ScissorX != scissorX || ScissorY != scissorY || ScissorWidth != scissorWidth || ScissorHeight != scissorHeight)
			{
				glScissor(scissorX, scissorY, scissorWidth, scissorHeight);
				ScissorX = scissorX;
				ScissorY = scissorY;
				ScissorWidth = scissorWidth;
				ScissorHeight = scissorHeight;
			}
		}

		void COGLES3CallBridge::setMatrixMode(GLenum mode)
		{
			if (MatrixMode != mode)
			{
				MatrixMode = mode;
			}
		}

		void COGLES3CallBridge::setActiveTexture(GLenum texture)
		{
			if (ActiveTexture != texture)
			{
				glActiveTexture(texture);
				ActiveTexture = texture;
			}
		}

		void COGLES3CallBridge::setClientActiveTexture(GLenum texture)
		{
			if (ClientActiveTexture != texture)
			{
				ClientActiveTexture = texture;
			}
		}

		void COGLES3CallBridge::resetTexture(const ITexture* texture)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				setActiveTexture(GL_TEXTURE0 + i);

				if (Texture[i] == texture)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					Texture[i] = 0;
				}
			}
		}

		void COGLES3CallBridge::setTexture(GLuint stage)
		{
			if (stage < MATERIAL_MAX_TEXTURES)
			{
				setActiveTexture(GL_TEXTURE0 + stage);

				if (Texture[stage] != Driver->CurrentTexture[stage])
				{
					if (Driver->CurrentTexture[stage] != NULL)
					{
						if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_2D)
						{
							// texture 2d
							glBindTexture(GL_TEXTURE_2D, static_cast<const COGLES3Texture*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
						}
						else if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_CUBE)
						{
							// texture cube
							glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<const COGLES3TextureCube*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
						}
						else if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_ARRAY)
						{
							// texture array			
							glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<const COGLES3TextureArray*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
						}

						// todo profile
						Driver->incTextureChange();
					}

					Texture[stage] = Driver->CurrentTexture[stage];
				}
			}
		}

		void COGLES3CallBridge::setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
		{
			if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
			{
				glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
				ViewportX = viewportX;
				ViewportY = viewportY;
				ViewportWidth = viewportWidth;
				ViewportHeight = viewportHeight;
				
				Driver->ViewPort = core::recti(viewportX, viewportY, viewportWidth, viewportHeight);
			}
		}

	} // end namespace
} // end namespace

#endif

namespace irr
{
	namespace video
	{
		IVideoDriver* createOGLES3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io)
		{
#ifdef _IRR_COMPILE_WITH_OGLES3_
			COGLES3Driver* ogl = new COGLES3Driver(params, io);
			if (!ogl->initDriver())
			{
				ogl->drop();
				ogl = 0;
			}
			return ogl;
#else
			return 0;
#endif
		}
	} // end namespace
} // end namespace


