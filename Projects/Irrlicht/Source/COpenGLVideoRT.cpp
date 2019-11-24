#include "pch.h"
#include "COpenGLVideoRT.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_WINDOWS_API_)

#include "COpenGLDriver.h"

namespace irr
{
	namespace video
	{
		COpenGLVideoRT::COpenGLVideoRT(void *hwnd, u32 w, u32 h, const PIXELFORMATDESCRIPTOR& pixelFormat)
		{
			m_hwnd = reinterpret_cast<HWND>(hwnd);			
			HDC = GetDC(m_hwnd);
			HRC = NULL;

			PixelFormat = ChoosePixelFormat(HDC, &pixelFormat);
			if (!PixelFormat)
			{
				os::Printer::log("Cannot choose pixel format for VRT", ELL_ERROR);
				return;
			}

			SetPixelFormat(HDC, PixelFormat, &pixelFormat);
			HRC = wglCreateContext(HDC);
			if (!HRC)
			{
				os::Printer::log("Cannot create a GL rendering context for VRT", ELL_ERROR);
				return;
			}
		}

		COpenGLVideoRT::~COpenGLVideoRT()
		{
			if (HRC)
				wglDeleteContext(HRC);
		}

		u32 COpenGLVideoRT::getWidth()
		{
			return m_width;
		}

		u32 COpenGLVideoRT::getHeight()
		{
			return m_height;
		}

		void COpenGLVideoRT::resize(u32 w, u32 h)
		{
			os::Printer::log("COpenGLVideoRT resize.", ELL_INFORMATION);

			m_width = w;
			m_height = h;

			ScreenSize = core::dimension2du(w, h);			
		}
	}
}
#endif