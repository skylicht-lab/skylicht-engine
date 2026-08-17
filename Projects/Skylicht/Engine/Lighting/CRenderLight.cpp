/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CRenderLight.h"

namespace Skylicht
{
	CRenderLight::CRenderLight() :
		m_lightLayers(1),
		m_enableSortLight(false)
	{

	}

	CRenderLight::~CRenderLight()
	{

	}

	CObjectSerializable* CRenderLight::createSerializable()
	{
		CObjectSerializable* object = CEntityHandler::createSerializable();

		CUIntProperty* lightLayers = new CUIntProperty(object, "lightLayers", m_lightLayers);
		lightLayers->setHidden(true);
		object->autoRelease(lightLayers);

		object->autoRelease(new CBoolProperty(object, "enableSortLight", m_enableSortLight));

		return object;
	}

	void CRenderLight::loadSerializable(CObjectSerializable* object)
	{
		CEntityHandler::loadSerializable(object);

		m_enableSortLight = object->get<bool>("enableSortLight", false);
	}

	void CRenderLight::loadLightLayers(CObjectSerializable* object)
	{
		u32 lightLayers = object->get<u32>("lightLayers", 1);
		setLightLayers(lightLayers);
	}

	void CRenderLight::setLightLayers(u32 layers)
	{
		m_lightLayers = layers;
		applyLightLayerForEntities();
	}

	void CRenderLight::enableSortLight(bool enable)
	{
		m_enableSortLight = enable;
		applyLightLayerForEntities();
	}

	void CRenderLight::applyLightLayerForEntities()
	{
		for (u32 i = 0, n = m_entities.size(); i < n; i++)
		{
			CRenderLightData* data = m_entities[i]->getData<CRenderLightData>();
			if (data != NULL)
			{
				data->setLightLayers(m_lightLayers);
				data->enableSortLight(m_enableSortLight);
			}
		}
	}
}
