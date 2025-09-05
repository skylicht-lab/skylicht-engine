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
#include "CAreaLight.h"

#include "GameObject/CGameObject.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CAreaLight);

	CATEGORY_COMPONENT(CAreaLight, "Area Light", "Lighting");

	CAreaLight::CAreaLight() :
		m_sizeX(1.0f),
		m_sizeY(1.0f),
		m_needRenderShadowDepth(true)
	{
		// default 2 bounce
		m_bakeBounce = 2;
		m_castShadow = true;

		setIntensity(3.5f);
		setRadius(1.5f);
	}

	CAreaLight::~CAreaLight()
	{
		if (m_gameObject && m_cullingData)
			m_gameObject->getEntity()->removeData<CLightCullingData>();
	}

	void CAreaLight::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();
		m_cullingData = entity->addData<CLightCullingData>();
		m_cullingData->Light = this;
		m_cullingData->LightType = 3;

		m_gameObject->setEnableEndUpdate(true);
	}

	void CAreaLight::updateComponent()
	{

	}

	CObjectSerializable* CAreaLight::createSerializable()
	{
		CObjectSerializable* object = CLight::createSerializable();

		object->autoRelease(new CBoolProperty(object, "dynamic shadow", m_dynamicShadow));
		object->autoRelease(new CFloatProperty(object, "radius", m_radius, 0.0f));
		object->autoRelease(new CFloatProperty(object, "sizeX", m_sizeX, 0.0f));
		object->autoRelease(new CFloatProperty(object, "sizeY", m_sizeY, 0.0f));
		return object;
	}

	void CAreaLight::loadSerializable(CObjectSerializable* object)
	{
		CLight::loadSerializable(object);

		m_dynamicShadow = object->get<bool>("dynamic shadow", false);
		float radius = object->get<float>("radius", 3.0f);
		setRadius(radius);

		float sizeX = object->get<float>("sizeX", 1.0f);
		float sizeY = object->get<float>("sizeY", 1.0f);
		setSize(sizeX, sizeY);

		m_needRenderShadowDepth = true;
	}

	void CAreaLight::endUpdate()
	{
		m_cullingData->BBox.MaxEdge.set(m_radius + m_sizeX * 0.5f, m_radius + m_sizeY * 0.5f, 0.1f);
		m_cullingData->BBox.MinEdge.set(-(m_radius + m_sizeX * 0.5f), -(m_radius + m_sizeY * 0.5f), -0.1f);

		CTransform* t = m_gameObject->getTransform();
		if (t->hasChanged() || m_needValidate)
		{
			m_direction.set(0.0f, 0.0f, -1.0f);
			core::matrix4 transform = t->calcWorldTransform();
			transform.rotateVect(m_direction);
			m_direction.normalize();
			m_needValidate = false;
		}
	}

	bool CAreaLight::needRenderShadowDepth()
	{
		return m_needRenderShadowDepth;
	}

	void CAreaLight::beginRenderShadowDepth()
	{

	}

	void CAreaLight::endRenderShadowDepth()
	{
		m_needRenderShadowDepth = false;
	}

	core::vector3df CAreaLight::getPosition()
	{
		return m_gameObject->getWorldTransform().getTranslation();
	}

	const core::matrix4& CAreaLight::getWorldTransform()
	{
		return m_gameObject->getWorldTransform();
	}
}