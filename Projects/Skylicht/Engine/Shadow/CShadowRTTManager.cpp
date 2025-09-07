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
		for (SDepthShadowRTT* d : m_depthStatic)
		{
			d->Light->setShadowTexture(NULL);
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_depthStatic.clear();

		for (SDepthShadowRTT* d : m_depthDynamic)
		{
			getVideoDriver()->removeTexture(d->Texture);
			delete d;
		}
		m_depthDynamic.clear();
	}

	void CShadowRTTManager::clearDynamicTextures()
	{
		for (SDepthShadowRTT* d : m_depthDynamic)
		{
			if (d->Light)
				d->Light->setShadowTexture(NULL);
			d->Light = NULL;
		}
	}

	void CShadowRTTManager::onLightRemoved(CLight* light)
	{
		auto it = m_depthStatic.begin(), end = m_depthStatic.end();
		while (it != end)
		{
			SDepthShadowRTT* d = (*it);
			if (d->Light == light)
			{
				getVideoDriver()->removeTexture(d->Texture);
				delete d;

				m_depthStatic.erase(it);
				break;
			}
			++it;
		}

		it = m_depthDynamic.begin(), end = m_depthDynamic.end();
		while (it != end)
		{
			SDepthShadowRTT* d = (*it);
			if (d->Light == light)
			{
				getVideoDriver()->removeTexture(d->Texture);
				delete d;

				m_depthDynamic.erase(it);
				break;
			}
			++it;
		}
	}

	ITexture* CShadowRTTManager::createGetDepthStatic(CLight* light, bool texture2d)
	{
		ITexture* result = light->getShadowTexture();
		if (result)
			return result;

		// Find the light texture in list
		for (SDepthShadowRTT* d : m_depthStatic)
		{
			if (d->Light == light)
			{
				light->setShadowTexture(d->Texture);
				return d->Texture;
			}
		}

		// New light depth
		int size = 512;

		SDepthShadowRTT* d = new SDepthShadowRTT();
		d->Light = light;
		d->Tex2D = texture2d;

		if (texture2d)
			d->Texture = getVideoDriver()->addRenderTargetTexture(core::dimension2du(size, size), "DepthMap", video::ECF_R32F);
		else
			d->Texture = getVideoDriver()->addRenderTargetCubeTexture(core::dimension2du(size, size), "CubeDepthMap", video::ECF_R32F);

		m_depthStatic.push_back(d);

		light->setShadowTexture(d->Texture);

		return d->Texture;
	}

	ITexture* CShadowRTTManager::createGetDepthDynamic(CLight* light, bool texture2d)
	{
		ITexture* result = light->getShadowTexture();
		if (result)
			return result;

		SDepthShadowRTT* slot = NULL;

		// Find the light texture in list
		for (SDepthShadowRTT* d : m_depthDynamic)
		{
			if (d->Light == light)
			{
				light->setShadowTexture(d->Texture);
				return d->Texture;
			}
			else if (slot == NULL && d->Light == NULL && d->Tex2D == texture2d)
			{
				slot = d;
				break;
			}
		}

		if (slot)
		{
			light->setShadowTexture(slot->Texture);

			slot->Light = light;
			return slot->Texture;
		}

		// New light depth
		int size = 512;

		SDepthShadowRTT* d = new SDepthShadowRTT();
		d->Light = light;
		d->Tex2D = texture2d;

		if (texture2d)
			d->Texture = getVideoDriver()->addRenderTargetTexture(core::dimension2du(size, size), "DepthMap", video::ECF_R32F);
		else
			d->Texture = getVideoDriver()->addRenderTargetCubeTexture(core::dimension2du(size, size), "CubeDepthMap", video::ECF_R32F);

		m_depthDynamic.push_back(d);

		light->setShadowTexture(d->Texture);
		return d->Texture;
	}

	ITexture* CShadowRTTManager::createGetDepthCube(CLight* light)
	{
		if (light->isDynamicShadow())
			return createGetDepthDynamic(light, false);

		return createGetDepthStatic(light, false);
	}

	ITexture* CShadowRTTManager::createGetDepth(CLight* light)
	{
		if (light->isDynamicShadow())
			return createGetDepthDynamic(light, true);

		return createGetDepthStatic(light, true);
	}
}