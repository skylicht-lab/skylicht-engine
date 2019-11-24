// Copyright (C) 2002-2012 Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "pch.h"

#include "CImageLoaderDDS.h"

#ifdef _IRR_COMPILE_WITH_DDS_LOADER_

#include "IReadFile.h"
#include "irrOS.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "irrString.h"


#define DDS_MAGIC         0x20534444 

//  S_SURFACE_FORMAT_header.Flags 
#define DDSD_CAPS         0x00000001 
#define DDSD_HEIGHT         0x00000002 
#define DDSD_WIDTH         0x00000004 
#define DDSD_PITCH         0x00000008 
#define DDSD_PIXELFORMAT   0x00001000 
#define DDSD_MIPMAPCOUNT   0x00020000 
#define DDSD_LINEARSIZE      0x00080000 
#define DDSD_DEPTH         0x00800000 

//  S_SURFACE_FORMAT_HEADER.S_PIXEL_FORMAT.Flags 
#define DDPF_ALPHAPIXELS   0x00000001 
#define DDPF_ALPHA         0x00000002 
#define DDPF_FOURCC         0x00000004 
#define DDPF_RGB         0x00000040 
#define DDPF_COMPRESSED      0x00000080 
#define DDPF_LUMINANCE      0x00020000 

//  S_SURFACE_FORMAT_HEADER.Caps.caps1 
#define DDSCAPS1_COMPLEX   0x00000008 
#define DDSCAPS1_TEXTURE   0x00001000 
#define DDSCAPS1_MIPMAP      0x00400000 

//  S_SURFACE_FORMAT_HEADER.Caps.caps2 
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#ifndef MAKEFOURCC 
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
   ((s32)(u8)(ch0) | ((s32)(u8)(ch1) << 8) | \
   ((s32)(u8)(ch2) << 16) | ((s32)(u8)(ch3) << 24 ))
#endif

#define D3DFMT_R8G8B8               20 
#define D3DFMT_A8R8G8B8             21 
#define D3DFMT_X8R8G8B8             22 
#define D3DFMT_R5G6B5               23 
#define D3DFMT_A1R5G5B5             25 
#define D3DFMT_A4R4G4B4             26 
#define D3DFMT_A8B8G8R8             32 
#define D3DFMT_X8B8G8R8             33 
#define D3DFMT_DXT1                 MAKEFOURCC('D', 'X', 'T', '1') 
#define D3DFMT_DXT2                 MAKEFOURCC('D', 'X', 'T', '2') 
#define D3DFMT_DXT3                 MAKEFOURCC('D', 'X', 'T', '3') 
#define D3DFMT_DXT4                 MAKEFOURCC('D', 'X', 'T', '4') 
#define D3DFMT_DXT5                 MAKEFOURCC('D', 'X', 'T', '5') 

namespace irr
{

namespace video
{


struct SDDSPixelFormat 
{ 
    u32	Size; 
    u32 Flags; 
    u32 FourCC; 
    u32 RGBBitCount; 
    u32 RedBitMask; 
    u32 GreenBitMask; 
    u32 BlueBitMask; 
    u32 RGBAlphaMask; 
}; 

struct SDDSCaps2 
{ 
    u32   Caps1; 
    u32   Caps2; 
    u32   Reserved[2]; 
}; 

struct SDDSSurfaceFormatHeader
{ 
    u32			Size; 
    u32			Flags; 
    u32			Height; 
    u32			Width; 
    u32			Pitch; 
    u32			Depth; 
    u32			MipMapCount; 
    u32			Reserved1[11]; 
    SDDSPixelFormat	PixelFormat; 
    SDDSCaps2	Caps; 
    u32			Reserved2; 
}; 


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderDDS::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "dds" );
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderDDS::isALoadableFileFormat(io::IReadFile* file) const
{
	if(!file) 
		return false; 

	char magicWord[4]; 
	file->read( &magicWord, 4 ); 
	return (magicWord[0] == 'D' && magicWord[1] == 'D' && magicWord[2] == 'S' ); 
}

u32 CImageLoaderDDS::getImageSizeInBytes(core::dimension2d<s32>& dimension, u32 blockSize, u32 mipMapCount) const
{
	u32 size = 0;
	int nWidth = dimension.Width;
	int nHeight = dimension.Height;

	u32 mipMap = mipMapCount;

	if (mipMap == 0)
	{
		mipMap = 1;
	}

	for( u32 i = 0; i < mipMap && nWidth && nHeight; ++i )
	{
		size += ((nWidth+3)/4) * ((nHeight+3)/4) * blockSize;

		// Half the image size for the next mip-map level...
		nWidth  = (nWidth  / 2);
		nHeight = (nHeight / 2);	
	}

	return size;
}

//! creates a surface from the file
IImage* CImageLoaderDDS::loadImage(io::IReadFile* file) const
{
	IImage* image = NULL; 
	SDDSSurfaceFormatHeader	header; 

	file->seek( 4 ); 
	file->read( &header, sizeof(SDDSSurfaceFormatHeader) ); 

#ifdef __BIG_ENDIAN__
	  header.Size = os::Byteswap::byteswap(header.Size);
	  header.Flags = os::Byteswap::byteswap(header.Flags);
	  header.Height = os::Byteswap::byteswap(header.Height);
	  header.Width = os::Byteswap::byteswap(header.Width);
	  header.Pitch = os::Byteswap::byteswap(header.Pitch);
	  header.Depth = os::Byteswap::byteswap(header.Depth);
	  header.MipMapCount = os::Byteswap::byteswap(header.MipMapCount);

	  header.PixelFormat.Size = os::Byteswap::byteswap(header.PixelFormat.Size);
	  header.PixelFormat.Flags = os::Byteswap::byteswap(header.PixelFormat.Flags);
	  header.PixelFormat.FourCC = os::Byteswap::byteswap(header.PixelFormat.FourCC);
	  header.PixelFormat.RGBBitCount = os::Byteswap::byteswap(header.PixelFormat.RGBBitCount);
	  header.PixelFormat.RedBitMask = os::Byteswap::byteswap(header.PixelFormat.RedBitMask);
	  header.PixelFormat.GreenBitMask = os::Byteswap::byteswap(header.PixelFormat.GreenBitMask);
	  header.PixelFormat.BlueBitMask = os::Byteswap::byteswap(header.PixelFormat.BlueBitMask);
	  header.PixelFormat.RGBAlphaMask = os::Byteswap::byteswap(header.PixelFormat.RGBAlphaMask);

	  header.Caps.Caps1 = os::Byteswap::byteswap(header.Caps.Caps1);
	  header.Caps.Caps2 = os::Byteswap::byteswap(header.Caps.Caps2);
	  header.Caps.Reserved[0] = os::Byteswap::byteswap(header.Caps.Reserved[0]);
	  header.Caps.Reserved[1] = os::Byteswap::byteswap(header.Caps.Reserved[1]);

	  header.Reserved1[0] = os::Byteswap::byteswap(header.Reserved1[0]);
	  header.Reserved1[1] = os::Byteswap::byteswap(header.Reserved1[1]);
	  header.Reserved1[2] = os::Byteswap::byteswap(header.Reserved1[2]);
	  header.Reserved1[3] = os::Byteswap::byteswap(header.Reserved1[3]);
	  header.Reserved1[4] = os::Byteswap::byteswap(header.Reserved1[4]);
	  header.Reserved1[5] = os::Byteswap::byteswap(header.Reserved1[5]);
	  header.Reserved1[6] = os::Byteswap::byteswap(header.Reserved1[6]);
	  header.Reserved1[7] = os::Byteswap::byteswap(header.Reserved1[7]);
	  header.Reserved1[8] = os::Byteswap::byteswap(header.Reserved1[8]);
	  header.Reserved1[9] = os::Byteswap::byteswap(header.Reserved1[9]);
	  header.Reserved1[10] = os::Byteswap::byteswap(header.Reserved1[10]);
	  header.Reserved2 = os::Byteswap::byteswap(header.Reserved2);
#endif

      if( header.Size == 124 && ( header.Flags & DDSD_PIXELFORMAT ) && ( header.Flags & DDSD_CAPS ) ) 
      { 

         // determine if texture is 3d( has depth ) 
         bool is3D = header.Depth > 0 && ( header.Flags & DDSD_DEPTH ); 

         if( !is3D ) 
		 {
            header.Depth = 1; 
		 }
		 else
		 {
			os::Printer::log( "UNSUPORTED DDS FORMAT TEXTURE", ELL_ERROR );
			return image; 
		 }

         // determine if texture has alpha 
         bool usingAlpha = header.PixelFormat.Flags & DDPF_ALPHAPIXELS; 

         // color format to create image 
         ECOLOR_FORMAT format = ECF_UNKNOWN; 

         u32 dataSize = 0;

		 if( header.PixelFormat.Flags & DDPF_FOURCC )
         { 
		    core::dimension2d<s32> dimension(header.Width, header.Height);

            switch( header.PixelFormat.FourCC ) 
            { 
               case D3DFMT_DXT1: 
               {
				  dataSize = getImageSizeInBytes(dimension, 8, header.MipMapCount);
				  format = ECF_DXT1; 
                  break; 
               } 
               case D3DFMT_DXT2: 
               case D3DFMT_DXT3: 
               { 
				  dataSize = getImageSizeInBytes(dimension, 16, header.MipMapCount);
				  format = ECF_DXT3; 
                  break; 
               } 
               case D3DFMT_DXT4: 
               case D3DFMT_DXT5: 
               { 
				  dataSize = getImageSizeInBytes(dimension, 16, header.MipMapCount);
				  format = ECF_DXT5; 
                  break; 
               } 
            } 

            if( format != ECF_UNKNOWN ) 
            { 
               u8* data = new u8[ dataSize ]; 
			   irr::u32 bytesRead = file->read( data, dataSize ); 

			   // compress image
			   image = new CImage(format, core::dimension2d<u32>(header.Width, header.Height), data, true, true, true, header.MipMapCount > 1);
            } 
         }
         else 
         { 
            os::Printer::log( "UNKNOWN DDS FORMAT TEXTURE", ELL_ERROR ); 
         } 
      } 

      return image; 
}


//! creates a loader which is able to load dds images
IImageLoader* createImageLoaderDDS()
{
	return new CImageLoaderDDS();
}


} // end namespace video
} // end namespace irr

#endif

