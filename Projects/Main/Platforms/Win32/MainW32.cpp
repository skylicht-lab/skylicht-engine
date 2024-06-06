/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"

#if !defined(SDL)

#if !defined(WINDOWS_STORE) && (defined(WIN32) || defined(CYGWIN) || defined(MINGW))
#include <Windows.h>

#include "Resource.h"
#include "CApplication.h"
#include "CWindowConfig.h"
#include "BuildConfig/CBuildConfig.h"
#include "Utils/CStringImp.h"
#include "Control/CTouchManager.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND hWnd;
DWORD hWndStyle;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CApplication* g_application;
bool g_restartApplication = false;
bool g_update = true;
float g_displayScale = 1.0f;

void installApplication(const std::vector<std::string>& argv);

#if defined(CYGWIN) || defined(MINGW)
int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
#else
// Visual Studio Main Function
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
#endif
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	int useDX11 = MessageBox(hWnd, L"Use directX11 driver?", L"Question", MB_YESNO | MB_ICONQUESTION);
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_SKYLICHT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int show = nCmdShow;

#if defined(SKYLICHT_EDITOR)
	// read size from last session
	u32 x, y, w, h;
	bool maximize = false;
	bool haveConfig = false;
	haveConfig = CWindowConfig::loadConfig(x, y, w, h, maximize);
#endif

	// Get screen scale (windows 10)
	HDC screenDC = GetDC(NULL);
	int virtualWidth = GetDeviceCaps(screenDC, HORZRES);
	int physicalWidth = GetDeviceCaps(screenDC, DESKTOPHORZRES);
	g_displayScale = physicalWidth / (float)virtualWidth;
	ReleaseDC(NULL, screenDC);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	std::vector<std::string> params;

#if defined(CYGWIN) || defined(MINGW)
	CStringImp::splitString(lpCmdLine, " ", params);
#else
	// need convert LPWSTR to LPSTR
	if (CStringImp::length<WCHAR>(lpCmdLine) > 0)
	{
		char s[512];
		CStringImp::convertUnicodeToUTF8(lpCmdLine, s);
		CStringImp::splitString(s, " ", params);
	}
#endif

	//core::dimension2du winSize(960, 540);
	//core::dimension2du winSize(960, 720);
	core::dimension2du winSize(960, 640);
	//core::dimension2du winSize(800, 600);
	//core::dimension2du winSize(1280, 800);
	//core::dimension2du winSize(1600, 850);
	//core::dimension2du winSize(854, 480);
	//core::dimension2du winSize(800, 480);
	//core::dimension2du winSize(1024, 768);
	//core::dimension2du winSize(480, 320);
	//core::dimension2du winSize(320, 240);

#if defined(SKYLICHT_EDITOR)
	if (haveConfig)
	{
		winSize.Width = w;
		winSize.Height = h;
	}
	else
	{
		winSize.Width = 1440;
		winSize.Height = 900;
	}
#endif

	if (params.size() >= 2)
	{
		winSize.Width = atoi(params[0].c_str());
		winSize.Height = atoi(params[1].c_str());
	}

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = winSize.Width;
	clientSize.bottom = winSize.Height;

	AdjustWindowRect(&clientSize, hWndStyle, FALSE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if (windowLeft < 0)
		windowLeft = 0;
	if (windowTop < 0)
		windowTop = 0;

#if defined(SKYLICHT_EDITOR)
	if (haveConfig)
	{
		windowLeft = x;
		windowTop = y;
	}
#endif

	MoveWindow(hWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	// init application
	g_application = new CApplication();
	g_application->setParams(params);

#if defined(SKYLICHT_EDITOR)
	g_application->enableWriteLog(true);
#endif

	// create irrlicht device
	SIrrlichtCreationParameters p;
	if (useDX11 == IDNO)
	{
#ifdef _IRR_COMPILE_WITH_OPENGL_
		p.DriverType = video::EDT_OPENGL;
#else
		p.DriverType = video::EDT_OPENGLES;
#endif
	}
	else
	{
		p.DriverType = video::EDT_DIRECT3D11;
	}

	p.WindowSize = winSize;
	p.Bits = (u8)32;
	p.ZBufferBits = (u8)32;
	p.Fullscreen = false;
	p.Stencilbuffer = false;
	p.Vsync = false;
	p.EventReceiver = g_application;
	p.AntiAlias = 0;
	p.WindowId = reinterpret_cast<void*>(hWnd);

	if (params.size() >= 3 && strcmp(params[2].c_str(), "true") == 0)
		p.Fullscreen = true;

	if (useDX11 == IDYES)
		p.AntiAlias = 1;

	// create device
	IrrlichtDevice* device = irr::createDeviceEx(p);

	if (device == NULL)
		return 1;

	// init application
	g_application->initApplication(device);

	g_application->setLimitFPS(60);

	installApplication(g_application->getParams());

	g_application->onInit();

#if defined(SKYLICHT_EDITOR)
	if (maximize)
		device->maximizeWindow();
#endif

	bool close = false;
	while (close == false)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// No message translation because we don't use WM_CHAR and it would conflict with our
			// deadkey handling.

			if (msg.hwnd == hWnd)
				WndProc(hWnd, msg.message, msg.wParam, msg.lParam);
			else
				DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				close = true;
		}

		if (g_update == true && !close)
			g_application->mainLoop();

		if (g_restartApplication == true)
		{
			// need restart application
			g_application->destroyApplication();
			delete g_application;

			// renew application
			g_application = new CApplication();

			g_application->initApplication(device);
			g_restartApplication = false;

			installApplication(g_application->getParams());

			g_application->onInit();
		}
	}

	g_application->destroyApplication();
	delete g_application;

	device->drop();

	CApplication::reportLeakMemory();

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SKYLICHT_DEMO));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SKYLICHT_DEMO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWndStyle = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// #if defined(SKYLICHT_EDITOR)
	hWndStyle |= WS_OVERLAPPEDWINDOW;
	// #endif

	hWnd = CreateWindow(szWindowClass, szTitle, hWndStyle,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

bool IsMainWindow(HWND h)
{
	return h == hWnd;
}

bool g_mouseDown = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static irr::s32 ClickCount = 0;
	if (GetCapture() != hWnd && ClickCount > 0)
		ClickCount = 0;

	struct messageMap
	{
		irr::s32 group;
		UINT winMessage;
		irr::s32 irrMessage;
	};

	static messageMap mouseMap[] =
	{
		{0, WM_LBUTTONDOWN, irr::EMIE_LMOUSE_PRESSED_DOWN},
		{1, WM_LBUTTONUP,   irr::EMIE_LMOUSE_LEFT_UP},
		{0, WM_RBUTTONDOWN, irr::EMIE_RMOUSE_PRESSED_DOWN},
		{1, WM_RBUTTONUP,   irr::EMIE_RMOUSE_LEFT_UP},
		{0, WM_MBUTTONDOWN, irr::EMIE_MMOUSE_PRESSED_DOWN},
		{1, WM_MBUTTONUP,   irr::EMIE_MMOUSE_LEFT_UP},
		{2, WM_MOUSEMOVE,   irr::EMIE_MOUSE_MOVED},
		{3, WM_MOUSEWHEEL,  irr::EMIE_MOUSE_WHEEL},
		{-1, 0, 0}
	};

	// handle grouped events
	messageMap* m = mouseMap;
	while (m->group >= 0 && m->winMessage != message)
		m += 1;

	irr::SEvent event;

	if (m->group >= 0)
	{
		if (m->group == 0)	// down
		{
			ClickCount++;
			SetCapture(hWnd);
		}
		else
			if (m->group == 1)	// up
			{
				ClickCount--;
				if (ClickCount < 1)
				{
					ClickCount = 0;
					ReleaseCapture();
				}
			}

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = (irr::EMOUSE_INPUT_EVENT)m->irrMessage;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		event.MouseInput.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
		event.MouseInput.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
		// left and right mouse buttons
		event.MouseInput.ButtonStates = wParam & (MK_LBUTTON | MK_RBUTTON);
		// middle and extra buttons
		if (wParam & MK_MBUTTON)
			event.MouseInput.ButtonStates |= irr::EMBSM_MIDDLE;
#if(_WIN32_WINNT >= 0x0500)
		if (wParam & MK_XBUTTON1)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA1;
		if (wParam & MK_XBUTTON2)
			event.MouseInput.ButtonStates |= irr::EMBSM_EXTRA2;
#endif
		event.MouseInput.Wheel = 0.f;

		// wheel
		if (m->group == 3)
		{
			POINT p; // fixed by jox
			p.x = 0; p.y = 0;
			ClientToScreen(hWnd, &p);
			event.MouseInput.X -= p.x;
			event.MouseInput.Y -= p.y;

			int nDelta = (int)wParam;
			if (nDelta > 0)
				event.MouseInput.Wheel = -1.0f;
			else
				event.MouseInput.Wheel = 1.0f;
		}

		IrrlichtDevice* dev = getIrrlichtDevice();
		if (dev)
			dev->postEventFromUser(event);
		return 0;
	}

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		if (IsMainWindow(hWnd) && g_application != NULL && g_application->onClose() == false)
			return (LRESULT)1;
		else
			return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
#if defined(SKYLICHT_EDITOR)
		RECT client;
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(hWnd, &wndpl);
		GetClientRect(hWnd, &client);
		CWindowConfig::saveConfig(
			wndpl.rcNormalPosition.left,
			wndpl.rcNormalPosition.top,
			client.right - client.left,
			client.bottom - client.top,
			wndpl.showCmd == SW_SHOWMAXIMIZED
		);
#endif
		PostQuitMessage(0);
		break;
	case WM_WINDOWPOSCHANGED:
#if defined(SKYLICHT_EDITOR)
		/*
		RECT client;
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(hWnd, &wndpl);
		GetClientRect(hWnd, &client);
		CWindowConfig::saveConfig(
			wndpl.rcNormalPosition.left,
			wndpl.rcNormalPosition.top,
			client.right - client.left,
			client.bottom - client.top,
			wndpl.showCmd == SW_SHOWMAXIMIZED
		);
		*/
#endif
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_SIZE:
	{
		u32 width = LOWORD(lParam);
		u32 height = HIWORD(lParam);
		IrrlichtDevice* dev = getIrrlichtDevice();
		if (width > 0 && height > 0)
		{
			if (dev != NULL)
			{
				SEvent resize;
				resize.EventType = irr::EET_GAME_RESIZE;
				resize.UserEvent.UserData1 = (s32)width;
				resize.UserEvent.UserData2 = (s32)height;
				dev->postEventFromUser(resize);
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		HKL KEYBOARD_INPUT_HKL = 0;
		unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;

		BYTE allKeys[256];
		SEvent event;

		event.EventType = irr::EET_KEY_INPUT_EVENT;
		event.KeyInput.Key = (irr::EKEY_CODE)wParam;
		event.KeyInput.PressedDown = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);

		const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.

		if (event.KeyInput.Key == irr::KEY_SHIFT)
		{
			// this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
			event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
		}
		if (event.KeyInput.Key == irr::KEY_CONTROL)
		{
			event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
			// some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
			if (lParam & 0x1000000)
				event.KeyInput.Key = irr::KEY_RCONTROL;
		}
		if (event.KeyInput.Key == irr::KEY_MENU)
		{
			event.KeyInput.Key = (irr::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
			if (lParam & 0x1000000)
				event.KeyInput.Key = irr::KEY_RMENU;
		}

		GetKeyboardState(allKeys);

		event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80) != 0);
		event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80) != 0);

		// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
		// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
		WCHAR keyChars[5];
		UINT scanCode = HIWORD(lParam);
		int conversionResult = ToUnicodeEx((UINT)wParam, scanCode, allKeys, keyChars, 5, 0, KEYBOARD_INPUT_HKL);
		if (conversionResult == 1)
		{
			WORD unicodeChar = keyChars[0];

			if (event.KeyInput.Control)
				unicodeChar += 64;

			event.KeyInput.Char = unicodeChar;
		}
		else
			event.KeyInput.Char = 0;

		// allow composing characters like '@' with Alt Gr on non-US keyboards
		if ((allKeys[VK_MENU] & 0x80) != 0)
			event.KeyInput.Control = 0;

		IrrlichtDevice* dev = getIrrlichtDevice();
		if (dev)
			dev->postEventFromUser(event);

		if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
			return DefWindowProc(hWnd, message, wParam, lParam);
		else
			return 0;
	}
	case WM_SETCURSOR:
	{
		// because Windows forgot about that in the meantime
		IrrlichtDevice* dev = getIrrlichtDevice();
		if (dev)
		{
			gui::ICursorControl* cursor = dev->getCursorControl();
			cursor->setVisible(cursor->isVisible());

			if (cursor->getActiveIcon() != gui::ECI_NORMAL)
				cursor->setActiveIcon(cursor->getActiveIcon());
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			if (g_application != NULL)
				g_application->pause();
		}
		else
		{
			if (g_application != NULL)
				g_application->resume();
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#endif

#endif