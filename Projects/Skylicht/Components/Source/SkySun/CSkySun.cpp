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

#include "Lighting/CDirectionalLight.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CSkySun);

	CATEGORY_COMPONENT(CSkySun, "Sky Sun", "Renderer");

	CSkySun::CSkySun() :
		m_skySunData(NULL),
		m_skyIntensity(1.1f),
		m_sunSize(800.0f),
		m_atmosphericColor(255, 255, 204, 178),
		m_atmosphericIntensity(0.1f),
		m_sunColor(255, 255, 153, 25),
		m_sunIntensity(0.5),
		m_glare1Color(255, 255, 153, 25),
		m_glare1Intensity(0.3),
		m_glare2Color(255, 255, 51, 25),
		m_glare2Intensity(0.15),
		m_changed(true),
		m_useDirectionLight(true),
		m_uniformDirection(NULL)
	{
	}

	CSkySun::~CSkySun()
	{
		if (m_gameObject)
			m_gameObject->getEntity()->removeData<CSkySunData>();
	}

	void CSkySun::initComponent()
	{
		m_skySunData = m_gameObject->getEntity()->addData<CSkySunData>();
		m_gameObject->getEntityManager()->addRenderSystem<CSkySunRender>();

		CMaterial* material = m_skySunData->SkySunMaterial;
		m_uniformDirection = material->getUniform("uLightDirection");
	}

	void CSkySun::updateComponent()
	{
		CMaterial* material = m_skySunData->SkySunMaterial;

		if (m_uniformDirection != NULL)
		{
			if (m_useDirectionLight)
			{
				// use direction light
				CDirectionalLight* light = CShaderLighting::getDirectionalLight();
				if (light != NULL)
				{
					const core::vector3df& d = -light->getDirection();
					m_uniformDirection->FloatValue[0] = d.X;
					m_uniformDirection->FloatValue[1] = d.Y;
					m_uniformDirection->FloatValue[2] = d.Z;
					m_uniformDirection->FloatValue[3] = 0.0f;
				}
				else
				{
					m_uniformDirection->FloatValue[0] = 0.0f;
					m_uniformDirection->FloatValue[1] = -1.0f;
					m_uniformDirection->FloatValue[2] = 0.0f;
					m_uniformDirection->FloatValue[3] = 0.0f;
				}
			}
			else
			{
				// use orientation
				const core::vector3df& front = m_gameObject->getTransformEuler()->getFront();
				m_uniformDirection->FloatValue[0] = front.X;
				m_uniformDirection->FloatValue[1] = front.Y;
				m_uniformDirection->FloatValue[2] = front.Z;
				m_uniformDirection->FloatValue[3] = 0.0f;
			}
		}

		if (m_changed)
		{
			float value[4] = { 0 };
			value[0] = m_skyIntensity;
			value[1] = m_sunSize;
			material->setUniform4("uIntensity", value);

			getUniformValue(m_atmosphericColor, m_atmosphericIntensity, value);
			material->setUniform4("uAtmospheric", value);

			getUniformValue(m_sunColor, m_sunIntensity, value);
			material->setUniform4("uSun", value);

			getUniformValue(m_glare1Color, m_glare1Intensity, value);
			material->setUniform4("uGlare1", value);

			getUniformValue(m_glare2Color, m_glare2Intensity, value);
			material->setUniform4("uGlare2", value);

			m_changed = false;
		}

		material->updateShaderParams();
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
		object->autoRelease(skyProperty);

		CColorProperty* atmosphericProperty = new CColorProperty(object, "Atmospheric Color", m_atmosphericColor);
		atmosphericProperty->setUIHeader("Atmospheric");
		object->autoRelease(atmosphericProperty);
		object->autoRelease(new CFloatProperty(object, "Atmospheric Intensity", m_atmosphericIntensity, 0.0f));

		CColorProperty* sunProperty = new CColorProperty(object, "Sun Color", m_sunColor);
		sunProperty->setUIHeader("Sun");
		object->autoRelease(sunProperty);
		object->autoRelease(new CFloatProperty(object, "Sun Intensity", m_sunIntensity, 0.0f));
		object->autoRelease(new CFloatProperty(object, "Sun Size", m_sunSize, 1.0f, 5000.0f));
		object->autoRelease(new CBoolProperty(object, "Direction Light", m_useDirectionLight));

		CColorProperty* glareProperty = new CColorProperty(object, "Glare1 Color", m_glare1Color);
		glareProperty->setUIHeader("Glare");
		object->autoRelease(glareProperty);
		object->autoRelease(new CFloatProperty(object, "Glare1 Intensity", m_glare1Intensity, 0.0f));

		object->autoRelease(new CColorProperty(object, "Glare2 Color", m_glare2Color));
		object->autoRelease(new CFloatProperty(object, "Glare2 Intensity", m_glare2Intensity, 0.0f));

		return object;
	}

	void CSkySun::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_useDirectionLight = object->get<bool>("Direction Light", true);

		m_skyIntensity = object->get<float>("Intensity", 1.1f);

		m_atmosphericColor = object->get<SColor>("Atmospheric Color", SColor(255, 255, 204, 178));
		m_atmosphericIntensity = object->get<float>("Atmospheric Intensity", 0.1f);

		m_sunColor = object->get<SColor>("Sun Color", SColor(255, 255, 204, 178));
		m_sunIntensity = object->get<float>("Sun Intensity", 0.1f);
		m_sunSize = object->get<float>("Sun Size", 800.0f);

		m_glare1Color = object->get<SColor>("Glare1 Color", SColor(255, 255, 153, 25));
		m_glare1Intensity = object->get<float>("Glare1 Intensity", 0.4f);

		m_glare2Color = object->get<SColor>("Glare2 Color", SColor(255, 255, 51, 25));
		m_glare2Intensity = object->get<float>("Glare2 Intensity", 0.2f);

		m_changed = true;
	}
}