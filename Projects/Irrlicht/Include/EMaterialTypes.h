// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_MATERIAL_TYPES_H_INCLUDED__
#define __E_MATERIAL_TYPES_H_INCLUDED__

namespace irr
{
namespace video
{

	//! Abstracted and easy to use fixed function/programmable pipeline material modes.
	enum E_MATERIAL_TYPE
	{
		//! Standard solid material.
		/** Only first texture is used, which is supposed to be the
		diffuse material. */
		EMT_SOLID = 0,

		//! A transparent material.
		/** Only the first texture is used. The new color is calculated
		by simply adding the source color and the dest color. This
		means if for example a billboard using a texture with black
		background and a red circle on it is drawn with this material,
		the result is that only the red circle will be drawn a little
		bit transparent, and everything which was black is 100%
		transparent and not visible. This material type is useful for
		particle effects. */
		EMT_TRANSPARENT_ADD_COLOR,

		//! Makes the material transparent based on the texture alpha channel.
		/** The final color is blended together from the destination
		color and the texture color, using the alpha channel value as
		blend factor. Only first texture is used. If you are using
		this material with small textures, it is a good idea to load
		the texture in 32 bit mode
		(video::IVideoDriver::setTextureCreationFlag()). Also, an alpha
		ref is used, which can be manipulated using
		SMaterial::MaterialTypeParam. This value controls how sharp the
		edges become when going from a transparent to a solid spot on
		the texture. */
		EMT_TRANSPARENT_ALPHA_CHANNEL,

		//! Makes the material transparent based on the texture alpha channel.
		/** If the alpha channel value is greater than 127, a
		pixel is written to the target, otherwise not. This
		material does not use alpha blending and is a lot faster
		than EMT_TRANSPARENT_ALPHA_CHANNEL. It is ideal for drawing
		stuff like leafes of plants, because the borders are not
		blurry but sharp. Only first texture is used. If you are
		using this material with small textures and 3d object, it
		is a good idea to load the texture in 32 bit mode
		(video::IVideoDriver::setTextureCreationFlag()). */
		EMT_TRANSPARENT_ALPHA_CHANNEL_REF,

		//! Makes the material transparent based on the vertex alpha value.
		EMT_TRANSPARENT_VERTEX_ALPHA,

		//! This value is not used. It only forces this enumeration to compile to 32 bit.
		EMT_FORCE_32BIT = 0x7fffffff
	};

	//! Array holding the built in material type names
	const char* const sBuiltInMaterialTypeNames[] =
	{
		"solid",		
		"trans_add",
		"trans_alphach",
		"trans_alphach_ref",
		"trans_vertex_alpha",		
		0
	};

} // end namespace video
} // end namespace irr


#endif // __E_MATERIAL_TYPES_H_INCLUDED__

