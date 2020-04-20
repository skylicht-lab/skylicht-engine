#ifndef _OPEN_GLES3_TEXTURE_ARRAY_H_
#define _OPEN_GLES3_TEXTURE_ARRAY_H_

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"
#include "irrArray.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OGLES3_

#if defined(_IRR_IOS_PLATFORM_)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif defined(_IRR_OSX_PLATFORM_)
#include "util/gles_loader_autogen.h"
#elif defined(_IRR_ANDROID_PLATFORM_)
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#else
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#include <EGL/egl.h>
#endif


#endif


namespace irr
{
	namespace video
	{

		class COGLES3Driver;
		class COGLES3Texture;

		class COGLES3TextureArray : public ITexture
		{
			ECOLOR_FORMAT ColorFormat;
			s32 Pitch;
			s32 MipmapCount;

			core::array<IImage*> Image;

			COGLES3Driver* Driver;
		public:
			COGLES3TextureArray(COGLES3Driver* driver, const io::path& name, IImage** images, int count);

			COGLES3TextureArray(COGLES3Driver* driver, const core::dimension2d<u32>& size, u32 arraySize, const io::path& name, const ECOLOR_FORMAT format);

			void uploadTexture(bool newTexture = false, u32 imageNumber = 0, bool regMipmap = false);

			//! destructor
			virtual ~COGLES3TextureArray();

			//! lock function
			virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0);
			virtual void* lock(bool readOnly, u32 mipmapLevel = 0, u32 arraySlice = 0);

			//! Bind RenderTargetTexture
			virtual void bindRTT(int id);

			//! Unbind RenderTargetTexture
			virtual void unbindRTT();

			//! unlock function
			virtual void unlock();

			//! Returns original size of the texture.
			virtual const core::dimension2d<u32>& getOriginalSize() const;

			//! Returns (=size) of the texture.
			virtual const core::dimension2d<u32>& getSize() const;

			//! returns color format of texture
			virtual ECOLOR_FORMAT getColorFormat() const;

			virtual u32 getPitch() const;

			//! Regenerates the mip map levels of the texture. Useful after locking and
			//! modifying the texture
			virtual void regenerateMipMapLevels(void* mipmapData = 0);

			GLint getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
				GLint& filtering,
				GLenum& colorformat,
				GLenum& type);

			const GLuint getOpenGLTextureName() const
			{
				return TextureName;
			}

			//! Set depth texture.
			bool setDepthTexture(ITexture* depthTexture);
			
			bool isFrameBufferObject();
		private:

			GLuint ColorFrameBuffer;

			GLuint TextureName;

			GLint FilteringType;
			GLint InternalFormat;
			GLenum PixelFormat;
			GLenum PixelType;

			COGLES3Texture* DepthTexture;
		};
	}
}

#endif
#endif
