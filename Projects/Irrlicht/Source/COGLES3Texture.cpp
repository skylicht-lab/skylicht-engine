// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES3_

#include "irrTypes.h"
#include "COGLES3Texture.h"
#include "COGLES3Driver.h"
#include "irrOS.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
	namespace video
	{

		//! constructor for usual textures
		COGLES3Texture::COGLES3Texture(IImage* origImage, const io::path& name, void* mipmapData, COGLES3Driver* driver)
			: ITexture(name), Driver(driver), Image(0), MipImage(0),
			TextureName(0), InternalFormat(GL_RGBA8), PixelFormat(GL_RGBA), FilteringType(0),
			PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
			IsCompressed(false), AutomaticMipmapUpdate(false),
			ReadOnlyLock(false), KeepImage(true), IsDepthTexture(false), IsRenderBuffer(false)
		{
#ifdef _DEBUG
			setDebugName("COGLES3Texture");
#endif

			DriverType = EDT_OPENGLES;
			ColorFormat = ECF_A8R8G8B8;
			HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			IsRenderTarget = false;

			getImageValues(origImage);

			switch (ColorFormat)
			{
			case ECF_A8R8G8B8:
			case ECF_A1R5G5B5:
			case ECF_DXT1:
			case ECF_DXT2:
			case ECF_DXT3:
			case ECF_DXT4:
			case ECF_DXT5:
			case ECF_A16B16G16R16F:
			case ECF_A32B32G32R32F:
				HasAlpha = true;
				break;
			default:
				break;
			}

			if (ColorFormat >= ECF_R16F)
			{
				Image = origImage;
				Image->grab();
				KeepImage = false;
			}
			else if (IImage::isCompressedFormat(ColorFormat))
			{
				if (OriginalSize != Size)
				{
					os::Printer::log("Invalid size of image for compressed texture, size of image must be POT.", ELL_ERROR);
					return;
				}
				else
				{
					IsCompressed = true;
					Image = origImage;
					Image->grab();
					KeepImage = false;
				}
			}
			else if (OriginalSize == Size)
			{
				Image = Driver->createImage(ColorFormat, OriginalSize);
				origImage->copyTo(Image);
				Image->swapBG();
			}
			else
			{
				Image = Driver->createImage(ColorFormat, Size);
				origImage->copyToScaling(Image);
				Image->swapBG();
			}

			Pitch = Image->getPitch();

			glGenTextures(1, &TextureName);
			uploadTexture(true, mipmapData);
			if (!KeepImage)
			{
				Image->drop();
				Image = 0;
			}
		}


		//! constructor for basic setup (only for derived classes)
		COGLES3Texture::COGLES3Texture(const io::path& name, COGLES3Driver* driver)
			: ITexture(name), Driver(driver), Image(0), MipImage(0),
			TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
			PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0),
			MipmapLegacyMode(true), IsCompressed(false),
			AutomaticMipmapUpdate(false), ReadOnlyLock(false), KeepImage(true),
			IsDepthTexture(false), IsRenderBuffer(false)
		{
#ifdef _DEBUG
			setDebugName("COGLES3Texture");
#endif

			DriverType = EDT_OPENGLES;
			ColorFormat = ECF_A8R8G8B8;
			HasMipMaps = true;
			HasAlpha = true;
		}


		//! destructor
		COGLES3Texture::~COGLES3Texture()
		{
			if (TextureName)
				glDeleteTextures(1, &TextureName);
			if (Image)
				Image->drop();

			Driver->getBridgeCalls()->resetTexture(this);
		}


		//! Choose best matching color format, based on texture creation flags
		ECOLOR_FORMAT COGLES3Texture::getBestColorFormat(ECOLOR_FORMAT format)
		{
			// render target format
			if (format >= ECF_R16F)
			{
				return format;
			}

			ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;

			if (!IImage::isCompressedFormat(format))
			{
				switch (format)
				{
				case ECF_A1R5G5B5:
					if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
						destFormat = ECF_A1R5G5B5;
					break;
				case ECF_R5G6B5:
					if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
						destFormat = ECF_A1R5G5B5;
					break;
				case ECF_A8R8G8B8:
					if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
						Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
						destFormat = ECF_A1R5G5B5;
					break;
				case ECF_R8G8B8:
					if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
						Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
						destFormat = ECF_A1R5G5B5;
				default:
					break;
				}
			}
			else
			{
				destFormat = format;
			}

			if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
			{
				switch (destFormat)
				{
				case ECF_A1R5G5B5:
					destFormat = ECF_R5G6B5;
					break;
				case ECF_A8R8G8B8:
					destFormat = ECF_R8G8B8;
					break;
				default:
					break;
				}
			}
			return destFormat;
		}


		//! Get opengl values for the GPU texture storage
		GLint COGLES3Texture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
			GLint& filtering,
			GLenum& colorformat,
			GLenum& type)
		{
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

			case ECF_DXT1:
#if !defined(ANDROID)
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
#endif
				break;
			case ECF_DXT2:
			case ECF_DXT3:
#if !defined(ANDROID)
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
#endif
				break;
			case ECF_DXT4:
			case ECF_DXT5:
#if !defined(ANDROID)
				colorformat = GL_BGRA_EXT;
				type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
				break;

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


		// prepare values ImageSize, TextureSize, and ColorFormat based on image
		void COGLES3Texture::getImageValues(IImage* image)
		{
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
				return;
			}

			OriginalSize = image->getDimension();

			if (!OriginalSize.Width || !OriginalSize.Height)
			{
				os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
				return;
			}

			const f32 ratio = (f32)OriginalSize.Width / (f32)OriginalSize.Height;

			if ((OriginalSize.Width > Driver->MaxTextureSize) && (ratio >= 1.0f))
			{
				OriginalSize.Width = Driver->MaxTextureSize;
				OriginalSize.Height = (u32)(Driver->MaxTextureSize / ratio);
			}
			else if (OriginalSize.Height > Driver->MaxTextureSize)
			{
				OriginalSize.Height = Driver->MaxTextureSize;
				OriginalSize.Width = (u32)(Driver->MaxTextureSize*ratio);
			}

			Size = OriginalSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));

			if (IImage::isCompressedFormat(image->getColorFormat()))
				ColorFormat = image->getColorFormat();
			else
				ColorFormat = getBestColorFormat(image->getColorFormat());
		}


		//! copies the the texture into an open gl texture.
		void COGLES3Texture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
		{
			// check which image needs to be uploaded
			IImage* image = level ? MipImage : Image;
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
				return;
			}

			// get correct opengl color data values
			GLenum oldinternalformat = InternalFormat;
			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);
			// make sure we don't change the internal format of existing images
			if (!newTexture)
				InternalFormat = oldinternalformat;

			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

#if _DEBUG
			Driver->testGLError();
#endif
			// mipmap handling for main texture
			if (!level && newTexture)
			{
				// auto generate if possible and no mipmap data is given
				if (!IsCompressed && HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
				{
					if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
						glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
					else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
						glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
					else
						glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);

					MipmapLegacyMode = false;
					AutomaticMipmapUpdate = true;
				}

				// enable bilinear filter without mipmaps
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				StatesCache.BilinearFilter = true;
				StatesCache.TrilinearFilter = false;
				StatesCache.MipMapStatus = false;
			}

			// now get image data and upload to GPU
			u32 compressedDataSize = 0;

			void* source = image->lock();
			if (newTexture)
			{
				if (IsCompressed)
				{
					compressedDataSize = IImage::getCompressedImageSize(ColorFormat, image->getDimension().Width, image->getDimension().Height);
					glCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, image->getDimension().Width, image->getDimension().Height, 0, compressedDataSize, source);
				}
				else
					glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width, image->getDimension().Height, 0, PixelFormat, PixelType, source);
			}
			else
			{
				if (IsCompressed)
				{
					compressedDataSize = IImage::getCompressedImageSize(ColorFormat, image->getDimension().Width, image->getDimension().Height);
					glCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width, image->getDimension().Height, PixelFormat, compressedDataSize, source);
				}
				else
					glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width, image->getDimension().Height, PixelFormat, PixelType, source);
			}
			image->unlock();

			if (!level && newTexture)
			{
				if (IsCompressed && !mipmapData)
				{
					if (image->hasMipMaps())
						mipmapData = static_cast<u8*>(image->lock()) + compressedDataSize;
					else
						HasMipMaps = false;
				}

				regenerateMipMapLevels(mipmapData);

				if (HasMipMaps) // might have changed in regenerateMipMapLevels
				{
					// enable bilinear mipmap filter
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					StatesCache.BilinearFilter = true;
					StatesCache.TrilinearFilter = false;
					StatesCache.MipMapStatus = true;
				}
			}

			if (Driver->testGLError())
				os::Printer::log("Could not glTexImage2D", ELL_ERROR);
		}


		//! lock function
		void* COGLES3Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
		{
			if (IsCompressed)
				return 0;

			// store info about which image is locked
			IImage* image = (mipmapLevel == 0) ? Image : MipImage;
			ReadOnlyLock |= (mode == ETLM_READ_ONLY);
			MipLevelStored = mipmapLevel;
			if (!ReadOnlyLock && mipmapLevel)
			{
				AutomaticMipmapUpdate = false;
			}

			// if data not available or might have changed on GPU download it
			if (!image || IsRenderTarget)
			{
				// prepare the data storage if necessary
				if (!image)
				{
					if (mipmapLevel)
					{
						u32 i = 0;
						u32 width = Size.Width;
						u32 height = Size.Height;
						do
						{
							if (width > 1)
								width >>= 1;
							if (height > 1)
								height >>= 1;
							++i;
						} while (i != mipmapLevel);
						MipImage = image = Driver->createImage(ECF_A8R8G8B8, core::dimension2du(width, height));
					}
					else
						Image = image = Driver->createImage(ECF_A8R8G8B8, OriginalSize);
					ColorFormat = ECF_A8R8G8B8;
				}
				if (!image)
					return 0;

				// duc.phamhong
				// OpenGLES donot support glGetTexImage, use glReadPixels
				// So i do it later
				// Just lock memory for unlock texture update
				if (mode != ETLM_WRITE_ONLY)
				{
					u8* pixels = static_cast<u8*>(image->lock());
					if (!pixels)
						return 0;

					os::Printer::log("[COGLES3Texture::unlock] Begin bind texture to FBO for read");

					// Bind this texture to GL_COLOR_ATTACHMENT0
					Driver->setRenderTarget(this, false, false, SColor(0, 0, 0, 0), NULL);

					// download GPU data as ARGB8 to pixels;
					glReadBuffer(GL_COLOR_ATTACHMENT0);
					glReadPixels(0, 0, Size.Width, Size.Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

					if (!mipmapLevel)
					{
						// opengl images are horizontally flipped, so we have to fix that here.
						const s32 pitch = image->getPitch();
						u8* p2 = pixels + (image->getDimension().Height - 1) * pitch;
						u8* tmpBuffer = new u8[pitch];
						for (u32 i = 0; i < image->getDimension().Height; i += 2)
						{
							memcpy(tmpBuffer, pixels, pitch);
							memcpy(pixels, p2, pitch);
							memcpy(p2, tmpBuffer, pitch);
							pixels += pitch;
							p2 -= pitch;
						}
						delete[] tmpBuffer;
					}

					image->unlock();

					// reset RTT
					Driver->setRenderTarget(NULL, false, false, SColor(0, 0, 0, 0), NULL);

					os::Printer::log("[COGLES3Texture::unlock] End bind texture to FBO");
				}
			}
			return image->lock();
		}


		//! unlock function
		void COGLES3Texture::unlock()
		{
			if (IsCompressed) // TO-DO
				return;

			// test if miplevel or main texture was locked
			IImage* image = MipImage ? MipImage : Image;
			if (!image)
				return;

			// unlock image to see changes
			image->unlock();
			// copy texture data to GPU
			if (!ReadOnlyLock)
			{
				os::Printer::log("[COGLES3Texture::unlock] Update texture");
				uploadTexture(false, 0, MipLevelStored);
			}
			ReadOnlyLock = false;
			// cleanup local image
			if (MipImage)
			{
				MipImage->drop();
				MipImage = 0;
			}
			else if (!KeepImage)
			{
				Image->drop();
				Image = 0;
			}
			// update information
			if (Image)
				ColorFormat = Image->getColorFormat();
			else
				ColorFormat = ECF_A8R8G8B8;
		}


		//! return open gl texture name
		GLuint COGLES3Texture::getOpenGLTextureName() const
		{
			return TextureName;
		}


		//! Regenerates the mip map levels of the texture. Useful after locking and
		//! modifying the texture
		void COGLES3Texture::regenerateMipMapLevels(void* mipmapData)
		{
			// texture require mipmaps?
			if (!HasMipMaps && !IsRenderTarget)
				return;

			// we don't use custom data for mipmaps.
			if (!mipmapData && !IsRenderTarget)
			{
				// compressed textures require custom data for prepare mipmaps.
				if (IsCompressed)
					return;

				// texture use legacy method for generate mipmaps?
				if (AutomaticMipmapUpdate && MipmapLegacyMode)
					return;

				// hardware doesn't support generate mipmaps for certain texture but image data doesn't exist or is wrong.
				if (!AutomaticMipmapUpdate && (!Image || (Image && ((Image->getDimension().Width == 1) && (Image->getDimension().Height == 1)))))
					return;
			}

			// hardware moethods for generate mipmaps.
			if (IsRenderTarget || (!mipmapData && AutomaticMipmapUpdate && !MipmapLegacyMode))
			{
				if (IsRenderTarget)
				{
					Driver->setActiveTexture(0, this);
					Driver->getBridgeCalls()->setTexture(0);
				}

				glEnable(GL_TEXTURE_2D);
				glGenerateMipmap(GL_TEXTURE_2D);

				HasMipMaps = true;
				return;
			}

			// Manually create mipmaps or use prepared version
			u32 compressedDataSize = 0;
			u32 width = Image->getDimension().Width;
			u32 height = Image->getDimension().Height;
			u32 i = 0;
			u8* target = static_cast<u8*>(mipmapData);
			u32 m = width < height ? width : height;

			do
			{
				if (width > 1)
					width >>= 1;
				if (height > 1)
					height >>= 1;

				++i;

				if (!target)
					target = new u8[width*height*Image->getBytesPerPixel()];

				// create scaled version if no mipdata available
				if (!mipmapData)
					Image->copyToScaling(target, width, height, Image->getColorFormat());

				if (IsCompressed)
				{
					compressedDataSize = IImage::getCompressedImageSize(ColorFormat, width, height);
					glCompressedTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height, 0, compressedDataSize, target);
				}
				else
					glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
						0, PixelFormat, PixelType, target);

				// get next prepared mipmap data if available
				if (mipmapData)
				{
					if (IsCompressed)
						mipmapData = static_cast<u8*>(mipmapData) + compressedDataSize;
					else
						mipmapData = static_cast<u8*>(mipmapData) + width * height*Image->getBytesPerPixel();

					target = static_cast<u8*>(mipmapData);
				}

				m = width < height ? width : height;
			} while (m > 1);

			// cleanup
			if (!mipmapData)
				delete[] target;
		}


		void COGLES3Texture::setIsRenderTarget(bool isTarget)
		{
			IsRenderTarget = isTarget;
		}


		bool COGLES3Texture::isFrameBufferObject() const
		{
			return false;
		}


		bool COGLES3Texture::isDepthTexture() const
		{
			return IsDepthTexture;
		}


		bool COGLES3Texture::isRenderBuffer() const
		{
			return IsRenderBuffer;
		}


		//! Bind Render Target Texture
		void COGLES3Texture::bindRTT()
		{
		}


		//! Unbind Render Target Texture
		void COGLES3Texture::unbindRTT()
		{
			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			// Copy Our ViewPort To The Texture
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
		}


		//! Get an access to texture states cache.
		COGLES3Texture::SStatesCache& COGLES3Texture::getStatesCache() const
		{
			return StatesCache;
		}


		/* FBO Textures */

		// helper function for render to texture
		bool checkFBOStatus(COGLES3Driver* Driver);

		//! RTT FBO constructor
		COGLES3FBOTexture::COGLES3FBOTexture(const core::dimension2d<u32>& size,
			const io::path& name, COGLES3Driver* driver,
			ECOLOR_FORMAT format)
			: COGLES3Texture(name, driver), BufferID(0), DepthTexture(0)
		{
#ifdef _DEBUG
			setDebugName("COGLES3FBOTexture");
#endif

			DriverType = EDT_OPENGLES;

			if (ECF_UNKNOWN == format)
				format = getBestColorFormat(driver->getColorFormat());

			IsDepthTexture = IImage::isDepthFormat(format);

			OriginalSize = size;
			Size = size;
			ColorFormat = format;

			switch (ColorFormat)
			{
			case ECF_A8R8G8B8:
			case ECF_A1R5G5B5:
			case ECF_A16B16G16R16F:
			case ECF_A32B32G32R32F:
				HasAlpha = true;
				break;
			default:
				break;
			}

			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(format, FilteringType, PixelFormat, PixelType);

			HasMipMaps = false;
			IsRenderTarget = true;

			// generate color texture

			glGenTextures(1, &TextureName);

			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringType);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			if (FilteringType == GL_NEAREST)
				StatesCache.BilinearFilter = false;
			else
				StatesCache.BilinearFilter = true;

			StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
			StatesCache.WrapV = ETC_CLAMP_TO_EDGE;

			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, OriginalSize.Width, OriginalSize.Height, 0, PixelFormat, PixelType, 0);

			Driver->setActiveTexture(0, 0);
			Driver->getBridgeCalls()->setTexture(0);

			// generate FBO
			glGenFramebuffers(1, &BufferID);

			if (BufferID != 0 && !IsDepthTexture)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureName, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

#if _DEBUG
			Driver->testGLError();
#endif
		}


		//! destructor
		COGLES3FBOTexture::~COGLES3FBOTexture()
		{
			if (DepthTexture)
			{
				bool remove = DepthTexture->isRenderBuffer();

				if (DepthTexture->drop() && remove)
					Driver->removeDepthTexture(DepthTexture);
			}

			if (BufferID)
				glDeleteFramebuffers(1, &BufferID);
		}


		bool COGLES3FBOTexture::isFrameBufferObject() const
		{
			return true;
		}


		//! Bind Render Target Texture
		void COGLES3FBOTexture::bindRTT()
		{
			if (BufferID != 0)
				glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

			GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, buffers);
		}


		//! Unbind Render Target Texture
		void COGLES3FBOTexture::unbindRTT()
		{
			if (BufferID != 0)
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		//! Get depth texture.
		ITexture* COGLES3FBOTexture::getDepthTexture() const
		{
			return DepthTexture;
		}


		//! Set depth texture.
		bool COGLES3FBOTexture::setDepthTexture(ITexture* depthTexture)
		{
			if (DepthTexture == depthTexture || BufferID == 0)
				return false;

			glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

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


		/* Render Buffer */
		//! constructor
		COGLES3RenderBuffer::COGLES3RenderBuffer(
			const core::dimension2d<u32>& size,
			const io::path& name,
			COGLES3Driver* driver,
			bool useStencil)
			: COGLES3Texture(name, driver), BufferID(0)
		{
#ifdef _DEBUG
			setDebugName("COGLES3RenderBuffer");
#endif

			DriverType = EDT_OPENGLES;

			IsDepthTexture = true;
			IsRenderBuffer = true;

			OriginalSize = size;
			Size = size;
			InternalFormat = GL_RGBA;
			PixelFormat = GL_RGBA;
			PixelType = GL_UNSIGNED_BYTE;
			HasMipMaps = false;

			// generate depth buffer
			glGenRenderbuffers(1, &BufferID);
			glBindRenderbuffer(GL_RENDERBUFFER, BufferID);
			glRenderbufferStorage(GL_RENDERBUFFER, Driver->getZBufferBits(), OriginalSize.Width, OriginalSize.Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}


		//! destructor
		COGLES3RenderBuffer::~COGLES3RenderBuffer()
		{
			if (BufferID)
				glDeleteRenderbuffers(1, &BufferID);
		}


		//! Bind Render Target Texture
		void COGLES3RenderBuffer::bindRTT()
		{
		}


		//! Unbind Render Target Texture
		void COGLES3RenderBuffer::unbindRTT()
		{
		}

		GLuint COGLES3RenderBuffer::getBufferID() const
		{
			return BufferID;
		}


		bool checkFBOStatus(COGLES3Driver* Driver)
		{
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			switch (status)
			{
				//Our FBO is perfect, return true
			case GL_FRAMEBUFFER_COMPLETE:
				return true;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				os::Printer::log("FBO missing an image attachment", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				os::Printer::log("FBO format unsupported", ELL_ERROR);
				break;

			default:
				break;
			}

			os::Printer::log("FBO error", ELL_ERROR);
			return false;
		}

	} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

