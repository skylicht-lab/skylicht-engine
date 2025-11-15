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
#include "Skylicht.h"

// Event
#include "EventManager/CEventManager.h"

// Control
#include "Control/CTouchManager.h"
#include "Control/CAccelerometer.h"
#include "Control/CJoystick.h"

// Debug
#include "Debug/CSceneDebug.h"

// Shader Manager
#include "Material/CMaterialManager.h"
#include "Material/Shader/CShaderManager.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/CGUIFactory.h"
#include "Shadow/CShadowRTTManager.h"

// Mesh & Texture
#include "MeshManager/CMeshManager.h"
#include "Animation/CAnimationManager.h"
#include "TextureManager/CTextureManager.h"
#include "Graphics2D/SpriteFrame/CSpriteManager.h"
#include "Graphics2D/SpriteFrame/CFontManager.h"
#include "TextBillboard/CTextBillboardManager.h"

// Tween
#include "Tween/easing.h"
#include "Tween/CTweenManager.h"

// Activator
#include "Serializable/CObjectSerializable.h"
#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"
#include "Components/CComponentCategory.h"

#include "Graphics2D/Glyph/CGlyphFreetype.h"


namespace Skylicht
{

	IrrlichtDevice* g_device = NULL;
	IVideoDriver* g_video = NULL;

	float g_timestep = 0.0f;
	float g_totalTime = 0.0f;
	float g_timescale = 1.0f;
	float g_fixedTimeStep = 16.666f;
	bool g_useFixedTimeStep = false;

	void initSkylicht(IrrlichtDevice* device, bool server)
	{
		g_device = device;
		g_video = device->getVideoDriver();

		os::Printer::log("Init Skylicht Engine");
		CEventManager::createGetInstance();

		CTouchManager::createGetInstance();
		CAccelerometer::createGetInstance();
		CJoystick::createGetInstance();

		CGlyphFreetype::createGetInstance();

		CShaderManager::createGetInstance();
		CGraphics2D::createGetInstance();
		CGUIFactory::createGetInstance();
		CTextureManager::createGetInstance();
		CMeshManager::createGetInstance();
		CAnimationManager::createGetInstance();
		CMaterialManager::createGetInstance();
		CSpriteManager::createGetInstance();
		CFontManager::createGetInstance();

		CShadowRTTManager::createGetInstance();

		initEasing();
		CTweenManager::createGetInstance();

		CActivator::createGetInstance();
		CSerializableActivator::createGetInstance();
		CDependentComponent::createGetInstance();
		CComponentCategory::createGetInstance();

		CSceneDebug::createGetInstance();
		CTextBillboardManager::createGetInstance();

		// alway use HW
		g_video->setMinHardwareBufferVertexCount(0);

		// reset random time
		os::Randomizer::reset(os::Timer::getRealTime());
		srand((unsigned int)time(NULL));
	}

	void releaseSkylicht()
	{
		os::Printer::log("Close skylicht core");

		CSceneDebug::releaseInstance();
		CTextBillboardManager::releaseInstance();

		CComponentCategory::releaseInstance();
		CDependentComponent::releaseInstance();
		CActivator::releaseInstance();
		CSerializableActivator::releaseInstance();

		CTweenManager::releaseInstance();
		releaseEasing();

		CShadowRTTManager::releaseInstance();

		CFontManager::releaseInstance();
		CSpriteManager::releaseInstance();
		CMaterialManager::releaseInstance();
		CAnimationManager::releaseInstance();
		CMeshManager::releaseInstance();
		CTextureManager::releaseInstance();
		CGUIFactory::releaseInstance();
		CGraphics2D::releaseInstance();
		CShaderManager::releaseInstance();

		CGlyphFreetype::releaseInstance();

		CTouchManager::releaseInstance();
		CAccelerometer::releaseInstance();
		CJoystick::releaseInstance();

		CEventManager::releaseInstance();
	}

	void updateSkylicht()
	{
		CTouchManager::getInstance()->update();
		CAccelerometer::getInstance()->update();
		CJoystick::getInstance()->update();
		CTweenManager::getInstance()->update();

		CSceneDebug* debug = CSceneDebug::getInstance();
		CSceneDebug* noZDebug = debug->getNoZDebug();
		debug->clear();
		noZDebug->clear();
	}

	IrrlichtDevice* getIrrlichtDevice()
	{
		return g_device;
	}

	IVideoDriver* getVideoDriver()
	{
		return g_video;
	}

	float getTimeStep()
	{
		// return the current time step (milisecond)
		float ts = g_useFixedTimeStep ? g_fixedTimeStep : g_timestep;
		return ts * g_timescale;
	}

	float getNonScaledTimestep()
	{
		// return the current time step (milisecond)
		return g_useFixedTimeStep ? g_fixedTimeStep : g_timestep;
	}

	void setTimeStep(float timestep)
	{
		g_timestep = timestep;
	}

	float getTotalTime()
	{
		return g_totalTime;
	}

	void setTotalTime(float t)
	{
		g_totalTime = t;
	}

	void enableFixedTimeStep(bool b)
	{
		g_useFixedTimeStep = b;
	}

	void setFixedTimeStep(float s)
	{
		g_fixedTimeStep = s;
	}

	void setTimeScale(float scale)
	{
		g_timescale = scale;
	}

	float getTimeScale()
	{
		return g_timescale;
	}

#ifdef ANDROID
	jobject g_androidActivity = NULL;

	JNIEnv* g_androidJni = NULL;

	JNIEnv* getJniEnv()
	{
		return g_androidJni;
	}

	jobject getMainActivity()
	{
		return g_androidActivity;
	}

	void setJniEnv(JNIEnv* env)
	{
		g_androidJni = env;
	}

	void setMainActivity(jobject activity)
	{
		g_androidActivity = activity;
	}
#endif
}