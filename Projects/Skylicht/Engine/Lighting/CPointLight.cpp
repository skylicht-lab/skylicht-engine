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
#include "CPointLight.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntity.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CPointLight);

	CATEGORY_COMPONENT(CPointLight, "Point Light", "Lighting");

	CPointLight::CPointLight() :
		m_needRenderShadowDepth(true)
	{
		setIntensity(2.0f);
	}

	CPointLight::~CPointLight()
	{
		if (m_gameObject && m_cullingData)
			m_gameObject->getEntity()->removeData<CLightCullingData>();
	}

	void CPointLight::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();
		m_cullingData = entity->addData<CLightCullingData>();
		m_cullingData->Light = this;
		m_cullingData->LightType = 1;

		entity->addData<CWorldInverseTransformData>();

		m_gameObject->setEnableEndUpdate(true);
	}

	void CPointLight::updateComponent()
	{

	}

	void CPointLight::endUpdate()
	{
		float r = m_radius;
		m_cullingData->BBox.MaxEdge.set(r, r, r);
		m_cullingData->BBox.MinEdge.set(-r, -r, -r);

		if (m_gameObject->getTransform()->hasChanged() || m_needValidate)
		{
			m_needRenderShadowDepth = true;
			m_needValidate = false;
		}
	}

	CObjectSerializable* CPointLight::createSerializable()
	{
		CObjectSerializable* object = CLight::createSerializable();

		object->autoRelease(new CBoolProperty(object, "dynamic shadow", m_dynamicShadow));
		object->autoRelease(new CFloatProperty(object, "radius", m_radius, 0.0f));
		return object;
	}

	void CPointLight::loadSerializable(CObjectSerializable* object)
	{
		CLight::loadSerializable(object);

		m_dynamicShadow = object->get<bool>("dynamic shadow", false);
		float radius = object->get<float>("radius", 3.0f);
		setRadius(radius);

		m_needRenderShadowDepth = true;
	}

	core::vector3df CPointLight::getPosition()
	{
		return m_gameObject->getWorldTransform().getTranslation();
	}

	bool CPointLight::needRenderShadowDepth()
	{
		return m_needRenderShadowDepth;
	}

	void CPointLight::beginRenderShadowDepth()
	{

	}

	void CPointLight::endRenderShadowDepth()
	{
		m_needRenderShadowDepth = false;
	}
}