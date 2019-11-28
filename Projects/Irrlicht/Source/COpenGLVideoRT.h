#ifndef __C_VIDEO_OPENGL_VRT_H_INCLUDED__
#define __C_VIDEO_OPENGL_VRT_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_WINDOWS_API_)
#include <Windows.h>

#include "IVideoDriver.h"
#include "SIrrCreationParameters.h"
#include "irrOS.h"

namespace irr
{
	namespace video
	{
		class COpenGLDriver;

		class COpenGLVideoRT : public IVideoRenderTarget
		{
		protected:
			HWND m_hwnd;
			u32 m_width;
			u32 m_height;

		public:
			core::dimension2du ScreenSize;

			HGLRC	HRC;
			HDC		HDC;

			int		PixelFormat;
		public:
			COpenGLVideoRT(void *hwnd, u32 w, u32 h, const PIXELFORMATDESCRIPTOR& pixelFormat);

			virtual ~COpenGLVideoRT();

			virtual u32 getWidth();

			virtual u32 getHeight();

			virtual void resize(u32 w, u32 h);
		};
	}
}

#endif

#endif