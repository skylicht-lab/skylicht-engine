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
	class CBoundShadowMaps
	{
	protected:
		int m_shadowMapSize;
		float m_nearOffset;

		core::vector3df m_lightDirection;

		core::matrix4 m_bias;
		core::matrix4 m_viewMatrices;
		core::matrix4 m_projMatrices;
		core::matrix4 m_textureMatrices;

		core::aabbox3df m_frustumBox;

		float m_shadowMatrices[16];

		float m_farValue;

	public:
		CBoundShadowMaps();

		virtual ~CBoundShadowMaps();

		void init(int shadowMapSize, float farValue, int screenWidth, int screenHeight);

		void update(CCamera* camera, const core::vector3df& lightDir, const core::aabbox3df& bound);

		const core::aabbox3df& getFrustumBox()
		{
			return m_frustumBox;
		}

		const core::matrix4& getViewMatrices()
		{
			return m_viewMatrices;
		}

		const core::matrix4& getProjectionMatrices()
		{
			return m_projMatrices;
		}

		float* getShadowMatrices()
		{
			return m_shadowMatrices;
		}

	protected:

		void updateMatrix(core::vector3df& camPos, const core::aabbox3df& bound);
	};
}