/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CShadowRTTManager.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CShadowRTTManager);

	CShadowRTTManager::CShadowRTTManager()
	{

	}

	CShadowRTTManager::~CShadowRTTManager()
	{
		clearTextures();
	}

	void CShadowRTTManager::clearTextures()
	{
		for (SDepthShadowRTT* d : m_pointLightDepthStatic)
		{
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_pointLightDepthStatic.clear();

		for (SDepthShadowRTT* d : m_pointLightDepthDynamic)
		{
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_pointLightDepthDynamic.clear();


		for (SDepthShadowRTT* d : m_spotLightDepthStatic)
		{
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_spotLightDepthStatic.clear();

		for (SDepthShadowRTT* d : m_spotLightDepthDynamic)
		{
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_spotLightDepthDynamic.clear();
	}

	void CShadowRTTManager::clearLightData()
	{
		for (SDepthShadowRTT* d : m_pointLightDepthDynamic)
		{
			d->Light = NULL;
		}
	}

	ITexture* CShadowRTTManager::createGetPointLightDepthStatic(CLight* light)
	{
		// Find the light texture in list
		for (SDepthShadowRTT* d : m_pointLightDepthStatic)
		{
			if (d->Light == light)
			{
				return d->Texture;
			}
		}

		// New light depth
		int size = 512;

		SDepthShadowRTT* d = new SDepthShadowRTT();
		d->Light = light;
		d->Texture = getVideoDriver()->addRenderTargetCubeTexture(core::dimension2du(size, size), "CubeDepthMap", video::ECF_R32F);
		m_pointLightDepthStatic.push_back(d);

		return d->Texture;
	}

	ITexture* CShadowRTTManager::createGetPointLightDepthDynamic(CLight* light)
	{
		SDepthShadowRTT* slot = NULL;

		// Find the light texture in list
		for (SDepthShadowRTT* d : m_pointLightDepthDynamic)
		{
			if (d->Light == light)
			{
				return d->Texture;
			}
			else if (slot == NULL && d->Light == NULL)
			{
				slot = d;
			}
		}

		if (slot)
		{
			slot->Light = light;
			return slot->Texture;
		}

		// New light depth
		int size = 512;

		SDepthShadowRTT* d = new SDepthShadowRTT();
		d->Light = light;
		d->Texture = getVideoDriver()->addRenderTargetCubeTexture(core::dimension2du(size, size), "CubeDepthMap", video::ECF_R32F);
		m_pointLightDepthDynamic.push_back(d);

		return d->Texture;
	}

	ITexture* CShadowRTTManager::createGetPointLightDepth(CLight* light)
	{
		if (light->isDynamicShadow())
			return createGetPointLightDepthDynamic(light);

		return createGetPointLightDepthStatic(light);
	}
}