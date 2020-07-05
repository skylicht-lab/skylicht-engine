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

// References
// https://github.com/diharaw/CascadedShadowMaps/

#include "pch.h"
#include "CCascadedShadowMaps.h"
#include "Transform/CTransform.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CCascadedShadowMaps::CCascadedShadowMaps() :
		m_shadowMapSize(2048),
		m_lambda(0.9f),
		m_nearOffset(50.0f),
		m_farValue(200.0f)
	{

	}

	CCascadedShadowMaps::~CCascadedShadowMaps()
	{

	}

	void CCascadedShadowMaps::init(int splitCount, int shadowMapSize, float farValue, int screenWidth, int screenHeight)
	{
		m_splitCount = splitCount;
		m_shadowMapSize = shadowMapSize;
		m_farValue = farValue;

		float ratio = 1.0f;
		for (int i = 0; i < m_splitCount; i++)
			m_splits[i].Ratio = ratio;

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

	void CCascadedShadowMaps::update(CCamera *camera, const core::vector3df& lightDir)
	{
		float cameraFov = camera->getFOV();

		// note that fov is in radians here and in OpenGL it is in degrees.
		// the 0.2f factor is important because we might get artifacts at
		// the screen borders.
		for (int i = 0; i < m_splitCount; i++)
			m_splits[i].Fov = cameraFov * core::DEGTORAD + 0.2f;

		m_lightDirection = lightDir;
		m_lightDirection.normalize();

		CTransform *cameraTransform = camera->getGameObject()->getTransform();

		// camera position
		const core::matrix4& mat = cameraTransform->getMatrixTransform();
		core::vector3df cameraPosition = mat.getTranslation();

		// camera forward
		core::vector3df cameraForward(0.0f, 0.0f, 1.0f);
		mat.rotateVect(cameraForward);
		cameraForward.normalize();

		// calc shadow volume
		updateSplits(camera);
		updateFrustumCorners(cameraPosition, cameraForward);
		updateMatrix();
	}

	void CCascadedShadowMaps::updateSplits(CCamera *camera)
	{
		float nd = camera->getNearValue();
		float fd = m_farValue;

		float lambda = m_lambda;
		float ratio = fd / nd;

		m_splits[0].NearPlane = nd;

		for (int i = 1; i < m_splitCount; i++)
		{
			float si = i / (float)m_splitCount;

			// Practical Split Scheme: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			float t_near = lambda * (nd * powf(ratio, si)) + (1 - lambda) * (nd + (fd - nd) * si);
			float t_far = t_near * 1.005f;
			m_splits[i].NearPlane = t_near;
			m_splits[i - 1].FarPlane = t_far;

			m_farBounds[i - 1] = t_far;
		}

		m_splits[m_splitCount - 1].FarPlane = fd;

		m_farBounds[m_splitCount - 1] = fd;
	}

	void CCascadedShadowMaps::updateFrustumCorners(const core::vector3df& camPos, const core::vector3df& camForward)
	{
		core::vector3df center = camPos;
		core::vector3df viewDir = camForward;

		core::vector3df right = viewDir.crossProduct(CTransform::s_oy);
		right.normalize();

		for (int i = 0; i < m_splitCount; i++)
		{
			SFrustumSplit& frustum = m_splits[i];

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
	}

	void CCascadedShadowMaps::updateMatrix()
	{
		core::matrix4 projection;

		for (int i = 0; i < m_splitCount; i++)
		{
			SFrustumSplit& frustum = m_splits[i];

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
			m_frustumBox[i].MinEdge = frustum.Center - radius3;
			m_frustumBox[i].MaxEdge = frustum.Center + radius3;
			m_frustumBox[i].addInternalPoint(shadowCameraPos);

			// Add the near offset to the Z value of the cascade extents to make sure the orthographic frustum captures the entire frustum split (else it will exhibit cut-off issues).
			core::matrix4 ortho;
			ortho.buildProjectionMatrixOrthoLH(max.X - min.X, max.Y - min.Y, -m_nearOffset, m_nearOffset + cascadeExtents.Z);

			core::matrix4 view;
			view.buildCameraLookAtMatrixLH(shadowCameraPos, frustum.Center, CTransform::s_oy);

			m_projMatrices[i] = ortho;
			m_viewMatrices[i] = view;


			core::matrix4 cropMatrix = ortho * view;

			core::vector3df shadowOrigin(0.0f, 0.0f, 0.0f);
			cropMatrix.transformVect(shadowOrigin);
			shadowOrigin = shadowOrigin * (m_shadowMapSize / 2.0f);

			core::vector3df roundedOrigin;
			roundedOrigin.X = round(shadowOrigin.X);
			roundedOrigin.Y = round(shadowOrigin.Y);
			roundedOrigin.Z = round(shadowOrigin.Z);

			core::vector3df roundOffset = roundedOrigin - shadowOrigin;

			roundOffset = roundOffset * (2.0f / m_shadowMapSize);
			roundOffset.Z = 0.0f;

			core::matrix4& shadowProj = m_projMatrices[i];

			shadowProj(3, 0) += roundOffset.X;
			shadowProj(3, 1) += roundOffset.Y;
			shadowProj(3, 2) += roundOffset.Z;

			core::matrix4 mvp = m_projMatrices[i] * m_viewMatrices[i];
			core::matrix4 shadowMatrix = m_bias * mvp;
			memcpy(m_shadowMatrices + i * 16, shadowMatrix.pointer(), 16 * sizeof(float));
		}
	}
}