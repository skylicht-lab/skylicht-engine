// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__

#include "IImage.h"
#include "rect.h"

namespace irr
{
namespace video
{

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class CImage : public IImage
{
public:

	//! constructor from raw image data
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size,
		void* data, bool ownForeignMemory=true, bool deleteMemory = true,
		bool compressed = false, bool mipMaps = false);

	//! constructor for empty image
	CImage(ECOLOR_FORMAT format, const core::dimension2d<u32>& size);

	//! destructor
	virtual ~CImage();

	//! Lock function.
	virtual void* lock() _IRR_OVERRIDE_
	{
		return Data;
	}

	//! Unlock function.
	virtual void unlock() _IRR_OVERRIDE_ {}

	//! Returns width and height of image data.
	virtual const core::dimension2d<u32>& getDimension() const _IRR_OVERRIDE_;

	//! Returns bits per pixel.
	virtual u32 getBitsPerPixel() const _IRR_OVERRIDE_;

	//! Returns bytes per pixel
	virtual u32 getBytesPerPixel() const _IRR_OVERRIDE_;

	//! Returns image data size in bytes
	virtual u32 getImageDataSizeInBytes() const _IRR_OVERRIDE_;

	//! Returns image data size in pixels
	virtual u32 getImageDataSizeInPixels() const _IRR_OVERRIDE_;

	//! returns mask for red value of a pixel
	virtual u32 getRedMask() const _IRR_OVERRIDE_;

	//! returns mask for green value of a pixel
	virtual u32 getGreenMask() const _IRR_OVERRIDE_;

	//! returns mask for blue value of a pixel
	virtual u32 getBlueMask() const _IRR_OVERRIDE_;

	//! returns mask for alpha value of a pixel
	virtual u32 getAlphaMask() const _IRR_OVERRIDE_;

	//! returns a pixel
	virtual SColor getPixel(u32 x, u32 y) const _IRR_OVERRIDE_;

	//! sets a pixel
	virtual void setPixel(u32 x, u32 y, const SColor &color, bool blend = false ) _IRR_OVERRIDE_;

	//! returns the color format
	virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

	//! returns pitch of image
	virtual u32 getPitch() const _IRR_OVERRIDE_ { return Pitch; }

    //! Pham Hong Duc add
    // swap Blue and G channel
    virtual void swapBG() _IRR_OVERRIDE_;
    
	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(void* target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch=0, bool swapBG=false) _IRR_OVERRIDE_;

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(IImage* target) _IRR_OVERRIDE_;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos=core::position2d<s32>(0,0)) _IRR_OVERRIDE_;

	//! copies this surface into another
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0) _IRR_OVERRIDE_;

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	virtual void copyToWithAlpha(IImage* target, const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect, const SColor &color,
			const core::rect<s32>* clipRect = 0) _IRR_OVERRIDE_;

	//! copies this surface into another, scaling it to fit, appyling a box filter
	virtual void copyToScalingBoxFilter(IImage* target, s32 bias = 0, bool blend = false) _IRR_OVERRIDE_;

	//! fills the surface with given color
	virtual void fill(const SColor &color) _IRR_OVERRIDE_;

	//! Inform whether the image is compressed
	virtual bool isCompressed() const _IRR_OVERRIDE_;

	//! Check whether the image has MipMaps
	/** \return True if image has MipMaps, else false. */
	virtual bool hasMipMaps() const _IRR_OVERRIDE_;

private:

	//! assumes format and size has been set and creates the rest
	void initData();

	inline SColor getPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias ) const;

	u8* Data;
	core::dimension2d<u32> Size;
	u32 BytesPerPixel;
	u32 Pitch;
	ECOLOR_FORMAT Format;

	bool IsCompressed;
	bool HasMipMaps;

	bool DeleteMemory;
};

} // end namespace video
} // end namespace irr


#endif

