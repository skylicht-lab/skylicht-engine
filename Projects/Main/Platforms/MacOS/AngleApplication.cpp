//
// Copyright 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "AngleApplication.h"

#include "util/EGLWindow.h"
#include "util/gles_loader_autogen.h"
#include "util/util_gl.h"

#include <string.h>
#include <iostream>
#include <utility>

namespace
{

bool IsGLExtensionEnabled(const std::string &extName)
{
    return angle::CheckExtensionExists(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)),
                                       extName);
}
}  // anonymous namespace

AngleApplication::AngleApplication(std::string name,
                                     int argc,
                                     char **argv,
                                     EGLint glesMajorVersion,
                                     EGLint glesMinorVersion,
                                     uint32_t width,
                                     uint32_t height)
    : mName(std::move(name)),
      mWidth(width),
      mHeight(height),
      mRunning(false),
      mFrameCount(0),
      mGLWindow(nullptr),
      mEGLWindow(nullptr),
      mOSWindow(nullptr),
      mDriverType(angle::GLESDriverType::AngleEGL)
{
    // mPlatformParams.renderer = EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE; // Maybe OpenGL backend
    mPlatformParams.renderer = EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE; // Metal backend
    mPlatformParams.deviceType = EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE;
	
    mOSWindow = OSWindow::New();

    // Load EGL library so we can initialize the display.
    mGLWindow = mEGLWindow = EGLWindow::New(EGL_OPENGL_ES_API, glesMajorVersion, glesMinorVersion, 0);
    mEntryPointsLib.reset(angle::OpenSharedLibrary(ANGLE_EGL_LIBRARY_NAME, angle::SearchType::SystemDir));
}

AngleApplication::~AngleApplication()
{
    GLWindowBase::Delete(&mGLWindow);
    OSWindow::Delete(&mOSWindow);
}

bool AngleApplication::initialize()
{
    return true;
}

void AngleApplication::destroy() {}

void AngleApplication::step(float dt, double totalTime) {}

void AngleApplication::draw()
{
    // Set the viewport
    int w = getWindow()->getWidth();
    int h = getWindow()->getHeight();
    glViewport(0, 0, w, h);

    // Clear the color buffer
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void AngleApplication::swap()
{
    mGLWindow->swap();
}

OSWindow *AngleApplication::getWindow() const
{
    return mOSWindow;
}

EGLConfig AngleApplication::getConfig() const
{
    return mEGLWindow->getConfig();
}

EGLDisplay AngleApplication::getDisplay() const
{
    return mEGLWindow->getDisplay();
}

EGLSurface AngleApplication::getSurface() const
{
    return mEGLWindow->getSurface();
}

EGLContext AngleApplication::getContext() const
{
    return mEGLWindow->getContext();
}

int AngleApplication::run()
{
    if (!mOSWindow->initialize(mName, mWidth, mHeight))
    {
        return -1;
    }

    mOSWindow->setVisible(true);

    ConfigParameters configParams;
    configParams.redBits     = 8;
    configParams.greenBits   = 8;
    configParams.blueBits    = 8;
    configParams.alphaBits   = 8;
    configParams.depthBits   = 24;
    configParams.stencilBits = 8;

    if (!mGLWindow->initializeGL(mOSWindow, mEntryPointsLib.get(), mDriverType, mPlatformParams, configParams))
    {
        return -1;
    }

    // Disable vsync
    if (!mGLWindow->setSwapInterval(0))
    {
        return -1;
    }

    mRunning   = true;
    int result = 0;

#if defined(ANGLE_ENABLE_ASSERTS)
    if (IsGLExtensionEnabled("GL_KHR_debug"))
    {
        //EnableDebugCallback(this);
    }
#endif

    if (!initialize())
    {
        mRunning = false;
        result   = -1;
    }

    mTimer.start();
    double prevTime = 0.0;
    
    // process resized event
    mOSWindow->OnEvents = [&](){
        Event event;
        
        // check resized event
        mOSWindow->getEvent(&event);
        if (event.Type == Event::EVENT_RESIZED)
        {
            onResized((int)event.Size.Width, (int)event.Size.Height);
            
            swap();
            
            // pop this event
            mOSWindow->popEvent(&event);
            
            mWidth = event.Size.Width;
            mHeight = event.Size.Height;
        }
    };
    
    while (mRunning)
    {
        double elapsedTime = mTimer.getElapsedCpuTime();
        double deltaTime   = elapsedTime - prevTime;

        step(static_cast<float>(deltaTime), elapsedTime);
        
        // Clear events that the application did not process from this frame
        Event event;
        while (popEvent(&event))
        {
            // If the application did not catch a close event, close now
            switch (event.Type)
            {
                case Event::EVENT_CLOSED:
                    exit();
                    break;
                case Event::EVENT_KEY_RELEASED:
                    onKeyUp(event.Key);
                    break;
                case Event::EVENT_KEY_PRESSED:
                    onKeyDown(event.Key);
                    break;
                case Event::EVENT_MOUSE_MOVED:
                    onMouseMoved(event.MouseMove);
                    break;
                case Event::EVENT_MOUSE_BUTTON_PRESSED:
                    onMouseButtonPressed(event.MouseButton);
                    break;
                case Event::EVENT_MOUSE_BUTTON_RELEASED:
                    onMouseButtonRelease(event.MouseButton);
                    break;
                case Event::EVENT_MOUSE_WHEEL_MOVED:
                    onWheel(event.MouseWheel);
                    break;
                default:
                    break;
            }
        }

        if (!mRunning)
        {
            break;
        }

        draw();
        swap();

        mOSWindow->messageLoop();
        
        prevTime = elapsedTime;

        mFrameCount++;

        if (mFrameCount % 1000 == 0)
        {
            printf("Rate: %0.2lf frames / second\n",
                   static_cast<double>(mFrameCount) / mTimer.getElapsedCpuTime());
        }
    }

    destroy();
    mGLWindow->destroyGL();
    mOSWindow->destroy();

    return result;
}

void AngleApplication::exit()
{
    mRunning = false;
}

bool AngleApplication::popEvent(Event *event)
{
    return mOSWindow->popEvent(event);
}

void AngleApplication::onKeyUp(const Event::KeyEvent &keyEvent)
{
    // Default no-op.
}

void AngleApplication::onKeyDown(const Event::KeyEvent &keyEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseMoved(const Event::MouseMoveEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseButtonPressed(const Event::MouseButtonEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseButtonRelease(const Event::MouseButtonEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onWheel(const Event::MouseWheelEvent &wheelEvent)
{
    // Default no-op.
}

void AngleApplication::onResized(int width, int height)
{
    // Default no-op.
}
