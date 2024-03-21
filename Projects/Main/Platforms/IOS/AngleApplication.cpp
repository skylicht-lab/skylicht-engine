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
    mPlatformParams.renderer = EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE;
    mPlatformParams.deviceType = EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE;

    mOSWindow = OSWindow::New();

    // Load EGL library so we can initialize the display.
    mGLWindow = mEGLWindow = EGLWindow::New(EGL_OPENGL_ES_API, glesMajorVersion, glesMinorVersion, 0);
    mEntryPointsLib.reset(angle::OpenSharedLibrary(ANGLE_EGL_LIBRARY_NAME, angle::SearchType::ModuleDir));
}

AngleApplication::~AngleApplication()
{
    destroy();
    
    GLWindowBase::Delete(&mGLWindow);
    OSWindow::Delete(&mOSWindow);
}

bool AngleApplication::initialize()
{
    if (!mOSWindow->initialize(mName, mWidth, mHeight))
    {
        return false;
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
        return false;
    }

    // Disable vsync
    if (!mGLWindow->setSwapInterval(0))
    {
        return false;
    }
    
    return true;
}

void AngleApplication::destroy() 
{
    mGLWindow->destroyGL();
    mOSWindow->destroy();
}

void AngleApplication::step(float dt, double totalTime) {}

void AngleApplication::draw()
{
    // Set the viewport
    int w = getWindow()->getWidth();
    int h = getWindow()->getHeight();
    glViewport(0, 0, w, h);

    // Clear the color buffer
    glClearColor(1.0f,0.0f,0.0f,1.0f);
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


void AngleApplication::exit()
{
    
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
    mOSWindow->resize(width, height);
}
