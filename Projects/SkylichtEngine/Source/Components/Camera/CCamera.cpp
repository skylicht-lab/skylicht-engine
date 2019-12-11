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
#include "CCamera.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CCamera::CCamera() :
		m_projectionType(CCamera::Perspective)
	{
		m_nearValue = 0.05f;
		m_farValue = 1500;
		m_fov = 54.0f;
	}

	CCamera::~CCamera()
	{

	}

	void CCamera::initComponent()
	{
		recalculateProjectionMatrix();
	}

	void CCamera::updateComponent()
	{

	}

	void CCamera::applyTransform()
	{
		m_viewArea.getTransform(video::ETS_VIEW) = m_gameObject->getTransform()->getMatrixTransform();

		video::IVideoDriver* driver = getVideoDriver();
		if (driver)
		{
			driver->setTransform(video::ETS_PROJECTION, m_viewArea.getTransform(video::ETS_PROJECTION));
			driver->setTransform(video::ETS_VIEW, m_viewArea.getTransform(video::ETS_VIEW));
		}
	}

	const core::matrix4& CCamera::getProjectionMatrix() const
	{
		return m_viewArea.getTransform(video::ETS_PROJECTION);
	}

	const core::matrix4& CCamera::getViewMatrix() const
	{
		return m_viewArea.getTransform(video::ETS_VIEW);
	}

	void CCamera::lookAt(const core::vector3df& position, const core::vector3df& target, const core::vector3df& up)
	{
		CTransformEuler *t = m_gameObject->getTransformEuler();
		if (t != NULL)
		{
			core::vector3df rot = position - target;
			t->setOrientation(rot, up);
			t->setPosition(position);
		}
	}

	void CCamera::lookAt(const core::vector3df& target, const core::vector3df& up)
	{
		CTransformEuler *t = m_gameObject->getTransformEuler();
		if (t != NULL)
		{			
			core::vector3df rot = t->getPosition() - target;
			t->setOrientation(rot, up);
		}
	}

	void CCamera::recalculateProjectionMatrix()
	{	
		if (m_projectionType == CCamera::Perspective)
		{
			core::dimension2du screenSize = getVideoDriver()->getCurrentRenderTargetSize();
			float aspect = (float)screenSize.Width / (float)screenSize.Height;

			m_viewArea.getTransform(video::ETS_PROJECTION).buildProjectionMatrixPerspectiveFovLH(
				m_fov * core::DEGTORAD, 
				aspect, 
				m_nearValue, 
				m_farValue);
		}
	}
}