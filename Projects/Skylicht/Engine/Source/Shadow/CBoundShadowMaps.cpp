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
#include "CBoundShadowMaps.h"
#include "Transform/CTransform.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	CBoundShadowMaps::CBoundShadowMaps() :
		m_shadowMapSize(2048),
		m_farValue(500.0f),
		m_nearOffset(300.0f)
	{

	}

	CBoundShadowMaps::~CBoundShadowMaps()
	{

	}

	void CBoundShadowMaps::init(int shadowMapSize, float farValue, int screenWidth, int screenHeight)
	{
		m_shadowMapSize = shadowMapSize;
		m_farValue = farValue;

		float ratio = 1.0f;
		if (screenHeight > 0)
			ratio = screenWidth / (float)screenHeight;

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

	void CBoundShadowMaps::update(CCamera* camera, const core::vector3df& lightDir, const core::aabbox3df& bound)
	{
		m_lightDirection = lightDir;
		m_lightDirection.normalize();

		CTransform* cameraTransform = camera->getGameObject()->getTransform();

		// camera position
		const core::matrix4& mat = cameraTransform->getRelativeTransform();
		core::vector3df cameraPosition = mat.getTranslation();

		// calc shadow volume
		updateMatrix(cameraPosition, bound);
	}

	void CBoundShadowMaps::updateMatrix(core::vector3df& camPos, const core::aabbox3df& bound)
	{
		core::matrix4 projection;

		// Calculate bounding sphere radius
		core::vector3df center = bound.getCenter();

		float radius = 0.0f;

		float length = (bound.MinEdge - center).getLength();
		radius = core::max_(radius, length);

		radius = ceil(radius * 16.0f) / 16.0f;

		// Find bounding box that fits the sphere
		core::vector3df radius3(radius, radius, radius);

		core::vector3df max = radius3;
		core::vector3df min = -radius3;

		core::vector3df cascadeExtents = max - min;

		// Push the light position back along the light direction by the near offset.
		core::vector3df shadowCameraPos = center - m_lightDirection * m_nearOffset;

		// Compute bounding box for culling
		m_frustumBox.MinEdge = center - radius3;
		m_frustumBox.MaxEdge = center + radius3;

		// Fix: object shadow culling above camera
		core::vector3df highCameraPos = center - m_lightDirection * radius * 2.0f;
		m_frustumBox.addInternalPoint(highCameraPos);

		// Add the near offset to the Z value of the cascade extents to make sure the orthographic frustum captures the entire frustum split (else it will exhibit cut-off issues).
		core::matrix4 ortho;
		ortho.buildProjectionMatrixOrthoLH(max.X - min.X, max.Y - min.Y, -m_nearOffset, m_nearOffset + cascadeExtents.Z + m_farValue);

		core::matrix4 view;
		view.buildCameraLookAtMatrixLH(shadowCameraPos, center, CTransform::s_oy);

		m_projMatrices = ortho;
		m_viewMatrices = view;

		core::matrix4 mvp = m_projMatrices * m_viewMatrices;
		core::matrix4 shadowMatrix = m_bias * mvp;
		memcpy(m_shadowMatrices, shadowMatrix.pointer(), 16 * sizeof(float));
	}
}