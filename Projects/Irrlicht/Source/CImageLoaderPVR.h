// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_PVR_H_INCLUDED__
#define __C_IMAGE_LOADER_PVR_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PVR_LOADER_

#include "IImageLoader.h"

namespace irr
{
namespace video
{

// byte-align structures
#include "irrpack.h"

enum E_PVR_FLAGS
{
	EPVRF_FORMAT_MASK = 0xFF,
	EPVRF_HAS_MIPMAPS = 0x100,
	EPVRF_TWIDDLED = 0x200,
	EPVRF_NORMAL_MAP = 0x400,
	EPVRF_BORDER = 0x800,
	EPVRF_CUBE_MAP = 0x1000,
	EPVRF_FALSE_COLOR_MIPMAPS = 0x2000,
	EPVRF_VOLUME_TEXTURE = 0x4000,
	EPVRF_ALPHA_IN_TEXTURE = 0x8000,
	EPVRF_FLIPPED = 0x10000
};

struct SPVRHeader
{
	u32 HeaderLength;
	u32 Height;
	u32 Width;
	u32 MipmapCount;
	u32 Flags;
	u32 DataLength;
	u32 BitsPerPixel;
	u32 BitmaskRed;
	u32 BitmaskGreen;
	u32 BitmaskBlue;
	u32 BitmaskAlpha;
	char PVRTag[4];
	u32 SurfaceCount;
} PACK_STRUCT;

struct SPVRHeaderV3 {
	char	PVRTag[4];
	u32		Flags;
	u8		Channels[4];
	u8		ChannelDepth[4];
	u32		ColourSpace;
	u32		ChannelType;
	u32		Height;
	u32		Width;
	u32		Depth;
	u32		NumSurfaces;
	u32		NumFaces;
	u32		MipmapCount;
	u32		MetaDataSize;
} PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

/*!
	Surface Loader for PVR images
*/
class CImageLoaderPVR : public IImageLoader
{
public:

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(io::IReadFile* file) const;

	//! creates a surface from the file
	virtual IImage* loadImage(io::IReadFile* file) const;

	//! load header
	virtual bool readPVRHeader(io::IReadFile* file, SPVRHeader& header) const;
	virtual bool readPVRHeaderV3(io::IReadFile* file, SPVRHeaderV3& header) const;

	IImage* loadData(io::IReadFile* file, SPVRHeader& header) const;
	IImage* loadDataV3(io::IReadFile* file, SPVRHeaderV3& header) const;
};

} // end namespace video
} // end namespace irr

#endif // compiled with PVR loader
#endif
