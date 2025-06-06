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
#include "CSpotLight.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntity.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CSpotLight);

	CATEGORY_COMPONENT(CSpotLight, "Spotlight", "Lighting");

	CSpotLight::CSpotLight()
	{

	}

	CSpotLight::~CSpotLight()
	{

	}

	void CSpotLight::initComponent()
	{
		CPointLight::initComponent();
	}

	void CSpotLight::updateComponent()
	{
		m_direction.set(0.0f, 0.0f, 1.0f);
		const core::matrix4& transform = m_gameObject->getTransform()->getRelativeTransform();
		transform.rotateVect(m_direction);
		m_direction.normalize();

		float r = m_radius * m_radius * 0.5f;
		m_cullingData->BBox.MaxEdge.set(r, r, r);
		m_cullingData->BBox.MinEdge.set(-r, -r, -r);

		if (m_gameObject->getTransform()->hasChanged() == true)
			m_needRenderShadowDepth = true;
	}

	CObjectSerializable* CSpotLight::createSerializable()
	{
		CObjectSerializable* object = CLight::createSerializable();

		object->autoRelease(new CBoolProperty(object, "dynamic shadow", m_dynamicShadow));
		object->autoRelease(new CFloatProperty(object, "radius", m_radius, 0.0f));
		object->autoRelease(new CFloatProperty(object, "outer cutoff", m_spotCutoff, 0.0f, 180.0f));
		object->autoRelease(new CFloatProperty(object, "inner cutoff", m_spotInnerCutoff, 0.0f, 180.0f));
		object->autoRelease(new CFloatProperty(object, "spot exponent", m_spotExponent, 0.0f, 128.0f));

		return object;
	}

	void CSpotLight::loadSerializable(CObjectSerializable* object)
	{
		CLight::loadSerializable(object);

		m_dynamicShadow = object->get<bool>("dynamic shadow", false);
		float radius = object->get<float>("radius", 3.0f);
		m_spotCutoff = object->get<float>("outer cutoff", 180.0f / 4.0f);
		m_spotInnerCutoff = object->get<float>("inner cutoff", 180.0f / 5.0f);
		m_spotExponent = object->get<float>("spot exponent", 10.0f);

		setRadius(radius);

		m_needRenderShadowDepth = true;
	}
}