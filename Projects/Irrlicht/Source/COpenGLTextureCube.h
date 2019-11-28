// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Pham Hong Duc add to Skylicht Engine

#ifndef __C_OPEN_GL_TEXTURECUBE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURECUBE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"
#include "irrArray.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
#define GL_GLEXT_LEGACY 1
#else
#define GL_GLEXT_PROTOTYPES 1
#endif
#ifdef _IRR_WINDOWS_API_
// include windows headers for HWND
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#elif defined(_IRR_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
#define NO_SDL_GLEXT
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#else
#if defined(_IRR_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif


namespace irr
{
namespace video
{

class COpenGLDriver;

class COpenGLTextureCube : public ITexture
{
private:
	ECOLOR_FORMAT ColorFormat;
	s32 Pitch;

	core::array<IImage*> Image;
public:
	
	//! rendertarget constructor
	COpenGLTextureCube(COpenGLDriver* driver, const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! static cube map
	COpenGLTextureCube(COpenGLDriver* driver, const io::path& name,
		IImage* posXImage, IImage* negXImage, 
		IImage* posYImage, IImage* negYImage, 
		IImage* posZImage, IImage* negZImage);

	void uploadTexture(bool newTexture=false, u32 imageNumber=0, bool regMipmap = false);

	//! destructor
	virtual ~COpenGLTextureCube();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0);
	virtual void* lock(bool readOnly , u32 mipmapLevel=0, u32 arraySlice = 0);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture.
	virtual const core::dimension2d<u32>& getOriginalSize() const;

	//! Returns (=size) of the texture.
	virtual const core::dimension2d<u32>& getSize() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;

	virtual u32 getPitch() const;

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels(void* mipmapData=0);

	//! returns if it is a render target
	virtual bool isRenderTarget() const;

public:
	
	//! Bind RenderTargetTexture
	virtual void bindRTT(int face);

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	const GLuint getOpenGLTextureName() const
	{
		return TextureName;
	}

	GLint getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
		GLint& filtering,
		GLenum& colorformat,
		GLenum& type);

private:
	COpenGLDriver	*Driver;

	GLuint ColorFrameBuffer;

	GLuint TextureName;
	GLuint DepthTexture;

	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
};

}
}


#endif

#endif