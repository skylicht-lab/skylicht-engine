// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTextureCube.h"
#include "COpenGLDriver.h"
#include "irrOS.h"
#include "CImage.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
	namespace video
	{

		extern bool checkFBOStatus(COpenGLDriver* Driver);

		//! rendertarget constructor
		COpenGLTextureCube::COpenGLTextureCube(COpenGLDriver* driver, const core::dimension2d<u32>& size, const io::path& name,
			const ECOLOR_FORMAT format)
			: ITexture(name), Driver(driver)
		{
			TextureType = ETT_TEXTURE_CUBE;
			DriverType = EDT_OPENGL;
			Size = size;
			OriginalSize = size;
			IsRenderTarget = true;
			ColorFormat = format;

			if (ECF_UNKNOWN == format)
				ColorFormat = ECF_A8R8G8B8;

			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);

#ifdef GL_EXT_framebuffer_object
			// generate frame buffer
			Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
			bindRTT(0);

			glEnable(GL_TEXTURE_CUBE_MAP);

			// Color Buffer
			glGenTextures(1, &TextureName);
			glBindTexture(GL_TEXTURE_CUBE_MAP, TextureName);

			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			for (int face = 0; face < 6; face++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					InternalFormat,
					OriginalSize.Width, OriginalSize.Height,
					0, PixelFormat, PixelType, NULL);
			}

			// Depth Buffer
			glGenTextures(1, &DepthTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, DepthTexture);

			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			for (int face = 0; face < 6; face++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					Driver->getZBufferBits(),
					OriginalSize.Width, OriginalSize.Height,
					0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}

#ifdef _DEBUG
			driver->testGLError();
#endif

			// attach color texture to frame buffer
			for (int i = 0; i < 6; i++)
			{
				Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
					GL_COLOR_ATTACHMENT0_EXT + i,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					TextureName,
					0);

				// attach depth texture to frame buffer
				Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
					GL_DEPTH_ATTACHMENT_EXT,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					DepthTexture,
					0);
			}

#ifdef _DEBUG
			checkFBOStatus(Driver);
#endif


#endif
			unbindRTT();
		}

		//! destructor
		COpenGLTextureCube::~COpenGLTextureCube()
		{
			if (TextureName)
				glDeleteTextures(1, &TextureName);

			if (DepthTexture)
				glDeleteTextures(1, &DepthTexture);

			if (ColorFrameBuffer)
				Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
		}

		//! Get opengl values for the GPU texture storage
		GLint COpenGLTextureCube::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
			GLint& filtering,
			GLenum& colorformat,
			GLenum& type)
		{
			// default
			filtering = GL_LINEAR;
			colorformat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			GLenum internalformat = GL_RGBA;

			switch (format)
			{
			case ECF_A1R5G5B5:
				colorformat = GL_BGRA_EXT;
				type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
				internalformat = GL_RGBA;
				break;
			case ECF_R5G6B5:
				colorformat = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				internalformat = GL_RGB;
				break;
			case ECF_R8G8B8:
				colorformat = GL_BGR;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_RGB;
				break;
			case ECF_A8R8G8B8:
				colorformat = GL_BGRA_EXT;
				if (Driver->Version > 101)
					type = GL_UNSIGNED_INT_8_8_8_8_REV;
				internalformat = GL_RGBA;
				break;
			case ECF_DXT1:
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			case ECF_DXT2:
			case ECF_DXT3:
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case ECF_DXT4:
			case ECF_DXT5:
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			case ECF_D16:
				colorformat = GL_DEPTH_COMPONENT;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_DEPTH_COMPONENT16;
				break;
			case ECF_D32:
				colorformat = GL_DEPTH_COMPONENT;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_DEPTH_COMPONENT32;
				break;
			case ECF_D24S8:
#ifdef GL_EXT_packed_depth_stencil
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_EXT_packed_depth_stencil))
				{
					colorformat = GL_DEPTH_STENCIL_EXT;
					type = GL_UNSIGNED_INT_24_8_EXT;
					internalformat = GL_DEPTH_STENCIL_EXT;
				}
				else
#endif
					os::Printer::log("ECF_D24S8 color format is not supported", ELL_ERROR);
				break;
			case ECF_R8:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					colorformat = GL_RED;
					type = GL_UNSIGNED_BYTE;
					internalformat = GL_R8;
				}
				else
					os::Printer::log("ECF_R8 color format is not supported", ELL_ERROR);
				break;
			case ECF_R8G8:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					colorformat = GL_RG;
					type = GL_UNSIGNED_BYTE;
					internalformat = GL_RG8;
				}
				else
					os::Printer::log("ECF_R8G8 color format is not supported", ELL_ERROR);
				break;
			case ECF_R16:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					colorformat = GL_RED;
					type = GL_UNSIGNED_SHORT;
					internalformat = GL_R16;
				}
				else
					os::Printer::log("ECF_R16 color format is not supported", ELL_ERROR);
				break;
			case ECF_R16G16:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					colorformat = GL_RG;
					type = GL_UNSIGNED_SHORT;
					internalformat = GL_RG16;
				}
				else
					os::Printer::log("ECF_R16G16 color format is not supported", ELL_ERROR);
				break;
			case ECF_R16F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RED;
					internalformat = GL_R16F;
#ifdef GL_ARB_half_float_pixel
					if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_half_float_pixel))
						type = GL_HALF_FLOAT_ARB;
					else
#endif
						type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_R16F color format is not supported", ELL_ERROR);
				break;
			case ECF_G16R16F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RG;
					internalformat = GL_RG16F;
#ifdef GL_ARB_half_float_pixel
					if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_half_float_pixel))
						type = GL_HALF_FLOAT_ARB;
					else
#endif
						type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_G16R16F color format is not supported", ELL_ERROR);
				break;
			case ECF_A16B16G16R16F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_float))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RGBA;
					internalformat = GL_RGBA16F_ARB;
#ifdef GL_ARB_half_float_pixel
					if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_half_float_pixel))
						type = GL_HALF_FLOAT_ARB;
					else
#endif
						type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_A16B16G16R16F color format is not supported", ELL_ERROR);
				break;
			case ECF_R32F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RED;
					internalformat = GL_R32F;
					type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_R32F color format is not supported", ELL_ERROR);
				break;
			case ECF_G32R32F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_rg))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RG;
					internalformat = GL_RG32F;
					type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_G32R32F color format is not supported", ELL_ERROR);
				break;
			case ECF_A32B32G32R32F:
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_texture_float))
				{
					filtering = GL_NEAREST;
					colorformat = GL_RGBA;
					internalformat = GL_RGBA32F_ARB;
					type = GL_FLOAT;
				}
				else
					os::Printer::log("ECF_A32B32G32R32F color format is not supported", ELL_ERROR);
				break;
			default:
				os::Printer::log("Unsupported texture format", ELL_ERROR);
				break;
			}
#if defined(GL_ARB_framebuffer_sRGB) || defined(GL_EXT_framebuffer_sRGB)
			if (Driver->Params.HandleSRGB)
			{
				if (internalformat == GL_RGBA)
					internalformat = GL_SRGB_ALPHA_EXT;
				else if (internalformat == GL_RGB)
					internalformat = GL_SRGB_EXT;
			}
#endif
			return internalformat;
		}

		COpenGLTextureCube::COpenGLTextureCube(COpenGLDriver* driver, const io::path& name,
			IImage* posXImage, IImage* negXImage,
			IImage* posYImage, IImage* negYImage,
			IImage* posZImage, IImage* negZImage)
			: ITexture(name), Driver(driver),
			TextureName(0), InternalFormat(GL_RGB8), PixelFormat(GL_RGB),
			PixelType(GL_UNSIGNED_BYTE)
		{
			TextureType = ETT_TEXTURE_CUBE;
			DriverType = EDT_OPENGL;
			OriginalSize = posXImage->getDimension();
			Size = OriginalSize;
			TextureName = 0;
			DepthTexture = 0;

			ColorFormat = ECF_R8G8B8;

			PixelFormat = GL_RGB;
			PixelType = GL_UNSIGNED_BYTE;
			InternalFormat = GL_RGB8;

			glEnable(GL_TEXTURE_CUBE_MAP);

			// create image
			for (u32 i = 0; i < 6; ++i)
				Image.push_back(Driver->createImage(ColorFormat, OriginalSize));

			posXImage->copyTo(Image[0]);
			negXImage->copyTo(Image[1]);
			posYImage->copyTo(Image[2]);
			negYImage->copyTo(Image[3]);
			posZImage->copyTo(Image[4]);
			negZImage->copyTo(Image[5]);

			glGenTextures(1, &TextureName);

			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);

			// upload texture
			for (u32 i = 0; i < 5; ++i)
				uploadTexture(true, i, false);

			uploadTexture(true, 5, true);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			Driver->setActiveTexture(0, 0);
			Driver->getBridgeCalls()->setTexture(0);

			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);

			// drop image
			for (u32 i = 0; i < Image.size(); ++i)
				Image[i]->drop();
			Image.clear();
		}

		void COpenGLTextureCube::uploadTexture(bool newTexture, u32 imageNumber, bool regMipmap)
		{
			// check which image needs to be uploaded
			IImage* image = Image[imageNumber];
			if (!image)
			{
				os::Printer::log("No image for OpenGL ES texture to upload", ELL_ERROR);
				return;
			}

			// get texture type
			GLenum tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

			switch (imageNumber)
			{
			case 0:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				break;
			case 1:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
				break;
			case 2:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
				break;
			case 3:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
				break;
			case 4:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
				break;
			case 5:
				tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
				break;
			default:
				break;
			}

			// now get image data and upload to GPU
			void* source = image->lock();

			if (newTexture)
			{
				glTexImage2D(tmpTextureType, 0, InternalFormat, image->getDimension().Width,
					image->getDimension().Height, 0, PixelFormat, PixelType, source);
			}
			else
			{
				glTexSubImage2D(tmpTextureType, 0, 0, 0, image->getDimension().Width,
					image->getDimension().Height, PixelFormat, PixelType, source);
			}

			image->unlock();

			if (Driver->testGLError())
				os::Printer::log("Could not glTexImage2D", ELL_ERROR);
		}

		//! lock function
		void* COpenGLTextureCube::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
		{
			return NULL;
		}

		void* COpenGLTextureCube::lock(bool readOnly, u32 mipmapLevel, u32 arraySlice)
		{
			return NULL;
		}

		//! unlock function
		void COpenGLTextureCube::unlock()
		{
		}

		//! Returns original size of the texture.
		const core::dimension2d<u32>& COpenGLTextureCube::getOriginalSize() const
		{
			return OriginalSize;
		}

		//! Returns (=size) of the texture.
		const core::dimension2d<u32>& COpenGLTextureCube::getSize() const
		{
			return OriginalSize;
		}

		//! returns color format of texture
		ECOLOR_FORMAT COpenGLTextureCube::getColorFormat() const
		{
			return ColorFormat;
		}

		u32 COpenGLTextureCube::getPitch() const
		{
			return Pitch;
		}

		//! Regenerates the mip map levels of the texture. Useful after locking and
		//! modifying the texture
		void COpenGLTextureCube::regenerateMipMapLevels(void* mipmapData)
		{
			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			Driver->extGlGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			HasMipMaps = true;

#ifdef _DEBUG
			Driver->testGLError();
#endif
		}

		//! returns if it is a render target
		bool COpenGLTextureCube::isRenderTarget() const
		{
			return true;
		}

		//! Bind RenderTargetTexture
		void COpenGLTextureCube::bindRTT(int face)
		{
#ifdef GL_EXT_framebuffer_object
			if (ColorFrameBuffer != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + face);
#endif
		}

		//! Unbind RenderTargetTexture
		void COpenGLTextureCube::unbindRTT()
		{
#ifdef GL_EXT_framebuffer_object
			if (ColorFrameBuffer != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
		}

	}
}

#endif