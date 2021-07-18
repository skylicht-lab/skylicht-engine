// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CImageLoaderPVR.h"

#ifdef _IRR_COMPILE_WITH_PVR_LOADER_

#include "IReadFile.h"
#include "irrOS.h"
#include "CImage.h"
#include "irrString.h"
#include "CColorConverter.h"

namespace irr
{

namespace video
{

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPVR::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension(filename, "pvr") || core::hasFileExtension(filename, "etc2");
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPVR::isALoadableFileFormat(io::IReadFile* file) const
{
	if(file != NULL) 
	{
		SPVRHeader header;
		long bytesRead = file->read(&header, sizeof(SPVRHeader));
#ifdef __BIG_ENDIAN__
		header.HeaderLength = os::byteswap(header.HeaderLength);
#endif
		if(bytesRead == sizeof(SPVRHeader)
		   && header.HeaderLength == sizeof(SPVRHeader)
		   && strncmp(header.PVRTag, "PVR", 3) == 0)
		{
			return true;
		}
	}
	return false; 
}

IImage* CImageLoaderPVR::loadData(io::IReadFile* file, SPVRHeader& header) const
{
	u32 format = header.Flags & 0xff;
	ECOLOR_FORMAT colorFormat = ECF_UNKNOWN;

	switch (format)
	{
	case 0x10: // OGL_RGBA_4444
	{
		colorFormat = ECT_PVRTC2_RGBA4444;
	}
	break;

	case 0x12:	// OGL_RGBA_8888
	{
		colorFormat = ECT_PVRTC2_RGBA8888;
	}
	break;

	case 0x13: // OGL_RGB_565
	{
		colorFormat = ECT_PVRTC2_RGB565;
	}
	break;

	case 0x15: // OGL_RGB_888
	{
		colorFormat = ECT_PVRTC2_RGB888;
	}
	break;

	case 0x0C:
	case 0x18: // OGL_PVRTC2
	{
		colorFormat = (header.Flags & 0x8000 ? ECF_PVRTC_ARGB2 : ECF_PVRTC_RGB2);
	}
	break;

	case 0x0D:
	case 0x19: // OGL_PVRTC4
	{
		colorFormat = (header.Flags & 0x8000 ? ECF_PVRTC_ARGB4 : ECF_PVRTC_RGB4);
	}
	break;
	case 0x36: // ETC
	{
		colorFormat = ECF_ETC1;
	}
	break;
	default:
	{
		os::Printer::log("PVR loader", "unsupported format", ELL_ERROR);
		return NULL;
	}
	}

	bool isCubeMap = (header.Flags & EPVRF_CUBE_MAP) != 0;
	if (isCubeMap)
	{
		if (header.MipmapCount > 1) {
			isCubeMap = false;
		}
	}

	IImage* result = NULL;

	if (isCubeMap)
	{
		os::Printer::log("No support PVR Cubemap file", file->getFileName(), ELL_ERROR);
	}
	else
	{
		u32 curWidth = header.Width;
		u32 curHeight = header.Height;

		u32 dataSize = IImage::getCompressedImageSize(colorFormat, curWidth, curHeight);
		bool hasMipMap = (header.MipmapCount > 0) ? true : false;

		if (hasMipMap == true)
		{
			do
			{
				if (curWidth > 1)
					curWidth >>= 1;

				if (curHeight > 1)
					curHeight >>= 1;

				dataSize += IImage::getCompressedImageSize(colorFormat, curWidth, curHeight);
			} while (curWidth != 1 || curWidth != 1);
		}

		if (dataSize != header.DataLength)
		{
			os::Printer::log("corrupt PVR file", file->getFileName(), ELL_ERROR);
			return NULL;
		}

		// read data
		u8* data = new u8[dataSize];
		file->read(data, dataSize);

		// create compress image
		result = new CImage(colorFormat, core::dimension2d<u32>(header.Width, header.Height), data, true, true, true, hasMipMap);
	}

	return result;
}

IImage* CImageLoaderPVR::loadDataV3(io::IReadFile* file, SPVRHeaderV3& header) const
{
	u32 format = header.Channels[0];
	ECOLOR_FORMAT colorFormat = ECF_UNKNOWN;

	if (((format == 'l') && (header.Channels[1] == 0)) && (header.ChannelDepth[0] == 8))
		colorFormat = ECF_UNKNOWN;	//UNCOMPRESSED_GRAYSCALE;
	else if (((format == 'l') && (header.Channels[1] == 'a')) && ((header.ChannelDepth[0] == 8) && (header.ChannelDepth[1] == 8)))
		colorFormat = ECF_UNKNOWN;	//UNCOMPRESSED_GRAY_ALPHA;
	else if ((format == 'r') && (header.Channels[1] == 'g') && (header.Channels[2] == 'b'))
	{
		if (header.Channels[3] == 'a')
		{
			if ((header.ChannelDepth[0] == 5) && (header.ChannelDepth[1] == 5) && (header.ChannelDepth[2] == 5) && (header.ChannelDepth[3] == 1))
				colorFormat = ECF_A1R5G5B5;
			else if ((header.ChannelDepth[0] == 4) && (header.ChannelDepth[1] == 4) && (header.ChannelDepth[2] == 4) && (header.ChannelDepth[3] == 4))
				colorFormat = ECF_UNKNOWN;
			else if ((header.ChannelDepth[0] == 8) && (header.ChannelDepth[1] == 8) && (header.ChannelDepth[2] == 8) && (header.ChannelDepth[3] == 8))
				colorFormat = ECF_A8R8G8B8;
		}
		else if (header.Channels[3] == 0)
		{
			if ((header.ChannelDepth[0] == 5) && (header.ChannelDepth[1] == 6) && (header.ChannelDepth[2] == 5))
				colorFormat = ECF_R5G6B5;
			else if ((header.ChannelDepth[0] == 8) && (header.ChannelDepth[1] == 8) && (header.ChannelDepth[2] == 8))
				colorFormat = ECF_R8G8B8;
		}
	}
	else if (format == 0)
		colorFormat = ECF_PVRTC_RGB2;
	else if (format == 1)
		colorFormat = ECF_PVRTC_ARGB2;
	else if (format == 2)
		colorFormat = ECF_PVRTC_RGB4;
	else if (format == 3)
		colorFormat = ECF_PVRTC_ARGB4;
	else if (format == 6)
		colorFormat = ECF_ETC1;
	else if (format == 22)
		colorFormat = ECF_ETC2_RGB;
	else if (format == 23)
		colorFormat = ECF_ETC2_ARGB;

	if (colorFormat == ECF_UNKNOWN)
	{
		os::Printer::log("PVR loader", "unsupported format", ELL_ERROR);
		return NULL;
	}

	file->seek(header.MetaDataSize, true);

	u32 curWidth = header.Width;
	u32 curHeight = header.Height;

	u32 dataSize = IImage::getCompressedImageSize(colorFormat, curWidth, curHeight);

	bool hasMipMap = (header.MipmapCount > 0) ? true : false;
	if (hasMipMap == true)
	{
		do
		{
			if (curWidth > 1)
				curWidth >>= 1;

			if (curHeight > 1)
				curHeight >>= 1;

			dataSize += IImage::getCompressedImageSize(colorFormat, curWidth, curHeight);
		} while (curWidth != 1 || curWidth != 1);
	}	
	
	// read data
	u8* data = new u8[dataSize];
	file->read(data, dataSize);

	// create compress image
	return new CImage(colorFormat, core::dimension2d<u32>(header.Width, header.Height), data, true, true, true, hasMipMap);	
}

//! creates a surface from the file
IImage* CImageLoaderPVR::loadImage(io::IReadFile* file) const
{
	SPVRHeader		header;
	SPVRHeaderV3	headerV3;
	
	bool isHeaderV3 = false;

	if(!readPVRHeader(file, header))
	{
		if (!readPVRHeaderV3(file, headerV3))
		{
			os::Printer::log("Read PVR HEADER error");
			return NULL;
		}

		isHeaderV3 = true;
	}

	IImage* result = NULL;

	if (isHeaderV3 == true)
		result = loadDataV3(file, headerV3);
	else	
		result = loadData(file, header);	
	
	return result;
}

bool CImageLoaderPVR::readPVRHeaderV3(io::IReadFile* file, SPVRHeaderV3& header) const
{
	file->seek(0);
	s32 readCount = file->read(&header, sizeof(SPVRHeaderV3));

#ifdef __BIG_ENDIAN__
	...
#endif

	if (readCount != sizeof(SPVRHeaderV3)
		|| header.PVRTag[3] != 3
		|| strncmp(header.PVRTag, "PVR", 3) != 0
		|| ((header.Flags & EPVRF_HAS_MIPMAPS) != 0 && header.MipmapCount == 0)
		|| (header.Flags & EPVRF_CUBE_MAP) != 0)
	{
		return false;
	}

	header.MipmapCount++;
	return true;
}

bool CImageLoaderPVR::readPVRHeader(io::IReadFile* file, SPVRHeader& header) const
{
	file->seek(0);
	s32 readCount = file->read(&header, sizeof(SPVRHeader));

#ifdef __BIG_ENDIAN__
	header.HeaderLength = os::byteswap(header.HeaderLength);
	header.Height = os::byteswap(header.Height);
	header.Width = os::byteswap(header.Width);
	header.MipmapCount = os::byteswap(header.MipmapCount);
	header.Flags = os::byteswap(header.Flags);
	header.DataLength = os::byteswap(header.DataLength);
	header.BitsPerPixel = os::byteswap(header.BitsPerPixel);
	header.BitmaskRed = os::byteswap(header.BitmaskRed);
	header.BitmaskGreen = os::byteswap(header.BitmaskGreen);
	header.BitmaskBlue = os::byteswap(header.BitmaskBlue);
	header.BitmaskAlpha = os::byteswap(header.BitmaskAlpha);
	header.SurfaceCount = os::byteswap(header.SurfaceCount);
#endif

	if(readCount != sizeof(SPVRHeader)
	   || strncmp(header.PVRTag, "PVR!", 4) != 0
	   || header.HeaderLength != sizeof(SPVRHeader)
	   || ((header.Flags & EPVRF_HAS_MIPMAPS) != 0 && header.MipmapCount == 0)
	   || (header.Flags & EPVRF_CUBE_MAP) != 0)
	{
		return false;
	}

	header.MipmapCount++;

	return true;	
}

//! creates a loader which is able to load pvr images
IImageLoader* createImageLoaderPVR()
{
	return new CImageLoaderPVR();
}


} // end namespace video
} // end namespace irr

#endif

