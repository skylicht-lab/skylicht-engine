// Copyright (C) 2002-2012 Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_DDS_H_INCLUDED__
#define __C_IMAGE_LOADER_DDS_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_DDS_LOADER_)

#include "IImageLoader.h"

namespace irr
{
namespace video
{

/*!
	Surface Loader for DDS images
*/
class CImageLoaderDDS : public IImageLoader
{
public:

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(io::IReadFile* file) const;

	//! creates a surface from the file
	virtual IImage* loadImage(io::IReadFile* file) const;
	
	u32 getImageSizeInBytes(core::dimension2d<s32>& dimension, u32 blockSize, u32 mipMapCount) const;

};


} // end namespace video
} // end namespace irr

#endif // compiled with DDS loader
#endif

