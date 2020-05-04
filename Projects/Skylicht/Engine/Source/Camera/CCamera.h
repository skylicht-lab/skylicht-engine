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

#include "Components/CComponentSystem.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	class CCamera : public CComponentSystem
	{
	public:
		enum ECameraProjection
		{
			Ortho,
			Frustum,
			Perspective,
			OrthoUI,
		};

	protected:
		ECameraProjection m_projectionType;

		float m_nearValue;
		float m_farValue;
		float m_fov;
		float m_aspect;

		core::vector3df m_up;

		SViewFrustum m_viewArea;

		core::dimension2du m_screenSize;

		bool m_inputReceiver;
	public:
		CCamera();

		virtual ~CCamera();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void endUpdate();

	public:

		void setProjectionType(ECameraProjection projection);

		inline ECameraProjection getProjectionType()
		{
			return m_projectionType;
		}

		const core::matrix4& getProjectionMatrix() const;

		const core::matrix4& getViewMatrix() const;

		void setViewMatrix(const core::matrix4& view);

		void setPosition(const core::vector3df& position);

		void lookAt(const core::vector3df& position, const core::vector3df& target, const core::vector3df& up);

		void lookAt(const core::vector3df& target, const core::vector3df& up);

		void setUpVector(const core::vector3df& up);

		inline void setNearValue(float f)
		{
			m_nearValue = f;
			recalculateProjectionMatrix();
		}

		inline float getNearValue()
		{
			return m_nearValue;
		}

		inline void setFarValue(float f)
		{
			m_farValue = f;
			recalculateProjectionMatrix();
		}

		inline float getFarValue()
		{
			return m_farValue;
		}

		inline void setFOV(float fov)
		{
			m_fov = fov;
			recalculateProjectionMatrix();
		}

		inline float getFOV()
		{
			return m_fov;
		}

		inline void setAspect(float f)
		{
			m_aspect = f;
			recalculateProjectionMatrix();
		}

		void recalculateProjectionMatrix();

		void recalculateViewMatrix();

		const SViewFrustum& getViewFrustum()
		{
			return m_viewArea;
		}

		inline void setInputReceiver(bool b)
		{
			m_inputReceiver = b;
		}

		inline bool isInputReceiverEnabled()
		{
			return m_inputReceiver;
		}
	};
}