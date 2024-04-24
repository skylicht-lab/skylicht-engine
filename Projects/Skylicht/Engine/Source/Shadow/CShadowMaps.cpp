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
#include "CShadowMaps.h"
#include "Transform/CTransform.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CShadowMaps::CShadowMaps() :
		m_shadowMapSize(2048),
		m_farValue(500.0f),
		m_nearOffset(300.0f)
	{

	}

	CShadowMaps::~CShadowMaps()
	{

	}

	void CShadowMaps::init(int shadowMapSize, float farValue, int screenWidth, int screenHeight)
	{
		m_shadowMapSize = shadowMapSize;
		m_farValue = farValue;

		m_frustum.Ratio = 1.0f;
		if (screenHeight > 0)
			m_frustum.Ratio = screenWidth / (float)screenHeight;

		if (getVideoDriver()->getDriverType() == EDT_DIRECT3D11)
		{
			const float biasDX[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasDX);
		}
		else
		{
			const float biasGL[16] = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};
			m_bias.setM(biasGL);
		}
	}

	void CShadowMaps::update(CCamera* camera, const core::vector3df& lightDir)
	{
		m_lightDirection = lightDir;
		m_lightDirection.normalize();

		m_frustum.Fov = camera->getFOV() * core::DEGTORAD + 0.2f;
		m_frustum.NearPlane = camera->getNearValue();
		m_frustum.FarPlane = m_farValue;

		for (int i = 0; i < 4; i++)
			m_farBounds[i] = m_farValue;

		CTransform* cameraTransform = camera->getGameObject()->getTransform();

		// camera position
		const core::matrix4& mat = cameraTransform->getRelativeTransform();
		core::vector3df cameraPosition = mat.getTranslation();

		// camera forward
		core::vector3df cameraForward(0.0f, 0.0f, 1.0f);
		mat.rotateVect(cameraForward);
		cameraForward.normalize();

		// calc shadow volume
		updateFrustumCorners(cameraPosition, cameraForward);
		updateMatrix(cameraPosition);
	}

	void CShadowMaps::updateFrustumCorners(const core::vector3df& camPos, const core::vector3df& camForward)
	{
		core::vector3df center = camPos;
		core::vector3df viewDir = camForward;

		core::vector3df right = viewDir.crossProduct(Transform::Oy);
		right.normalize();

		SFrustumSplit& frustum = m_frustum;

		core::vector3df fc = center + viewDir * frustum.FarPlane;
		core::vector3df nc = center + viewDir * frustum.NearPlane;

		core::vector3df up = right.crossProduct(viewDir);
		up.normalize();

		// these heights and widths are half the heights and widths of
		// the near and far plane rectangles
		float near_height = tan(frustum.Fov / 2.0f) * frustum.NearPlane;
		float near_width = near_height * frustum.Ratio;
		float far_height = tan(frustum.Fov / 2.0f) * frustum.FarPlane;
		float far_width = far_height * frustum.Ratio;

		frustum.Corners[0] = nc - up * near_height - right * near_width; // near-bottom-left
		frustum.Corners[1] = nc + up * near_height - right * near_width; // near-top-left
		frustum.Corners[2] = nc + up * near_height + right * near_width; // near-top-right
		frustum.Corners[3] = nc - up * near_height + right * near_width; // near-bottom-right

		frustum.Corners[4] = fc - up * far_height - right * far_width; // far-bottom-left
		frustum.Corners[5] = fc + up * far_height - right * far_width; // far-top-left
		frustum.Corners[6] = fc + up * far_height + right * far_width; // far-top-right
		frustum.Corners[7] = fc - up * far_height + right * far_width; // far-bottom-right
	}

	void CShadowMaps::updateMatrix(core::vector3df& camPos)
	{
		SFrustumSplit& frustum = m_frustum;

		// Calculate frustum split center
		frustum.Center = core::vector3df(0.0f, 0.0f, 0.0f);

		for (int j = 0; j < 8; j++)
			frustum.Center += frustum.Corners[j];

		frustum.Center /= 8.0f;

		// Calculate bounding sphere radius
		float radius = 0.0f;

		for (int j = 0; j < 8; j++)
		{
			float length = (frustum.Corners[j] - frustum.Center).getLength();
			radius = core::max_(radius, length);
		}

		radius = ceil(radius * 16.0f) / 16.0f;

		// Find bounding box that fits the sphere
		core::vector3df radius3(radius, radius, radius);

		core::vector3df max = radius3;
		core::vector3df min = -radius3;

		core::vector3df cascadeExtents = max - min;

		// Push the light position back along the light direction by the near offset.
		core::vector3df shadowCameraPos = frustum.Center - m_lightDirection * m_nearOffset;

		// Compute bounding box for culling
		m_frustumBox.MinEdge = frustum.Center - radius3;
		m_frustumBox.MaxEdge = frustum.Center + radius3;

		// Fix: object shadow culling above camera
		core::vector3df highCameraPos = frustum.Center - m_lightDirection * radius * 2.0f;
		m_frustumBox.addInternalPoint(highCameraPos);

		// Add the near offset to the Z value of the cascade extents to make sure the orthographic frustum captures the entire frustum split (else it will exhibit cut-off issues).
		core::matrix4 ortho;
		ortho.buildProjectionMatrixOrthoLH(max.X - min.X, max.Y - min.Y, -m_nearOffset, m_nearOffset + cascadeExtents.Z + m_farValue);

		core::matrix4 view;
		view.buildCameraLookAtMatrixLH(shadowCameraPos, frustum.Center, Transform::Oy);

		m_projMatrices = ortho;
		m_viewMatrices = view;

		core::matrix4 mvp = m_projMatrices * m_viewMatrices;
		core::matrix4 shadowMatrix = m_bias * mvp;

		// we clone to 4 matrices to fit with cascaded shadow shader
		float* p = m_shadowMatrices;
		for (int i = 0; i < 4; i++)
		{
			memcpy(p, shadowMatrix.pointer(), 16 * sizeof(float));
			p += 16;
		}
	}
}