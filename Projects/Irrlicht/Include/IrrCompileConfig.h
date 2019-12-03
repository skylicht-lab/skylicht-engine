// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 8
#define IRRLICHT_VERSION_REVISION 0

// This flag will be defined only in SVN, the official release code will have
// it undefined
//#define IRRLICHT_VERSION_SVN -alpha
#define IRRLICHT_SDK_VERSION "1.8.0"

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

//! The defines for different operating system are:
//! _IRR_XBOX_PLATFORM_ for XBox
//! _IRR_WINDOWS_ for all irrlicht supported Windows versions
//! _IRR_WINDOWS_API_ for Windows or XBox
//! _IRR_OSX_PLATFORM_ for Apple systems running OSX
//! _IRR_POSIX_API_ for Posix compatible systems
//! Note: PLATFORM defines the OS specific layer, API can group several platforms

//! DEVICE is the windowing system used, several PLATFORMs support more than one DEVICE
//! Irrlicht can be compiled with more than one device
//! _IRR_COMPILE_WITH_WINDOWS_DEVICE_ for Windows API based device
//! _IRR_COMPILE_WITH_WINDOWS_CE_DEVICE_ for Windows CE API based device
//! _IRR_COMPILE_WITH_OSX_DEVICE_ for Cocoa native windowing on OSX
//! _IRR_COMPILE_WITH_X11_DEVICE_ for Linux X11 based device
//! _IRR_COMPILE_WITH_SDL_DEVICE_ for platform independent SDL framework
//! _IRR_COMPILE_WITH_CONSOLE_DEVICE_ for no windowing system, used as a fallback
//! _IRR_COMPILE_WITH_FB_DEVICE_ for framebuffer systems

//! Passing defines to the compiler which have NO in front of the _IRR definename is an alternative
//! way which can be used to disable defines (instead of outcommenting them in this header).
//! So defines can be controlled from Makefiles or Projectfiles which allows building
//! different library versions without having to change the sources.
//! Example: NO_IRR_COMPILE_WITH_X11_ would disable X11

//! Comment this line to compile without the fallback console device.
#define _IRR_COMPILE_WITH_CONSOLE_DEVICE_
#ifdef NO_IRR_COMPILE_WITH_CONSOLE_DEVICE_
#undef _IRR_COMPILE_WITH_CONSOLE_DEVICE_
#endif

//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(CYGWIN)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_

#ifdef WINDOWS_STORE
	#define _IRR_WINDOW_UNIVERSAL_PLATFORM_	
	#define _IRR_COMPILE_WITH_PHONE_DEVICE_
	#define NO_IRR_COMPILE_WITH_OGLES3_
	#define NO_IRR_COMPILE_WITH_OPENGL_
#elif SDL
	#define _IRR_COMPILE_WITH_SDL_DEVICE_
#else
	#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif

#endif

#ifdef ANDROID
	#define _IRR_ANDROID_PLATFORM_
	#define _IRR_COMPILE_WITH_OGLES3_
	#define _IRR_COMPILE_WITH_PHONE_DEVICE_
#endif

#ifdef LINUX
	#define _IRR_LINUX_PLATFORM
	#ifdef SDL
		#define _IRR_COMPILE_WITH_SDL_DEVICE_
	#else
		#define _IRR_COMPILE_WITH_LINUX_DEVICE_
	#endif
#endif

#ifdef EMSCRIPTEN
	#define _IRR_WEBASM_PLATFORM_
	#define _IRR_COMPILE_WITH_OGLES3_
	#define _IRR_COMPILE_WITH_SDL_DEVICE_
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#  error "Only Microsoft Visual Studio 7.0 and later are supported."
#endif

// XBox only suppots the native Window stuff
#if defined(_XBOX)
	#undef _IRR_WINDOWS_
	#define _IRR_XBOX_PLATFORM_
	#define _IRR_WINDOWS_API_
	//#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	#undef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	//#define _IRR_COMPILE_WITH_SDL_DEVICE_

	#include <xtl.h>
#endif

#if defined(__APPLE__)
#if defined(IOS)
    // Apple IOS
    #define _IRR_IOS_PLATFORM_
	#define _IRR_COMPILE_WITH_OGLES3_
	#define _IRR_COMPILE_WITH_PHONE_DEVICE_
#else
    // Apple MAC OCX
    #if !defined(MACOSX)
        #define MACOSX // legacy support
    #endif
    #define _IRR_OSX_PLATFORM_
    #define _IRR_COMPILE_WITH_OSX_DEVICE_
#endif
#endif


//! Define _IRR_COMPILE_WITH_JOYSTICK_SUPPORT_ if you want joystick events.
#define _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#ifdef NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif


//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 8

//! Whether to support XML and XML-based formats (irrmesh, collada...)
#define _IRR_COMPILE_WITH_XML_
#ifdef NO_IRR_COMPILE_WITH_XML_
#undef _IRR_COMPILE_WITH_XML_
#endif

//! Add a leak-hunter to Irrlicht which helps finding unreleased reference counted objects.
//! NOTE: This is slow and should only be used for debugging
//#define _IRR_COMPILE_WITH_LEAK_HUNTER_
#ifdef NO_IRR_COMPILE_WITH_LEAK_HUNTER_
#undef _IRR_COMPILE_WITH_LEAK_HUNTER_
#endif

#if defined(_IRR_WINDOWS_API_) && (!defined(__GNUC__) || defined(IRR_COMPILE_WITH_DX11_DEV_PACK))

//! Define _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ if you want to use DirectInput for joystick handling.
/** This only applies to Windows devices, currently only supported under Win32 device.
If not defined, Windows Multimedia library is used, which offers also broad support for joystick devices. */
#define _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#ifdef NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#undef _IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_
#endif

//! Do not compile DX11 on MingW
#ifndef __MINGW32__

//! Only define _IRR_COMPILE_WITH_DIRECT3D_11_ if you have an appropriate DXSDK
//! Download: http://msdn.microsoft.com/en-us/windows/desktop/hh852363.aspx
#define _IRR_COMPILE_WITH_DIRECT3D_11_

#endif

#endif

//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _IRR_COMPILE_WITH_OPENGL_
//#define _IRR_COMPILE_WITH_OGLES3_

#ifdef NO_IRR_COMPILE_WITH_OPENGL_
#undef _IRR_COMPILE_WITH_OPENGL_
#endif

#if defined(_IRR_COMPILE_WITH_OPENGL_)
	#if !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_WEBASM_PLATFORM_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
	#endif
#endif

#ifdef NO_IRR_COMPILE_WITH_OGLES3_
#undef _IRR_COMPILE_WITH_OGLES3_
#endif

#if defined(_IRR_COMPILE_WITH_OGLES3_)
	#undef _IRR_COMPILE_WITH_OPENGL_
	#define _IRR_COMPILE_WITH_PVR_LOADER_
#endif

//! Define _IRR_WCHAR_FILESYSTEM to enable unicode filesystem support for the engine.
/** This enables the engine to read/write from unicode filesystem. If you
disable this feature, the engine behave as before (ansi). This is currently only supported
for Windows based systems. You also have to set #define UNICODE for this to compile.
*/
//#define _IRR_WCHAR_FILESYSTEM
#ifdef NO_IRR_WCHAR_FILESYSTEM
#undef _IRR_WCHAR_FILESYSTEM
#endif

//! Define _IRR_COMPILE_WITH_JPEGLIB_ to enable compiling the engine using libjpeg.
/** This enables the engine to read jpeg images. If you comment this out,
the engine will no longer read .jpeg images. */
#define _IRR_COMPILE_WITH_LIBJPEG_
#ifdef NO_IRR_COMPILE_WITH_LIBJPEG_
#undef _IRR_COMPILE_WITH_LIBJPEG_
#endif

//! Define _IRR_USE_NON_SYSTEM_JPEG_LIB_ to let irrlicht use the jpeglib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the jpeg lib installed in the system.
	This is only used when _IRR_COMPILE_WITH_LIBJPEG_ is defined. */
#define _IRR_USE_NON_SYSTEM_JPEG_LIB_
#ifdef NO_IRR_USE_NON_SYSTEM_JPEG_LIB_
#undef _IRR_USE_NON_SYSTEM_JPEG_LIB_
#endif

//! Define _IRR_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libpng.
/** This enables the engine to read png images. If you comment this out,
the engine will no longer read .png images. */
#define _IRR_COMPILE_WITH_LIBPNG_
#ifdef NO_IRR_COMPILE_WITH_LIBPNG_
#undef _IRR_COMPILE_WITH_LIBPNG_
#endif

//! Define _IRR_USE_NON_SYSTEM_LIBPNG_ to let irrlicht use the libpng which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the libpng installed in the system.
	This is only used when _IRR_COMPILE_WITH_LIBPNG_ is defined. */
#define _IRR_USE_NON_SYSTEM_LIB_PNG_
#ifdef NO_IRR_USE_NON_SYSTEM_LIB_PNG_
#undef _IRR_USE_NON_SYSTEM_LIB_PNG_
#endif

//! Define _IRR_D3D_NO_SHADER_DEBUGGING to disable shader debugging in D3D9
/** If _IRR_D3D_NO_SHADER_DEBUGGING is undefined in IrrCompileConfig.h,
it is possible to debug all D3D9 shaders in VisualStudio. All shaders
(which have been generated in memory or read from archives for example) will be emitted
into a temporary file at runtime for this purpose. To debug your shaders, choose
Debug->Direct3D->StartWithDirect3DDebugging in Visual Studio, and for every shader a
file named 'irr_dbg_shader_%%.vsh' or 'irr_dbg_shader_%%.psh' will be created. Drag'n'drop
the file you want to debug into visual studio. That's it. You can now set breakpoints and
watch registers, variables etc. This works with ASM, HLSL, and both with pixel and vertex shaders.
Note that the engine will run in D3D REF for this, which is a lot slower than HAL. */
#define _IRR_D3D_NO_SHADER_DEBUGGING
#ifdef NO_IRR_D3D_NO_SHADER_DEBUGGING
#undef _IRR_D3D_NO_SHADER_DEBUGGING
#endif

//! Define _IRR_D3D_USE_LEGACY_HLSL_COMPILER to enable the old HLSL compiler in recent DX SDKs
/** This enables support for ps_1_x shaders for recent DX SDKs. Otherwise, support
for this shader model is not available anymore in SDKs after Oct2006. You need to
distribute the OCT2006_d3dx9_31_x86.cab or OCT2006_d3dx9_31_x64.cab though, in order
to provide the user with the proper DLL. That's why it's disabled by default. */
//#define _IRR_D3D_USE_LEGACY_HLSL_COMPILER
#ifdef NO_IRR_D3D_USE_LEGACY_HLSL_COMPILER
#undef _IRR_D3D_USE_LEGACY_HLSL_COMPILER
#endif

//! Define _IRR_COMPILE_WITH_CG_ to enable Cg Shading Language support
//#define _IRR_COMPILE_WITH_CG_
#ifdef NO_IRR_COMPILE_WITH_CG_
#undef _IRR_COMPILE_WITH_CG_
#endif
#if !defined(_IRR_COMPILE_WITH_OPENGL_) && !defined(_IRR_COMPILE_WITH_DIRECT3D_9_)
#undef _IRR_COMPILE_WITH_CG_
#endif

//! Define _IRR_USE_NVIDIA_PERFHUD_ to opt-in to using the nVidia PerHUD tool
/** Enable, by opting-in, to use the nVidia PerfHUD performance analysis driver
tool <http://developer.nvidia.com/object/nvperfhud_home.html>. */
#undef _IRR_USE_NVIDIA_PERFHUD_

//! Define one of the three setting for Burning's Video Software Rasterizer
/** So if we were marketing guys we could say Irrlicht has 4 Software-Rasterizers.
	In a Nutshell:
		All Burnings Rasterizers use 32 Bit Backbuffer, 32Bit Texture & 32 Bit Z or WBuffer,
		16 Bit/32 Bit can be adjusted on a global flag.

		BURNINGVIDEO_RENDERER_BEAUTIFUL
			32 Bit + Vertexcolor + Lighting + Per Pixel Perspective Correct + SubPixel/SubTexel Correct +
			Bilinear Texturefiltering + WBuffer

		BURNINGVIDEO_RENDERER_FAST
			32 Bit + Per Pixel Perspective Correct + SubPixel/SubTexel Correct + WBuffer +
			Bilinear Dithering TextureFiltering + WBuffer

		BURNINGVIDEO_RENDERER_ULTRA_FAST
			16Bit + SubPixel/SubTexel Correct + ZBuffer
*/

//! Uncomment the following line if you want to ignore the deprecated warnings
//#define IGNORE_DEPRECATED_WARNING


//! Define _IRR_COMPILE_WITH_BMP_LOADER_ if you want to load .bmp files
//! Disabling this loader will also disable the built-in font
#define _IRR_COMPILE_WITH_BMP_LOADER_
#ifdef NO_IRR_COMPILE_WITH_BMP_LOADER_
#undef _IRR_COMPILE_WITH_BMP_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_JPG_LOADER_ if you want to load .jpg files
#define _IRR_COMPILE_WITH_JPG_LOADER_
#ifdef NO_IRR_COMPILE_WITH_JPG_LOADER_
#undef _IRR_COMPILE_WITH_JPG_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_PCX_LOADER_ if you want to load .pcx files
#define _IRR_COMPILE_WITH_PCX_LOADER_
#ifdef NO_IRR_COMPILE_WITH_PCX_LOADER_
#undef _IRR_COMPILE_WITH_PCX_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_PNG_LOADER_ if you want to load .png files
#define _IRR_COMPILE_WITH_PNG_LOADER_
#ifdef NO_IRR_COMPILE_WITH_PNG_LOADER_
#undef _IRR_COMPILE_WITH_PNG_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_PPM_LOADER_ if you want to load .ppm/.pgm/.pbm files
#define _IRR_COMPILE_WITH_PPM_LOADER_
#ifdef NO_IRR_COMPILE_WITH_PPM_LOADER_
#undef _IRR_COMPILE_WITH_PPM_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_PSD_LOADER_ if you want to load .psd files
#define _IRR_COMPILE_WITH_PSD_LOADER_
#ifdef NO_IRR_COMPILE_WITH_PSD_LOADER_
#undef _IRR_COMPILE_WITH_PSD_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_DDS_LOADER_ if you want to load .dds files
// Outcommented because
// a) it doesn't compile on 64-bit currently
// b) anyone enabling it should be aware that S3TC compression algorithm which might be used in that loader
// is patented in the US by S3 and they do collect license fees when it's used in applications.
// So if you are unfortunate enough to develop applications for US market and their broken patent system be careful.
#define _IRR_COMPILE_WITH_DDS_LOADER_
#ifdef NO_IRR_COMPILE_WITH_DDS_LOADER_
#undef _IRR_COMPILE_WITH_DDS_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_TGA_LOADER_ if you want to load .tga files
#define _IRR_COMPILE_WITH_TGA_LOADER_
#ifdef NO_IRR_COMPILE_WITH_TGA_LOADER_
#undef _IRR_COMPILE_WITH_TGA_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_WAL_LOADER_ if you want to load .wal files
#define _IRR_COMPILE_WITH_WAL_LOADER_
#ifdef NO_IRR_COMPILE_WITH_WAL_LOADER_
#undef _IRR_COMPILE_WITH_WAL_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_LMP_LOADER_ if you want to load .lmp files
#define _IRR_COMPILE_WITH_LMP_LOADER_
#ifdef NO_IRR_COMPILE_WITH_LMP_LOADER_
#undef _IRR_COMPILE_WITH_LMP_LOADER_
#endif
//! Define _IRR_COMPILE_WITH_RGB_LOADER_ if you want to load Silicon Graphics .rgb/.rgba/.sgi/.int/.inta/.bw files
#define _IRR_COMPILE_WITH_RGB_LOADER_
#ifdef NO_IRR_COMPILE_WITH_RGB_LOADER_
#undef _IRR_COMPILE_WITH_RGB_LOADER_
#endif

//! Define _IRR_COMPILE_WITH_BMP_WRITER_ if you want to write .bmp files
#define _IRR_COMPILE_WITH_BMP_WRITER_
#ifdef NO_IRR_COMPILE_WITH_BMP_WRITER_
#undef _IRR_COMPILE_WITH_BMP_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_JPG_WRITER_ if you want to write .jpg files
#define _IRR_COMPILE_WITH_JPG_WRITER_
#ifdef NO_IRR_COMPILE_WITH_JPG_WRITER_
#undef _IRR_COMPILE_WITH_JPG_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_PCX_WRITER_ if you want to write .pcx files
#define _IRR_COMPILE_WITH_PCX_WRITER_
#ifdef NO_IRR_COMPILE_WITH_PCX_WRITER_
#undef _IRR_COMPILE_WITH_PCX_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_PNG_WRITER_ if you want to write .png files
#define _IRR_COMPILE_WITH_PNG_WRITER_
#ifdef NO_IRR_COMPILE_WITH_PNG_WRITER_
#undef _IRR_COMPILE_WITH_PNG_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_PPM_WRITER_ if you want to write .ppm files
#define _IRR_COMPILE_WITH_PPM_WRITER_
#ifdef NO_IRR_COMPILE_WITH_PPM_WRITER_
#undef _IRR_COMPILE_WITH_PPM_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_PSD_WRITER_ if you want to write .psd files
#define _IRR_COMPILE_WITH_PSD_WRITER_
#ifdef NO_IRR_COMPILE_WITH_PSD_WRITER_
#undef _IRR_COMPILE_WITH_PSD_WRITER_
#endif
//! Define _IRR_COMPILE_WITH_TGA_WRITER_ if you want to write .tga files
#define _IRR_COMPILE_WITH_TGA_WRITER_
#ifdef NO_IRR_COMPILE_WITH_TGA_WRITER_
#undef _IRR_COMPILE_WITH_TGA_WRITER_
#endif

//! Define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_ if you want to open ZIP and GZIP archives
/** ZIP reading has several more options below to configure. */
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#endif
#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
//! Define _IRR_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
/** This enables the engine to read from compressed .zip archives. If you
disable this feature, the engine can still read archives, but only uncompressed
ones. */
#define _IRR_COMPILE_WITH_ZLIB_
#ifdef NO_IRR_COMPILE_WITH_ZLIB_
#undef _IRR_COMPILE_WITH_ZLIB_
#endif
//! Define _IRR_USE_NON_SYSTEM_ZLIB_ to let irrlicht use the zlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the zlib
installed on the system. This is only used when _IRR_COMPILE_WITH_ZLIB_ is
defined. */
#define _IRR_USE_NON_SYSTEM_ZLIB_
#ifdef NO_IRR_USE_NON_SYSTEM_ZLIB_
#undef _IRR_USE_NON_SYSTEM_ZLIB_
#endif
//! Define _IRR_COMPILE_WITH_ZIP_ENCRYPTION_ if you want to read AES-encrypted ZIP archives
#define _IRR_COMPILE_WITH_ZIP_ENCRYPTION_
#ifdef NO_IRR_COMPILE_WITH_ZIP_ENCRYPTION_
#undef _IRR_COMPILE_WITH_ZIP_ENCRYPTION_
#endif
//! Define _IRR_COMPILE_WITH_BZIP2_ if you want to support bzip2 compressed zip archives
/** bzip2 is superior to the original zip file compression modes, but requires
a certain amount of memory for decompression and adds several files to the
library. */
#define _IRR_COMPILE_WITH_BZIP2_
#ifdef NO_IRR_COMPILE_WITH_BZIP2_
#undef _IRR_COMPILE_WITH_BZIP2_
#endif
//! Define _IRR_USE_NON_SYSTEM_BZLIB_ to let irrlicht use the bzlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the bzlib
installed on the system. This is only used when _IRR_COMPILE_WITH_BZLIB_ is
defined. */
#define _IRR_USE_NON_SYSTEM_BZLIB_
#ifdef NO_IRR_USE_NON_SYSTEM_BZLIB_
#undef _IRR_USE_NON_SYSTEM_BZLIB_
#endif
//! Define _IRR_COMPILE_WITH_LZMA_ if you want to use LZMA compressed zip files.
/** LZMA is a very efficient compression code, known from 7zip. Irrlicht
currently only supports zip archives, though. */
#define _IRR_COMPILE_WITH_LZMA_
#ifdef NO_IRR_COMPILE_WITH_LZMA_
#undef _IRR_COMPILE_WITH_LZMA_
#endif
#endif

//! Define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_ if you want to mount folders as archives
//#define __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#endif
//! Define __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_ if you want to open ID software PAK archives
#define __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#endif
//! Define __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_ if you want to open Nebula Device NPK archives
#define __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#endif
//! Define __IRR_COMPILE_WITH_TAR_ARCHIVE_LOADER_ if you want to open TAR archives
#define __IRR_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#endif
//! Define __IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_ if you want to open WAD archives
#define __IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#ifdef NO__IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#undef __IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#endif

//! Set FPU settings
/** Irrlicht should use approximate float and integer fpu techniques
precision will be lower but speed higher. currently X86 only
*/
#if !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_SOLARIS_PLATFORM_)
	//#define IRRLICHT_FAST_MATH
	#ifdef NO_IRRLICHT_FAST_MATH
	#undef IRRLICHT_FAST_MATH
	#endif
#endif

// Some cleanup and standard stuff

#ifdef _IRR_WINDOWS_API_

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _IRR_STATIC_LIB_
#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define IRRLICHT_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_

#if defined(_IRR_XBOX_PLATFORM_)
	#undef _IRR_COMPILE_WITH_OPENGL_
	#undef _IRR_COMPILE_WITH_DIRECT3D_11_
#endif

#ifndef _IRR_WINDOWS_API_
	#undef _IRR_WCHAR_FILESYSTEM
#endif

#if defined(__sparc__) || defined(__sun__)
#define __BIG_ENDIAN__
#endif

#if defined(_IRR_SOLARIS_PLATFORM_)
	#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif

//! Define __IRR_HAS_S64 if the irr::s64 type should be enable (needs long long, available on most platforms, but not part of ISO C++ 98)
#define __IRR_HAS_S64
#ifdef NO__IRR_HAS_S64
#undef __IRR_HAS_S64
#endif

// These depend on XML
#ifndef _IRR_COMPILE_WITH_XML_
	#undef _IRR_COMPILE_WITH_IRR_MESH_LOADER_
	#undef _IRR_COMPILE_WITH_IRR_WRITER_
	#undef _IRR_COMPILE_WITH_COLLADA_WRITER_
	#undef _IRR_COMPILE_WITH_COLLADA_LOADER_
#endif

#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__

