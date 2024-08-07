// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_OPEN_GL_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"

namespace irr
{
	class CIrrDeviceWin32;	
}

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
// also includes the OpenGL stuff
#include "COpenGLExtensionHandler.h"

#ifdef _IRR_COMPILE_WITH_CG_
#include "Cg/cg.h"
#endif

namespace irr
{

namespace video
{
	class COpenGLDriver;
    class COpenGLCallBridge;
	class COpenGLTexture;

	class COpenGLVertexAttribute : public IVertexAttribute
	{
	public:
		COpenGLVertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID, u32 layerCount);

		virtual void setOffset(u32 offset);

		// Add location layer.
		void addLocationLayer();

		// Get attribute location in a shader program.
		s32 getLocation(u32 materialType) const;

		// Set attribute location in a shader program.
		void setLocation(u32 location, u32 materialType);

		// -1 -> location entry doesn't exist, eg. material type or id is wrong; 0 - location isn't cached, so use glGetAttribLocation call; 1 - location is cached.
		s32 getLocationStatus(u32 materialType) const;

	protected:
		core::array<bool> Cache;
		core::array<s32> Location;
	};

	class COpenGLVertexDescriptor : public IVertexDescriptor
	{
	public:
		COpenGLVertexDescriptor(const core::stringc& name, u32 id, u32 layerCount);

		virtual void setID(u32 id);

		virtual IVertexAttribute* addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID) _IRR_OVERRIDE_;

		virtual void clearAttribute() _IRR_OVERRIDE_;

		void addLocationLayer();

	protected:
		core::array<COpenGLVertexAttribute> Attribute;

		u32 LayerCount;
	};

	class COpenGLHardwareBuffer : public IHardwareBuffer
	{
	public:
		COpenGLHardwareBuffer(scene::IIndexBuffer* indexBuffer, COpenGLDriver* driver);
		COpenGLHardwareBuffer(scene::IVertexBuffer* vertexBuffer, COpenGLDriver* driver);
		~COpenGLHardwareBuffer();

		bool update(const scene::E_HARDWARE_MAPPING mapping, const u32 size, const void* data);

		inline GLuint getBufferID() const;
		inline void removeFromArray(bool status);
	
	private:
		COpenGLDriver* Driver;

		GLuint BufferID;
		bool RemoveFromArray;

		void* LinkedBuffer;
	};

	class COpenGLDriver : public CNullDriver, public IMaterialRendererServices, public COpenGLExtensionHandler
	{
		friend class COpenGLCallBridge;
		friend class COpenGLHardwareBuffer;
		friend class COpenGLTexture;
		friend class COpenGLTextureArray;
		friend class COpenGLTextureCube;
	public:
		// Information about state of fixed pipeline activity.
		enum E_OPENGL_FIXED_PIPELINE_STATE
		{
			EOFPS_ENABLE = 0, // fixed pipeline.
			EOFPS_DISABLE, // programmable pipeline.
			EOFPS_ENABLE_TO_DISABLE, // switch from fixed to programmable pipeline.
			EOFPS_DISABLE_TO_ENABLE // switch from programmable to fixed pipeline.
		};

		#if defined(_IRR_WINDOWS_API_) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		PIXELFORMATDESCRIPTOR PixelFormatInfo;
		IVideoRenderTarget *CurrentVRT;

		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io);
		//! inits the windows specific parts of the open gl driver
		bool initDriver();
		bool changeRenderContext(const SExposedVideoData& videoData);

		virtual IVideoRenderTarget* addVideoRenderTarget(void *hwnd, u32 w, u32 h) _IRR_OVERRIDE_;

		virtual void removeVideoRenderTarget(IVideoRenderTarget* vrt) _IRR_OVERRIDE_;

		virtual void setVideoRenderTarget(IVideoRenderTarget* vrt) _IRR_OVERRIDE_;
		#endif

		#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io);
		//! inits the windows specific parts of the open gl driver
		bool initDriver();		
		#endif

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io);
		//! inits the GLX specific parts of the open gl driver
		bool initDriver();
		bool changeRenderContext(const SExposedVideoData& videoData);
#endif

		//! destructor
		virtual ~COpenGLDriver();

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
				SColor color=SColor(255,0,0,0),
				const SExposedVideoData& videoData=SExposedVideoData(),
				core::rect<s32>* sourceRect=0) _IRR_OVERRIDE_;

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene() _IRR_OVERRIDE_;

		virtual IHardwareBuffer* createHardwareBuffer(scene::IIndexBuffer* indexBuffer) _IRR_OVERRIDE_;

		virtual IHardwareBuffer* createHardwareBuffer(scene::IVertexBuffer* vertexBuffer) _IRR_OVERRIDE_;

		void removeAllHardwareBuffers();

		//! Create occlusion query.
		/** Use node for identification and mesh for occlusion test. */
		virtual void addOcclusionQuery(scene::ISceneNode* node,
				const scene::IMesh* mesh=0) _IRR_OVERRIDE_;

		//! Remove occlusion query.
		virtual void removeOcclusionQuery(scene::ISceneNode* node) _IRR_OVERRIDE_;

		//! Run occlusion query. Draws mesh stored in query.
		/** If the mesh shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		virtual void runOcclusionQuery(scene::ISceneNode* node, bool visible=false) _IRR_OVERRIDE_;

		//! Update occlusion query. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		virtual void updateOcclusionQuery(scene::ISceneNode* node, bool block=true) _IRR_OVERRIDE_;

		//! Return query result.
		/** Return value is the number of visible pixels/fragments.
		The value is a safe approximation, i.e. can be larger then the
		actual value of pixels. */
		virtual u32 getOcclusionQueryResult(scene::ISceneNode* node) const _IRR_OVERRIDE_;

		virtual void drawMeshBuffer(const scene::IMeshBuffer* mb) _IRR_OVERRIDE_;
		
		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const _IRR_OVERRIDE_
		{
			return FeatureEnabled[feature] && COpenGLExtensionHandler::queryFeature(feature);
		}

		//! Sets a material. All 3d drawing functions draw geometry now
		//! using this material.
		//! \param material: Material to be used from now on.
		virtual void setMaterial(const SMaterial& material) _IRR_OVERRIDE_;

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
					const core::vector3df& end,
					SColor color = SColor(255,255,255,255)) _IRR_OVERRIDE_;

		//! \return Returns the name of the video driver. Example: In case of the Direct3D8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const _IRR_OVERRIDE_;

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color) _IRR_OVERRIDE_;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) _IRR_OVERRIDE_;

		virtual void setScissor(const core::rect<s32>& area) _IRR_OVERRIDE_;

		virtual void enableScissor(bool b) _IRR_OVERRIDE_;

		//! Sets the fog mode.
		virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog) _IRR_OVERRIDE_;

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Adds a new material renderer to the video device.
		virtual s32 addMaterialRenderer(IMaterialRenderer* renderer, const char* name = 0) _IRR_OVERRIDE_;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! Can be called by an IMaterialRenderer to make its work easier.
		virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
			bool resetAllRenderstates) _IRR_OVERRIDE_;

		//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
		virtual void setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates);

		//! sets the current Texture
		//! Returns whether setting was a success or not.
		bool setActiveTexture(u32 stage, const video::ITexture* texture);

		//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
		//! Returns whether disabling was successful or not.
		bool disableTextures(u32 fromStage=0);

		//! Adds a new material renderer to the VideoDriver, using
		//! extGLGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status)
		//! pixel and/or vertex shaders to render geometry.
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData) _IRR_OVERRIDE_;

		//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
		virtual s32 addHighLevelShaderMaterial(
				const c8* vertexShaderProgram,
				const c8* vertexShaderEntryPointName,
				E_VERTEX_SHADER_TYPE vsCompileTarget,
				const c8* pixelShaderProgram,
				const c8* pixelShaderEntryPointName,
				E_PIXEL_SHADER_TYPE psCompileTarget,
				const c8* geometryShaderProgram,
				const c8* geometryShaderEntryPointName = "main",
				E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
				scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
				scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
				u32 verticesOut = 0,
				IShaderConstantSetCallBack* callback = 0,
				E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
				s32 userData = 0,
				E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT) _IRR_OVERRIDE_;

		//! Returns a pointer to the IVideoDriver interface. (Implementation for
		//! IMaterialRendererServices)
		virtual IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount() const _IRR_OVERRIDE_;

		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
				const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;
		
		virtual ITexture* addRenderTargetTextureArray(const core::dimension2d<u32>& size, u32 arraySize,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;

		//! sets a render target
		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
						bool clearZBuffer, SColor color, video::ITexture* depthStencil) _IRR_OVERRIDE_;

		//! set a cube render target
		virtual bool setRenderTargetCube(video::ITexture* texture, E_CUBEMAP_FACE face, bool clearTarget = true,
			bool clearZBuffer = true,
			SColor color = video::SColor(0, 0, 0, 0)) _IRR_OVERRIDE_;		

		virtual bool setRenderTargetArray(video::ITexture* texture, int arrayID, bool clearTarget = true,
			bool clearZBuffer = true,
			SColor color = video::SColor(0, 0, 0, 0)) _IRR_OVERRIDE_;

		//! sets multiple render targets
		virtual bool setRenderTarget(const core::array<video::IRenderTarget>& texture,
					bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil) _IRR_OVERRIDE_;

		//! set or reset special render targets
		virtual bool setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget,
					bool clearZBuffer, SColor color) _IRR_OVERRIDE_;

		//! add cubemap render target
		virtual ITexture* addRenderTargetCubeTexture(const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format) _IRR_OVERRIDE_;

		//! add texture cube
		virtual ITexture* getTextureCube(
			IImage *imageX1,
			IImage *imageX2,
			IImage *imageY1,
			IImage *imageY2,
			IImage *imageZ1,
			IImage *imageZ2) _IRR_OVERRIDE_;

		//! add texture array
		virtual ITexture* getTextureArray(IImage** images, u32 num) _IRR_OVERRIDE_;

		//! Clears the ZBuffer.
		virtual void clearZBuffer() _IRR_OVERRIDE_;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) _IRR_OVERRIDE_;

		//! checks if an OpenGL error has happend and prints it
		//! for performance reasons only available in debug mode
		bool testGLError();

		//! Set/unset a clipping plane.
		//! There are at least 6 clipping planes available for the user to set at will.
		//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
		//! \param plane: The plane itself.
		//! \param enable: If true, enable the clipping plane else disable it.
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable=false) _IRR_OVERRIDE_;

		//! Enable/disable a clipping plane.
		//! There are at least 6 clipping planes available for the user to set at will.
		//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
		//! \param enable: If true, enable the clipping plane else disable it.
		virtual void enableClipPlane(u32 index, bool enable) _IRR_OVERRIDE_;

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo() _IRR_OVERRIDE_ {return VendorName;}

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const _IRR_OVERRIDE_;

		ITexture* createDepthTexture(ITexture* texture, bool shared=true);

		void removeDepthTexture(ITexture* texture);
		
		//! Removes a texture from the texture cache and deletes it, freeing lot of memory.
		virtual void removeTexture(ITexture* texture) _IRR_OVERRIDE_;
		
		virtual IVertexDescriptor* addVertexDescriptor(const core::stringc& pName) _IRR_OVERRIDE_;

		//! Convert E_PRIMITIVE_TYPE to OpenGL equivalent
		GLenum primitiveTypeToGL(scene::E_PRIMITIVE_TYPE type) const;

		//! Convert E_BLEND_FACTOR to OpenGL equivalent
		GLenum getGLBlend(E_BLEND_FACTOR factor) const;

		//! Get ZBuffer bits.
		GLenum getZBufferBits() const;

		//! Get current material.
		const SMaterial& getCurrentMaterial() const;
		
		GLuint getActiveGLSLProgram();
		
		void setActiveGLSLProgram(GLuint program);

		//! Get bridge calls.
		COpenGLCallBridge* getBridgeCalls() const;

		//! Get Cg context
		#ifdef _IRR_COMPILE_WITH_CG_
		const CGcontext& getCgContext();
		#endif

	private:

		void setVertexDescriptor(IVertexDescriptor* vertexDescriptor);

		//! clears the zbuffer and color buffer
		void clearBuffers(bool backBuffer, bool zBuffer, bool stencilBuffer, SColor color);

		void uploadClipPlane(u32 index);

		//! inits the parts of the open gl driver used on all platforms
		bool genericDriverInit();
		//! returns a device dependent texture from a software surface (IImage)
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData) _IRR_OVERRIDE_;

		//! creates a transposed matrix in supplied GLfloat array to pass to OpenGL
		inline void getGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m);
		inline void getGLTextureMatrix(GLfloat gl_matrix[16], const core::matrix4& m);

		//! get native wrap mode value
		GLint getTextureWrapMode(const u8 clamp);

		//! sets the needed renderstates
		void setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		// returns the current size of the screen or rendertarget
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const _IRR_OVERRIDE_;

		void createMaterialRenderers();

		//! Assign a hardware light to the specified requested light, if any
		//! free hardware lights exist.
		//! \param[in] lightIndex: the index of the requesting light
		void assignHardwareLight(u32 lightIndex);

		void renderArray(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType);
		void renderInstance(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType, u32 instanceCount);

		void unbindRTT(ITexture *texture);

		// Bridge calls.
		COpenGLCallBridge* BridgeCalls;

		core::stringw Name;
		core::matrix4 Matrices[ETS_COUNT];

		//! bool to make all renderstates reset if set to true.
		bool ResetRenderStates;
		u8 AntiAlias;

		SMaterial Material, LastMaterial;
		ITexture* RenderTargetTexture;
		core::array<video::IRenderTarget> MRTargets;

		class STextureStageCache
		{
			const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		public:
			STextureStageCache()
			{
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				{
					CurrentTexture[i] = 0;
				}
			}

			~STextureStageCache()
			{
				clear();
			}

			void set(u32 stage, const ITexture* tex)
			{
				if (stage<MATERIAL_MAX_TEXTURES)
				{
					const ITexture* oldTexture=CurrentTexture[stage];
					if (tex)
						tex->grab();
					CurrentTexture[stage]=tex;
					if (oldTexture)
						oldTexture->drop();
				}
			}

			const ITexture* operator[](int stage) const
			{
				//if ((u32)stage<MATERIAL_MAX_TEXTURES)
					return CurrentTexture[stage];
				//else
				//	return 0;
			}

			void remove(ITexture* tex)
			{
				for (s32 i = MATERIAL_MAX_TEXTURES-1; i>= 0; --i)
				{
					if (CurrentTexture[i] == tex)
					{
						tex->drop();
						CurrentTexture[i] = 0;
					}
				}
			}

			void clear()
			{
				// Drop all the CurrentTexture handles
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				{
					if (CurrentTexture[i])
					{
						CurrentTexture[i]->drop();
						CurrentTexture[i] = 0;
					}
				}
			}
		};
		STextureStageCache CurrentTexture;

		core::array<ITexture*> DepthTextures;
		struct SUserClipPlane
		{
			SUserClipPlane() : Enabled(false) {}
			core::plane3df Plane;
			bool Enabled;
		};
		core::array<SUserClipPlane> UserClipPlanes;

		core::dimension2d<u32> CurrentRendertargetSize;

		core::stringc VendorName;

		core::matrix4 TextureFlipMatrix;

		//! Color buffer format
		ECOLOR_FORMAT ColorFormat;

		//! Render target type for render operations
		E_RENDER_TARGET CurrentTarget;

		SIrrlichtCreationParameters Params;

		//! Status of vertex attribute (Disabled/Enabled). Max 16 attributes per vertex.
		bool VertexAttributeStatus[16];

		//! Active GLSL program
		GLuint ActiveGLSLProgram;

		//! Last used vertex descriptor
		IVertexDescriptor* LastVertexDescriptor;
		core::array<COpenGLHardwareBuffer*> HardwareBuffer;

		#ifdef _IRR_WINDOWS_API_
			HDC HDc; // Private GDI Device Context
			HWND Window;
		#endif
		
		#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
			GLXDrawable Drawable;
			Display* X11Display;
		#endif

		#ifdef _IRR_COMPILE_WITH_CG_
		CGcontext CgContext;
		#endif

		E_DEVICE_TYPE DeviceType;
	};

	//! This bridge between Irlicht pseudo OpenGL calls
	//! and true OpenGL calls.

	class COpenGLCallBridge
	{
	public:
		COpenGLCallBridge(COpenGLDriver* driver);
		~COpenGLCallBridge();

		// Alpha calls.

		void setAlphaFunc(GLenum mode, GLclampf ref);

		void setAlphaTest(bool enable);

		// Blending calls.

		void setBlendEquation(GLenum mode);

		void setBlendEquationIndexed(GLuint index, GLenum mode);

		void setBlendFunc(GLenum source, GLenum destination);

		void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

		void setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination);

		void setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

		void setBlend(bool enable);

		void setBlendIndexed(GLuint index, bool enable);

		// Client state calls.

		void setClientState(bool vertex, bool normal, bool color);

		// Color Mask.

		void setColorMask(bool red, bool green, bool blue, bool alpha);

		void setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha);

		// Cull face calls.

		void setCullFaceFunc(GLenum mode);

		void setCullFace(bool enable);

		// Depth calls.

		void setDepthFunc(GLenum mode);

		void setDepthMask(bool enable);

		void setDepthTest(bool enable);

		// Scissor

		void enableScissor(bool enable);

		void setScissor(GLint scissorX, GLint scissorY, GLsizei scissorWidth, GLsizei scissorHeight);

		// Matrix calls.

		void setMatrixMode(GLenum mode);

		// Texture calls.

		void resetTexture(const ITexture* texture);

		void setActiveTexture(GLenum texture);

		void setClientActiveTexture(GLenum texture);

		void setTexture(GLuint stage);

		// Viewport calls.

		void setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight);

	private:
		COpenGLDriver* Driver;

		GLuint FrameBufferCount;

		GLenum AlphaMode;
		GLclampf AlphaRef;
		bool AlphaTest;

		GLenum* BlendEquation;
		GLenum* BlendSourceRGB;
		GLenum* BlendDestinationRGB;
		GLenum* BlendSourceAlpha;
		GLenum* BlendDestinationAlpha;
		bool* Blend;

		bool ClientStateVertex;
		bool ClientStateNormal;
		bool ClientStateColor;

		bool (*ColorMask)[4];

		GLenum CullFaceMode;
		bool CullFace;

		GLenum DepthFunc;
		bool DepthMask;
		bool DepthTest;
		bool EnableScissor;		

		GLenum MatrixMode;

		GLenum ActiveTexture;
		GLenum ClientActiveTexture;

		const ITexture* Texture[MATERIAL_MAX_TEXTURES];

		GLint ViewportX;
		GLint ViewportY;
		GLsizei ViewportWidth;
		GLsizei ViewportHeight;

		GLint ScissorX;
		GLint ScissorY;
		GLsizei ScissorWidth;
		GLsizei ScissorHeight;
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OPENGL_
#endif

