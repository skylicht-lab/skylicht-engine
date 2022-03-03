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
#include "CSkySun.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CSkySun::CSkySun() :
		m_skyIntensity(1.1f),
		m_atmosphericColor(255, 255, 204, 178),
		m_atmosphericIntensity(0.1f),
		m_sunColor(255, 255, 153, 25),
		m_sunIntensity(0.5),
		m_glare1Color(255, 255, 153, 25),
		m_glare1Intensity(0.4),
		m_glare2Color(255, 255, 51, 25),
		m_glare2Intensity(0.2),
		m_changed(true)
	{
	}

	CSkySun::~CSkySun()
	{

	}

	void CSkySun::initComponent()
	{
		m_skySunData = m_gameObject->getEntity()->addData<CSkySunData>();
		m_gameObject->getEntityManager()->addRenderSystem<CSkySunRender>();
	}

	void CSkySun::updateComponent()
	{
		if (m_changed)
		{
			CMaterial* material = m_skySunData->SkySunMaterial;
			float value[4];

			getUniformValue(SColor(255, 255, 255, 255), m_skyIntensity, value);
			material->setUniform4("uIntensity", value);

			getUniformValue(m_atmosphericColor, m_atmosphericIntensity, value);
			material->setUniform4("uAtmospheric", value);

			getUniformValue(m_sunColor, m_sunIntensity, value);
			material->setUniform4("uSun", value);

			getUniformValue(m_glare1Color, m_glare1Intensity, value);
			material->setUniform4("uGlare1", value);

			getUniformValue(m_glare2Color, m_glare2Intensity, value);
			material->setUniform4("uGlare2", value);
			
			material->updateShaderParams();
			m_changed = false;
		}
	}

	void CSkySun::getUniformValue(const SColor& c, float intensity, float* value)
	{
		value[0] = c.getRed() / 255.0f;
		value[1] = c.getGreen() / 255.0f;
		value[2] = c.getBlue() / 255.0f;
		value[3] = intensity;
	}

	CObjectSerializable* CSkySun::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CFloatProperty* skyProperty = new CFloatProperty(object, "Intensity", m_skyIntensity, 0.0f);
		skyProperty->setUIHeader("Sky");
		skyProperty->setUISpace(10.0f);
		object->addAutoRelease(skyProperty);

		CColorProperty* atmosphericProperty = new CColorProperty(object, "Atmospheric Color", m_atmosphericColor);
		atmosphericProperty->setUIHeader("Atmospheric");
		object->addAutoRelease(atmosphericProperty);
		object->addAutoRelease(new CFloatProperty(object, "Atmospheric Intensity", m_atmosphericIntensity, 0.0f));

		CColorProperty* sunProperty = new CColorProperty(object, "Sun Color", m_sunColor);
		sunProperty->setUIHeader("Sun");
		object->addAutoRelease(sunProperty);
		object->addAutoRelease(new CFloatProperty(object, "Sun Intensity", m_sunIntensity, 0.0f));

		CColorProperty* glareProperty = new CColorProperty(object, "Glare1 Color", m_glare1Color);
		glareProperty->setUIHeader("Glare");
		object->addAutoRelease(glareProperty);
		object->addAutoRelease(new CFloatProperty(object, "Glare1 Intensity", m_glare1Intensity, 0.0f));

		object->addAutoRelease(new CColorProperty(object, "Glare2 Color", m_glare2Color));
		object->addAutoRelease(new CFloatProperty(object, "Glare2 Intensity", m_glare2Intensity, 0.0f));

		return object;
	}

	void CSkySun::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_skyIntensity = object->get<float>("Intensity", 1.1f);

		m_atmosphericColor = object->get<SColor>("Atmospheric Color", SColor(255, 255, 204, 178));
		m_atmosphericIntensity = object->get<float>("Atmospheric Intensity", 0.1f);

		m_sunColor = object->get<SColor>("Sun Color", SColor(255, 255, 204, 178));
		m_sunIntensity = object->get<float>("Sun Intensity", 0.1f);

		m_glare1Color = object->get<SColor>("Glare1 Color", SColor(255, 255, 153, 25));
		m_glare1Intensity = object->get<float>("Glare1 Intensity", 0.4f);

		m_glare2Color = object->get<SColor>("Glare2 Color", SColor(255, 255, 51, 25));
		m_glare2Intensity = object->get<float>("Glare2 Intensity", 0.2f);

		m_changed = true;
	}
}