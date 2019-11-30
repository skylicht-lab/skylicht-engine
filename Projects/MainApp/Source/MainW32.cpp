#include "pch.h"

#if defined(WIN32) || defined(CYGWIN) || defined(MINGW)
#include <Windows.h>

#include "Resource.h"
#include "CApplication.h"
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
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

CApplication		*g_application;
bool				g_restartApplication = false;
bool				g_show2D = true;
bool				g_update = true;

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
	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_SKYLICHTDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

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
	//core::dimension2du winSize(960, 640);
	//core::dimension2du winSize(800, 600);
	core::dimension2du winSize(1280, 800);
	//core::dimension2du winSize(1600, 850);
	//core::dimension2du winSize(854, 480);
	//core::dimension2du winSize(800, 480);
	//core::dimension2du winSize(1024, 768);
	//core::dimension2du winSize(480, 320);
	//core::dimension2du winSize(320, 240);

	if (params.size() >= 2 && params.size() <= 3)
	{
		winSize.Width = atoi(params[0].c_str());
		winSize.Height = atoi(params[1].c_str());
	}

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = winSize.Width;
	clientSize.bottom = winSize.Height;

	AdjustWindowRect(&clientSize, hWndStyle, TRUE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if (windowLeft < 0)
		windowLeft = 0;
	if (windowTop < 0)
		windowTop = 0;

	MoveWindow(hWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	// init application
	g_application = new CApplication();

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

	if (params.size() == 3)
		p.Fullscreen = true;

	if (useDX11 == IDYES)
		p.AntiAlias = 1;

	// create device
	IrrlichtDevice *device = irr::createDeviceEx(p);

	if (device == NULL)
		return 1;

	// init application
	g_application->initApplication(device);

	g_application->notifyResizeWin(winSize.Width, winSize.Height);

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

		if (g_update == true)
			g_application->mainLoop();

		if (g_restartApplication == true)
		{
			// need restart application
			g_application->destroyApplication();
			delete g_application;

			// renew application
			g_application = new CApplication();

			device->setEventReceiver(g_application);

			g_application->initApplication(device);
			g_restartApplication = false;
		}
	}

	g_application->destroyApplication();
	delete g_application;

	device->drop();

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
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

bool g_mouseDown = false;

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	struct messageMap
	{
		s32		Group;
		UINT	WinMessage;
		CTouchManager::ETouchEvent	TouchEvent;
	};

	static messageMap mouseMap[] =
	{
		{ 0, WM_LBUTTONDOWN, CTouchManager::TouchDown },
		{ 1, WM_LBUTTONUP,   CTouchManager::TouchUp },
		{ 2, WM_MOUSEMOVE,   CTouchManager::TouchMove },
		{ -1, 0, CTouchManager::TouchNone }
	};

	// handle grouped events
	messageMap *m = mouseMap;
	while (m->Group >= 0 && m->WinMessage != message)
		m += 1;

	if (m->Group >= 0)
	{
		if (m->Group == 0)	// down
		{
			g_mouseDown = true;
			SetCapture(hWnd);
		}
		else if (m->Group == 1)	// up
		{
			g_mouseDown = false;
			ReleaseCapture();
		}

		int touchX = (int)((short)LOWORD(lParam));
		int touchY = (int)((short)HIWORD(lParam));

		if (m->TouchEvent == CTouchManager::TouchMove)
		{
			if (g_mouseDown == true)
				CTouchManager::getInstance()->touchEvent(m->TouchEvent, touchX, touchY, 0);
		}
		else
			CTouchManager::getInstance()->touchEvent(m->TouchEvent, touchX, touchY, 0);

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
	case WM_DESTROY:
		PostQuitMessage(0);
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

		// todo pause game
		if (event.KeyInput.PressedDown == false && event.KeyInput.Key == irr::KEY_F9)
			g_update = !g_update;

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
		WORD keyChars[2];
		UINT scanCode = HIWORD(lParam);
		int conversionResult = ToAsciiEx(wParam, scanCode, allKeys, keyChars, 0, KEYBOARD_INPUT_HKL);
		if (conversionResult == 1)
		{
			WORD unicodeChar;
			MultiByteToWideChar(
				KEYBOARD_INPUT_CODEPAGE,
				MB_PRECOMPOSED, // default
				(LPCSTR)keyChars,
				sizeof(keyChars),
				(WCHAR*)&unicodeChar,
				1);
			event.KeyInput.Char = unicodeChar;
		}
		else
			event.KeyInput.Char = 0;

		// allow composing characters like '@' with Alt Gr on non-US keyboards
		if ((allKeys[VK_MENU] & 0x80) != 0)
			event.KeyInput.Control = 0;

		IrrlichtDevice *dev = getIrrlichtDevice();
		if (dev)
			dev->postEventFromUser(event);

		if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
			return DefWindowProc(hWnd, message, wParam, lParam);
		else
			return 0;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#endif