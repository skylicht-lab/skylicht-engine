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

#pragma once

#include "Camera/CCamera.h"

#define MAX_FRUSTUM_SPLITS 8

namespace Skylicht
{
	struct SFrustumSplit
	{
		float NearPlane;
		float FarPlane;
		float Ratio;
		float Fov;
		core::vector3df Center;
		core::vector3df Corners[8];
	};

	class CCascadedShadowMaps
	{
	protected:
		float m_lambda;
		float m_nearOffset;
		int m_splitCount;
		int m_shadowMapSize;

		SFrustumSplit m_splits[MAX_FRUSTUM_SPLITS];
		float m_farBounds[MAX_FRUSTUM_SPLITS];

		core::vector3df m_lightDirection;

		core::matrix4 m_bias;
		core::matrix4 m_lightView;
		core::matrix4 m_viewMatrices[MAX_FRUSTUM_SPLITS];
		core::matrix4 m_projMatrices[MAX_FRUSTUM_SPLITS];
		core::matrix4 m_textureMatrices[MAX_FRUSTUM_SPLITS];

		float m_shadowMatrices[16 * MAX_FRUSTUM_SPLITS];

		float m_farValue;

	public:
		CCascadedShadowMaps();

		virtual ~CCascadedShadowMaps();

		void init(int splitCount, int shadowMapSize, float farValue, int screenWidth, int screenHeight);

		void update(CCamera *camera, const core::vector3df& lightDir);

		int getSplitCount()
		{
			return m_splitCount;
		}

		const core::matrix4& getViewMatrices(int i)
		{
			return m_viewMatrices[i];
		}

		const core::matrix4& getProjectionMatrices(int i)
		{
			return m_projMatrices[i];
		}

		float* getShadowDistance()
		{
			return m_farBounds;
		}

		float* getShadowMatrices()
		{
			return m_shadowMatrices;
		}

	protected:

		void updateSplits(CCamera *camera);

		void updateFrustumCorners(const core::vector3df& camPos, const core::vector3df& camForward);

		void updateMatrix();
	};
}