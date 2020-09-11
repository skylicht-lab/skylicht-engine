// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "COpenGLDriver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLTexture.h"
#include "COpenGLTextureCube.h"
#include "COpenGLTextureArray.h"
#include "COpenGLSLMaterialRenderer.h"
#include "COpenGLVideoRT.h"
#include "irrOS.h"

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
void SDLSwapBuffers();
#endif

namespace irr
{
namespace video
{
COpenGLVertexAttribute::COpenGLVertexAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 offset, u32 bufferID, u32 layerCount) :
	IVertexAttribute(name, elementCount, semantic, type, offset, bufferID)
{
	Cache.reallocate(layerCount);

	for (u32 i = 0; i < layerCount; ++i)
		Cache.push_back(false);

	Location.reallocate(layerCount);

	for (u32 i = 0; i < layerCount; ++i)
		Location.push_back(-1);
}

void COpenGLVertexAttribute::setOffset(u32 offset)
{
	Offset = offset;
}

void COpenGLVertexAttribute::addLocationLayer()
{
	Cache.push_back(false);
	Location.push_back(-1);
}

s32 COpenGLVertexAttribute::getLocation(u32 materialType) const
{
	s32 location = -1;

	if (materialType < Location.size())
		location = Location[materialType];

	return location;
}

void COpenGLVertexAttribute::setLocation(u32 location, u32 materialType)
{
	if (materialType < Location.size())
	{
		Cache[materialType] = true;
		Location[materialType] = location;
	}
}

s32 COpenGLVertexAttribute::getLocationStatus(u32 materialType) const
{
	s32 status = -1;

	if (materialType < Cache.size())
	{
		if (Cache[materialType])
			status = 1;
		else
			status = 0;
	}

	return status;
}

COpenGLVertexDescriptor::COpenGLVertexDescriptor(const core::stringc& name, u32 id, u32 layerCount) :
	IVertexDescriptor(name, id), LayerCount(layerCount)
{
#ifdef _DEBUG
	setDebugName("COpenGLVertexDescriptor");
#endif
}

void COpenGLVertexDescriptor::setID(u32 id)
{
	ID = id;
}

IVertexAttribute* COpenGLVertexDescriptor::addAttribute(const core::stringc& name, u32 elementCount, E_VERTEX_ATTRIBUTE_SEMANTIC semantic, E_VERTEX_ATTRIBUTE_TYPE type, u32 bufferID)
{
	for (u32 i = 0; i < Attribute.size(); ++i)
		if (name == Attribute[i].getName() || (semantic != EVAS_CUSTOM && semantic == Attribute[i].getSemantic()))
			return NULL;

	if (elementCount < 1)
		elementCount = 1;

	if (elementCount > 4)
		elementCount = 4;

	for (u32 i = VertexSize.size(); i <= bufferID; ++i)
		VertexSize.push_back(0);

	for (u32 i = InstanceDataStepRate.size(); i <= bufferID; ++i)
		InstanceDataStepRate.push_back(EIDSR_PER_VERTEX);

	COpenGLVertexAttribute attribute(name, elementCount, semantic, type, VertexSize[bufferID], bufferID, LayerCount);
	Attribute.push_back(attribute);

	AttributeSemanticIndex[(u32)attribute.getSemantic()] = Attribute.size() - 1;

	VertexSize[bufferID] += attribute.getTypeSize() * attribute.getElementCount();

	// Assign data to the pointers.

	AttributePointer.push_back(0);

	for (u32 i = 0; i < AttributePointer.size(); ++i)
		AttributePointer[i] = &Attribute[i];

	return AttributePointer.getLast();
}

void COpenGLVertexDescriptor::clearAttribute()
{
	AttributePointer.clear();
	VertexSize.clear();

	for (u32 i = 0; i < EVAS_COUNT; ++i)
		AttributeSemanticIndex[i] = -1;

	Attribute.clear();
}

void COpenGLVertexDescriptor::addLocationLayer()
{
	LayerCount++;

	for (u32 i = 0; i < Attribute.size(); ++i)
		Attribute[i].addLocationLayer();
}

COpenGLHardwareBuffer::COpenGLHardwareBuffer(scene::IIndexBuffer* indexBuffer, COpenGLDriver* driver) :
	IHardwareBuffer(scene::EHM_NEVER, 0, 0, EHBT_NONE, EDT_OPENGL), Driver(driver), BufferID(0),
	RemoveFromArray(true), LinkedBuffer(0)
{
#ifdef _DEBUG
	setDebugName("COpenGLHardwareBuffer");
#endif

	Type = EHBT_INDEX;

	if (indexBuffer)
	{
		if (update(indexBuffer->getHardwareMappingHint(), indexBuffer->getIndexSize()*indexBuffer->getIndexCount(), indexBuffer->getIndices()))
		{
			indexBuffer->setHardwareBuffer(this);
			LinkedBuffer = indexBuffer;
		}
	}
}

COpenGLHardwareBuffer::COpenGLHardwareBuffer(scene::IVertexBuffer* vertexBuffer, COpenGLDriver* driver) :
	IHardwareBuffer(scene::EHM_NEVER, 0, 0, EHBT_NONE, EDT_OPENGL), Driver(driver), BufferID(0),
	RemoveFromArray(true), LinkedBuffer(0)
{
#ifdef _DEBUG
	setDebugName("COpenGLHardwareBuffer");
#endif

	Type = EHBT_VERTEX;

	if (vertexBuffer)
	{
		if (update(vertexBuffer->getHardwareMappingHint(), vertexBuffer->getVertexSize()*vertexBuffer->getVertexCount(), vertexBuffer->getVertices()))
		{
			vertexBuffer->setHardwareBuffer(this);
			LinkedBuffer = vertexBuffer;
		}
	}
}

COpenGLHardwareBuffer::~COpenGLHardwareBuffer()
{
	if (RemoveFromArray)
	{
		for (u32 i = 0; i < Driver->HardwareBuffer.size(); ++i)
		{
			if (Driver->HardwareBuffer[i] == this)
			{
				Driver->HardwareBuffer[i] = 0;
				break;
			}
		}
	}

	if (LinkedBuffer)
	{
		switch (Type)
		{
		case EHBT_INDEX:
			((scene::IIndexBuffer*)LinkedBuffer)->setHardwareBuffer(0, true);
			break;
		case EHBT_VERTEX:
			((scene::IVertexBuffer*)LinkedBuffer)->setHardwareBuffer(0, true);
			break;
		default:
			break;
		}
	}

#if defined(GL_ARB_vertex_buffer_object)
	if (BufferID)
		Driver->extGlDeleteBuffers(1, &BufferID);
#endif
}

bool COpenGLHardwareBuffer::update(const scene::E_HARDWARE_MAPPING mapping, const u32 size, const void* data)
{
	u32 oldSize = Size;

	Mapping = mapping;
	Size = size;

	if (Mapping == scene::EHM_NEVER || Size == 0 || !data || !Driver || !Driver->FeatureAvailable[COpenGLDriver::IRR_ARB_vertex_buffer_object])
		return false;

#if defined(GL_ARB_vertex_buffer_object)
	GLenum target = 0;

	switch (Type)
	{
	case EHBT_INDEX:
		target = GL_ELEMENT_ARRAY_BUFFER;
		break;
	case EHBT_VERTEX:
		target = GL_ARRAY_BUFFER;
		break;
	default:
		return false;
	}

	bool createBuffer = false;

	if (!BufferID)
	{
		Driver->extGlGenBuffers(1, &BufferID);

		if (!BufferID)
			return false;

		createBuffer = true;
	}
	else if (oldSize < Size)
		createBuffer = true;

	Driver->extGlBindBuffer(target, BufferID);

	if (!createBuffer)
		Driver->extGlBufferSubData(target, 0, Size, data);
	else
	{
		if (Mapping == scene::EHM_STATIC)
			Driver->extGlBufferData(target, Size, data, GL_STATIC_DRAW);
		else if (Mapping == scene::EHM_DYNAMIC)
			Driver->extGlBufferData(target, Size, data, GL_DYNAMIC_DRAW);
		else // scene::EHM_STREAM
			Driver->extGlBufferData(target, Size, data, GL_STREAM_DRAW);
	}

	Driver->extGlBindBuffer(target, 0);

	RequiredUpdate = false;

	return true;
#else
	return false;
#endif
}

GLuint COpenGLHardwareBuffer::getBufferID() const
{
	return BufferID;
}

void COpenGLHardwareBuffer::removeFromArray(bool status)
{
	RemoveFromArray = status;
}

// -----------------------------------------------------------------------
// SDL CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
//! SDL constructor and init code
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
	io::IFileSystem* io)
	: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(),
	ResetRenderStates(true),
	AntiAlias(params.AntiAlias),
	RenderTargetTexture(0), CurrentRendertargetSize(0, 0),
	ColorFormat(ECF_R8G8B8),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params),
	ActiveGLSLProgram(0), LastVertexDescriptor(0),
	BridgeCalls(0), DeviceType(EIDT_SDL)
{
#ifdef _DEBUG
	setDebugName("COpenGLDriver");
#endif

	for (u32 i = 0; i < 16; ++i)
		VertexAttributeStatus[i] = false;

#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
#endif	
}

bool COpenGLDriver::initDriver()
{
	return genericDriverInit();
}

#endif // _IRR_COMPILE_WITH_SDL_DEVICE_

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const irr::SIrrlichtCreationParameters& params,
		io::IFileSystem* io)
: CNullDriver(io, params.WindowSize), COpenGLExtensionHandler(), BridgeCalls(0),
	ResetRenderStates(true),
	AntiAlias(params.AntiAlias), RenderTargetTexture(0),
	CurrentRendertargetSize(0,0), ColorFormat(ECF_R8G8B8),
	CurrentTarget(ERT_FRAME_BUFFER), Params(params),
	ActiveGLSLProgram(0), LastVertexDescriptor(0),
	HDc(0), Window(static_cast<HWND>(params.WindowId)),
	DeviceType(EIDT_WIN32),
	CurrentVRT(NULL)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	for(u32 i = 0; i < 16; ++i)
		VertexAttributeStatus[i] = false;

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = 0;
	#endif
}


bool COpenGLDriver::changeRenderContext(const SExposedVideoData& videoData)
{
	if (CurrentVRT == NULL)
	{
		if (videoData.OpenGLWin32.HWnd && videoData.OpenGLWin32.HDc && videoData.OpenGLWin32.HRc)
		{
			if (!wglMakeCurrent((HDC)videoData.OpenGLWin32.HDc, (HGLRC)videoData.OpenGLWin32.HRc))
			{
				os::Printer::log("Render Context switch failed.");
				return false;
			}
			else
			{
				HDc = (HDC)videoData.OpenGLWin32.HDc;
			}
		}
	}
	else
	{
		COpenGLVideoRT *glVRT = (COpenGLVideoRT*)CurrentVRT;
		if (glVRT->HDC && glVRT->HRC)
		{
			if (!wglMakeCurrent(glVRT->HDC, glVRT->HRC))
			{
				os::Printer::log("Render Context switch failed.");
				return false;
			}
			else
			{
				HDc = glVRT->HDC;
			}
		}
	}

	return true;
}

//! inits the open gl driver
bool COpenGLDriver::initDriver()
{
	// Create a window to test antialiasing support
	const wchar_t* ClassName = __TEXT("GLCIrrDeviceWin32");
	HINSTANCE lhInstance = GetModuleHandle(0);

	// Register Class
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)DefWindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = lhInstance;
	wcex.hIcon         = NULL;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = 0;
	wcex.lpszClassName = ClassName;
	wcex.hIconSm       = 0;
	wcex.hIcon         = 0;
	RegisterClassEx(&wcex);

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = Params.WindowSize.Width;
	clientSize.bottom = Params.WindowSize.Height;

	DWORD style = WS_POPUP;
	if (!Params.Fullscreen)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	AdjustWindowRect(&clientSize, style, FALSE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	const s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	HWND temporary_wnd=CreateWindow(ClassName, __TEXT(""), style, windowLeft,
			windowTop, realWidth, realHeight, NULL, NULL, lhInstance, NULL);

	if (!temporary_wnd)
	{
		os::Printer::log("Cannot create a temporary window.", ELL_ERROR);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}

	HDc = GetDC(temporary_wnd);

	// Set up pixel format descriptor with desired parameters
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),             // Size Of This Pixel Format Descriptor
		1,                                         // Version Number
		(DWORD)(PFD_DRAW_TO_WINDOW |               // Format Must Support Window
		PFD_SUPPORT_OPENGL |                       // Format Must Support OpenGL
		(Params.Doublebuffer?PFD_DOUBLEBUFFER:0) | // Must Support Double Buffering
		(Params.Stereobuffer?PFD_STEREO:0)),       // Must Support Stereo Buffer
		PFD_TYPE_RGBA,                             // Request An RGBA Format
		Params.Bits,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                          // Color Bits Ignored
		0,                                         // No Alpha Buffer
		0,                                         // Shift Bit Ignored
		0,                                         // No Accumulation Buffer
		0, 0, 0, 0,	                               // Accumulation Bits Ignored
		Params.ZBufferBits,                        // Z-Buffer (Depth Buffer)
		BYTE(Params.Stencilbuffer ? 1 : 0),        // Stencil Buffer Depth
		0,                                         // No Auxiliary Buffer
		PFD_MAIN_PLANE,                            // Main Drawing Layer
		0,                                         // Reserved
		0, 0, 0                                    // Layer Masks Ignored
	};

	GLuint PixelFormat;

	for (u32 i=0; i<6; ++i)
	{
		if (i == 1)
		{
			if (Params.Stencilbuffer)
			{
				os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
				Params.Stencilbuffer = false;
				pfd.cStencilBits = 0;
			}
			else
				continue;
		}
		else
		if (i == 2)
		{
			pfd.cDepthBits = 24;
		}
		else
		if (i == 3)
		{
			if (Params.Bits!=16)
				pfd.cDepthBits = 16;
			else
				continue;
		}
		else
		if (i == 4)
		{
			// try single buffer
			if (Params.Doublebuffer)
				pfd.dwFlags &= ~PFD_DOUBLEBUFFER;
			else
				continue;
		}
		else
		if (i == 5)
		{
			os::Printer::log("Cannot create a GL device context", "No suitable format for temporary window.", ELL_ERROR);
			ReleaseDC(temporary_wnd, HDc);
			DestroyWindow(temporary_wnd);
			UnregisterClass(ClassName, lhInstance);
			return false;
		}

		// choose pixelformat
		PixelFormat = ChoosePixelFormat(HDc, &pfd);
		if (PixelFormat)
			break;
	}

	SetPixelFormat(HDc, PixelFormat, &pfd);
	HGLRC hrc=wglCreateContext(HDc);
	if (!hrc)
	{
		os::Printer::log("Cannot create a temporary GL rendering context.", ELL_ERROR);
		ReleaseDC(temporary_wnd, HDc);
		DestroyWindow(temporary_wnd);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}

	SExposedVideoData data;
	data.OpenGLWin32.HDc = HDc;
	data.OpenGLWin32.HRc = hrc;
	data.OpenGLWin32.HWnd = temporary_wnd;
	
	if (!changeRenderContext(data))
	{
		os::Printer::log("Cannot activate a temporary GL rendering context.", ELL_ERROR);
		wglDeleteContext(hrc);
		ReleaseDC(temporary_wnd, HDc);
		DestroyWindow(temporary_wnd);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}

	core::stringc wglExtensions;
#ifdef WGL_ARB_extensions_string
	PFNWGLGETEXTENSIONSSTRINGARBPROC irrGetExtensionsString = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (irrGetExtensionsString)
		wglExtensions = irrGetExtensionsString(HDc);
#elif defined(WGL_EXT_extensions_string)
	PFNWGLGETEXTENSIONSSTRINGEXTPROC irrGetExtensionsString = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	if (irrGetExtensionsString)
		wglExtensions = irrGetExtensionsString(HDc);
#endif
	const bool pixel_format_supported = (wglExtensions.find("WGL_ARB_pixel_format") != -1);
	const bool multi_sample_supported = ((wglExtensions.find("WGL_ARB_multisample") != -1) ||
		(wglExtensions.find("WGL_EXT_multisample") != -1) || (wglExtensions.find("WGL_3DFX_multisample") != -1) );
#ifdef _DEBUG
	os::Printer::log("WGL_extensions", wglExtensions);
#endif

#ifdef WGL_ARB_pixel_format
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat_ARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (pixel_format_supported && wglChoosePixelFormat_ARB)
	{
		// This value determines the number of samples used for antialiasing
		// My experience is that 8 does not show a big
		// improvement over 4, but 4 shows a big improvement
		// over 2.

		if(AntiAlias > 32)
			AntiAlias = 32;

		f32 fAttributes[] = {0.0, 0.0};
		s32 iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,1,
			WGL_SUPPORT_OPENGL_ARB,1,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,(Params.Bits==32) ? 24 : 15,
			WGL_ALPHA_BITS_ARB,(Params.Bits==32) ? 8 : 1,
			WGL_DEPTH_BITS_ARB,Params.ZBufferBits, // 10,11
			WGL_STENCIL_BITS_ARB,Params.Stencilbuffer ? 1 : 0,
			WGL_DOUBLE_BUFFER_ARB,Params.Doublebuffer ? 1 : 0,
			WGL_STEREO_ARB,Params.Stereobuffer ? 1 : 0,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
#ifdef WGL_ARB_multisample
			WGL_SAMPLES_ARB,AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_ARB, 1,
#elif defined(WGL_EXT_multisample)
			WGL_SAMPLES_EXT,AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_EXT, 1,
#elif defined(WGL_3DFX_multisample)
			WGL_SAMPLES_3DFX,AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_3DFX, 1,
#endif
#ifdef WGL_ARB_framebuffer_sRGB
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, Params.HandleSRGB ? 1:0,
#elif defined(WGL_EXT_framebuffer_sRGB)
			WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT, Params.HandleSRGB ? 1:0,
#endif
//			WGL_DEPTH_FLOAT_EXT, 1,
			0,0,0,0
		};
		int iAttrSize = sizeof(iAttributes)/sizeof(int);
		const bool framebuffer_srgb_supported = ((wglExtensions.find("WGL_ARB_framebuffer_sRGB") != -1) ||
			(wglExtensions.find("WGL_EXT_framebuffer_sRGB") != -1));
		if (!framebuffer_srgb_supported)
		{
			memmove(&iAttributes[24],&iAttributes[26],sizeof(int)*(iAttrSize-26));
			iAttrSize -= 2;
		}
		if (!multi_sample_supported)
		{
			memmove(&iAttributes[20],&iAttributes[24],sizeof(int)*(iAttrSize-24));
			iAttrSize -= 4;
		}

		s32 rv=0;
		// Try to get an acceptable pixel format
		do
		{
			int pixelFormat=0;
			UINT numFormats=0;
			const BOOL valid = wglChoosePixelFormat_ARB(HDc,iAttributes,fAttributes,1,&pixelFormat,&numFormats);

			if (valid && numFormats)
				rv = pixelFormat;
			else
				iAttributes[21] -= 1;
		}
		while(rv==0 && iAttributes[21]>1);
		if (rv)
		{
			PixelFormat=rv;
			AntiAlias=iAttributes[21];
		}
	}
	else
#endif
		AntiAlias=0;

	wglMakeCurrent(HDc, NULL);
	wglDeleteContext(hrc);
	ReleaseDC(temporary_wnd, HDc);
	DestroyWindow(temporary_wnd);
	UnregisterClass(ClassName, lhInstance);

	// get hdc
	HDc=GetDC(Window);
	if (!HDc)
	{
		os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
		return false;
	}

	// search for pixel format the simple way
	if (PixelFormat==0 || (!SetPixelFormat(HDc, PixelFormat, &pfd)))
	{
		for (u32 i=0; i<5; ++i)
		{
			if (i == 1)
			{
				if (Params.Stencilbuffer)
				{
					os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
					Params.Stencilbuffer = false;
					pfd.cStencilBits = 0;
				}
				else
					continue;
			}
			else
			if (i == 2)
			{
				pfd.cDepthBits = 24;
			}
			if (i == 3)
			{
				if (Params.Bits!=16)
					pfd.cDepthBits = 16;
				else
					continue;
			}
			else
			if (i == 4)
			{
				os::Printer::log("Cannot create a GL device context", "No suitable format.", ELL_ERROR);
				return false;
			}

			// choose pixelformat
			PixelFormat = ChoosePixelFormat(HDc, &pfd);
			if (PixelFormat)
				break;
		}

		// set pixel format
		if (!SetPixelFormat(HDc, PixelFormat, &pfd))
		{
			os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
			return false;
		}
	}

	os::Printer::log("Pixel Format", core::stringc(PixelFormat).c_str(), ELL_DEBUG);

	// create rendering context
#ifdef WGL_ARB_create_context
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs_ARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribs_ARB)
	{
		int iAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			0
		};
		hrc=wglCreateContextAttribs_ARB(HDc, 0, iAttribs);
	}
	else
#endif
		hrc=wglCreateContext(HDc);

	if (!hrc)
	{
		os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
		return false;
	}

	// set exposed data
	ExposedData.OpenGLWin32.HDc = HDc;
	ExposedData.OpenGLWin32.HRc = hrc;
	ExposedData.OpenGLWin32.HWnd = Window;

	// activate rendering context
	if (!changeRenderContext(ExposedData))
	{
		os::Printer::log("Cannot activate GL rendering context", ELL_ERROR);
		wglDeleteContext(hrc);
		return false;
	}

	int pf = GetPixelFormat(HDc);
	DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.cAlphaBits != 0)
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_A8R8G8B8;
		else
			ColorFormat = ECF_A1R5G5B5;
	}
	else
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_R8G8B8;
		else
			ColorFormat = ECF_R5G6B5;
	}

	PixelFormatInfo = pfd;

	genericDriverInit();

	extGlSwapInterval(Params.Vsync ? 1 : 0);
	return true;
}

IVideoRenderTarget* COpenGLDriver::addVideoRenderTarget(void *hwnd, u32 w, u32 h)
{
#ifdef _IRR_WINDOWS_API_
	IVideoRenderTarget *vrt = new COpenGLVideoRT(hwnd, w, h, PixelFormatInfo);

	addVRT(vrt);
	vrt->drop();
	return vrt;
#endif
}

void COpenGLDriver::removeVideoRenderTarget(IVideoRenderTarget* vrt)
{
#ifdef _IRR_WINDOWS_API_
	removeVRT(vrt);
#endif
}

void COpenGLDriver::setVideoRenderTarget(IVideoRenderTarget* vrt)
{
#ifdef _IRR_WINDOWS_API_
	CurrentVRT = vrt;

	if (vrt == NULL)
	{
		if (!wglMakeCurrent((HDC)ExposedData.OpenGLWin32.HDc, (HGLRC)ExposedData.OpenGLWin32.HRc))
		{
			os::Printer::log("Render Context switch failed.");
		}
		else
		{
			HDc = (HDC)ExposedData.OpenGLWin32.HDc;
		}
	}
	else
	{
		COpenGLVideoRT *glVRT = dynamic_cast<COpenGLVideoRT*>(vrt);
		if (glVRT->HRC)
		{
			if (!wglMakeCurrent(glVRT->HDC, glVRT->HRC))
			{
				os::Printer::log("Render Context switch failed.");
			}
			else
			{
				HDc = glVRT->HDC;
			}
		}
	}

	// don't forget to set viewport
	core::dimension2du size = getCurrentRenderTargetSize();
	setViewPort(core::rect<s32>(0, 0, size.Width, size.Height));
#endif
}

#endif // _IRR_COMPILE_WITH_WINDOWS_DEVICE_

//! destructor
COpenGLDriver::~COpenGLDriver()
{
	#ifdef _IRR_COMPILE_WITH_CG_
	if (CgContext)
		cgDestroyContext(CgContext);
	#endif
	
	deleteMaterialRenders();

	CurrentTexture.clear();
	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.
	deleteAllTextures();
	removeAllOcclusionQueries();
	removeAllHardwareBuffers();

	delete BridgeCalls;

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	if (DeviceType == EIDT_WIN32)
	{

		if (ExposedData.OpenGLWin32.HRc)
		{
			if (!wglMakeCurrent(HDc, 0))
				os::Printer::log("Release of dc and rc failed.", ELL_WARNING);

			if (!wglDeleteContext((HGLRC)ExposedData.OpenGLWin32.HRc))
				os::Printer::log("Release of rendering context failed.", ELL_WARNING);
		}

		if (HDc)
			ReleaseDC(Window, HDc);
	}
#endif
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

bool COpenGLDriver::genericDriverInit()
{
	// init creates vertex descriptors based on the driver for built-in vertex structures.
	createVertexDescriptors();

	// Create driver.

	Name=L"OpenGL ";
	Name.append(glGetString(GL_VERSION));
	s32 pos=Name.findNext(L' ', 7);
	if (pos != -1)
		Name=Name.subString(0, pos);
	printVersion();

	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		os::Printer::log(reinterpret_cast<const c8*>(renderer), reinterpret_cast<const c8*>(vendor), ELL_INFORMATION);
		VendorName = reinterpret_cast<const c8*>(vendor);
	}

	u32 i;
	CurrentTexture.clear();
	// load extensions
	initExtensions(Params.Stencilbuffer);

    if (!BridgeCalls)
        BridgeCalls = new COpenGLCallBridge(this);

	if (queryFeature(EVDF_ARB_GLSL))
	{
		char buf[32];
		const u32 maj = ShaderLanguageVersion/100;
		snprintf(buf, 32, "%u.%u", maj, ShaderLanguageVersion-maj*100);
		os::Printer::log("GLSL version", buf, ELL_INFORMATION);
	}
	else
		os::Printer::log("GLSL not available.", ELL_INFORMATION);
	DriverAttributes->setAttribute("MaxTextures", MaxTextureUnits);
	DriverAttributes->setAttribute("MaxSupportedTextures", MaxSupportedTextures);
	DriverAttributes->setAttribute("MaxLights", MaxLights);
	DriverAttributes->setAttribute("MaxAnisotropy", MaxAnisotropy);
	DriverAttributes->setAttribute("MaxUserClipPlanes", MaxUserClipPlanes);
	DriverAttributes->setAttribute("MaxAuxBuffers", MaxAuxBuffers);
	DriverAttributes->setAttribute("MaxMultipleRenderTargets", MaxMultipleRenderTargets);
	DriverAttributes->setAttribute("MaxIndices", (s32)MaxIndices);
	DriverAttributes->setAttribute("MaxTextureSize", (s32)MaxTextureSize);
	DriverAttributes->setAttribute("MaxGeometryVerticesOut", (s32)MaxGeometryVerticesOut);
	DriverAttributes->setAttribute("MaxTextureLODBias", MaxTextureLODBias);
	DriverAttributes->setAttribute("Version", Version);
	DriverAttributes->setAttribute("ShaderLanguageVersion", ShaderLanguageVersion);
	DriverAttributes->setAttribute("AntiAlias", AntiAlias);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	UserClipPlanes.reallocate(MaxUserClipPlanes);
	for (i=0; i<MaxUserClipPlanes; ++i)
		UserClipPlanes.push_back(SUserClipPlane());

	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));
#ifdef GL_EXT_separate_specular_color
	if (FeatureAvailable[IRR_EXT_separate_specular_color])
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	Params.HandleSRGB &= ((FeatureAvailable[IRR_ARB_framebuffer_sRGB] || FeatureAvailable[IRR_EXT_framebuffer_sRGB]) &&
		FeatureAvailable[IRR_EXT_texture_sRGB]);
#if defined(GL_ARB_framebuffer_sRGB)
	if (Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);
#elif defined(GL_EXT_framebuffer_sRGB)
	if (Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
#endif

// This is a fast replacement for NORMALIZE_NORMALS
//	if ((Version>101) || FeatureAvailable[IRR_EXT_rescale_normal])
//		glEnable(GL_RESCALE_NORMAL_EXT);

	glClearDepth(1.0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glFrontFace(GL_CW);
	// adjust flat coloring scheme to DirectX version
#if defined(GL_ARB_provoking_vertex) || defined(GL_EXT_provoking_vertex)
	extGlProvokingVertex(GL_FIRST_VERTEX_CONVENTION_EXT);
#endif

	// set the renderstates
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0,0), core::vector2df(0,1.0f), core::vector2df(1.0f,-1.0f));

	// We need to reset once more at the beginning of the first rendering.
	// This fixes problems with intermediate changes to the material during texture load.
	ResetRenderStates = true;

	#ifdef _IRR_COMPILE_WITH_CG_
	CgContext = cgCreateContext();
	#endif

	return true;
}

//! presents the rendered scene on the screen, returns false if failed
bool COpenGLDriver::endScene()
{
	CNullDriver::endScene();

	glFlush();

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	if (DeviceType == EIDT_WIN32)
		return SwapBuffers(HDc) == TRUE;
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_	
	if (DeviceType == EIDT_SDL)
	{
		SDLSwapBuffers();
		return true;
	}
#endif

	return false;
}


//! clears the zbuffer and color buffer
void COpenGLDriver::clearBuffers(bool backBuffer, bool zBuffer, bool stencilBuffer, SColor color)
{
	GLbitfield mask = 0;
	if (backBuffer)
	{
		BridgeCalls->setColorMask(true, true, true, true);

		const f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (zBuffer)
	{
		BridgeCalls->setDepthMask(true);
 		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencilBuffer)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
		glClear(mask);
}


//! init call for rendering start
bool COpenGLDriver::beginScene(bool backBuffer, bool zBuffer, SColor color,
		const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

	switch (DeviceType)
	{
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
	case EIDT_WIN32:
		changeRenderContext(videoData);
		break;
#endif

	default:
		break;
	}
	clearBuffers(backBuffer, zBuffer, false, color);
	return true;
}

IHardwareBuffer* COpenGLDriver::createHardwareBuffer(scene::IIndexBuffer* indexBuffer)
{
	if (!indexBuffer)
		return 0;

	COpenGLHardwareBuffer* hardwareBuffer = new COpenGLHardwareBuffer(indexBuffer, this);

	bool extendArray = true;

	for (u32 i = 0; i < HardwareBuffer.size(); ++i)
	{
		if (!HardwareBuffer[i])
		{
			HardwareBuffer[i] = hardwareBuffer;
			extendArray = false;
			break;
		}
	}

	if (extendArray)
		HardwareBuffer.push_back(hardwareBuffer);

	return hardwareBuffer;
}


IHardwareBuffer* COpenGLDriver::createHardwareBuffer(scene::IVertexBuffer* vertexBuffer)
{
	if (!vertexBuffer)
		return 0;

	COpenGLHardwareBuffer* hardwareBuffer = new COpenGLHardwareBuffer(vertexBuffer, this);

	bool extendArray = true;

	for (u32 i = 0; i < HardwareBuffer.size(); ++i)
	{
		if (!HardwareBuffer[i])
		{
			HardwareBuffer[i] = hardwareBuffer;
			extendArray = false;
			break;
		}
	}

	if (extendArray)
		HardwareBuffer.push_back(hardwareBuffer);

	return hardwareBuffer;
}


void COpenGLDriver::removeAllHardwareBuffers()
{
	for (u32 i = 0; i < HardwareBuffer.size(); ++i)
	{
		if (HardwareBuffer[i])
		{
			HardwareBuffer[i]->removeFromArray(false);
			delete HardwareBuffer[i];
		}
	}

	HardwareBuffer.clear();
}


//! Create occlusion query.
/** Use node for identification and mesh for occlusion test. */
void COpenGLDriver::addOcclusionQuery(scene::ISceneNode* node,
		const scene::IMesh* mesh)
{
	if (!queryFeature(EVDF_OCCLUSION_QUERY))
		return;

	CNullDriver::addOcclusionQuery(node, mesh);
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if ((index != -1) && (OcclusionQueries[index].UID == 0))
		extGlGenQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
}


//! Remove occlusion query.
void COpenGLDriver::removeOcclusionQuery(scene::ISceneNode* node)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		if (OcclusionQueries[index].UID != 0)
			extGlDeleteQueries(1, reinterpret_cast<GLuint*>(&OcclusionQueries[index].UID));
		CNullDriver::removeOcclusionQuery(node);
	}
}


//! Run occlusion query. Draws mesh stored in query.
/** If the mesh shall not be rendered visible, use
overrideMaterial to disable the color and depth buffer. */
void COpenGLDriver::runOcclusionQuery(scene::ISceneNode* node, bool visible)
{
	if (!node)
		return;

	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		if (OcclusionQueries[index].UID)
			extGlBeginQuery(
#ifdef GL_ARB_occlusion_query
				GL_SAMPLES_PASSED_ARB,
#else
				0,
#endif
				OcclusionQueries[index].UID);
		CNullDriver::runOcclusionQuery(node,visible);
		if (OcclusionQueries[index].UID)
			extGlEndQuery(
#ifdef GL_ARB_occlusion_query
				GL_SAMPLES_PASSED_ARB);
#else
				0);
#endif
		testGLError();
	}
}


//! Update occlusion query. Retrieves results from GPU.
/** If the query shall not block, set the flag to false.
Update might not occur in this case, though */
void COpenGLDriver::updateOcclusionQuery(scene::ISceneNode* node, bool block)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		// not yet started
		if (OcclusionQueries[index].Run==u32(~0))
			return;
		GLint available = block?GL_TRUE:GL_FALSE;
		if (!block)
			extGlGetQueryObjectiv(OcclusionQueries[index].UID,
#ifdef GL_ARB_occlusion_query
						GL_QUERY_RESULT_AVAILABLE_ARB,
#elif defined(GL_NV_occlusion_query)
						GL_PIXEL_COUNT_AVAILABLE_NV,
#else
						0,
#endif
						&available);
		testGLError();
		if (available==GL_TRUE)
		{
			extGlGetQueryObjectiv(OcclusionQueries[index].UID,
#ifdef GL_ARB_occlusion_query
						GL_QUERY_RESULT_ARB,
#elif defined(GL_NV_occlusion_query)
						GL_PIXEL_COUNT_NV,
#else
						0,
#endif
						&available);
			if (queryFeature(EVDF_OCCLUSION_QUERY))
				OcclusionQueries[index].Result = available;
		}
		testGLError();
	}
}


//! Return query result.
/** Return value is the number of visible pixels/fragments.
The value is a safe approximation, i.e. can be larger than the
actual value of pixels. */
u32 COpenGLDriver::getOcclusionQueryResult(scene::ISceneNode* node) const
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
		return OcclusionQueries[index].Result;
	else
		return ~0;
}


// small helper function to create vertex buffer object adress offsets
static inline u8* buffer_offset(const long offset)
{
	return ((u8*)0 + offset);
}


void COpenGLDriver::drawMeshBuffer(const scene::IMeshBuffer* mb)
{
	if (!mb || !mb->isVertexBufferCompatible())
		return;

	if (!checkPrimitiveCount(mb->getPrimitiveCount()))
		return;

	CNullDriver::drawMeshBuffer(mb);

	COpenGLVertexDescriptor* descriptor = (COpenGLVertexDescriptor*)mb->getVertexDescriptor();

	scene::IIndexBuffer* indexBuffer = mb->getIndexBuffer();

	const u32 indexSize = indexBuffer->getIndexSize();
	const u32 indexCount = indexBuffer->getIndexCount();
	const GLenum indexType = (indexBuffer->getType() == EIT_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
	const scene::E_HARDWARE_MAPPING indexMapping = indexBuffer->getHardwareMappingHint();
	const void* indexData = indexBuffer->getIndices();

	const u32 primitiveCount = mb->getPrimitiveCount();
	const scene::E_PRIMITIVE_TYPE primitiveType = mb->getPrimitiveType();

	const bool hwRecommended = isHardwareBufferRecommend(mb);

	// draw everything
	setRenderStates3DMode();

//due to missing defines in OSX headers, we have to be more specific with this check
//#if defined(GL_ARB_vertex_array_bgra) || defined(GL_EXT_vertex_array_bgra)
#ifdef GL_BGRA
	const GLint colorSize=(FeatureAvailable[IRR_ARB_vertex_array_bgra] || FeatureAvailable[IRR_EXT_vertex_array_bgra])?GL_BGRA:4;
#else
	const GLint colorSize=4;
#endif

	const u32 attributeCount = descriptor->getAttributeCount();

	// Enable semantics, attributes and hardware buffers.

	GLuint hwIndexBuffer = 0;
	GLuint hwVertexBuffer = 0;

	GLuint glslProgram = getActiveGLSLProgram();

	u32 instanceVertexCount = 0;
	bool perInstanceBufferPresent = false;
	
	// support hw instancing
	for (u32 i = 0; i < mb->getVertexBufferCount(); ++i)
	{
		if (descriptor->getInstanceDataStepRate(i) == EIDSR_PER_INSTANCE)
		{
			instanceVertexCount = mb->getVertexBuffer(i)->getVertexCount();
			if (!instanceVertexCount)
				return;
			perInstanceBufferPresent = true;
		}
	}

	for (u32 i = 0; i < attributeCount && i < 16; ++i)
	{
		COpenGLVertexAttribute* attribute = static_cast<COpenGLVertexAttribute*>(descriptor->getAttribute(i));

		const u32 attribElementCount = attribute->getElementCount();
		const E_VERTEX_ATTRIBUTE_SEMANTIC attribSemantic = attribute->getSemantic();
		const E_VERTEX_ATTRIBUTE_TYPE attribType = attribute->getType();
		const u32 attribOffset = attribute->getOffset();
		const u32 attribBufferID = attribute->getBufferID();

		scene::IVertexBuffer* vertexBuffer = mb->getVertexBuffer(attribBufferID);

		const u32 vertexCount = vertexBuffer->getVertexCount();
		const u32 vertexSize = vertexBuffer->getVertexSize();
		const scene::E_HARDWARE_MAPPING vertexMapping = vertexBuffer->getHardwareMappingHint();
		u8* vertexData = static_cast<u8*>(vertexBuffer->getVertices());

		u8* attribData = vertexData + attribOffset;

		GLenum attribTypeGL = 0;		

		switch (attribType)
		{
		case EVAT_BYTE:
			attribTypeGL = GL_BYTE;
			break;
		case EVAT_UBYTE:
			attribTypeGL = GL_UNSIGNED_BYTE;
			break;
		case EVAT_SHORT:
			attribTypeGL = GL_SHORT;
			break;
		case EVAT_USHORT:
			attribTypeGL = GL_UNSIGNED_SHORT;
			break;
		case EVAT_INT:
			attribTypeGL = GL_INT;
			break;
		case EVAT_UINT:
			attribTypeGL = GL_UNSIGNED_INT;
			break;
		case EVAT_FLOAT:
			attribTypeGL = GL_FLOAT;
			break;
		case EVAT_DOUBLE:
			attribTypeGL = GL_DOUBLE;
			break;
		}

		// Update VBO.

#if defined(GL_ARB_vertex_buffer_object)
		if (FeatureAvailable[IRR_ARB_vertex_array_object])
		{
			COpenGLHardwareBuffer* vertexBufferObject = (COpenGLHardwareBuffer*)vertexBuffer->getHardwareBuffer();

			if (vertexBufferObject)
			{
				if (vertexMapping != scene::EHM_NEVER)
				{
					if (vertexBufferObject->isRequiredUpdate())
						vertexBufferObject->update(vertexMapping, vertexSize*vertexCount, vertexData);

					hwVertexBuffer = vertexBufferObject->getBufferID();
				}
				else
				{
					vertexBuffer->setHardwareBuffer(0);
					hwVertexBuffer = 0;
				}
			}
			else if (vertexMapping != scene::EHM_NEVER && hwRecommended)
			{
				vertexBufferObject = (COpenGLHardwareBuffer*)createHardwareBuffer(vertexBuffer);
				vertexBuffer->setHardwareBuffer(vertexBufferObject);
				vertexBufferObject->drop();

				hwVertexBuffer = vertexBufferObject->getBufferID();
			}
			else
			{
				hwVertexBuffer = 0;
			}

			extGlBindBuffer(GL_ARRAY_BUFFER, hwVertexBuffer);
		}
#endif
		
		if (glslProgram)
		{
			s32 status = attribute->getLocationStatus((u32)Material.MaterialType);
			GLint location = -1;

			if (status != -1)
			{
				if (status)
				{
					location = attribute->getLocation((u32)Material.MaterialType);
				}
				else
				{
					if (perInstanceBufferPresent == true)
						location = i;
					else
						location = extGlGetAttribLocation(glslProgram, attribute->getName().c_str());

					attribute->setLocation(location, (u32)Material.MaterialType);
				}
			}

			if (location != -1)
			{
				VertexAttributeStatus[location] = true;

				extGlEnableVertexAttribArray(location);

				if (hwVertexBuffer)
					extGlVertexAttribPointer(location, attribElementCount, attribTypeGL, GL_FALSE, vertexSize, buffer_offset(attribOffset));
				else
					extGlVertexAttribPointer(location, attribElementCount, attribTypeGL, GL_FALSE, vertexSize, attribData);

				// instancing
				if (descriptor->getInstanceDataStepRate(attribBufferID) == video::EIDSR_PER_INSTANCE)
					extGlVertexAttribDivisor(location, 1);
			}
		}
		
	}

	// Update IBO.

#if defined(GL_ARB_vertex_buffer_object)
	if (FeatureAvailable[IRR_ARB_vertex_array_object])
	{
		COpenGLHardwareBuffer* indexBufferObject = (COpenGLHardwareBuffer*)indexBuffer->getHardwareBuffer();

		if (indexBufferObject)
		{
			if (indexMapping != scene::EHM_NEVER)
			{
				if (indexBufferObject->isRequiredUpdate())
					indexBufferObject->update(indexMapping, indexSize*indexCount, indexData);

				hwIndexBuffer = indexBufferObject->getBufferID();
			}
			else
			{
				indexBuffer->setHardwareBuffer(0);
				hwIndexBuffer = 0;
			}
		}
		else if (indexMapping != scene::EHM_NEVER && hwRecommended)
		{
			indexBufferObject = (COpenGLHardwareBuffer*)createHardwareBuffer(indexBuffer);
			indexBuffer->setHardwareBuffer(indexBufferObject);
			indexBufferObject->drop();

			hwIndexBuffer = indexBufferObject->getBufferID();
		}
		else
		{
			hwIndexBuffer = 0;
		}

		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hwIndexBuffer);
	}
#endif

	// Draw.
	if (perInstanceBufferPresent)
		renderInstance(hwIndexBuffer ? 0 : indexData, indexType, primitiveCount, primitiveType, instanceVertexCount);
	else
		renderArray(hwIndexBuffer ? 0 : indexData, indexType, primitiveCount, primitiveType);

	LastVertexDescriptor = descriptor;

	// Disable hardware buffers.
#if defined(GL_ARB_vertex_buffer_object)
	if (hwIndexBuffer)
		extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (hwVertexBuffer)
		extGlBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void COpenGLDriver::renderInstance(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType, u32 instanceCount)
{
	switch (primitiveType)
	{	
	case scene::EPT_LINE_STRIP:
		extGlDrawElementsInstanced(GL_LINE_STRIP, primitiveCount + 1, indexType, indices, instanceCount);
		break;
	case scene::EPT_LINE_LOOP:
		extGlDrawElementsInstanced(GL_LINE_LOOP, primitiveCount, indexType, indices, instanceCount);
		break;
	case scene::EPT_LINES:
		extGlDrawElementsInstanced(GL_LINES, primitiveCount * 2, indexType, indices, instanceCount);
		break;
	case scene::EPT_TRIANGLE_STRIP:
		extGlDrawElementsInstanced(GL_TRIANGLE_STRIP, primitiveCount + 2, indexType, indices, instanceCount);
		break;
	case scene::EPT_TRIANGLE_FAN:
		extGlDrawElementsInstanced(GL_TRIANGLE_FAN, primitiveCount + 2, indexType, indices, instanceCount);
		break;
	case scene::EPT_TRIANGLES:
		extGlDrawElementsInstanced(GL_TRIANGLES, primitiveCount * 3, indexType, indices, instanceCount);
		break;
	default:
		break;
	}
}

void COpenGLDriver::renderArray(const void* indices, GLenum indexType, u32 primitiveCount, scene::E_PRIMITIVE_TYPE primitiveType)
{
	switch (primitiveType)
	{
		case scene::EPT_POINTS:
		case scene::EPT_POINT_SPRITES:
		{
#ifdef GL_ARB_point_sprite
			if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
				glEnable(GL_POINT_SPRITE_ARB);
#endif

			// prepare size and attenuation (where supported)
			GLfloat particleSize=Material.Thickness;
//			if (AntiAlias)
//				particleSize=core::clamp(particleSize, DimSmoothedPoint[0], DimSmoothedPoint[1]);
//			else
				particleSize=core::clamp(particleSize, DimAliasedPoint[0], DimAliasedPoint[1]);
#if defined(GL_VERSION_1_4) || defined(GL_ARB_point_parameters) || defined(GL_EXT_point_parameters) || defined(GL_SGIS_point_parameters)
			const float att[] = {1.0f, 1.0f, 0.0f};
#if defined(GL_VERSION_1_4)
			extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);
#elif defined(GL_ARB_point_parameters)
			extGlPointParameterfv(GL_POINT_DISTANCE_ATTENUATION_ARB, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_ARB,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_ARB, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f);
#elif defined(GL_EXT_point_parameters)
			extGlPointParameterfv(GL_DISTANCE_ATTENUATION_EXT, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_EXT,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_EXT, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_EXT, 1.0f);
#elif defined(GL_SGIS_point_parameters)
			extGlPointParameterfv(GL_DISTANCE_ATTENUATION_SGIS, att);
//			extGlPointParameterf(GL_POINT_SIZE_MIN_SGIS,1.f);
			extGlPointParameterf(GL_POINT_SIZE_MAX_SGIS, particleSize);
			extGlPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE_SGIS, 1.0f);
#endif
#endif
			glPointSize(particleSize);

#ifdef GL_ARB_point_sprite
			if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
				glTexEnvf(GL_POINT_SPRITE_ARB,GL_COORD_REPLACE, GL_TRUE);
#endif
			glDrawArrays(GL_POINTS, 0, primitiveCount);
#ifdef GL_ARB_point_sprite
			if (primitiveType == scene::EPT_POINT_SPRITES && FeatureAvailable[IRR_ARB_point_sprite])
			{
				glDisable(GL_POINT_SPRITE_ARB);
				glTexEnvf(GL_POINT_SPRITE_ARB,GL_COORD_REPLACE, GL_FALSE);
			}
#endif
		}
			break;
		case scene::EPT_LINE_STRIP:
			glDrawElements(GL_LINE_STRIP, primitiveCount + 1, indexType, indices);
			break;
		case scene::EPT_LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, primitiveCount, indexType, indices);
			break;
		case scene::EPT_LINES:
			glDrawElements(GL_LINES, primitiveCount * 2, indexType, indices);
			break;
		case scene::EPT_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, primitiveCount + 2, indexType, indices);
			break;
		case scene::EPT_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount + 2, indexType, indices);
			break;
		case scene::EPT_TRIANGLES:
			glDrawElements(GL_TRIANGLES, primitiveCount * 3, indexType, indices);
			break;
	}
}

bool COpenGLDriver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= MaxSupportedTextures)
		return false;

	if (CurrentTexture[stage]==texture)
		return true;

	CurrentTexture.set(stage,texture);

	if (!texture)
		return true;
	else if (texture->getDriverType() != EDT_OPENGL)
	{
		CurrentTexture.set(stage, 0);
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	return true;
}


//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
//! Returns whether disabling was successful or not.
bool COpenGLDriver::disableTextures(u32 fromStage)
{
	bool result=true;
	for (u32 i=fromStage; i<MaxSupportedTextures; ++i)
	{
		result &= setActiveTexture(i, 0);
		BridgeCalls->setTexture(i);
	}
	return result;
}

//! returns a device dependent texture from a software surface (IImage)
video::ITexture* COpenGLDriver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
{
	return new COpenGLTexture(surface, name, mipmapData, this);
}


//! Sets a material. All 3d drawing functions draw geometry now using this material.
void COpenGLDriver::setMaterial(const SMaterial& material)
{
	Material = material;

	for (u32 i = 0; i < MaxTextureUnits; ++i)
	{
		setActiveTexture(i, material.getTexture(i));
	}
}


//! prints error if an error happened.
bool COpenGLDriver::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch (g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	case GL_TABLE_TOO_LARGE:
		os::Printer::log("GL_TABLE_TOO_LARGE", ELL_ERROR); break;
#if defined(GL_EXT_framebuffer_object)
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		os::Printer::log("GL_INVALID_FRAMEBUFFER_OPERATION", ELL_ERROR); break;
#endif
	};
//	_IRR_DEBUG_BREAK_IF(true);
	return true;
#else
	return false;
#endif
}


//! sets the needed renderstates
void COpenGLDriver::setRenderStates3DMode()
{
	// Reset Texture Stages
	/*
	BridgeCalls->setBlend(false);
	BridgeCalls->setAlphaTest(false);
	BridgeCalls->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ResetRenderStates = true;	

#ifdef GL_EXT_clip_volume_hint
	if (FeatureAvailable[IRR_EXT_clip_volume_hint])
		glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
#endif
	*/

	bool shaderChanged = LastMaterial.MaterialType != Material.MaterialType;
	s32 numMaterialRenderers = (s32)MaterialRenderers.size();

	if (ResetRenderStates || LastMaterial != Material)
	{
		// unset old material
		if (shaderChanged &&
			LastMaterial.MaterialType < numMaterialRenderers)
			MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

		// set new material.
		if (Material.MaterialType < numMaterialRenderers)
			MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);

		LastMaterial = Material;		
	}

	if (Material.MaterialType < numMaterialRenderers)
		MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, shaderChanged || RendererTransformChanged);

	RendererTransformChanged = false;
	ResetRenderStates = false;
}


//! Get native wrap mode value
GLint COpenGLDriver::getTextureWrapMode(const u8 clamp)
{
	GLint mode=GL_REPEAT;
	switch (clamp)
	{
		case ETC_REPEAT:
			mode=GL_REPEAT;
			break;
		case ETC_CLAMP:
			mode=GL_CLAMP;
			break;
		case ETC_CLAMP_TO_EDGE:
#ifdef GL_VERSION_1_2
			if (Version>101)
				mode=GL_CLAMP_TO_EDGE;
			else
#endif
#ifdef GL_SGIS_texture_edge_clamp
			if (FeatureAvailable[IRR_SGIS_texture_edge_clamp])
				mode=GL_CLAMP_TO_EDGE_SGIS;
			else
#endif
				// fallback
				mode=GL_CLAMP;
			break;
		case ETC_CLAMP_TO_BORDER:
#ifdef GL_VERSION_1_3
			if (Version>102)
				mode=GL_CLAMP_TO_BORDER;
			else
#endif
#ifdef GL_ARB_texture_border_clamp
			if (FeatureAvailable[IRR_ARB_texture_border_clamp])
				mode=GL_CLAMP_TO_BORDER_ARB;
			else
#endif
#ifdef GL_SGIS_texture_border_clamp
			if (FeatureAvailable[IRR_SGIS_texture_border_clamp])
				mode=GL_CLAMP_TO_BORDER_SGIS;
			else
#endif
				// fallback
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR:
#ifdef GL_VERSION_1_4
			if (Version>103)
				mode=GL_MIRRORED_REPEAT;
			else
#endif
#ifdef GL_ARB_texture_border_clamp
			if (FeatureAvailable[IRR_ARB_texture_mirrored_repeat])
				mode=GL_MIRRORED_REPEAT_ARB;
			else
#endif
#ifdef GL_IBM_texture_mirrored_repeat
			if (FeatureAvailable[IRR_IBM_texture_mirrored_repeat])
				mode=GL_MIRRORED_REPEAT_IBM;
			else
#endif
				mode=GL_REPEAT;
			break;
		case ETC_MIRROR_CLAMP:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_EXT;
			else
#endif
#if defined(GL_ATI_texture_mirror_once)
			if (FeatureAvailable[IRR_ATI_texture_mirror_once])
				mode=GL_MIRROR_CLAMP_ATI;
			else
#endif
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR_CLAMP_TO_EDGE:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_TO_EDGE_EXT;
			else
#endif
#if defined(GL_ATI_texture_mirror_once)
			if (FeatureAvailable[IRR_ATI_texture_mirror_once])
				mode=GL_MIRROR_CLAMP_TO_EDGE_ATI;
			else
#endif
				mode=GL_CLAMP;
			break;
		case ETC_MIRROR_CLAMP_TO_BORDER:
#ifdef GL_EXT_texture_mirror_clamp
			if (FeatureAvailable[IRR_EXT_texture_mirror_clamp])
				mode=GL_MIRROR_CLAMP_TO_BORDER_EXT;
			else
#endif
				mode=GL_CLAMP;
			break;
	}
	return mode;
}


//! Can be called by an IMaterialRenderer to make its work easier.
void COpenGLDriver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial, bool resetAllRenderStates)
{
	// fillmode - fixed pipeline call, but it emulate GL_LINES behaviour in rendering, so it stay here.
	if (resetAllRenderStates || (lastmaterial.Wireframe != material.Wireframe) || (lastmaterial.PointCloud != material.PointCloud))
		glPolygonMode(GL_FRONT_AND_BACK, material.Wireframe ? GL_LINE : material.PointCloud? GL_POINT : GL_FILL);

	// ZBuffer
	switch (material.ZBuffer)
	{
	case ECFN_DISABLED:
		BridgeCalls->setDepthTest(false);
		break;
	case ECFN_LESSEQUAL:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_LEQUAL);
		break;
	case ECFN_EQUAL:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_EQUAL);
		break;
	case ECFN_LESS:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_LESS);
		break;
	case ECFN_NOTEQUAL:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_NOTEQUAL);
		break;
	case ECFN_GREATEREQUAL:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_GEQUAL);
		break;
	case ECFN_GREATER:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_GREATER);
		break;
	case ECFN_ALWAYS:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_ALWAYS);
		break;
	case ECFN_NEVER:
		BridgeCalls->setDepthTest(true);
		BridgeCalls->setDepthFunc(GL_NEVER);
		break;
	default:
		break;
	}

	// Scissor
	BridgeCalls->enableScissor(EnableScissor);
	if (EnableScissor == true)
		BridgeCalls->setScissor(ScissorRect.UpperLeftCorner.X, ScissorRect.UpperLeftCorner.Y, ScissorRect.getWidth(), ScissorRect.getHeight());

	// ZWrite
	if (material.ZWriteEnable && (AllowZWriteOnTransparent || (material.BlendOperation == EBO_NONE &&
		!MaterialRenderers[material.MaterialType].Renderer->isTransparent())))
	{
		BridgeCalls->setDepthMask(true);
	}
	else
	{
		BridgeCalls->setDepthMask(false);
	}

	// Back face culling
	if ((material.FrontfaceCulling) && (material.BackfaceCulling))
	{
		BridgeCalls->setCullFaceFunc(GL_FRONT_AND_BACK);
		BridgeCalls->setCullFace(true);
	}
	else if (material.BackfaceCulling)
	{
		BridgeCalls->setCullFaceFunc(GL_BACK);
		BridgeCalls->setCullFace(true);
	}
	else if (material.FrontfaceCulling)
	{
		BridgeCalls->setCullFaceFunc(GL_FRONT);
		BridgeCalls->setCullFace(true);
	}
	else
	{
		BridgeCalls->setCullFace(false);
	}

	// Color Mask
	BridgeCalls->setColorMask(
		(material.ColorMask & ECP_RED)?GL_TRUE:GL_FALSE,
		(material.ColorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
		(material.ColorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
		(material.ColorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);

	// Blend Equation
	/*
    if (material.BlendOperation == EBO_NONE)
        BridgeCalls->setBlend(false);
    else
    {
        BridgeCalls->setBlend(true);

#if defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op) || defined(GL_VERSION_1_4)
        if (queryFeature(EVDF_BLEND_OPERATIONS))
        {
            switch (material.BlendOperation)
            {
            case EBO_SUBTRACT:
#if defined(GL_VERSION_1_4)
                BridgeCalls->setBlendEquation(GL_FUNC_SUBTRACT);
#elif defined(GL_EXT_blend_subtract)
                BridgeCalls->setBlendEquation(GL_FUNC_SUBTRACT_EXT);
#endif
                break;
            case EBO_REVSUBTRACT:
#if defined(GL_VERSION_1_4)
                BridgeCalls->setBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
#elif defined(GL_EXT_blend_subtract)
                BridgeCalls->setBlendEquation(GL_FUNC_REVERSE_SUBTRACT_EXT);
#endif
                break;
            case EBO_MIN:
#if defined(GL_VERSION_1_4)
                BridgeCalls->setBlendEquation(GL_MIN);
#elif defined(GL_EXT_blend_minmax)
                BridgeCalls->setBlendEquation(GL_MIN_EXT);
#endif
                break;
            case EBO_MAX:
#if defined(GL_VERSION_1_4)
                BridgeCalls->setBlendEquation(GL_MAX);
#elif defined(GL_EXT_blend_minmax)
                BridgeCalls->setBlendEquation(GL_MAX_EXT);
#endif
                break;
            case EBO_MIN_FACTOR:
#if defined(GL_AMD_blend_minmax_factor)
                if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
                    BridgeCalls->setBlendEquation(GL_FACTOR_MIN_AMD);
#endif
				// fallback in case of missing extension
#if defined(GL_VERSION_1_4)
#if defined(GL_AMD_blend_minmax_factor)
                else
#endif
                    BridgeCalls->setBlendEquation(GL_MIN);
#endif
                break;
            case EBO_MAX_FACTOR:
#if defined(GL_AMD_blend_minmax_factor)
                if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
                    BridgeCalls->setBlendEquation(GL_FACTOR_MAX_AMD);
#endif
				// fallback in case of missing extension
#if defined(GL_VERSION_1_4)
#if defined(GL_AMD_blend_minmax_factor)
                else
#endif
                    BridgeCalls->setBlendEquation(GL_MAX);
#endif
                break;
            case EBO_MIN_ALPHA:
#if defined(GL_SGIX_blend_alpha_minmax)
                if (FeatureAvailable[IRR_SGIX_blend_alpha_minmax])
                    BridgeCalls->setBlendEquation(GL_ALPHA_MIN_SGIX);
                // fallback in case of missing extension
                else
                    if (FeatureAvailable[IRR_EXT_blend_minmax])
                        BridgeCalls->setBlendEquation(GL_MIN_EXT);
#endif
                break;
            case EBO_MAX_ALPHA:
#if defined(GL_SGIX_blend_alpha_minmax)
                if (FeatureAvailable[IRR_SGIX_blend_alpha_minmax])
                    BridgeCalls->setBlendEquation(GL_ALPHA_MAX_SGIX);
                // fallback in case of missing extension
                else
                    if (FeatureAvailable[IRR_EXT_blend_minmax])
                        BridgeCalls->setBlendEquation(GL_MAX_EXT);
#endif
                break;
            default:
#if defined(GL_VERSION_1_4)
                BridgeCalls->setBlendEquation(GL_FUNC_ADD);
#elif defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
                BridgeCalls->setBlendEquation(GL_FUNC_ADD_EXT);
#endif
                break;
            }
		}
#endif
	}

    // Blend Factor
	if (IR(material.BlendFactor) & 0xFFFFFFFF)
	{
        E_BLEND_FACTOR srcRGBFact = EBF_ZERO;
        E_BLEND_FACTOR dstRGBFact = EBF_ZERO;
        E_BLEND_FACTOR srcAlphaFact = EBF_ZERO;
        E_BLEND_FACTOR dstAlphaFact = EBF_ZERO;
        E_MODULATE_FUNC modulo = EMFN_MODULATE_1X;
        u32 alphaSource = 0;

        unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo, alphaSource, material.BlendFactor);

        if (queryFeature(EVDF_BLEND_SEPARATE))
        {
            BridgeCalls->setBlendFuncSeparate(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact), getGLBlend(srcAlphaFact), getGLBlend(dstAlphaFact));
        }
        else
        {
            BridgeCalls->setBlendFunc(getGLBlend(srcRGBFact), getGLBlend(dstRGBFact));
        }
	}
	*/

	// thickness
	if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
	{
		if (AntiAlias)
		{
//			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedPoint[0], DimSmoothedPoint[1]));
			// we don't use point smoothing
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimSmoothedLine[0], DimSmoothedLine[1]));
		}
		else
		{
			glPointSize(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedPoint[0], DimAliasedPoint[1]));
			glLineWidth(core::clamp(static_cast<GLfloat>(material.Thickness), DimAliasedLine[0], DimAliasedLine[1]));
		}
	}

	// Anti aliasing
	if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing)
	{
		if (FeatureAvailable[IRR_ARB_multisample])
		{
			if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
			else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

			if ((AntiAlias >= 2) && (material.AntiAliasing & (EAAM_SIMPLE|EAAM_QUALITY)))
			{
				glEnable(GL_MULTISAMPLE_ARB);
#ifdef GL_NV_multisample_filter_hint
				if (FeatureAvailable[IRR_NV_multisample_filter_hint])
				{
					if ((material.AntiAliasing & EAAM_QUALITY) == EAAM_QUALITY)
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
					else
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
				}
#endif
			}
			else
				glDisable(GL_MULTISAMPLE_ARB);
		}
		if ((material.AntiAliasing & EAAM_LINE_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_LINE_SMOOTH)
				glEnable(GL_LINE_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_LINE_SMOOTH)
				glDisable(GL_LINE_SMOOTH);
		}
		if ((material.AntiAliasing & EAAM_POINT_SMOOTH) != (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH))
		{
			if (material.AntiAliasing & EAAM_POINT_SMOOTH)
				// often in software, and thus very slow
				glEnable(GL_POINT_SMOOTH);
			else if (lastmaterial.AntiAliasing & EAAM_POINT_SMOOTH)
				glDisable(GL_POINT_SMOOTH);
		}
	}

	// Texture parameters
	setTextureRenderStates(material, resetAllRenderStates);
}

//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
void COpenGLDriver::setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates)
{
	// Set textures to TU/TIU and apply filters to them

	for (s32 i = MaxTextureUnits - 1; i >= 0; --i)
	{		
		BridgeCalls->setTexture(i);

		if (!CurrentTexture[i])
			continue;		

		if (CurrentTexture[i]->getTextureType() == ETT_TEXTURE_CUBE)
		{			
			if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
			{
				if (material.TextureLayer[i].AnisotropicFilter > 1 &&
					material.TextureLayer[i].BilinearFilter == false &&
					material.TextureLayer[i].TrilinearFilter == false)
				{
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT,
						material.TextureLayer[i].AnisotropicFilter > 1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);
				}
				else
				{
					// enable mipmap
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
			}
			else
			{
				// disable mipmap
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		}
		else if (CurrentTexture[i]->getTextureType() == ETT_TEXTURE_ARRAY)
		{
			if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
			{
				if (material.TextureLayer[i].AnisotropicFilter > 1 &&
					material.TextureLayer[i].BilinearFilter == false &&
					material.TextureLayer[i].TrilinearFilter == false)
				{
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT,
						material.TextureLayer[i].AnisotropicFilter > 1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);
				}
				else
				{
					// enable mipmap
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
			}
			else
			{
				// disable mipmap
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			
			// wrap mode
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
		}
		else
		{
			const COpenGLTexture* tmpTexture = static_cast<const COpenGLTexture*>(CurrentTexture[i]);
			if (resetAllRenderstates)
				tmpTexture->getStatesCache().IsCached = false;

#ifdef GL_VERSION_2_1
			if (Version >= 210)
			{
				if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].LODBias != tmpTexture->getStatesCache().LODBias)
				{
					if (material.TextureLayer[i].LODBias)
					{
						const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, tmp);
					}
					else
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.f);

					tmpTexture->getStatesCache().LODBias = material.TextureLayer[i].LODBias;
				}
			}
			else if (FeatureAvailable[IRR_EXT_texture_lod_bias])
			{
				if (material.TextureLayer[i].LODBias)
				{
					const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tmp);
				}
				else
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.f);
			}
#elif defined(GL_EXT_texture_lod_bias)
			if (FeatureAvailable[IRR_EXT_texture_lod_bias])
			{
				if (material.TextureLayer[i].LODBias)
				{
					const float tmp = core::clamp(material.TextureLayer[i].LODBias * 0.125f, -MaxTextureLODBias, MaxTextureLODBias);
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tmp);
				}
				else
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.f);
			}
#endif

			if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
				material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

				tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
				tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
			}

			if (material.UseMipMaps && CurrentTexture[i]->hasMipMaps())
			{
				if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
					material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || !tmpTexture->getStatesCache().MipMapStatus)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						material.TextureLayer[i].TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
						material.TextureLayer[i].BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
						GL_NEAREST_MIPMAP_NEAREST);

					tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
					tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
					tmpTexture->getStatesCache().MipMapStatus = true;
				}
			}
			else
			{
				if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].BilinearFilter != tmpTexture->getStatesCache().BilinearFilter ||
					material.TextureLayer[i].TrilinearFilter != tmpTexture->getStatesCache().TrilinearFilter || tmpTexture->getStatesCache().MipMapStatus)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						(material.TextureLayer[i].BilinearFilter || material.TextureLayer[i].TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

					tmpTexture->getStatesCache().BilinearFilter = material.TextureLayer[i].BilinearFilter;
					tmpTexture->getStatesCache().TrilinearFilter = material.TextureLayer[i].TrilinearFilter;
					tmpTexture->getStatesCache().MipMapStatus = false;
				}
			}

#ifdef GL_EXT_texture_filter_anisotropic
			// Pham Hong Duc
			// AnisotropicFilter is defalt TrilinearFilter
			if (CurrentTexture[i] && CurrentTexture[i]->hasMipMaps() == true)
			{
				if (material.TextureLayer[i].AnisotropicFilter > 1 &&
					material.TextureLayer[i].BilinearFilter == false &&
					material.TextureLayer[i].TrilinearFilter == false)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					tmpTexture->getStatesCache().BilinearFilter = false;
					tmpTexture->getStatesCache().TrilinearFilter = true;
					tmpTexture->getStatesCache().MipMapStatus = true;
				}
			}
			else
			{
				// default no mipmap
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				tmpTexture->getStatesCache().BilinearFilter = false;
				tmpTexture->getStatesCache().TrilinearFilter = false;
				tmpTexture->getStatesCache().MipMapStatus = false;
			}

			if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic] &&
				(!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].AnisotropicFilter != tmpTexture->getStatesCache().AnisotropicFilter))
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
					material.TextureLayer[i].AnisotropicFilter>1 ? core::min_(MaxAnisotropy, material.TextureLayer[i].AnisotropicFilter) : 1);

				tmpTexture->getStatesCache().AnisotropicFilter = material.TextureLayer[i].AnisotropicFilter;
			}
#endif

			if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapU != tmpTexture->getStatesCache().WrapU)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapMode(material.TextureLayer[i].TextureWrapU));
				tmpTexture->getStatesCache().WrapU = material.TextureLayer[i].TextureWrapU;
			}

			if (!tmpTexture->getStatesCache().IsCached || material.TextureLayer[i].TextureWrapV != tmpTexture->getStatesCache().WrapV)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapMode(material.TextureLayer[i].TextureWrapV));
				tmpTexture->getStatesCache().WrapV = material.TextureLayer[i].TextureWrapV;
			}

			if (material.TextureLayer[i].TextureWrapU == ETC_CLAMP_TO_BORDER ||
				material.TextureLayer[i].TextureWrapV == ETC_CLAMP_TO_BORDER)
			{
				const SColorf &color = material.TextureLayer[i].BorderColor;
				GLfloat borderColor[] = { color.r, color.g, color.b, color.a };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);				
			}

			tmpTexture->getStatesCache().IsCached = true;
		}
	}

	// be sure to leave in texture stage 0
	BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
}

//! \return Returns the name of the video driver.
const wchar_t* COpenGLDriver::getName() const
{
	return Name.c_str();
}

//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void COpenGLDriver::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


// this code was sent in by Oliver Klems, thank you! (I modified the glViewport
// method just a bit.
void COpenGLDriver::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight() > 0 && vp.getWidth() > 0)
		BridgeCalls->setViewport(vp.UpperLeftCorner.X, getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(), vp.getWidth(), vp.getHeight());

	ViewPort = vp;
}

void COpenGLDriver::setScissor(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight() > 0 && vp.getWidth() > 0)
		BridgeCalls->setScissor(vp.UpperLeftCorner.X, getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(), vp.getWidth(), vp.getHeight());

	ScissorRect = vp;
}

void COpenGLDriver::enableScissor(bool b)
{
	EnableScissor = b;
	BridgeCalls->enableScissor(EnableScissor);
}

//! Sets the fog mode.
void COpenGLDriver::setFog(SColor c, E_FOG_TYPE fogType, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(c, fogType, start, end, density, pixelFog, rangeFog);
}


//! Draws a 3d line.
void COpenGLDriver::draw3DLine(const core::vector3df& start, const core::vector3df& end, SColor color)
{
	
}


//! Removes a texture from the texture cache and deletes it, freeing lot of memory.
void COpenGLDriver::removeTexture(ITexture* texture)
{
	if (!texture)
		return;

	CNullDriver::removeTexture(texture);
	// Remove this texture from CurrentTexture as well
	CurrentTexture.remove(texture);
}


//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COpenGLDriver::OnResize(const core::dimension2d<u32>& size)
{
	CNullDriver::OnResize(size);
	BridgeCalls->setViewport(0, 0, size.Width, size.Height);
	ResetRenderStates = true;
}


//! Adds a new material renderer to the video device.
s32 COpenGLDriver::addMaterialRenderer(IMaterialRenderer* renderer, const char* name)
{
	s32 id = CNullDriver::addMaterialRenderer(renderer, name);

	if (id != -1)
	{
		for(u32 i = 0; i < VertexDescriptor.size(); ++i)
		{
			((COpenGLVertexDescriptor*)VertexDescriptor[i])->addLocationLayer();
		}
	}

	return id;
}


//! Returns type of video driver
E_DRIVER_TYPE COpenGLDriver::getDriverType() const
{
	return EDT_OPENGL;
}


//! returns color format
ECOLOR_FORMAT COpenGLDriver::getColorFormat() const
{
	return ColorFormat;
}

//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 COpenGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	return nr;
}


//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 COpenGLDriver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	const c8* geometryShaderProgram,
	const c8* geometryShaderEntryPointName,
	E_GEOMETRY_SHADER_TYPE gsCompileTarget,
	scene::E_PRIMITIVE_TYPE inType,
	scene::E_PRIMITIVE_TYPE outType,
	u32 verticesOut,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
{
	s32 nr = -1;

	#ifdef _IRR_COMPILE_WITH_CG_
	if (shadingLang == EGSL_CG)
	{
		COpenGLCgMaterialRenderer* r = new COpenGLCgMaterialRenderer(
			this, nr,
			vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
			pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
			geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
			inType, outType, verticesOut,
			callback,baseMaterial, userData);

		r->drop();
	}
	else
	#endif
	{
		COpenGLSLMaterialRenderer* r = new COpenGLSLMaterialRenderer(
			this, nr,
			vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
			pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
			geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
			inType, outType, verticesOut,
			callback,baseMaterial, userData);

		r->drop();
	}

	return nr;
}


//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* COpenGLDriver::getVideoDriver()
{
	return this;
}


ITexture* COpenGLDriver::addRenderTargetTexture(const core::dimension2d<u32>& size,
					const io::path& name,
					const ECOLOR_FORMAT format)
{
	//disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	video::ITexture* rtt = 0;
#if defined(GL_EXT_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = new COpenGLFBOTexture(size, name, this, format);
		addTexture(rtt);
		rtt->drop();
	}
	else
#endif
	{
		// the simple texture is only possible for size <= screensize
		// we try to find an optimal size with the original constraints
		core::dimension2du destSize(core::min_(size.Width,ScreenSize.Width), core::min_(size.Height,ScreenSize.Height));
		destSize = destSize.getOptimalSize((size==size.getOptimalSize()), false, false);
		rtt = addTexture(destSize, name, ECF_A8R8G8B8);
		if (rtt)
		{
			static_cast<video::COpenGLTexture*>(rtt)->setIsRenderTarget(true);
		}
	}

	//restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return rtt;
}

ITexture* COpenGLDriver::addRenderTargetTextureArray(const core::dimension2d<u32>& size, u32 arraySize,
	const io::path& name, const ECOLOR_FORMAT format)
{
	video::ITexture* rtt = 0;

#if defined(GL_EXT_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = new COpenGLTextureArray(this, size, arraySize, name, format);
		if (rtt)
		{
			addTexture(rtt);
			rtt->drop();
		}
	}
#endif
	return rtt;
}

ITexture* COpenGLDriver::addRenderTargetCubeTexture(const core::dimension2d<u32>& size, const io::path& name, const ECOLOR_FORMAT format)
{
	video::ITexture* rtt = 0;

#if defined(GL_EXT_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = new COpenGLTextureCube(this, size, name, format);
		if (rtt)
		{
			addTexture(rtt);
			rtt->drop();
		}
	}
#endif
	return rtt;
}

ITexture* COpenGLDriver::getTextureCube(
	IImage *imageX1,
	IImage *imageX2,
	IImage *imageY1,
	IImage *imageY2,
	IImage *imageZ1,
	IImage *imageZ2)
{
	return new COpenGLTextureCube(this, "TextureCube", imageX1, imageX2, imageY1, imageY2, imageZ1, imageZ2);
}

//! add texture array
ITexture* COpenGLDriver::getTextureArray(IImage** images, u32 num)
{
	return new COpenGLTextureArray(this, "TextureArray", images, num);
}

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 COpenGLDriver::getMaximalPrimitiveCount() const
{
	return 0x7fffffff;
}

void COpenGLDriver::unbindRTT(ITexture *texture)
{
	if (texture->getTextureType() == ETT_TEXTURE_2D)
		static_cast<COpenGLTexture*>(texture)->unbindRTT();
	else if (texture->getTextureType() == ETT_TEXTURE_CUBE)
		static_cast<COpenGLTextureCube*>(texture)->unbindRTT();
	else if (texture->getTextureType() == ETT_TEXTURE_ARRAY)
		static_cast<COpenGLTextureArray*>(texture)->unbindRTT();
}

//! set or reset render target
bool COpenGLDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil)
{
	// check for right driver type

	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	if (texture && texture->getTextureType() != ETT_TEXTURE_2D)
	{
		os::Printer::log("Fatal Error: This is not texture 2D", ELL_ERROR);
		return false;
	}

#if defined(GL_EXT_framebuffer_object)
	if (CurrentTarget==ERT_MULTI_RENDER_TEXTURES)
	{
		for (u32 i=0; i<MRTargets.size(); ++i)
		{
			if (MRTargets[i].TargetType==ERT_RENDER_TEXTURE)
			{
				for (++i; i<MRTargets.size(); ++i)
					if (MRTargets[i].TargetType==ERT_RENDER_TEXTURE)
						extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, 0, 0);
			}
		}
		MRTargets.clear();
	}
#endif

	// check if we should set the previous RT back
	if ((RenderTargetTexture != texture) ||
		(CurrentTarget==ERT_MULTI_RENDER_TEXTURES))
	{
		BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
		ResetRenderStates=true;

		if (RenderTargetTexture!=0)
		{
			unbindRTT(RenderTargetTexture);
		}

		if (texture)
		{
			// we want to set a new target. so do this.
			BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
			
			COpenGLTexture *rtt = static_cast<COpenGLTexture*>(texture);
			RenderTargetTexture = rtt;

			if (rtt->isFrameBufferObject())
			{
				if (depthStencil)
				{
					static_cast<COpenGLFBOTexture*>(RenderTargetTexture)->setDepthTexture(depthStencil);
				}
				else
				{
					ITexture* renderBuffer = createDepthTexture(RenderTargetTexture, true);
					static_cast<COpenGLFBOTexture*>(RenderTargetTexture)->setDepthTexture(renderBuffer);
					renderBuffer->drop();
				}
			}

			// calls glDrawBuffer as well
			rtt->bindRTT();

			CurrentRendertargetSize = texture->getSize();
			CurrentTarget = ERT_RENDER_TEXTURE;
		}
		else
		{
			BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
			RenderTargetTexture = 0;
			CurrentRendertargetSize = core::dimension2d<u32>(0,0);
			CurrentTarget = ERT_FRAME_BUFFER;
			glDrawBuffer(Params.Doublebuffer?GL_BACK_LEFT:GL_FRONT_LEFT);
		}

		// we need to update the matrices due to the rendersize change.
		ResetRenderStates=true;
	}

	clearBuffers(clearBackBuffer, clearZBuffer, false, color);
	return true;
}

bool COpenGLDriver::setRenderTargetCube(video::ITexture* texture, E_CUBEMAP_FACE face, bool clearBackBuffer, bool clearZBuffer, SColor color)
{
	// check for right driver type
	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	if (texture && texture->getTextureType() != ETT_TEXTURE_CUBE)
	{
		os::Printer::log("Fatal Error: Set render target is not texture cube", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back
	BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
	ResetRenderStates = true;

	if (RenderTargetTexture != 0 && RenderTargetTexture != texture)
	{
		unbindRTT(RenderTargetTexture);
	}

	if (texture)
	{
		// we want to set a new target. so do this.
		BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
		RenderTargetTexture = texture;

		int mapFace[] =
		{
			0,	// left
			1,	// right
			2,	// up
			3,	// down
			5,	// front
			4	// back
		};

		// calls glDrawBuffer as well
		static_cast<COpenGLTextureCube*>(RenderTargetTexture)->bindRTT(mapFace[(int)face]);
		CurrentRendertargetSize = texture->getSize();
	}
	else
	{
		BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
		RenderTargetTexture = 0;
		CurrentRendertargetSize = core::dimension2d<u32>(0, 0);
		glDrawBuffer(Params.Doublebuffer ? GL_BACK_LEFT : GL_FRONT_LEFT);
	}

	clearBuffers(clearBackBuffer, clearZBuffer, false, color);
	return true;
}

bool COpenGLDriver::setRenderTargetArray(video::ITexture* texture, int arrayID, bool clearTarget, bool clearZBuffer, SColor color)
{
	// check for right driver type
	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	if (texture && texture->getTextureType() != ETT_TEXTURE_ARRAY)
	{
		os::Printer::log("Fatal Error: Set render target is not texture cube", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back
	BridgeCalls->setActiveTexture(GL_TEXTURE0_ARB);
	ResetRenderStates = true;

	if (RenderTargetTexture != 0 && RenderTargetTexture != texture)
	{
		unbindRTT(RenderTargetTexture);
	}

	if (texture)
	{
		// we want to set a new target. so do this.
		BridgeCalls->setViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
		RenderTargetTexture = texture;

		
		COpenGLTextureArray *rtt = static_cast<COpenGLTextureArray*>(texture);
		RenderTargetTexture = rtt;

		if (rtt->isFrameBufferObject())
		{
			ITexture* renderBuffer = createDepthTexture(RenderTargetTexture, true);
			static_cast<COpenGLTextureArray*>(RenderTargetTexture)->setDepthTexture(renderBuffer);
			renderBuffer->drop();
		}

		// calls glDrawBuffer as well
		rtt->bindRTT(arrayID);

		CurrentRendertargetSize = texture->getSize();
		CurrentTarget = ERT_RENDER_TEXTURE;
	}
	else
	{
		BridgeCalls->setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
		RenderTargetTexture = 0;
		CurrentRendertargetSize = core::dimension2d<u32>(0, 0);
		glDrawBuffer(Params.Doublebuffer ? GL_BACK_LEFT : GL_FRONT_LEFT);
	}

	clearBuffers(clearTarget, clearZBuffer, false, color);
	return true;
}

//! Sets multiple render targets
bool COpenGLDriver::setRenderTarget(const core::array<video::IRenderTarget>& targets, bool clearBackBuffer, bool clearZBuffer, SColor color, video::ITexture* depthStencil)
{
	// if simply disabling the MRT via array call
	if (targets.size()==0)
		return setRenderTarget(0, clearBackBuffer, clearZBuffer, color, depthStencil);
	// if disabling old MRT, but enabling new one as well
	if ((MRTargets.size()!=0) && (targets != MRTargets))
		setRenderTarget(0, clearBackBuffer, clearZBuffer, color, depthStencil);
	// if no change, simply clear buffers
	else if (targets == MRTargets)
	{
		clearBuffers(clearBackBuffer, clearZBuffer, false, color);
		return true;
	}

	// copy to storage for correct disabling
	MRTargets=targets;

	u32 maxMultipleRTTs = core::min_(static_cast<u32>(MaxMultipleRenderTargets), targets.size());

	// determine common size
	core::dimension2du rttSize = CurrentRendertargetSize;
	if (targets[0].TargetType==ERT_RENDER_TEXTURE)
	{
		if (!targets[0].RenderTexture)
		{
			os::Printer::log("Missing render texture for MRT.", ELL_ERROR);
			return false;
		}
		rttSize=targets[0].RenderTexture->getSize();
	}

	for (u32 i = 0; i < maxMultipleRTTs; ++i)
	{
		// check for right driver type
		if (targets[i].TargetType==ERT_RENDER_TEXTURE)
		{
			if (!targets[i].RenderTexture)
			{
				maxMultipleRTTs=i;
				os::Printer::log("Missing render texture for MRT.", ELL_WARNING);
				break;
			}
			if (targets[i].RenderTexture->getDriverType() != EDT_OPENGL)
			{
				maxMultipleRTTs=i;
				os::Printer::log("Tried to set a texture not owned by this driver.", ELL_WARNING);
				break;
			}

			// check for valid render target
			if (!targets[i].RenderTexture->isRenderTarget() || !static_cast<COpenGLTexture*>(targets[i].RenderTexture)->isFrameBufferObject())
			{
				maxMultipleRTTs=i;
				os::Printer::log("Tried to set a non FBO-RTT as render target.", ELL_WARNING);
				break;
			}

			// check for valid size
			if (rttSize != targets[i].RenderTexture->getSize())
			{
				maxMultipleRTTs=i;
				os::Printer::log("Render target texture has wrong size.", ELL_WARNING);
				break;
			}
		}
	}
	if (maxMultipleRTTs==0)
	{
		os::Printer::log("No valid MRTs.", ELL_ERROR);
		return false;
	}

	// init FBO, if any
	for (u32 i=0; i<maxMultipleRTTs; ++i)
	{
		if (targets[i].TargetType==ERT_RENDER_TEXTURE)
		{
			setRenderTarget(targets[i].RenderTexture, false, false, 0x0, depthStencil);
			break; // bind only first RTT
		}
	}
	// init other main buffer, if necessary
	if (targets[0].TargetType!=ERT_RENDER_TEXTURE)
		setRenderTarget(targets[0].TargetType, false, false, 0x0, depthStencil);

	// attach other textures and store buffers into array
	if (maxMultipleRTTs > 1)
	{
		CurrentTarget=ERT_MULTI_RENDER_TEXTURES;
		core::array<GLenum> MRTs;
		MRTs.set_used(maxMultipleRTTs);
		for(u32 i = 0; i < maxMultipleRTTs; i++)
		{
			if (FeatureAvailable[IRR_EXT_draw_buffers2])
			{
				BridgeCalls->setColorMaskIndexed(i,
					(targets[i].ColorMask & ECP_RED)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_GREEN)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_BLUE)?GL_TRUE:GL_FALSE,
					(targets[i].ColorMask & ECP_ALPHA)?GL_TRUE:GL_FALSE);

				if (targets[i].BlendOp==EBO_NONE)
					BridgeCalls->setBlendIndexed(i, false);
				else
					BridgeCalls->setBlendIndexed(i, true);
			}
#if defined(GL_AMD_draw_buffers_blend) || defined(GL_ARB_draw_buffers_blend)
			if (FeatureAvailable[IRR_AMD_draw_buffers_blend] || FeatureAvailable[IRR_ARB_draw_buffers_blend])
			{
				BridgeCalls->setBlendFuncIndexed(i, getGLBlend(targets[i].BlendFuncSrc), getGLBlend(targets[i].BlendFuncDst));
				switch(targets[i].BlendOp)
				{
				case EBO_SUBTRACT:
					BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_SUBTRACT);
					break;
				case EBO_REVSUBTRACT:
					BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_REVERSE_SUBTRACT);
					break;
				case EBO_MIN:
					BridgeCalls->setBlendEquationIndexed(i, GL_MIN);
					break;
				case EBO_MAX:
					BridgeCalls->setBlendEquationIndexed(i, GL_MAX);
					break;
				case EBO_MIN_FACTOR:
				case EBO_MIN_ALPHA:
#if defined(GL_AMD_blend_minmax_factor)
					if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
						BridgeCalls->setBlendEquationIndexed(i, GL_FACTOR_MIN_AMD);
					// fallback in case of missing extension
					else
#endif
						BridgeCalls->setBlendEquationIndexed(i, GL_MIN);
					break;
				case EBO_MAX_FACTOR:
				case EBO_MAX_ALPHA:
#if defined(GL_AMD_blend_minmax_factor)
					if (FeatureAvailable[IRR_AMD_blend_minmax_factor])
						BridgeCalls->setBlendEquationIndexed(i, GL_FACTOR_MAX_AMD);
					// fallback in case of missing extension
					else
#endif
						BridgeCalls->setBlendEquationIndexed(i, GL_MAX);
				break;
				default:
					BridgeCalls->setBlendEquationIndexed(i, GL_FUNC_ADD);
					break;
				}
			}
#endif
			if (targets[i].TargetType==ERT_RENDER_TEXTURE)
			{
				GLenum attachment = GL_NONE;
#ifdef GL_EXT_framebuffer_object
				// attach texture to FrameBuffer Object on Color [i]
				attachment = GL_COLOR_ATTACHMENT0_EXT+i;
				if ((i != 0) && (targets[i].RenderTexture != RenderTargetTexture))
					extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, static_cast<COpenGLTexture*>(targets[i].RenderTexture)->getOpenGLTextureName(), 0);
#endif
				MRTs[i]=attachment;
			}
			else
			{
				switch(targets[i].TargetType)
				{
					case ERT_FRAME_BUFFER:
						MRTs[i]=GL_BACK_LEFT;
						break;
					case ERT_STEREO_BOTH_BUFFERS:
						MRTs[i]=GL_BACK;
						break;
					case ERT_STEREO_RIGHT_BUFFER:
						MRTs[i]=GL_BACK_RIGHT;
						break;
					case ERT_STEREO_LEFT_BUFFER:
						MRTs[i]=GL_BACK_LEFT;
						break;
					default:
						MRTs[i]=GL_AUX0+(targets[i].TargetType-ERT_AUX_BUFFER0);
						break;
				}
			}
		}

		extGlDrawBuffers(maxMultipleRTTs, MRTs.const_pointer());
	}

	clearBuffers(clearBackBuffer, clearZBuffer, false, color);
	return true;
}


//! set or reset render target
bool COpenGLDriver::setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget, bool clearZBuffer, SColor color)
{
	if (target != CurrentTarget)
		setRenderTarget(0, false, false, 0x0, 0);

	if (ERT_RENDER_TEXTURE == target)
	{
		os::Printer::log("For render textures call setRenderTarget with the actual texture as first parameter.", ELL_ERROR);
		return false;
	}
	if (ERT_MULTI_RENDER_TEXTURES == target)
	{
		os::Printer::log("For multiple render textures call setRenderTarget with the texture array as first parameter.", ELL_ERROR);
		return false;
	}

	if (Params.Stereobuffer && (ERT_STEREO_RIGHT_BUFFER == target))
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK_RIGHT);
		else
			glDrawBuffer(GL_FRONT_RIGHT);
	}
	else if (Params.Stereobuffer && ERT_STEREO_BOTH_BUFFERS == target)
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK);
		else
			glDrawBuffer(GL_FRONT);
	}
	else if ((target >= ERT_AUX_BUFFER0) && (target-ERT_AUX_BUFFER0 < MaxAuxBuffers))
	{
			glDrawBuffer(GL_AUX0+target-ERT_AUX_BUFFER0);
	}
	else
	{
		if (Params.Doublebuffer)
			glDrawBuffer(GL_BACK_LEFT);
		else
			glDrawBuffer(GL_FRONT_LEFT);
		// exit with false, but also with working color buffer
		if (target != ERT_FRAME_BUFFER)
			return false;
	}
	CurrentTarget=target;
	clearBuffers(clearTarget, clearZBuffer, false, color);
	return true;
}


// returns the current size of the screen or rendertarget
const core::dimension2d<u32>& COpenGLDriver::getCurrentRenderTargetSize() const
{
	if (CurrentRendertargetSize.Width == 0)
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}


//! Clears the ZBuffer.
void COpenGLDriver::clearZBuffer()
{
	clearBuffers(false, true, false, 0x0);
}


//! Returns an image created from the last rendered frame.
IImage* COpenGLDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target != video::ERT_FRAME_BUFFER)
		return 0;

	if (format==video::ECF_UNKNOWN)
		format=getColorFormat();

	if (IImage::isRenderTargetOnlyFormat(format) || IImage::isCompressedFormat(format) || IImage::isDepthFormat(format))
		return 0;

	// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
		glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

	GLenum fmt;
	GLenum type;

	switch (format)
	{
	case ECF_A1R5G5B5:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case ECF_R5G6B5:
		fmt = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case ECF_R8G8B8:
		fmt = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case ECF_A8R8G8B8:
		fmt = GL_BGRA;
		if (Version > 101)
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
		else
			type = GL_UNSIGNED_BYTE;
		break;
	default:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	}
	IImage* newImage = createImage(format, ScreenSize);

	u8* pixels = 0;
	if (newImage)
		pixels = static_cast<u8*>(newImage->lock());
	if (pixels)
	{
		GLenum tgt=GL_FRONT;
		switch (target)
		{
		case video::ERT_FRAME_BUFFER:
			break;
		case video::ERT_STEREO_LEFT_BUFFER:
			tgt=GL_FRONT_LEFT;
			break;
		case video::ERT_STEREO_RIGHT_BUFFER:
			tgt=GL_FRONT_RIGHT;
			break;
		default:
			tgt=GL_AUX0+(target-video::ERT_AUX_BUFFER0);
			break;
		}
		glReadBuffer(tgt);
		glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, fmt, type, pixels);
		testGLError();
		glReadBuffer(GL_BACK);
	}

#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
		glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
	else
#endif
	if (pixels)
	{
		// opengl images are horizontally flipped, so we have to fix that here.
		const s32 pitch=newImage->getPitch();
		u8* p2 = pixels + (ScreenSize.Height - 1) * pitch;
		u8* tmpBuffer = new u8[pitch];
		for (u32 i=0; i < ScreenSize.Height; i += 2)
		{
			memcpy(tmpBuffer, pixels, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				pixels[j]=(u8)(p2[j]*255.f);
//			}
			memcpy(pixels, p2, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				p2[j]=(u8)(tmpBuffer[j]*255.f);
//			}
			memcpy(p2, tmpBuffer, pitch);
			pixels += pitch;
			p2 -= pitch;
		}
		delete [] tmpBuffer;
	}

	if (newImage)
	{
		newImage->unlock();
		if (testGLError() || !pixels)
		{
			newImage->drop();
			return 0;
		}
	}
	return newImage;
}


//! get depth texture for the given render target texture
ITexture* COpenGLDriver::createDepthTexture(ITexture* texture, bool shared)
{
	if ((texture->getDriverType() != EDT_OPENGL) || (!texture->isRenderTarget()))
		return 0;

	if (shared)
	{
		for (u32 i=0; i<DepthTextures.size(); ++i)
		{
			if (DepthTextures[i]->getSize()==texture->getSize())
			{
				DepthTextures[i]->grab();
				return DepthTextures[i];
			}
		}
		DepthTextures.push_back(new COpenGLRenderBuffer(texture->getSize(), "depth1", this));
		return DepthTextures.getLast();
	}
	return (new COpenGLRenderBuffer(texture->getSize(), "depth1", this));
}


void COpenGLDriver::removeDepthTexture(ITexture* texture)
{
	for (u32 i=0; i<DepthTextures.size(); ++i)
	{
		if (texture==DepthTextures[i])
		{
			DepthTextures.erase(i);
			return;
		}
	}
}


IVertexDescriptor* COpenGLDriver::addVertexDescriptor(const core::stringc& pName)
{
	for(u32 i = 0; i < VertexDescriptor.size(); ++i)
		if(pName == VertexDescriptor[i]->getName())
			return VertexDescriptor[i];

	IVertexDescriptor* vertexDescriptor = new COpenGLVertexDescriptor(pName, VertexDescriptor.size(), MaterialRenderers.size());
	VertexDescriptor.push_back(vertexDescriptor);

	return vertexDescriptor;
}


void COpenGLDriver::setVertexDescriptor(IVertexDescriptor* vertexDescriptor)
{
	if(LastVertexDescriptor != vertexDescriptor)
	{
		u32 ID = 0;

		for(u32 i = 0; i < VertexDescriptor.size(); ++i)
		{
			if(vertexDescriptor == VertexDescriptor[i])
			{
				ID = i;
				LastVertexDescriptor = VertexDescriptor[ID];
				break;
			}
		}

		// TODO
	}
}


//! Set/unset a clipping plane.
bool COpenGLDriver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return false;

	UserClipPlanes[index].Plane=plane;
	enableClipPlane(index, enable);
	return true;
}


void COpenGLDriver::uploadClipPlane(u32 index)
{
	// opengl needs an array of doubles for the plane equation
	GLdouble clip_plane[4];
	clip_plane[0] = UserClipPlanes[index].Plane.Normal.X;
	clip_plane[1] = UserClipPlanes[index].Plane.Normal.Y;
	clip_plane[2] = UserClipPlanes[index].Plane.Normal.Z;
	clip_plane[3] = UserClipPlanes[index].Plane.D;
	glClipPlane(GL_CLIP_PLANE0 + index, clip_plane);
}


//! Enable/disable a clipping plane.
void COpenGLDriver::enableClipPlane(u32 index, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return;
	if (enable)
	{
		if (!UserClipPlanes[index].Enabled)
		{
			uploadClipPlane(index);
			glEnable(GL_CLIP_PLANE0 + index);
		}
	}
	else
		glDisable(GL_CLIP_PLANE0 + index);

	UserClipPlanes[index].Enabled=enable;
}


core::dimension2du COpenGLDriver::getMaxTextureSize() const
{
	return core::dimension2du(MaxTextureSize, MaxTextureSize);
}


//! Convert E_PRIMITIVE_TYPE to OpenGL equivalent
GLenum COpenGLDriver::primitiveTypeToGL(scene::E_PRIMITIVE_TYPE type) const
{
	switch (type)
	{
		case scene::EPT_POINTS:
			return GL_POINTS;
		case scene::EPT_LINE_STRIP:
			return GL_LINE_STRIP;
		case scene::EPT_LINE_LOOP:
			return GL_LINE_LOOP;
		case scene::EPT_LINES:
			return GL_LINES;
		case scene::EPT_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case scene::EPT_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		case scene::EPT_TRIANGLES:
			return GL_TRIANGLES;
		case scene::EPT_POINT_SPRITES:
#ifdef GL_ARB_point_sprite
			return GL_POINT_SPRITE_ARB;
#else
			return GL_POINTS;
#endif
	}
	return GL_TRIANGLES;
}


GLenum COpenGLDriver::getGLBlend(E_BLEND_FACTOR factor) const
{
	GLenum r = 0;
	switch (factor)
	{
		case EBF_ZERO:			r = GL_ZERO; break;
		case EBF_ONE:			r = GL_ONE; break;
		case EBF_DST_COLOR:		r = GL_DST_COLOR; break;
		case EBF_ONE_MINUS_DST_COLOR:	r = GL_ONE_MINUS_DST_COLOR; break;
		case EBF_SRC_COLOR:		r = GL_SRC_COLOR; break;
		case EBF_ONE_MINUS_SRC_COLOR:	r = GL_ONE_MINUS_SRC_COLOR; break;
		case EBF_SRC_ALPHA:		r = GL_SRC_ALPHA; break;
		case EBF_ONE_MINUS_SRC_ALPHA:	r = GL_ONE_MINUS_SRC_ALPHA; break;
		case EBF_DST_ALPHA:		r = GL_DST_ALPHA; break;
		case EBF_ONE_MINUS_DST_ALPHA:	r = GL_ONE_MINUS_DST_ALPHA; break;
		case EBF_SRC_ALPHA_SATURATE:	r = GL_SRC_ALPHA_SATURATE; break;
	}
	return r;
}

GLuint COpenGLDriver::getActiveGLSLProgram()
{
	return ActiveGLSLProgram;
}

void COpenGLDriver::setActiveGLSLProgram(GLuint program)
{
	ActiveGLSLProgram = program;
}

GLenum COpenGLDriver::getZBufferBits() const
{
	GLenum bits = 0;
	switch (Params.ZBufferBits)
	{
	case 16:
		bits = GL_DEPTH_COMPONENT16;
		break;
	case 24:
		bits = GL_DEPTH_COMPONENT24;
		break;
	case 32:
		bits = GL_DEPTH_COMPONENT32;
		break;
	default:
		bits = GL_DEPTH_COMPONENT;
		break;
	}
	return bits;
}

const SMaterial& COpenGLDriver::getCurrentMaterial() const
{
	return Material;
}

COpenGLCallBridge* COpenGLDriver::getBridgeCalls() const
{
	return BridgeCalls;
}

#ifdef _IRR_COMPILE_WITH_CG_
const CGcontext& COpenGLDriver::getCgContext()
{
	return CgContext;
}
#endif

COpenGLCallBridge::COpenGLCallBridge(COpenGLDriver* driver) : Driver(driver),
	AlphaMode(GL_ALWAYS), AlphaRef(0.0f), AlphaTest(false),
	ClientStateVertex(false), ClientStateNormal(false), ClientStateColor(false),
	CullFaceMode(GL_BACK), CullFace(false), EnableScissor(false),
	DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), MatrixMode(GL_MODELVIEW),
	ActiveTexture(GL_TEXTURE0_ARB), ClientActiveTexture(GL_TEXTURE0_ARB), ViewportX(0), ViewportY(0), ScissorX(0), ScissorY(0)
{
	FrameBufferCount = core::max_(static_cast<GLuint>(1), static_cast<GLuint>(Driver->MaxMultipleRenderTargets));

	BlendEquation = new GLenum[FrameBufferCount];
	BlendSourceRGB = new GLenum[FrameBufferCount];
	BlendDestinationRGB = new GLenum[FrameBufferCount];
	BlendSourceAlpha = new GLenum[FrameBufferCount];
	BlendDestinationAlpha = new GLenum[FrameBufferCount];
	Blend = new bool[FrameBufferCount];

	ColorMask = new bool[FrameBufferCount][4];

	for (u32 i = 0; i < FrameBufferCount; ++i)
	{
#if defined(GL_VERSION_1_4)
		BlendEquation[i] = GL_FUNC_ADD;
#elif defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
		BlendEquation[i] = GL_FUNC_ADD_EXT;
#endif

		BlendSourceRGB[i] = GL_ONE;
		BlendDestinationRGB[i] = GL_ZERO;
		BlendSourceAlpha[i] = GL_ONE;
		BlendDestinationAlpha[i] = GL_ZERO;

		Blend[i] = false;

		for (u32 j = 0; j < 4; ++j)
			ColorMask[i][j] = true;
	}

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		Texture[i] = 0;
	}

	// Initial OpenGL values from specification.

	glAlphaFunc(GL_ALWAYS, 0.0f);
	glDisable(GL_ALPHA_TEST);

	if (Driver->queryFeature(EVDF_BLEND_OPERATIONS))
	{
#if defined(GL_VERSION_1_4)
		Driver->extGlBlendEquation(GL_FUNC_ADD);
#elif defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
		Driver->extGlBlendEquation(GL_FUNC_ADD_EXT);
#endif
	}

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);

	if(Driver->MultiTextureExtension)
	{
		Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
		Driver->extGlClientActiveTexture(GL_TEXTURE0_ARB);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
	ViewportWidth = ScreenSize.Width;
	ViewportHeight = ScreenSize.Height;
	glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);

	ScissorWidth = ViewportWidth;
	ScissorHeight = ViewportHeight;
}

COpenGLCallBridge::~COpenGLCallBridge()
{
	delete[] BlendEquation;
	delete[] BlendSourceRGB;
	delete[] BlendDestinationRGB;
    delete[] BlendSourceAlpha;
	delete[] BlendDestinationAlpha;
	delete[] Blend;

	delete[] ColorMask;
}

void COpenGLCallBridge::setAlphaFunc(GLenum mode, GLclampf ref)
{
	if(AlphaMode != mode || AlphaRef != ref)
	{
		glAlphaFunc(mode, ref);

		AlphaMode = mode;
		AlphaRef = ref;
	}
}

void COpenGLCallBridge::setAlphaTest(bool enable)
{
	if(AlphaTest != enable)
	{
		if (enable)
			glEnable(GL_ALPHA_TEST);
		else
			glDisable(GL_ALPHA_TEST);
		AlphaTest = enable;
	}
}

void COpenGLCallBridge::setBlendEquation(GLenum mode)
{
	if (BlendEquation[0] != mode)
	{
		Driver->extGlBlendEquation(mode);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
			BlendEquation[i] = mode;
	}
}

void COpenGLCallBridge::setBlendEquationIndexed(GLuint index, GLenum mode)
{
	if (index < FrameBufferCount && BlendEquation[index] != mode)
	{
		Driver->extGlBlendEquationIndexed(index, mode);

		BlendEquation[index] = mode;
	}
}

void COpenGLCallBridge::setBlendFunc(GLenum source, GLenum destination)
{
	if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
        BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination)
	{
		glBlendFunc(source, destination);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
		{
			BlendSourceRGB[i] = source;
			BlendDestinationRGB[i] = destination;
			BlendSourceAlpha[i] = source;
			BlendDestinationAlpha[i] = destination;
		}
	}
}

void COpenGLCallBridge::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
    if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
    {
        if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
            BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha)
        {
            Driver->extGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

            for (GLuint i = 0; i < FrameBufferCount; ++i)
            {
                BlendSourceRGB[i] = sourceRGB;
                BlendDestinationRGB[i] = destinationRGB;
                BlendSourceAlpha[i] = sourceAlpha;
                BlendDestinationAlpha[i] = destinationAlpha;
            }
        }
    }
    else
    {
        setBlendFunc(sourceRGB, destinationRGB);
    }
}

void COpenGLCallBridge::setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination)
{
	if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
        BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
	{
		Driver->extGlBlendFuncIndexed(index, source, destination);

        BlendSourceRGB[index] = source;
        BlendDestinationRGB[index] = destination;
        BlendSourceAlpha[index] = source;
        BlendDestinationAlpha[index] = destination;
	}
}

void COpenGLCallBridge::setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
    if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
    {
        if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
            BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
        {
            Driver->extGlBlendFuncSeparateIndexed(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

            BlendSourceRGB[index] = sourceRGB;
            BlendDestinationRGB[index] = destinationRGB;
            BlendSourceAlpha[index] = sourceAlpha;
            BlendDestinationAlpha[index] = destinationAlpha;
        }
    }
    else
    {
        setBlendFuncIndexed(index, sourceRGB, destinationRGB);
    }
}

void COpenGLCallBridge::setBlend(bool enable)
{
	if (Blend[0] != enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
			Blend[i] = enable;
	}
}

void COpenGLCallBridge::setBlendIndexed(GLuint index, bool enable)
{
	if (index < FrameBufferCount && Blend[index] != enable)
	{
		if (enable)
			Driver->extGlEnableIndexed(GL_BLEND, index);
		else
			Driver->extGlDisableIndexed(GL_BLEND, index);

		Blend[index] = enable;
	}
}

void COpenGLCallBridge::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	if (ColorMask[0][0] != red || ColorMask[0][1] != green || ColorMask[0][2] != blue || ColorMask[0][3] != alpha)
	{
		glColorMask(red, green, blue, alpha);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
		{
			ColorMask[i][0] = red;
			ColorMask[i][1] = green;
			ColorMask[i][2] = blue;
			ColorMask[i][3] = alpha;
		}
	}
}

void COpenGLCallBridge::setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha)
{
	if (index < FrameBufferCount && (ColorMask[index][0] != red || ColorMask[index][1] != green || ColorMask[index][2] != blue || ColorMask[index][3] != alpha))
	{
		Driver->extGlColorMaskIndexed(index, red, green, blue, alpha);

		ColorMask[index][0] = red;
		ColorMask[index][1] = green;
		ColorMask[index][2] = blue;
		ColorMask[index][3] = alpha;
	}
}

void COpenGLCallBridge::setClientState(bool vertex, bool normal, bool color)
{
	if(ClientStateVertex != vertex)
	{
		if(vertex)
			glEnableClientState(GL_VERTEX_ARRAY);
		else
			glDisableClientState(GL_VERTEX_ARRAY);

		ClientStateVertex = vertex;
	}

	if(ClientStateNormal != normal)
	{
		if(normal)
			glEnableClientState(GL_NORMAL_ARRAY);
		else
			glDisableClientState(GL_NORMAL_ARRAY);

		ClientStateNormal = normal;
	}

	if(ClientStateColor != color)
	{
		if(color)
			glEnableClientState(GL_COLOR_ARRAY);
		else
			glDisableClientState(GL_COLOR_ARRAY);

		ClientStateColor = color;
	}
}

void COpenGLCallBridge::setCullFaceFunc(GLenum mode)
{
	if(CullFaceMode != mode)
	{
		glCullFace(mode);
		CullFaceMode = mode;
	}
}

void COpenGLCallBridge::setCullFace(bool enable)
{
	if(CullFace != enable)
	{
		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		CullFace = enable;
	}
}

void COpenGLCallBridge::setDepthFunc(GLenum mode)
{
	if(DepthFunc != mode)
	{
		glDepthFunc(mode);
		DepthFunc = mode;
	}
}

void COpenGLCallBridge::setDepthMask(bool enable)
{
	if(DepthMask != enable)
	{
		if (enable)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		DepthMask = enable;
	}
}

void COpenGLCallBridge::setDepthTest(bool enable)
{
	if(DepthTest != enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		DepthTest = enable;
	}
}

void COpenGLCallBridge::enableScissor(bool enable)
{
	if (EnableScissor != enable)
	{
		if (enable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
		EnableScissor = enable;
	}
}

void COpenGLCallBridge::setScissor(GLint scissorX, GLint scissorY, GLsizei scissorWidth, GLsizei scissorHeight)
{
	if (ScissorX != scissorX || ScissorY != scissorY || ScissorWidth != scissorWidth || ScissorHeight != scissorHeight)
	{
		glScissor(scissorX, scissorY, scissorWidth, scissorHeight);
		ScissorX = scissorX;
		ScissorY = scissorY;
		ScissorWidth = scissorWidth;
		ScissorHeight = scissorHeight;
	}
}

void COpenGLCallBridge::setMatrixMode(GLenum mode)
{
	if (MatrixMode != mode)
	{
		glMatrixMode(mode);
		MatrixMode = mode;
	}
}

void COpenGLCallBridge::setActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ActiveTexture != texture)
	{
		Driver->extGlActiveTexture(texture);
		ActiveTexture = texture;
	}
}

void COpenGLCallBridge::setClientActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ClientActiveTexture != texture)
	{
		Driver->extGlClientActiveTexture(texture);
		ClientActiveTexture = texture;
	}
}

void COpenGLCallBridge::resetTexture(const ITexture* texture)
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		setActiveTexture(GL_TEXTURE0_ARB + i);

		if (Texture[i] == texture)
		{
			glBindTexture(GL_TEXTURE_2D, 0);			
			Texture[i] = 0;
		}
	}
}

void COpenGLCallBridge::setTexture(GLuint stage)
{
	if (stage < MATERIAL_MAX_TEXTURES)
	{
		setActiveTexture(GL_TEXTURE0_ARB + stage);

		if(Texture[stage] != Driver->CurrentTexture[stage])
		{
			if (Driver->CurrentTexture[stage] != NULL)
			{
				if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_2D)
				{
					// texture 2d
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, static_cast<const COpenGLTexture*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
				}
				else if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_CUBE)
				{
					// texture cube
					glEnable(GL_TEXTURE_CUBE_MAP);
					glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<const COpenGLTextureCube*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
				}
				else if (Driver->CurrentTexture[stage]->getTextureType() == ETT_TEXTURE_ARRAY)
				{
					// texture array					
					glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<const COpenGLTextureArray*>(Driver->CurrentTexture[stage])->getOpenGLTextureName());
				}

				// todo profile
				Driver->incTextureChange();
			}

			Texture[stage] = Driver->CurrentTexture[stage];
		}
	}
}

void COpenGLCallBridge::setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
{
	if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
	{
		glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
		ViewportX = viewportX;
		ViewportY = viewportY;
		ViewportWidth = viewportWidth;
		ViewportHeight = viewportHeight;
	}
}


} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_

namespace irr
{
namespace video
{

IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  new COpenGLDriver(params, io);
	if (!ogl->initDriver())
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OPENGL_
}

} // end namespace
} // end namespace


