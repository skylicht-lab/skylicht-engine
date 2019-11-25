#include "pch.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES3_

#include "irrTypes.h"
#include "COGLES3TextureArray.h"
#include "COGLES3Texture.h"
#include "COGLES3Driver.h"
#include "irrOS.h"
#include "CImage.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
	namespace video
	{
		extern bool checkFBOStatus(COGLES3Driver* Driver);

		COGLES3TextureArray::COGLES3TextureArray(COGLES3Driver* driver, const io::path& name, IImage** images, int count)
			:ITexture(name), ColorFormat(ECF_A8R8G8B8), Driver(driver),
			TextureName(0), InternalFormat(GL_RGB8), PixelFormat(GL_RGB), FilteringType(0),
			PixelType(GL_UNSIGNED_BYTE)
		{
			TextureType = ETT_TEXTURE_ARRAY;
			DriverType = EDT_OPENGLES;
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
			for (u32 i = 0; i < count; ++i)
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
			
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, FilteringType, PixelFormat, PixelType);		

			// init texture array
			glTexStorage3D(GL_TEXTURE_2D_ARRAY,
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
		COGLES3TextureArray::COGLES3TextureArray(COGLES3Driver* driver, const core::dimension2d<u32>& size, u32 arraySize, const io::path& name, const ECOLOR_FORMAT format)
			:ITexture(name), ColorFormat(format), Driver(driver),
			TextureName(0), InternalFormat(GL_RGB8), PixelFormat(GL_RGB),
			PixelType(GL_UNSIGNED_BYTE)
		{
			os::Printer::log("Init COGLES3TextureArray - Render Target");

			TextureType = ETT_TEXTURE_ARRAY;
			DriverType = EDT_OPENGLES;
			OriginalSize = size;
			Size = OriginalSize;
			MipmapCount = 1;
			DepthTexture = NULL;
			IsRenderTarget = true;

			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);
			
			// generate frame buffer
			glGenFramebuffers(1, &ColorFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);

			// Color Buffer			
			glGenTextures(1, &TextureName);
			glBindTexture(GL_TEXTURE_2D_ARRAY, TextureName);

			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0,
				InternalFormat,
				OriginalSize.Width, 
				OriginalSize.Height,
				arraySize,
				0, 
				PixelFormat, 
				PixelType,
				NULL);

			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

#ifdef _DEBUG
			driver->testGLError();
#endif			

			/*
			// attach color texture to frame buffer
			for (int i = 0; i < arraySize; i++)
			{				
				glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, TextureName, 0, i);
			}
			*/
			
			unbindRTT();
		}

		//! destructor
		COGLES3TextureArray::~COGLES3TextureArray()
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
				glDeleteFramebuffers(1, &ColorFrameBuffer);

			Driver->getBridgeCalls()->resetTexture(this);
		}

		//! Bind RenderTargetTexture
		void COGLES3TextureArray::bindRTT(int id)
		{
			if (ColorFrameBuffer != 0)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);
				glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName, 0, id);
			}

			GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, buffers);
		}

		//! Unbind RenderTargetTexture
		void COGLES3TextureArray::unbindRTT()
		{
			if (ColorFrameBuffer != 0)
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		
		bool COGLES3TextureArray::isFrameBufferObject()
		{
			return IsRenderTarget;
		}

		//! Set depth texture.
		bool COGLES3TextureArray::setDepthTexture(ITexture* depthTexture)
		{
			if (DepthTexture == depthTexture || ColorFrameBuffer == 0)
				return false;

			glBindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);

			if (DepthTexture)
			{
				if (DepthTexture->isRenderBuffer())
				{
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
				}
				else
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

					if (DepthTexture->getColorFormat() == ECF_D24S8)
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				}

				if (DepthTexture->drop())
					Driver->removeDepthTexture(DepthTexture);
			}

			COGLES3Texture* tex = static_cast<COGLES3Texture*>(depthTexture);

			DepthTexture = (tex && tex->isDepthTexture()) ? tex : 0;

			if (DepthTexture)
			{
				DepthTexture->grab();

				if (DepthTexture->isRenderBuffer())
				{
					COGLES3RenderBuffer* renderBuffer = static_cast<COGLES3RenderBuffer*>(DepthTexture);

					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getBufferID());
				}
				else
				{
					COGLES3FBOTexture* fboDepthTexture = static_cast<COGLES3FBOTexture*>(DepthTexture);

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture->getOpenGLTextureName(), 0);

					if (DepthTexture->getColorFormat() == ECF_D24S8)
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture->getOpenGLTextureName(), 0);
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			if (!checkFBOStatus(Driver))
			{
				os::Printer::log("FBO incomplete");
				return false;
			}

			return true;
		}

		void COGLES3TextureArray::uploadTexture(bool newTexture, u32 imageNumber, bool regMipmap)
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

					glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY,
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

				glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
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
		void* COGLES3TextureArray::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
		{
			return NULL;
		}

		void* COGLES3TextureArray::lock(bool readOnly, u32 mipmapLevel, u32 arraySlice)
		{
			return NULL;
		}

		//! unlock function
		void COGLES3TextureArray::unlock()
		{

		}

		//! Returns original size of the texture.
		const core::dimension2d<u32>& COGLES3TextureArray::getOriginalSize() const
		{
			return OriginalSize;
		}

		//! Returns (=size) of the texture.
		const core::dimension2d<u32>& COGLES3TextureArray::getSize() const
		{
			return Size;
		}

		//! returns color format of texture
		ECOLOR_FORMAT COGLES3TextureArray::getColorFormat() const
		{
			return ColorFormat;
		}

		u32 COGLES3TextureArray::getPitch() const
		{
			return 0;
		}

		//! Regenerates the mip map levels of the texture. Useful after locking and
		//! modifying the texture
		void COGLES3TextureArray::regenerateMipMapLevels(void* mipmapData)
		{
			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
			HasMipMaps = true;
		}		

		GLint COGLES3TextureArray::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
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
				internalformat = GL_RGB5_A1;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_UNSIGNED_SHORT_5_5_5_1;
				break;
			case ECF_R5G6B5:
				internalformat = GL_RGB565;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case ECF_R8G8B8:
				internalformat = GL_RGB8;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_UNSIGNED_BYTE;
				break;
			case ECT_PVRTC2_RGBA4444:
				internalformat = GL_RGBA4;
				colorformat = GL_RGBA;
				type = GL_UNSIGNED_SHORT_4_4_4_4;
				filtering = GL_LINEAR;
				break;
			case ECT_PVRTC2_RGB565:
				internalformat = GL_RGB565;
				colorformat = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				filtering = GL_LINEAR;
				break;
			case ECT_PVRTC2_RGB888:
				internalformat = GL_RGBA8;
				colorformat = GL_RGB;
				type = GL_UNSIGNED_BYTE;
				filtering = GL_LINEAR;
				break;
			case ECT_PVRTC2_RGBA8888:
				internalformat = GL_RGBA8;
				colorformat = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				filtering = GL_LINEAR;
				break;
			case ECF_A8R8G8B8:
				type = GL_UNSIGNED_BYTE;
				filtering = GL_LINEAR;
				internalformat = GL_RGBA8;
				colorformat = GL_RGBA;
				break;
#ifdef GL_IMG_texture_compression_pvrtc
			case ECF_PVRTC_RGB2:
				internalformat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
			case ECF_PVRTC_ARGB2:
				internalformat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
			case ECF_PVRTC_RGB4:
				internalformat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
			case ECF_PVRTC_ARGB4:
				internalformat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc2
			case ECF_PVRTC2_ARGB2:
				internalformat = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
				break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc2
			case ECF_PVRTC2_ARGB4:
				internalformat = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
				break;
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
			case ECF_ETC1:
				internalformat = GL_ETC1_RGB8_OES;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_ETC1_RGB8_OES;
				break;
#endif
#ifdef GL_ES_VERSION_3_0 // TO-DO - fix when extension name will be available
			case ECF_ETC2_RGB:
				internalformat = GL_COMPRESSED_RGB8_ETC2;
				filtering = GL_LINEAR;
				colorformat = GL_RGB;
				type = GL_COMPRESSED_RGB8_ETC2;
				break;
#endif
#ifdef GL_ES_VERSION_3_0 // TO-DO - fix when extension name will be available
			case ECF_ETC2_ARGB:
				internalformat = GL_COMPRESSED_RGBA8_ETC2_EAC;
				filtering = GL_LINEAR;
				colorformat = GL_RGBA;
				type = GL_COMPRESSED_RGBA8_ETC2_EAC;
				break;
#endif
			case ECF_D16:
				colorformat = GL_DEPTH_COMPONENT;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_DEPTH_COMPONENT16;
				break;
			case ECF_D32:
				colorformat = GL_DEPTH_COMPONENT;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_DEPTH_COMPONENT;
				break;
			case ECF_R8:
				colorformat = GL_RED;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_R8;
				break;
			case ECF_R8G8:
				colorformat = GL_RG;
				type = GL_UNSIGNED_BYTE;
				internalformat = GL_RG8;
				break;
			case ECF_R16F:
				filtering = GL_NEAREST;
				colorformat = GL_RED;
				internalformat = GL_R16F;
				type = GL_FLOAT;
				break;
			case ECF_G16R16F:
				filtering = GL_NEAREST;
				colorformat = GL_RG;
				internalformat = GL_RG16F;
				type = GL_FLOAT;
				break;
			case ECF_A16B16G16R16F:
				filtering = GL_NEAREST;
				colorformat = GL_RGBA;
				internalformat = GL_RGBA16F;
				type = GL_FLOAT;
				break;
			case ECF_R32F:
				filtering = GL_NEAREST;
				colorformat = GL_RED;
				internalformat = GL_R32F;
				type = GL_FLOAT;
				break;
			case ECF_G32R32F:
				filtering = GL_NEAREST;
				colorformat = GL_RG;
				internalformat = GL_RG32F;
				type = GL_FLOAT;
				break;
			case ECF_A32B32G32R32F:
				filtering = GL_NEAREST;
				colorformat = GL_RGBA;
				internalformat = GL_RGBA32F;
				type = GL_FLOAT;
				break;
			default:
				os::Printer::log("Unsupported texture format", ELL_ERROR);
				break;
			}

			return internalformat;
		}
	}
}

#endif