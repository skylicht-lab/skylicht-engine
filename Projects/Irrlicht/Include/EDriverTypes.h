// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DRIVER_TYPES_H_INCLUDED__
#define __E_DRIVER_TYPES_H_INCLUDED__

namespace irr
{
namespace video
{

	//! An enum for all types of drivers the Irrlicht Engine supports.
	enum E_DRIVER_TYPE
	{
		//! Null driver, useful for applications to run the engine without visualisation.
		/** The null device is able to load textures, but does not
		render and display any graphics. */
		EDT_NULL,

		//! OpenGL device, available on most platforms.
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		EDT_OPENGL,

		EDT_OPENGLES,

		//!Direct3D 11 device, only available on Win32 platforms from vista and higher
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		EDT_DIRECT3D11,

		//! No driver, just for counting the elements
		EDT_COUNT
	};

} // end namespace video
} // end namespace irr


#endif

