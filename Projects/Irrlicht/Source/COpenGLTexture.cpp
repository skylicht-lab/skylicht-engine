// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "irrOS.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
	namespace video
	{

		//! constructor for usual textures
		COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name, void* mipmapData, COpenGLDriver* driver)
			: ITexture(name), Driver(driver), Image(0), MipImage(0),
			TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
			PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
			IsCompressed(false), AutomaticMipmapUpdate(false),
			ReadOnlyLock(false), KeepImage(true), IsDepthTexture(false), IsRenderBuffer(false)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif

			DriverType = EDT_OPENGL;
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
				if (!Driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT))
				{
					os::Printer::log("DXT texture compression not available.", ELL_ERROR);
					return;
				}

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
			}
			else
			{
				Image = Driver->createImage(ColorFormat, Size);
				// scale texture
				origImage->copyToScaling(Image);
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
		COpenGLTexture::COpenGLTexture(const io::path& name, COpenGLDriver* driver)
			: ITexture(name), Driver(driver), Image(0), MipImage(0),
			TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
			PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0),
			MipmapLegacyMode(true), IsCompressed(false),
			AutomaticMipmapUpdate(false), ReadOnlyLock(false), KeepImage(true),
			IsDepthTexture(false), IsRenderBuffer(false)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif

			DriverType = EDT_OPENGL;
			ColorFormat = ECF_A8R8G8B8;
			HasMipMaps = true;
			HasAlpha = true;
		}


		//! destructor
		COpenGLTexture::~COpenGLTexture()
		{
			if (TextureName)
				glDeleteTextures(1, &TextureName);
			if (Image)
				Image->drop();

			Driver->getBridgeCalls()->resetTexture(this);
		}


		//! Choose best matching color format, based on texture creation flags
		ECOLOR_FORMAT COpenGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
		{
			// render target format
			if (format >= ECF_R16F)
			{
				return format;
			}

			ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
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
		GLint COpenGLTexture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
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


		// prepare values ImageSize, TextureSize, and ColorFormat based on image
		void COpenGLTexture::getImageValues(IImage* image)
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
		void COpenGLTexture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
		{
			// check which image needs to be uploaded
			IImage* image = level ? MipImage : Image;
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
				return;
			}

			// get correct opengl color data values
			GLenum oldInternalFormat = InternalFormat;
			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);
			// make sure we don't change the internal format of existing images
			if (!newTexture)
				InternalFormat = oldInternalFormat;

			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);

			// mipmap handling for main texture
			if (!level && newTexture)
			{
				// auto generate if possible and no mipmap data is given
				if (!IsCompressed && HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
				{
					if (!Driver->queryFeature(EVDF_FRAMEBUFFER_OBJECT))
					{
#ifdef GL_SGIS_generate_mipmap
						if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
							glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_FASTEST);
						else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
							glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
						else
							glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_DONT_CARE);

						glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
						MipmapLegacyMode = true;
						AutomaticMipmapUpdate = true;
#endif
					}
					else
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
					compressedDataSize = IImage::getCompressedImageSize(image->getColorFormat(), Size.Width, Size.Height);

					Driver->extGlCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, image->getDimension().Width,
						image->getDimension().Height, 0, compressedDataSize, source);
				}
				else
					glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width,
						image->getDimension().Height, 0, PixelFormat, PixelType, source);
			}
			else
			{
				if (IsCompressed)
				{
					compressedDataSize = IImage::getCompressedImageSize(image->getColorFormat(), Size.Width, Size.Height);

					Driver->extGlCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
						image->getDimension().Height, PixelFormat, compressedDataSize, source);
				}
				else
					glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
						image->getDimension().Height, PixelFormat, PixelType, source);
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
		void* COpenGLTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
		{
			if (IsCompressed) // TO-DO
				return 0;

			// store info about which image is locked
			IImage* image = (mipmapLevel == 0) ? Image : MipImage;
			ReadOnlyLock |= (mode == ETLM_READ_ONLY);
			MipLevelStored = mipmapLevel;
			if (!ReadOnlyLock && mipmapLevel)
			{
#ifdef GL_SGIS_generate_mipmap
				if (Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
				{
					// do not automatically generate and update mipmaps
					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
				}
#endif
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

				if (mode != ETLM_WRITE_ONLY)
				{
					u8* pixels = static_cast<u8*>(image->lock());
					if (!pixels)
						return 0;

					// we need to keep the correct texture bound later on
					GLint tmpTexture;
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
					glBindTexture(GL_TEXTURE_2D, TextureName);

					// we need to flip textures vertical
					// however, it seems that this does not hold for mipmap
					// textures, for unknown reasons.

					// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
					if (!mipmapLevel && Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_MESA_pack_invert))
						glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

					// download GPU data as ARGB8 to pixels;
					glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

					if (!mipmapLevel)
					{
#ifdef GL_MESA_pack_invert
						if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_MESA_pack_invert))
							glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
						else
#endif
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
					}
					image->unlock();

					//reset old bound texture
					glBindTexture(GL_TEXTURE_2D, tmpTexture);
				}
			}
			return image->lock();
		}


		//! unlock function
		void COpenGLTexture::unlock()
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
				uploadTexture(false, 0, MipLevelStored);
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
		GLuint COpenGLTexture::getOpenGLTextureName() const
		{
			return TextureName;
		}


		//! Regenerates the mip map levels of the texture. Useful after locking and
		//! modifying the texture
		void COpenGLTexture::regenerateMipMapLevels(void* mipmapData)
		{
			// texture require mipmaps?
			if (!HasMipMaps)
				return;

			// we don't use custom data for mipmaps.
			if (!mipmapData)
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
			if (!mipmapData && AutomaticMipmapUpdate && !MipmapLegacyMode)
			{
				glEnable(GL_TEXTURE_2D);
				Driver->extGlGenerateMipmap(GL_TEXTURE_2D);

				return;
			}

			// Manually create mipmaps or use prepared version
			u32 compressedDataSize = 0;
			u32 width = Image->getDimension().Width;
			u32 height = Image->getDimension().Height;
			u32 i = 0;
			u8* target = static_cast<u8*>(mipmapData);
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
					if (ColorFormat == ECF_DXT1)
						compressedDataSize = ((width + 3) / 4) * ((height + 3) / 4) * 8;
					else if (ColorFormat == ECF_DXT2 || ColorFormat == ECF_DXT3 || ColorFormat == ECF_DXT4 || ColorFormat == ECF_DXT5)
						compressedDataSize = ((width + 3) / 4) * ((height + 3) / 4) * 16;

					Driver->extGlCompressedTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width,
						height, 0, compressedDataSize, target);
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
			} while (width != 1 || height != 1);
			// cleanup
			if (!mipmapData)
				delete[] target;
		}


		void COpenGLTexture::setIsRenderTarget(bool isTarget)
		{
			IsRenderTarget = isTarget;
		}


		bool COpenGLTexture::isFrameBufferObject() const
		{
			return false;
		}


		bool COpenGLTexture::isDepthTexture() const
		{
			return IsDepthTexture;
		}


		bool COpenGLTexture::isRenderBuffer() const
		{
			return IsRenderBuffer;
		}


		//! Bind Render Target Texture
		void COpenGLTexture::bindRTT()
		{
		}


		//! Unbind Render Target Texture
		void COpenGLTexture::unbindRTT()
		{
			Driver->setActiveTexture(0, this);
			Driver->getBridgeCalls()->setTexture(0);

			// Copy Our ViewPort To The Texture
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
		}


		//! Get an access to texture states cache.
		COpenGLTexture::SStatesCache& COpenGLTexture::getStatesCache() const
		{
			return StatesCache;
		}


		/* FBO Textures */

		// helper function for render to texture
		bool checkFBOStatus(COpenGLDriver* Driver);

		//! RTT FBO constructor
		COpenGLFBOTexture::COpenGLFBOTexture(const core::dimension2d<u32>& size,
			const io::path& name, COpenGLDriver* driver,
			ECOLOR_FORMAT format)
			: COpenGLTexture(name, driver), BufferID(0), DepthTexture(0)
		{
#ifdef _DEBUG
			setDebugName("COpenGLFBOTexture");
#endif

			DriverType = EDT_OPENGL;

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

			GLint FilteringType = 0;
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

#ifdef GL_EXT_framebuffer_object
			// generate FBO

			Driver->extGlGenFramebuffers(1, &BufferID);

			if (BufferID != 0 && !IsDepthTexture)
			{
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, BufferID);

				Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, TextureName, 0);

				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
			}
#endif
		}


		//! destructor
		COpenGLFBOTexture::~COpenGLFBOTexture()
		{
			if (DepthTexture)
			{
				bool remove = DepthTexture->isRenderBuffer();

				if (DepthTexture->drop() && remove)
					Driver->removeDepthTexture(DepthTexture);
			}

#ifdef GL_EXT_framebuffer_object
			if (BufferID)
				Driver->extGlDeleteFramebuffers(1, &BufferID);
#endif
		}


		bool COpenGLFBOTexture::isFrameBufferObject() const
		{
			return true;
		}


		//! Bind Render Target Texture
		void COpenGLFBOTexture::bindRTT()
		{
#ifdef GL_EXT_framebuffer_object
			if (BufferID != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, BufferID);

			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
#endif
		}


		//! Unbind Render Target Texture
		void COpenGLFBOTexture::unbindRTT()
		{
#ifdef GL_EXT_framebuffer_object
			if (BufferID != 0)
				Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
		}


		//! Get depth texture.
		ITexture* COpenGLFBOTexture::getDepthTexture() const
		{
			return DepthTexture;
		}


		//! Set depth texture.
		bool COpenGLFBOTexture::setDepthTexture(ITexture* depthTexture)
		{
			if (DepthTexture == depthTexture || BufferID == 0)
				return false;

#ifdef GL_EXT_framebuffer_object
			Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, BufferID);

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


		/* Render Buffer */

		//! constructor
		COpenGLRenderBuffer::COpenGLRenderBuffer(
			const core::dimension2d<u32>& size,
			const io::path& name,
			COpenGLDriver* driver,
			bool useStencil)
			: COpenGLTexture(name, driver), BufferID(0)
		{
#ifdef _DEBUG
			setDebugName("COpenGLRenderBuffer");
#endif

			DriverType = EDT_OPENGL;

			IsDepthTexture = true;
			IsRenderBuffer = true;

			OriginalSize = size;
			Size = size;
			InternalFormat = GL_RGBA;
			PixelFormat = GL_RGBA;
			PixelType = GL_UNSIGNED_BYTE;
			HasMipMaps = false;

#ifdef GL_EXT_framebuffer_object
			// generate depth buffer
			Driver->extGlGenRenderbuffers(1, &BufferID);
			Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, BufferID);
			Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT, Driver->getZBufferBits(), OriginalSize.Width, OriginalSize.Height);
			Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
#endif
		}


		//! destructor
		COpenGLRenderBuffer::~COpenGLRenderBuffer()
		{
#ifdef GL_EXT_framebuffer_object
			if (BufferID)
				Driver->extGlDeleteRenderbuffers(1, &BufferID);
#endif
		}


		//! Bind Render Target Texture
		void COpenGLRenderBuffer::bindRTT()
		{
		}


		//! Unbind Render Target Texture
		void COpenGLRenderBuffer::unbindRTT()
		{
		}



		GLuint COpenGLRenderBuffer::getBufferID() const
		{
			return BufferID;
		}


		bool checkFBOStatus(COpenGLDriver* Driver)
		{
#ifdef GL_EXT_framebuffer_object
			GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

			switch (status)
			{
				//Our FBO is perfect, return true
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				return true;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
				break;

				// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
			case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
				os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
				break;
#endif

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				os::Printer::log("FBO missing an image attachment", ELL_ERROR);
				break;

#ifdef GL_EXT_framebuffer_multisample
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
				os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
				break;
#endif

			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				os::Printer::log("FBO format unsupported", ELL_ERROR);
				break;

			default:
				break;
			}
#endif
			os::Printer::log("FBO error", ELL_ERROR);
			//	_IRR_DEBUG_BREAK_IF(true);
			return false;
		}


	} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

