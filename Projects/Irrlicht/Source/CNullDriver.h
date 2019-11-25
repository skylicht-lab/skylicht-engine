// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_NULL_H_INCLUDED__
#define __C_VIDEO_NULL_H_INCLUDED__

#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"
#include "irrMap.h"
#include "IAttributes.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "IMeshSceneNode.h"
#include "CVertexDescriptor.h"
#include "CFPSCounter.h"
#include "S3DVertex.h"
#include "SLight.h"
#include "SExposedVideoData.h"

#ifdef _MSC_VER
#pragma warning( disable: 4996)
#endif

namespace irr
{
namespace io
{
	class IWriteFile;
	class IReadFile;
} // end namespace io
namespace video
{
	class IImageLoader;
	class IImageWriter;

	class CNullDriver : public IVideoDriver, public IGPUProgrammingServices
	{
	public:

		//! constructor
		CNullDriver(io::IFileSystem* io, const core::dimension2d<u32>& screenSize);

		//! destructor
		virtual ~CNullDriver();

		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
				SColor color=SColor(255,0,0,0),
				const SExposedVideoData& videoData=SExposedVideoData(),
				core::rect<s32>* sourceRect=0) _IRR_OVERRIDE_;

		virtual bool endScene() _IRR_OVERRIDE_;

		virtual bool createVertexDescriptors();

		//! Disable a feature of the driver.
		virtual void disableFeature(E_VIDEO_DRIVER_FEATURE feature, bool flag=true) _IRR_OVERRIDE_;

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const _IRR_OVERRIDE_;

		//! Get attributes of the actual video driver
		virtual const io::IAttributes& getDriverAttributes() const _IRR_OVERRIDE_;

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat) _IRR_OVERRIDE_;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const _IRR_OVERRIDE_;

		//! Retrieve the number of image loaders
		virtual u32 getImageLoaderCount() const _IRR_OVERRIDE_;

		//! Retrieve the given image loader
		virtual IImageLoader* getImageLoader(u32 n) _IRR_OVERRIDE_;

		//! Retrieve the number of image writers
		virtual u32 getImageWriterCount() const _IRR_OVERRIDE_;

		//! Retrieve the given image writer
		virtual IImageWriter* getImageWriter(u32 n) _IRR_OVERRIDE_;

		//! sets a material
		virtual void setMaterial(const SMaterial& material) _IRR_OVERRIDE_;

		//! loads a Texture
		virtual ITexture* getTexture(const io::path& filename) _IRR_OVERRIDE_;

		//! loads a Texture
		virtual ITexture* getTexture(io::IReadFile* file) _IRR_OVERRIDE_;

		//! load array Texture
		virtual ITexture* getTextureArray(core::array<io::path>& files) _IRR_OVERRIDE_;

		virtual ITexture* getTextureArray(IImage** images, u32 num) _IRR_OVERRIDE_;

		virtual ITexture* getTextureCube(
			const io::path& filenameX1,
			const io::path& filenameX2,
			const io::path& filenameY1,
			const io::path& filenameY2,
			const io::path& filenameZ1,
			const io::path& filenameZ2) _IRR_OVERRIDE_;

		virtual ITexture* getTextureCube(
			IImage *imageX1,
			IImage *imageX2,
			IImage *imageY1,
			IImage *imageY2,
			IImage *imageZ1,
			IImage *imageZ2) _IRR_OVERRIDE_;

		//! Returns a texture by index
		virtual ITexture* getTextureByIndex(u32 index) _IRR_OVERRIDE_;

		//! Returns amount of textures currently loaded
		virtual u32 getTextureCount() const _IRR_OVERRIDE_;

		//! Renames a texture
		virtual void renameTexture(ITexture* texture, const io::path& newName) _IRR_OVERRIDE_;

		//! creates a Texture
		virtual ITexture* addTexture(const core::dimension2d<u32>& size, const io::path& name, ECOLOR_FORMAT format = ECF_A8R8G8B8) _IRR_OVERRIDE_;

		//! sets a render target
		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
						bool clearZBuffer, SColor color, video::ITexture* depthStencil) _IRR_OVERRIDE_;

		//! Sets multiple render targets
		virtual bool setRenderTarget(const core::array<video::IRenderTarget>& texture,
					bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil) _IRR_OVERRIDE_;

		//! set or reset special render targets
		virtual bool setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget,
					bool clearZBuffer, SColor color) _IRR_OVERRIDE_;

		virtual bool setRenderTargetCube(video::ITexture* texture, E_CUBEMAP_FACE face,
			bool clearTarget, bool clearZBuffer,
			SColor color) _IRR_OVERRIDE_;

		virtual bool setRenderTargetArray(video::ITexture* texture, int arrayID, bool clearTarget,
			bool clearZBuffer,
			SColor color) _IRR_OVERRIDE_;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area) _IRR_OVERRIDE_;

		//! gets the area of the current viewport
		virtual const core::rect<s32>& getViewPort() const _IRR_OVERRIDE_;
		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color = SColor(255,255,255,255)) _IRR_OVERRIDE_;

		virtual void draw3DLine(const core::array<core::vector3df>& listPoint, SColor color) _IRR_OVERRIDE_;

		//! Draws a 3d triangle.
		virtual void draw3DTriangle(const core::triangle3df& triangle,
			SColor color = SColor(255,255,255,255)) _IRR_OVERRIDE_;

		//! Draws a 3d axis aligned box.
		virtual void draw3DBox(const core::aabbox3d<f32>& box,
			SColor color = SColor(255,255,255,255)) _IRR_OVERRIDE_;

		virtual void setFog(SColor color=SColor(0,255,255,255),
				E_FOG_TYPE fogType=EFT_FOG_LINEAR,
				f32 start=50.0f, f32 end=100.0f, f32 density=0.01f,
				bool pixelFog=false, bool rangeFog=false) _IRR_OVERRIDE_;

		virtual void getFog(SColor& color, E_FOG_TYPE& fogType,
				f32& start, f32& end, f32& density,
				bool& pixelFog, bool& rangeFog) _IRR_OVERRIDE_;

		//! Check material is transparent
		virtual bool isMaterialTransparent(const SMaterial& material) _IRR_OVERRIDE_;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! get screen size
		virtual const core::dimension2d<u32>& getScreenSize() const _IRR_OVERRIDE_;

		//! get render target size
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const _IRR_OVERRIDE_;

		// get current frames per second value
		virtual s32 getFPS() const _IRR_OVERRIDE_;
		virtual s32 getTextureChange() const _IRR_OVERRIDE_;
		virtual s32 getDrawCall() const _IRR_OVERRIDE_;

		//! returns amount of primitives (mostly triangles) were drawn in the last frame.
		//! very useful method for statistics.
		virtual u32 getPrimitiveCountDrawn( u32 param = 0 ) const _IRR_OVERRIDE_;
		virtual u32 getTextureChangeCount(u32 param = 0) const _IRR_OVERRIDE_;
		
		void incTextureChange();

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights() _IRR_OVERRIDE_;

		//! adds a dynamic light, returning an index to the light
		//! \param light: the light data to use to create the light
		//! \return An index to the light, or -1 if an error occurs
		virtual s32 addDynamicLight(const SLight& light) _IRR_OVERRIDE_;

		//! Turns a dynamic light on or off
		//! \param lightIndex: the index returned by addDynamicLight
		//! \param turnOn: true to turn the light on, false to turn it off
		virtual void turnLightOn(s32 lightIndex, bool turnOn) _IRR_OVERRIDE_;

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const _IRR_OVERRIDE_;

		//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const _IRR_OVERRIDE_;

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color) _IRR_OVERRIDE_;

		//! Adds an external image loader to the engine.
		virtual void addExternalImageLoader(IImageLoader* loader) _IRR_OVERRIDE_;

		//! Adds an external image writer to the engine.
		virtual void addExternalImageWriter(IImageWriter* writer) _IRR_OVERRIDE_;

		//! Returns current amount of dynamic lights set
		//! \return Current amount of dynamic lights set
		virtual u32 getDynamicLightCount() const _IRR_OVERRIDE_;

		//! Returns light data which was previously set with IVideDriver::addDynamicLight().
		//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
		//! than IVideoDriver()::getDynamicLightCount.
		//! \return Light data.
		virtual const SLight& getDynamicLight(u32 idx) const _IRR_OVERRIDE_;

		//! Removes a texture from the texture cache and deletes it, freeing lot of
		//! memory.
		virtual void removeTexture(ITexture* texture) _IRR_OVERRIDE_;

		//! Removes all texture from the texture cache and deletes them, freeing lot of
		//! memory.
		virtual void removeAllTextures() _IRR_OVERRIDE_;

		//! Creates a render target texture.
		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;

		//! addRenderTargetCubeTexture
		virtual ITexture* addRenderTargetCubeTexture(const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format) _IRR_OVERRIDE_;

		//! addRenderTargetTextureArray
		virtual ITexture* addRenderTargetTextureArray(const core::dimension2d<u32>& size, u32 arraySize,
			const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN) _IRR_OVERRIDE_;

		//! addVideoRenderTarget
		virtual IVideoRenderTarget* addVideoRenderTarget(void *hwnd, u32 w, u32 h)_IRR_OVERRIDE_;

		//! removeVideoRenderTarget
		virtual void removeVideoRenderTarget(IVideoRenderTarget* vrt) _IRR_OVERRIDE_;

		//! setVideoRenderTarget
		virtual void setVideoRenderTarget(IVideoRenderTarget* vrt) _IRR_OVERRIDE_;

		//! Creates an 1bit alpha channel of the texture based of an color key.
		virtual void makeColorKeyTexture(video::ITexture* texture, video::SColor color, bool zeroTexels) const _IRR_OVERRIDE_;

		//! Creates an 1bit alpha channel of the texture based of an color key position.
		virtual void makeColorKeyTexture(video::ITexture* texture, core::position2d<s32> colorKeyPixelPos,
			bool zeroTexels) const _IRR_OVERRIDE_;

		//! Creates a normal map from a height map texture.
		//! \param amplitude: Constant value by which the height information is multiplied.
		virtual void makeNormalMapTexture(video::ITexture* texture, f32 amplitude=1.0f) const _IRR_OVERRIDE_;

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual u32 getMaximalPrimitiveCount() const _IRR_OVERRIDE_;

		//! Enables or disables a texture creation flag.
		virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled) _IRR_OVERRIDE_;

		//! Returns if a texture creation flag is enabled or disabled.
		virtual bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) const _IRR_OVERRIDE_;

		//! Creates a software image from a file.
		virtual IImage* createImageFromFile(const io::path& filename) _IRR_OVERRIDE_;

		//! Creates a software image from a file.
		virtual IImage* createImageFromFile(io::IReadFile* file) _IRR_OVERRIDE_;

		//! Creates a software image from a byte array.
		/** \param useForeignMemory: If true, the image will use the data pointer
		directly and own it from now on, which means it will also try to delete [] the
		data when the image will be destructed. If false, the memory will by copied. */
		virtual IImage* createImageFromData(ECOLOR_FORMAT format,
			const core::dimension2d<u32>& size, void *data,
			bool ownForeignMemory=true, bool deleteForeignMemory = true) _IRR_OVERRIDE_;

		//! Creates an empty software image.
		virtual IImage* createImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size) _IRR_OVERRIDE_;


		//! Creates a software image from another image.
		virtual IImage* createImage(ECOLOR_FORMAT format, IImage *imageToCopy) _IRR_OVERRIDE_;

		//! Creates a software image from part of another image.
		virtual IImage* createImage(IImage* imageToCopy,
				const core::position2d<s32>& pos,
				const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Creates a software image from part of a texture.
		virtual IImage* createImage(ITexture* texture,
				const core::position2d<s32>& pos,
				const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Draws a mesh buffer
		virtual void drawMeshBuffer(const scene::IMeshBuffer* mb) _IRR_OVERRIDE_;

		//! Draws the normals of a mesh buffer
		virtual void drawMeshBufferNormals(const scene::IMeshBuffer* mb, f32 length=10.f,
			SColor color=0xffffffff) _IRR_OVERRIDE_;

		virtual IVertexDescriptor* addVertexDescriptor(const core::stringc& pName) _IRR_OVERRIDE_;

		virtual IVertexDescriptor* getVertexDescriptor(u32 id) const;

		virtual IVertexDescriptor* getVertexDescriptor(const core::stringc& pName) const;

		virtual u32 getVertexDescriptorCount() const;

		virtual IHardwareBuffer* createHardwareBuffer(scene::IIndexBuffer* indexBuffer) _IRR_OVERRIDE_;

		virtual IHardwareBuffer* createHardwareBuffer(scene::IVertexBuffer* vertexBuffer) _IRR_OVERRIDE_;

		virtual bool isHardwareBufferRecommend(const scene::IMeshBuffer* mb);

		//! Create occlusion query.
		/** Use node for identification and mesh for occlusion test. */
		virtual void addOcclusionQuery(scene::ISceneNode* node,
				const scene::IMesh* mesh=0) _IRR_OVERRIDE_;

		//! Remove occlusion query.
		virtual void removeOcclusionQuery(scene::ISceneNode* node) _IRR_OVERRIDE_;

		//! Remove all occlusion queries.
		virtual void removeAllOcclusionQueries() _IRR_OVERRIDE_;

		//! Run occlusion query. Draws mesh stored in query.
		/** If the mesh shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		virtual void runOcclusionQuery(scene::ISceneNode* node, bool visible=false) _IRR_OVERRIDE_;

		//! Run all occlusion queries. Draws all meshes stored in queries.
		/** If the meshes shall not be rendered visible, use
		overrideMaterial to disable the color and depth buffer. */
		virtual void runAllOcclusionQueries(bool visible=false) _IRR_OVERRIDE_;

		//! Update occlusion query. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		virtual void updateOcclusionQuery(scene::ISceneNode* node, bool block=true) _IRR_OVERRIDE_;

		//! Update all occlusion queries. Retrieves results from GPU.
		/** If the query shall not block, set the flag to false.
		Update might not occur in this case, though */
		virtual void updateAllOcclusionQueries(bool block=true) _IRR_OVERRIDE_;

		//! Return query result.
		/** Return value is the number of visible pixels/fragments.
		The value is a safe approximation, i.e. can be larger than the
		actual value of pixels. */
		virtual u32 getOcclusionQueryResult(scene::ISceneNode* node) const _IRR_OVERRIDE_;

		//! Only used by the engine internally.
		/** Used to notify the driver that the window was resized. */
		virtual void OnResize(const core::dimension2d<u32>& size) _IRR_OVERRIDE_;

		//! Adds a new material renderer to the video device.
		virtual s32 addMaterialRenderer(IMaterialRenderer* renderer,
				const char* name = 0) _IRR_OVERRIDE_;

		//! Returns driver and operating system specific data about the IVideoDriver.
		virtual const SExposedVideoData& getExposedVideoData() _IRR_OVERRIDE_;

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;		

		//! Returns pointer to the IGPUProgrammingServices interface.
		virtual IGPUProgrammingServices* getGPUProgrammingServices() _IRR_OVERRIDE_;

		//! Adds a new material renderer to the VideoDriver, using pixel and/or
		//! vertex shaders to render geometry.
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram = 0,
			const c8* pixelShaderProgram = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData=0) _IRR_OVERRIDE_;

		//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
		//! programs from files.
		virtual s32 addShaderMaterialFromFiles(io::IReadFile* vertexShaderProgram = 0,
			io::IReadFile* pixelShaderProgram = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData=0) _IRR_OVERRIDE_;

		//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
		//! programs from files.
		virtual s32 addShaderMaterialFromFiles(const io::path& vertexShaderProgramFileName,
			const io::path& pixelShaderProgramFileName,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData=0) _IRR_OVERRIDE_;

		//! Returns pointer to material renderer or null
		virtual IMaterialRenderer* getMaterialRenderer(u32 idx) _IRR_OVERRIDE_;

		//! Returns amount of currently available material renderers.
		virtual u32 getMaterialRendererCount() const _IRR_OVERRIDE_;

		//! Returns name of the material renderer
		virtual const char* getMaterialRendererName(u32 idx) const _IRR_OVERRIDE_;

		//! Adds a new material renderer to the VideoDriver, based on a high level shading
		//! language. Currently only HLSL in D3D9 is supported.
		virtual s32 addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName = 0,
			E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
			const c8* pixelShaderProgram = 0,
			const c8* pixelShaderEntryPointName = 0,
			E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
			const c8* geometryShaderProgram = 0,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0, E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT) _IRR_OVERRIDE_;

		//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
		//! but tries to load the programs from files.
		virtual s32 addHighLevelShaderMaterialFromFiles(
			const io::path& vertexShaderProgramFile,
			const c8* vertexShaderEntryPointName = "main",
			E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
			const io::path& pixelShaderProgramFile = "",
			const c8* pixelShaderEntryPointName = "main",
			E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
			const io::path& geometryShaderProgramFileName="",
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0, E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT) _IRR_OVERRIDE_;

		//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
		//! but tries to load the programs from files.
		virtual s32 addHighLevelShaderMaterialFromFiles(
			io::IReadFile* vertexShaderProgram,
			const c8* vertexShaderEntryPointName = "main",
			E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
			io::IReadFile* pixelShaderProgram = 0,
			const c8* pixelShaderEntryPointName = "main",
			E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
			io::IReadFile* geometryShaderProgram= 0,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0, E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT) _IRR_OVERRIDE_;

		//! Returns a pointer to the mesh manipulator.
		virtual scene::IMeshManipulator* getMeshManipulator() _IRR_OVERRIDE_;

		//! Clears the ZBuffer.
		virtual void clearZBuffer() _IRR_OVERRIDE_;

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER) _IRR_OVERRIDE_;

		//! Writes the provided image to disk file
		virtual bool writeImageToFile(IImage* image, const io::path& filename, u32 param = 0) _IRR_OVERRIDE_;

		//! Writes the provided image to a file.
		virtual bool writeImageToFile(IImage* image, io::IWriteFile * file, u32 param = 0) _IRR_OVERRIDE_;

		//! Sets the name of a material renderer.
		virtual void setMaterialRendererName(s32 idx, const char* name) _IRR_OVERRIDE_;

		//! Creates material attributes list from a material, usable for serialization and more.
		virtual io::IAttributes* createAttributesFromMaterial(const video::SMaterial& material,
			io::SAttributeReadWriteOptions* options=0) _IRR_OVERRIDE_;

		//! Fills an SMaterial structure from attributes.
		virtual void fillMaterialStructureFromAttributes(video::SMaterial& outMaterial, io::IAttributes* attributes) _IRR_OVERRIDE_;

		//! looks if the image is already loaded
		virtual video::ITexture* findTexture(const io::path& filename) _IRR_OVERRIDE_;

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
		virtual core::stringc getVendorInfo() _IRR_OVERRIDE_ {return "Not available on this driver.";}
		virtual core::stringc getGPUName() _IRR_OVERRIDE_ {return "Not available on this driver.";}

		//! Set the minimum number of vertices for which a hw buffer will be created
		/** \param count Number of vertices to set as minimum. */
		virtual void setMinHardwareBufferVertexCount(u32 count) _IRR_OVERRIDE_;
		
		//! Only used by the engine internally.
		virtual void setAllowZWriteOnTransparent(bool flag) _IRR_OVERRIDE_
		{ AllowZWriteOnTransparent=flag; }

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const _IRR_OVERRIDE_;

		//! Color conversion convenience function
		/** Convert an image (as array of pixels) from source to destination
		array, thereby converting the color format. The pixel size is
		determined by the color formats.
		\param sP Pointer to source
		\param sF Color format of source
		\param sN Number of pixels to convert, both array must be large enough
		\param dP Pointer to destination
		\param dF Color format of destination
		*/
		virtual void convertColor(const void* sP, ECOLOR_FORMAT sF, s32 sN,
				void* dP, ECOLOR_FORMAT dF) const _IRR_OVERRIDE_;

		virtual bool checkDriverReset() _IRR_OVERRIDE_ {return false;}

		//! set default frame
		virtual void setDefaultFrameBuffer(int framebuffer) _IRR_OVERRIDE_
		{
			DefaultFrameBuffer = framebuffer;
		}

		//! get default frame
		virtual int getDefaultFrameBuffer()
		{
			return DefaultFrameBuffer;
		}

	protected:

		//! deletes all textures
		void deleteAllTextures();

		//! opens the file and loads it into the surface
		video::ITexture* loadTextureFromFile(io::IReadFile* file, const io::path& hashName = "");

		//! adds a surface, not loaded or created by the Irrlicht Engine
		void addTexture(video::ITexture* surface);

		//! Creates a texture from a loaded IImage.
		virtual ITexture* addTexture(const io::path& name, IImage* image, void* mipmapData=0) _IRR_OVERRIDE_;

		//! returns a device dependent texture from a software surface (IImage)
		//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData=0);

		//! checks triangle count and print warning if wrong
		bool checkPrimitiveCount(u32 prmcnt) const;

		// adds a material renderer and drops it afterwards. To be used for internal creation
		s32 addAndDropMaterialRenderer(IMaterialRenderer* m);

		//! deletes all material renderers
		void deleteMaterialRenders();

		//! deletes all vertex descriptors
		void deleteVertexDescriptors();

		// prints renderer version
		void printVersion();

		//! add video render target
		void addVRT(IVideoRenderTarget *vrt);

		//! remove video render target
		void removeVRT(IVideoRenderTarget *vrt);

		//! remove all video render target
		void deleteAllVRTs();

		//! normal map lookup 32 bit version
		inline f32 nml32(int x, int y, int pitch, int height, s32 *p) const
		{
			if (x < 0) x = pitch-1; if (x >= pitch) x = 0;
			if (y < 0) y = height-1; if (y >= height) y = 0;
			return (f32)(((p[(y * pitch) + x])>>16) & 0xff);
		}

		//! normal map lookup 16 bit version
		inline f32 nml16(int x, int y, int pitch, int height, s16 *p) const
		{
			if (x < 0) x = pitch-1; if (x >= pitch) x = 0;
			if (y < 0) y = height-1; if (y >= height) y = 0;

			return (f32) getAverage ( p[(y * pitch) + x] );
		}
	
		struct SSurface
		{
			video::ITexture* Surface;

			bool operator < (const SSurface& other) const
			{
				return Surface->getName() < other.Surface->getName();
			}
		};

		struct SMaterialRenderer
		{
			core::stringc Name;
			IMaterialRenderer* Renderer;
		};

		struct SDummyTexture : public ITexture
		{
			SDummyTexture(const io::path& name) : ITexture(name) {};

			virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0) _IRR_OVERRIDE_ { return 0; }
			virtual void unlock()_IRR_OVERRIDE_ {}
			virtual void regenerateMipMapLevels(void* mipmapData=0) _IRR_OVERRIDE_ {}
		};
		
		core::array<SSurface> Textures;
		core::array<IVideoRenderTarget*> VRTs;

		struct SOccQuery
		{
			SOccQuery(scene::ISceneNode* node, const scene::IMesh* mesh=0) : Node(node), Mesh(mesh), PID(0), Result(0xffffffff), Run(0xffffffff)
			{
				if (Node)
					Node->grab();
				if (Mesh)
					Mesh->grab();
			}

			SOccQuery(const SOccQuery& other) : Node(other.Node), Mesh(other.Mesh), PID(other.PID), Result(other.Result), Run(other.Run)
			{
				if (Node)
					Node->grab();
				if (Mesh)
					Mesh->grab();
			}

			~SOccQuery()
			{
				if (Node)
					Node->drop();
				if (Mesh)
					Mesh->drop();
			}

			SOccQuery& operator=(const SOccQuery& other)
			{
				Node=other.Node;
				Mesh=other.Mesh;
				PID=other.PID;
				Result=other.Result;
				Run=other.Run;
				if (Node)
					Node->grab();
				if (Mesh)
					Mesh->grab();
				return *this;
			}

			bool operator==(const SOccQuery& other) const
			{
				return other.Node==Node;
			}

			scene::ISceneNode* Node;
			const scene::IMesh* Mesh;
			union
			{
				void* PID;
				unsigned int UID;
			};
			u32 Result;
			u32 Run;
		};

		core::matrix4 Matrices[ETS_COUNT];

		core::array<SOccQuery> OcclusionQueries;

		core::array<video::IImageLoader*> SurfaceLoader;
		core::array<video::IImageWriter*> SurfaceWriter;
		core::array<SLight> Lights;
		core::array<SMaterialRenderer> MaterialRenderers;

		io::IFileSystem* FileSystem;

		//! mesh manipulator
		scene::IMeshManipulator* MeshManipulator;

		core::rect<s32> ViewPort;
		core::dimension2d<u32> ScreenSize;
		core::matrix4 TransformationMatrix;

		CFPSCounter FPSCounter;

		u32 DefaultFrameBuffer;

		u32 PrimitivesDrawn;
		u32 DrawCall;
		u32 TextureChangedCount;
		u32 MinVertexCountForVBO;

		u32 TextureCreationFlags;

		f32 FogStart;
		f32 FogEnd;
		f32 FogDensity;
		SColor FogColor;
		SExposedVideoData ExposedData;

		io::IAttributes* DriverAttributes;

		E_FOG_TYPE FogType;
		bool PixelFog;
		bool RangeFog;
		bool AllowZWriteOnTransparent;

		irr::core::array<IVertexDescriptor*> VertexDescriptor;

		bool FeatureEnabled[video::EVDF_COUNT];

		//! Skylicht add for cache
		bool RendererTransformChanged;
	};

} // end namespace video
} // end namespace irr


#endif
