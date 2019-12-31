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
		m_projectionType(CCamera::Perspective),
		m_inputReceiver(true),
		m_nearValue(0.05f),
		m_farValue(1500.0f),
		m_fov(60.0f)
	{
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

	void CCamera::endUpdate()
	{
		// Update projection matrix
		const core::dimension2du& screenSize = getVideoDriver()->getCurrentRenderTargetSize();
		if (m_screenSize != screenSize)
			recalculateProjectionMatrix();

		// Update lookat matrix
		const core::matrix4& mat = m_gameObject->getTransform()->getMatrixTransform();
		core::vector3df position = mat.getTranslation();
		core::vector3df target = CTransform::s_oz;
		mat.rotateVect(target);
		m_viewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(position, position + target, CTransform::s_oy);

		// Update view area
		m_viewArea.cameraPosition = position;
		core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
		m.setbyproduct_nocheck(
			m_viewArea.getTransform(video::ETS_PROJECTION),
			m_viewArea.getTransform(video::ETS_VIEW));
		m_viewArea.setFrom(m);
	}

	const core::matrix4& CCamera::getProjectionMatrix() const
	{
		return m_viewArea.getTransform(video::ETS_PROJECTION);
	}

	const core::matrix4& CCamera::getViewMatrix() const
	{
		return m_viewArea.getTransform(video::ETS_VIEW);
	}

	void CCamera::setPosition(const core::vector3df& position)
	{
		CTransformEuler *t = m_gameObject->getTransformEuler();
		if (t != NULL)
			t->setPosition(position);
	}

	void CCamera::lookAt(const core::vector3df& position, const core::vector3df& target, const core::vector3df& up)
	{
		CTransformEuler *t = m_gameObject->getTransformEuler();
		if (t != NULL)
		{
			core::vector3df front = target - position;
			t->setOrientation(front, up);
			t->setPosition(position);
		}
	}

	void CCamera::lookAt(const core::vector3df& target, const core::vector3df& up)
	{
		CTransformEuler *t = m_gameObject->getTransformEuler();
		if (t != NULL)
		{
			core::vector3df front = target - t->getPosition();
			t->setOrientation(front, up);
		}
	}

	void CCamera::recalculateProjectionMatrix()
	{
		core::dimension2du screenSize = getVideoDriver()->getCurrentRenderTargetSize();
		float aspect = (float)screenSize.Width / (float)screenSize.Height;

		if (m_projectionType == CCamera::Perspective)
		{
			m_viewArea.getTransform(video::ETS_PROJECTION).buildProjectionMatrixPerspectiveFovLH(
				m_fov * core::DEGTORAD,
				aspect,
				m_nearValue,
				m_farValue);
		}
		else if (m_projectionType == CCamera::Frustum)
		{
			float n = m_nearValue;
			float f = m_farValue;

			float scale = tanf(m_fov * 0.5f * core::DEGTORAD) * n;
			float r = aspect * scale;
			float l = -r;
			float t = scale;
			float b = -t;

			f32 *m = m_viewArea.getTransform(video::ETS_PROJECTION).pointer();

#define mat(x, y) m[(x << 2) + y]
			mat(0, 0) = 2 * n / (r - l);
			mat(0, 1) = 0;
			mat(0, 2) = 0;
			mat(0, 3) = 0;

			mat(1, 0) = 0;
			mat(1, 1) = 2 * n / (t - b);
			mat(1, 2) = 0;
			mat(1, 3) = 0;

			mat(2, 0) = (r + l) / (r - l);
			mat(2, 1) = (t + b) / (t - b);
			mat(2, 2) = f / (f - n);			//!
			mat(2, 3) = 1;						//!

			mat(3, 0) = 0;
			mat(3, 1) = 0;
			mat(3, 2) = -(n * f) / (f - n);		//!
			mat(3, 3) = 0;
#undef mat
		}
		else if (m_projectionType == CCamera::Ortho)
		{
			float n = m_nearValue;
			float f = m_farValue;

			float scale = tanf(m_fov * 0.5f * core::DEGTORAD) * (float)screenSize.Width;
			float r = aspect * scale;
			float l = -r;
			float t = scale;
			float b = -t;

			m_viewArea.getTransform(video::ETS_PROJECTION).buildProjectionMatrixOrthoLH(r - l, t - b, n, f);
		}

		m_screenSize = screenSize;
	}
}