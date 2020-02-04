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

namespace Skylicht
{
	class CLight : public CComponentSystem
	{
	protected:
		//! Diffuse color emitted by the light.
		SColorf m_color;

		//! Attenuation factors (constant, linear, quadratic)
		/** Changes the light strength fading over distance.
		Can also be altered by setting the radius, Attenuation will change to 1.f/radius. Can be overridden after radius was set. */
		float m_attenuation;

		//! Radius of light. Everything within this radius will be lighted.
		float m_radius;

		//! The angle of the spot's outer cone. Ignored for other lights.
		float m_spotCutoff;

		//! Direction of the light.
		/** If Type is ELT_POINT, it is ignored. Changed via light scene node's rotation. */
		core::vector3df m_direction;

		//! Does the light cast shadows?
		bool m_castShadow;

	public:
		CLight();

		virtual ~CLight();

		inline const SColorf& getColor()
		{
			return m_color;
		}

		inline void setColor(const SColorf& c)
		{
			m_color = c;
		}

		inline float getAttenuation()
		{
			return m_attenuation;
		}

		inline float getRadius()
		{
			return m_radius;
		}

		void setRadius(float r)
		{
			r = core::max_(r, 0.01f);
			m_radius = r;
			m_attenuation = 1.0f / m_radius;
		}

		inline void setSpotAngle(float angle)
		{
			m_spotCutoff = core::DEGTORAD * angle;
		}

		inline float getSplotCutoff()
		{
			return m_spotCutoff;
		}

		inline core::vector3df& getDirection()
		{
			return m_direction;
		}

		inline bool isCastShadow()
		{
			return m_castShadow;
		}

		inline void setShadow(bool shadow)
		{
			m_castShadow = shadow;
		}
	};
}