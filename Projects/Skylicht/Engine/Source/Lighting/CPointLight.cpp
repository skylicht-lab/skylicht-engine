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
	CPointLight::CPointLight() :
		m_depth(NULL),
		m_needRenderShadowDepth(true)
	{

	}

	CPointLight::~CPointLight()
	{
		if (m_depth != NULL)
			getVideoDriver()->removeTexture(m_depth);
	}

	void CPointLight::initComponent()
	{
		CEntity *entity = m_gameObject->getEntity();
		m_cullingData = entity->addData<CLightCullingData>();
		m_cullingData->Light = this;

		entity->addData<CWorldInverseTransformData>();
	}

	void CPointLight::updateComponent()
	{
		float r = m_radius * m_radius * 0.5f;
		m_cullingData->BBox.MaxEdge.set(r, r, r);
		m_cullingData->BBox.MinEdge.set(-r, -r, -r);

		if (m_gameObject->getTransform()->hasChanged() == true)
			m_needRenderShadowDepth = true;
	}

	core::vector3df CPointLight::getPosition()
	{
		return m_gameObject->getPosition();
	}

	ITexture* CPointLight::createGetDepthTexture()
	{
		if (m_depth == NULL)
		{
			int size = 512;
			m_depth = getVideoDriver()->addRenderTargetCubeTexture(core::dimension2du(size, size), "CubeDepthMap", video::ECF_R32F);
		}

		return m_depth;
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