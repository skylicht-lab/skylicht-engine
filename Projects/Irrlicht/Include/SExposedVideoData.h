// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_EXPOSED_VIDEO_DATA_H_INCLUDED__
#define __S_EXPOSED_VIDEO_DATA_H_INCLUDED__

// forward declarations for internal pointers
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3D8;
struct IDirect3DDevice8;
struct ID3D11Device;
struct ID3D10Device;
struct IDXGISwapChain;

namespace irr
{
namespace video
{

//! structure for holding data describing a driver and operating system specific data.
/** This data can be retrived by IVideoDriver::getExposedVideoData(). Use this with caution.
This only should be used to make it possible to extend the engine easily without
modification of its source. Note that this structure does not contain any valid data, if
you are using the software or the null device.
*/
struct SExposedVideoData
{
	SExposedVideoData() {OpenGLWin32.HDc=0; OpenGLWin32.HRc=0; OpenGLWin32.HWnd=0;}
	explicit SExposedVideoData(void* Window) {OpenGLWin32.HDc=0; OpenGLWin32.HRc=0; OpenGLWin32.HWnd=Window;}

	struct SOpenGLWin32
	{
		//! Private GDI Device Context.
		/** Get if for example with: HDC h = reinterpret_cast<HDC>(exposedData.OpenGLWin32.HDc) */
		void* HDc;

		//! Permanent Rendering Context.
		/** Get if for example with: HGLRC h = reinterpret_cast<HGLRC>(exposedData.OpenGLWin32.HRc) */
		void* HRc;

		//! Window handle.
		/** Get with for example with: HWND h = reinterpret_cast<HWND>(exposedData.OpenGLWin32.HWnd) */
		void* HWnd;
	};

	struct SOpenGLLinux
	{
		// XWindow handles
		void* X11Display;
		void* X11Context;
		unsigned long X11Window;
	};

	union
	{
		SOpenGLWin32 OpenGLWin32;
		SOpenGLLinux OpenGLLinux;

		struct
		{
			//! Pointer to the ID3D11Device interface
			ID3D11Device* D3DDev11;

			//! Pointer to the IDirect3D9 interface
			IDXGISwapChain* SwapChain;

			//! Window handle.
			/** Get with for example HWND h = reinterpret_cast<HWND>(exposedData.D3D9.HWnd) */
			void* HWnd;
		} D3D11;
	};
};

} // end namespace video
} // end namespace irr


#endif

