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

	CSpotLight::CSpotLight() :
		m_depth(NULL),
		m_needRenderShadowDepth(true),
		m_cullingData(NULL)
	{

	}

	CSpotLight::~CSpotLight()
	{
		if (m_depth != NULL)
			getVideoDriver()->removeTexture(m_depth);

		if (m_gameObject)
			m_gameObject->getEntity()->removeData<CLightCullingData>();
	}

	void CSpotLight::initComponent()
	{
		CEntity* entity = m_gameObject->getEntity();
		m_cullingData = entity->addData<CLightCullingData>();
		m_cullingData->Light = this;

		entity->addData<CWorldInverseTransformData>();
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

		object->autoRelease(new CBoolProperty(object, "alway render shadow", m_alwayRenderShadowDepth));
		object->autoRelease(new CFloatProperty(object, "radius", m_radius, 0.0f));
		object->autoRelease(new CFloatProperty(object, "cutoff", m_spotCutoff, 0.0f));
		object->autoRelease(new CFloatProperty(object, "inner_cutoff", m_spotInnerCutoff, 0.0f));

		return object;
	}

	void CSpotLight::loadSerializable(CObjectSerializable* object)
	{
		CLight::loadSerializable(object);

		m_alwayRenderShadowDepth = object->get<bool>("alway render shadow", false);
		float radius = object->get<float>("radius", 3.0f);
		m_spotCutoff = object->get<float>("cutoff", 180.0f / 4.0f);
		m_spotInnerCutoff = object->get<float>("inner_cutoff", 180.0f / 5.0f);

		setRadius(radius);
		setSpotAngle(m_spotCutoff);
		setSpotInnerAngle(m_spotInnerCutoff);
	}

	core::vector3df CSpotLight::getPosition()
	{
		return m_gameObject->getPosition();
	}

	ITexture* CSpotLight::createGetDepthTexture()
	{
		if (m_depth == NULL)
		{
			int size = 512;
			m_depth = getVideoDriver()->addRenderTargetTexture(core::dimension2du(size, size), "SpotlightDepthMap", video::ECF_R32F);
		}

		return m_depth;
	}

	bool CSpotLight::alwayRenderShadowDepth()
	{
		return m_alwayRenderShadowDepth;
	}

	bool CSpotLight::needRenderShadowDepth()
	{
		return m_needRenderShadowDepth;
	}

	void CSpotLight::beginRenderShadowDepth()
	{

	}

	void CSpotLight::endRenderShadowDepth()
	{
		m_needRenderShadowDepth = false;
	}
}