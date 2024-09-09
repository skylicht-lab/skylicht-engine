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

#include "Utils/CActivator.h"
#include "Components/CComponentSystem.h"
#include "Transform/CTransform.h"

namespace Skylicht
{
	class SKYLICHT_API CCamera : public CComponentSystem
	{
	public:
		enum ECameraProjection
		{
			Ortho,
			Frustum,
			Perspective,
			OrthoUI,
			Custom,
		};

	protected:
		ECameraProjection m_projectionType;

		float m_nearValue;
		float m_farValue;
		float m_fov;
		float m_aspect;
		float m_viewportAspect;

		bool m_customOrthoSize;
		float m_orthoUIW;
		float m_orthoUIH;

		float m_orthoScale;

		core::vector3df m_up;

		SViewFrustum m_viewArea;

		core::dimension2du m_screenSize;

		bool m_inputReceiver;

		u32 m_cullingMask;

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

		void setViewMatrix(const core::matrix4& view, const core::vector3df& position);

		void setProjectionMatrix(const core::matrix4& prj);

		void setPosition(const core::vector3df& position);

		core::vector3df getPosition();

		void lookAt(const core::vector3df& position, const core::vector3df& target, const core::vector3df& up);

		void lookAt(const core::vector3df& target, const core::vector3df& up);

		void setUpVector(const core::vector3df& up);

		void setLookVector(core::vector3df look);

		inline const core::vector3df& getUpVector()
		{
			return m_up;
		}

		core::vector3df getLookVector();

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

		inline void setOrthoScale(float s)
		{
			m_orthoScale = s;
		}

		inline float getOrthoScale()
		{
			return m_orthoScale;
		}

		inline void setOrthoUISize(float w, float h)
		{
			m_orthoUIH = h;
			m_orthoUIW = w;
		}

		inline void enableCustomOrthoUISize(bool b)
		{
			m_customOrthoSize = b;
		}

		inline void setAspect(float f)
		{
			m_aspect = f;
			recalculateProjectionMatrix();
		}

		inline float getAspect()
		{
			if (m_aspect > 0)
				return m_aspect;

			return m_viewportAspect;
		}

		inline u32 getCullingMask()
		{
			return m_cullingMask;
		}

		inline void setCullingMask(u32 mask)
		{
			m_cullingMask = mask;
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

		DECLARE_GETTYPENAME(CCamera)
	};
}