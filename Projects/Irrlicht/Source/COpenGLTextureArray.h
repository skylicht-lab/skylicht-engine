#ifndef _OPENGL_TEXTURE_ARRAY_H_
#define _OPENGL_TEXTURE_ARRAY_H_

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"
#include "irrArray.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
#define GL_GLEXT_LEGACY 1
#else
#define GL_GLEXT_PROTOTYPES 1
#endif

#ifdef _IRR_WINDOWS_API_
// include windows headers for HWND
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#elif defined(_IRR_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
#define NO_SDL_GLEXT
#include <SDL/SDL_video.h>
#include <SDL/SDL_opengl.h>
#else
#if defined(_IRR_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif


namespace irr
{
	namespace video
	{

		class COpenGLDriver;
		class COpenGLTexture;

		class COpenGLTextureArray : public ITexture
		{
			ECOLOR_FORMAT ColorFormat;
			s32 Pitch;
			s32 MipmapCount;

			core::array<IImage*> Image;

			COpenGLDriver* Driver;
		public:
			COpenGLTextureArray(COpenGLDriver* driver, const io::path& name, IImage** images, int count);

			COpenGLTextureArray(COpenGLDriver* driver, const core::dimension2d<u32>& size, u32 arraySize, const io::path& name, const ECOLOR_FORMAT format);

			void uploadTexture(bool newTexture = false, u32 imageNumber = 0, bool regMipmap = false);

			//! destructor
			virtual ~COpenGLTextureArray();

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

			GLint InternalFormat;
			GLenum PixelFormat;
			GLenum PixelType;

			COpenGLTexture* DepthTexture;
		};
	}
}

#endif
#endif