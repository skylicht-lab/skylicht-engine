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

#include "CLight.h"

namespace Skylicht
{
	/// @brief This object holds the parameters for area lights.
	/// @ingroup Lighting
	/// 
	/// @image html Lighting/area_light.jpg width=1200px
	/// 
	/// @see CShaderLighting
	class SKYLICHT_API CAreaLight : public CLight
	{
	protected:
		float m_sizeX;
		float m_sizeY;
		bool m_needRenderShadowDepth;
		core::aabbox3df m_worldBounds;
	public:
		CAreaLight();

		virtual ~CAreaLight();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void endUpdate();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual void setRenderLightType(ERenderLightType type)
		{
			CLight::setRenderLightType(type);
			m_needRenderShadowDepth = true;
		}

		DECLARE_GETTYPENAME(CAreaLight)

		inline void setSize(float x, float y)
		{
			m_sizeX = x;
			m_sizeY = y;
		}

		inline float getSizeX()
		{
			return m_sizeX;
		}

		inline float getSizeY()
		{
			return m_sizeY;
		}

		bool needRenderShadowDepth();

		void beginRenderShadowDepth();

		void endRenderShadowDepth();

		core::vector3df getPosition();

		const core::matrix4& getWorldTransform();

		inline const core::aabbox3df& getWorldBounds()
		{
			return m_worldBounds;
		}
	};
}