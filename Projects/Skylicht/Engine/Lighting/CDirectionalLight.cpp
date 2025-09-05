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
#include "CDirectionalLight.h"

#include "GameObject/CGameObject.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CDirectionalLight);

	CATEGORY_COMPONENT(CDirectionalLight, "Direction Light", "Lighting");

	CDirectionalLight::CDirectionalLight()
	{
		// default 2 bounce
		m_bakeBounce = 2;
		m_castShadow = true;

		setIntensity(1.5f);
	}

	CDirectionalLight::~CDirectionalLight()
	{
		if (CShaderLighting::getDirectionalLight() == this)
			CShaderLighting::setDirectionalLight(NULL);

		if (CShaderLighting::getMainShadowLight() == this)
			CShaderLighting::setMainShadowLight(NULL);

		if (m_gameObject && m_cullingData)
			m_gameObject->getEntity()->removeData<CLightCullingData>();
	}

	void CDirectionalLight::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();
		m_cullingData = entity->addData<CLightCullingData>();
		m_cullingData->Light = this;
		m_cullingData->LightType = CLight::DirectionalLight;

		m_gameObject->setEnableEndUpdate(true);
	}

	void CDirectionalLight::updateComponent()
	{

	}

	void CDirectionalLight::endUpdate()
	{
		CTransform* t = m_gameObject->getTransform();
		if (t->hasChanged() || m_needValidate)
		{
			m_direction.set(0.0f, 0.0f, 1.0f);
			core::matrix4 transform = t->calcWorldTransform();
			transform.rotateVect(m_direction);
			m_direction.normalize();
			m_needValidate = false;
		}

		if (m_enable)
		{
			CDirectionalLight* currentLight = CShaderLighting::getDirectionalLight();
			if (currentLight == NULL)
			{
				if (isAffectingDefaultObjects())
					CShaderLighting::setDirectionalLight(this);
			}
			else
			{
				if (currentLight != this)
				{
					if (isAffectingDefaultObjects() && currentLight->getLightPriority() < m_lightPriority)
						CShaderLighting::setDirectionalLight(this);
				}
				else
				{
					if (!isAffectingDefaultObjects())
						CShaderLighting::setDirectionalLight(NULL);
				}
			}
		}
	}

	void CDirectionalLight::onEnable(bool b)
	{
		if (!b)
		{
			if (CShaderLighting::getDirectionalLight() == this)
				CShaderLighting::setDirectionalLight(NULL);
		}
	}
}