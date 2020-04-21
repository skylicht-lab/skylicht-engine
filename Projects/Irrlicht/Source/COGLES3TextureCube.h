// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Pham Hong Duc
// Add to skylicht engine

#ifndef __C_OPEN_GLES3_TEXTURECUBE_H_INCLUDED__
#define __C_OPEN_GLES3_TEXTURECUBE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"

#include "IrrCompileConfig.h"

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"
#include "irrArray.h"

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
	#include <GLES3/gl3.h>
#endif


namespace irr
{
	namespace video
	{

		class COGLES3Driver;

		class COGLES3TextureCube : public ITexture
		{
		private:

			core::array<IImage*> Image;
		public:

			//! rendertarget constructor
			COGLES3TextureCube(COGLES3Driver* driver, const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

			//! static cube map
			COGLES3TextureCube(COGLES3Driver* driver, const io::path& name,
				IImage* posXImage, IImage* negXImage,
				IImage* posYImage, IImage* negYImage,
				IImage* posZImage, IImage* negZImage);

			void uploadTexture(bool newTexture = false, u32 imageNumber = 0, bool regMipmap = false);

			//! destructor
			virtual ~COGLES3TextureCube();

			//! lock function
			virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0);
			virtual void* lock(bool readOnly, u32 mipmapLevel = 0, u32 arraySlice = 0);

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

			//! returns if it is a render target
			virtual bool isRenderTarget() const;

		public:

			//! Bind RenderTargetTexture
			virtual void bindRTT(int face);

			//! Unbind RenderTargetTexture
			virtual void unbindRTT();

			const GLuint getOpenGLTextureName() const
			{
				return TextureName;
			}

			GLint getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
				GLint& filtering,
				GLenum& colorformat,
				GLenum& type);

		private:
			COGLES3Driver * Driver;

			GLuint ColorFrameBuffer;

			GLuint TextureName;
			GLuint DepthTexture;

			GLint InternalFormat;
			GLenum PixelFormat;
			GLenum PixelType;
		};

	}
}


#endif

#endif
