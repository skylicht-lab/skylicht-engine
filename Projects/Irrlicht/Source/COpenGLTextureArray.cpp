#include "pch.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTextureArray.h"
#include "COpenGLTexture.h"
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

		COpenGLTextureArray::COpenGLTextureArray(COpenGLDriver* driver, const io::path& name, IImage** images, int count)
			:ITexture(name), ColorFormat(ECF_A8R8G8B8), Driver(driver),
			TextureName(0), InternalFormat(GL_RGB8), PixelFormat(GL_RGB),
			PixelType(GL_UNSIGNED_BYTE)
		{
			TextureType = ETT_TEXTURE_ARRAY;
			DriverType = EDT_OPENGL;
			OriginalSize = images[0]->getDimension();
			Size = OriginalSize;
			TextureName = 0;
			MipmapCount = 1;
			DepthTexture = NULL;

			// calc mipmap count
			int width = Size.Width;
			int height = Size.Height;
			int m = width < height ? width : height;

			while (m > 1)
			{
				MipmapCount++;
				m = m / 2;
			}

			// format
			ColorFormat = ECF_A8R8G8B8;
			IImage *baseImage = images[0];
			bool isCompressed = IImage::isCompressedFormat(baseImage->getColorFormat());
			if (isCompressed == true)
			{
				ColorFormat = baseImage->getColorFormat();
				if (baseImage->hasMipMaps() == false)
					MipmapCount = 1;
			}

			// create image
			for (int i = 0; i < count; ++i)
			{
				if (isCompressed == true)
				{
					Image.push_back(images[i]);
				}
				else
				{
					Image.push_back(Driver->createImage(ColorFormat, OriginalSize));

					const core::dimension2d<u32>& s = images[i]->getDimension();
					if (s != Size)
						images[i]->copyToScaling(Image[i]);
					else
						images[i]->copyTo(Image[i]);
				}
			}

			glGenTextures(1, &TextureName);

			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);
			
			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);

			// init texture array			
			Driver->extGlTexStorage3D(GL_TEXTURE_2D_ARRAY,
				MipmapCount,
				InternalFormat,
				Size.Width,
				Size.Height,
				Image.size());			

			if (Driver->testGLError())
				os::Printer::log("Could not create texture array", ELL_ERROR);

			// upload texture
			for (u32 i = 0, n = Image.size(); i < n; ++i)
				uploadTexture(true, i, false);
			
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// create mipmap
			if (isCompressed == false)
			{
				regenerateMipMapLevels();
			}

			Driver->setActiveTexture(0, 0);
			Driver->getBridgeCalls()->setTexture(0);

			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);

			// drop image
			if (isCompressed == false)
			{
				for (u32 i = 0; i < Image.size(); ++i)
					Image[i]->drop();
			}
			Image.clear();
		}

		// FBO array texture
		COpenGLTextureArray::COpenGLTextureArray(COpenGLDriver* driver, const core::dimension2d<u32>& size, u32 arraySize, const io::path& name, const ECOLOR_FORMAT format)
			:ITexture(name), ColorFormat(format), Driver(driver),
			TextureName(0), InternalFormat(GL_RGB8), PixelFormat(GL_RGB),
			PixelType(GL_UNSIGNED_BYTE)
		{
			TextureType = ETT_TEXTURE_ARRAY;
			DriverType = EDT_OPENGL;
			OriginalSize = size;
			Size = OriginalSize;
			MipmapCount = 1;
			DepthTexture = NULL;
			IsRenderTarget = true;

			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);

#ifdef GL_EXT_framebuffer_object
			// generate frame buffer
			Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
			bindRTT(0);

			// Color Buffer
			glGenTextures(1, &TextureName);
			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName);

			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			Driver->extGlTexImage3D(GL_TEXTURE_2D_ARRAY, 0,
				InternalFormat,
				OriginalSize.Width, 
				OriginalSize.Height,
				arraySize,
				0, 
				PixelFormat, 
				PixelType,
				NULL);			

#ifdef _DEBUG
			driver->testGLError();
#endif

			// attach color texture to frame buffer
			for (u32 i = 0; i < arraySize; i++)
			{
				driver->extGlFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, TextureName, 0, i);
			}

#ifdef _DEBUG
			checkFBOStatus(Driver);
#endif

#endif
			unbindRTT();
		}

		//! destructor
		COpenGLTextureArray::~COpenGLTextureArray()
		{
			if (DepthTexture)
			{
				bool remove = DepthTexture->isRenderBuffer();

				if (DepthTexture->drop() && remove)
					Driver->removeDepthTexture(DepthTexture);
			}

			if (TextureName)
				glDeleteTextures(1, &TextureName);
			
			if (ColorFrameBuffer)
				Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);

			Driver->getBridgeCalls()->resetTexture(this);
		}

		//! Bind RenderTargetTexture
		void COpenGLTextureArray::bindRTT(int id)
		{
#ifdef GL_EXT_framebuffer_object
			if (ColorFrameBuffer != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + id);
#endif
		}

		//! Unbind RenderTargetTexture
		void COpenGLTextureArray::unbindRTT()
		{
#ifdef GL_EXT_framebuffer_object
			if (ColorFrameBuffer != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
		}
		
		bool COpenGLTextureArray::isFrameBufferObject()
		{
			return IsRenderTarget;
		}

		//! Set depth texture.
		bool COpenGLTextureArray::setDepthTexture(ITexture* depthTexture)
		{
			if (DepthTexture == depthTexture || ColorFrameBuffer == 0)
				return false;

#ifdef GL_EXT_framebuffer_object
			Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);

			if (DepthTexture)
			{
				if (DepthTexture->isRenderBuffer())
				{
					Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
				}
				else
				{
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

					if (DepthTexture->getColorFormat() == ECF_D24S8)
						Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
				}

				if (DepthTexture->drop())
					Driver->removeDepthTexture(DepthTexture);
			}

			COpenGLTexture* tex = static_cast<COpenGLTexture*>(depthTexture);

			DepthTexture = (tex && tex->isDepthTexture()) ? tex : 0;

			if (DepthTexture)
			{
				DepthTexture->grab();

				if (DepthTexture->isRenderBuffer())
				{
					COpenGLRenderBuffer* renderBuffer = static_cast<COpenGLRenderBuffer*>(DepthTexture);

					Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderBuffer->getBufferID());
				}
				else
				{
					COpenGLFBOTexture* fboDepthTexture = static_cast<COpenGLFBOTexture*>(DepthTexture);

					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, fboDepthTexture->getOpenGLTextureName(), 0);

					if (DepthTexture->getColorFormat() == ECF_D24S8)
						Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, fboDepthTexture->getOpenGLTextureName(), 0);
				}
			}

			Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif

			if (!checkFBOStatus(Driver))
			{
				os::Printer::log("FBO incomplete");
				return false;
			}

			return true;
		}

		void COpenGLTextureArray::uploadTexture(bool newTexture, u32 imageNumber, bool regMipmap)
		{
			// check which image needs to be uploaded
			IImage* image = Image[imageNumber];
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture array to upload", ELL_ERROR);
				return;
			}
						
			int w = Size.Width;
			int h = Size.Height;
			int d = 1;

			if (image->isCompressed() == true)
			{
				u8* data = (u8*)image->lock();
				s32 compressedDataSize = 0;

				for (int i = 0; i < MipmapCount; i++)
				{ 
					compressedDataSize = IImage::getCompressedImageSize(ColorFormat, w, h);

					Driver->extGlCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY,
						i,
						0, 0, imageNumber,
						w, h, d,
						PixelType,
						compressedDataSize,
						data);

					if (w > 1)
						w >>= 1;
					if (h > 1)
						h >>= 1;

					data = static_cast<u8*>(data) + compressedDataSize;
				}

				image->unlock();
				
				if (MipmapCount > 1)
					HasMipMaps = true;
			}
			else
			{
				// now get image data and upload to GPU
				void* source = image->lock();

				Driver->extGlTexSubImage3D(GL_TEXTURE_2D_ARRAY,
					0,						//Mipmap number
					0, 0, imageNumber,		//xoffset, yoffset, zoffset
					w, h, d,				//width, height, depth
					PixelFormat,			//format
					PixelType,				//type
					source);				//pointer to data);

				image->unlock();
			}

			if (Driver->testGLError())
				os::Printer::log("Could not glTexSubImage3D", ELL_ERROR);			
		}		

		//! lock function
		void* COpenGLTextureArray::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
		{
			return NULL;
		}

		void* COpenGLTextureArray::lock(bool readOnly, u32 mipmapLevel, u32 arraySlice)
		{
			return NULL;
		}

		//! unlock function
		void COpenGLTextureArray::unlock()
		{

		}

		//! Returns original size of the texture.
		const core::dimension2d<u32>& COpenGLTextureArray::getOriginalSize() const
		{
			return OriginalSize;
		}

		//! Returns (=size) of the texture.
		const core::dimension2d<u32>& COpenGLTextureArray::getSize() const
		{
			return Size;
		}

		//! returns color format of texture
		ECOLOR_FORMAT COpenGLTextureArray::getColorFormat() const
		{
			return ColorFormat;
		}

		u32 COpenGLTextureArray::getPitch() const
		{
			return 0;
		}

		//! Regenerates the mip map levels of the texture. Useful after locking and
		//! modifying the texture
		void COpenGLTextureArray::regenerateMipMapLevels(void* mipmapData)
		{
			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			Driver->extGlGenerateMipmap(GL_TEXTURE_2D_ARRAY);
			HasMipMaps = true;
		}		

		GLint COpenGLTextureArray::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
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
	}
}

#endif