#include "pch.h"
#include "MainUWP.h"

#include <ppltasks.h>

using namespace MainApp;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

CApplication *m_application = NULL;
irr::IrrlichtDevice *m_device = NULL;

void installApplication(const std::vector<std::string>& argv);

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ applicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged +=
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed +=
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);
	window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerMoved);
	window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerRelease);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

	// handle core window
	m_coreWindow = window;
	m_dpi = currentDisplayInformation->LogicalDpi;
}

void App::OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ e)
{
	int id = (int)e->CurrentPoint->PointerId;
	int x = (int)ConvertDipsToPixels(e->CurrentPoint->Position.X, m_dpi);
	int y = (int)ConvertDipsToPixels(e->CurrentPoint->Position.Y, m_dpi);

	if (m_application != nullptr)
	{
		irr::SEvent	event;
		IrrlichtDevice *device = getIrrlichtDevice();

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.ID = id;
		event.MouseInput.X = x;
		event.MouseInput.Y = y;
		event.MouseInput.Shift = false;
		event.MouseInput.Control = false;
		event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;

		getIrrlichtDevice()->postEventFromUser(event);
	}
}

void App::OnPointerRelease(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ e)
{
	int id = (int)e->CurrentPoint->PointerId;
	int x = (int)ConvertDipsToPixels(e->CurrentPoint->Position.X, m_dpi);
	int y = (int)ConvertDipsToPixels(e->CurrentPoint->Position.Y, m_dpi);

	if (m_application != nullptr)
	{
		irr::SEvent	event;
		IrrlichtDevice *device = getIrrlichtDevice();

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.ID = id;
		event.MouseInput.X = x;
		event.MouseInput.Y = y;
		event.MouseInput.Shift = false;
		event.MouseInput.Control = false;
		event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;

		getIrrlichtDevice()->postEventFromUser(event);
	}
}

void App::OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ e)
{
	int id = (int)e->CurrentPoint->PointerId;
	int x = (int)ConvertDipsToPixels(e->CurrentPoint->Position.X, m_dpi);
	int y = (int)ConvertDipsToPixels(e->CurrentPoint->Position.Y, m_dpi);

	if (m_application != nullptr)
	{
		irr::SEvent	event;
		IrrlichtDevice *device = getIrrlichtDevice();

		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.ID = id;
		event.MouseInput.X = x;
		event.MouseInput.Y = y;
		event.MouseInput.Shift = false;
		event.MouseInput.Control = false;
		event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;

		getIrrlichtDevice()->postEventFromUser(event);
	}
}

core::dimension2du App::GetWindowPixelSize()
{
	core::dimension2du result;
	result.Width = lround(ConvertDipsToPixels(m_coreWindow->Bounds.Width, m_dpi));
	result.Height = lround(ConvertDipsToPixels(m_coreWindow->Bounds.Height, m_dpi));
	return result;
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String^ entryPoint)
{
	m_application = new CApplication();
	SIrrlichtCreationParameters p;
	p.DeviceType = irr::EIDT_PHONE;
	p.DriverType = video::EDT_DIRECT3D11;
	p.WindowSize = GetWindowPixelSize();
	p.Bits = (u8)32;
	p.ZBufferBits = (u8)32;
	p.Fullscreen = false;
	p.Stencilbuffer = false;
	p.Vsync = false;
	p.EventReceiver = m_application;
	p.AntiAlias = 0;
	p.WindowId = reinterpret_cast<void*>(m_coreWindow.Get());

	m_device = irr::createDeviceEx(p);

	if (m_device != NULL)
	{
		m_application->initApplication(m_device);
		m_application->setLimitFPS(60);
		installApplication(m_application->getParams());
		m_application->onInit();
	}
}

// This method is called after the window becomes active.
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			m_application->mainLoop();
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{

}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		// IDXGIDevice3::Trim

		// Insert your code here.

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	if (m_application != NULL)
	{
		core::dimension2du windowSize = core::dimension2du((u32)m_coreWindow->Bounds.Width, (u32)m_coreWindow->Bounds.Height);
		m_application->notifyResizeWin(windowSize.Width, windowSize.Height);
	}
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;

	m_application->destroyApplication();
	m_device->drop();

	delete m_application;
	m_application = NULL;
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	m_dpi = sender->LogicalDpi;

	if (m_application != NULL)
	{
		core::dimension2du windowSize = GetWindowPixelSize();
		m_application->notifyResizeWin(windowSize.Width, windowSize.Height);
	}
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{

}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{

}