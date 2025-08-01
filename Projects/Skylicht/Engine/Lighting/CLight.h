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
	class SKYLICHT_API CLight : public CComponentSystem
	{
	public:
		enum ELightType
		{
			Realtime = 0,
			Baked,
			Mixed
		};

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
		float m_spotInnerCutoff;
		float m_spotExponent;

		//! Direction of the light.
		/** If Type is ELT_POINT, it is ignored. Changed via light scene node's rotation. */
		core::vector3df m_direction;

		//! Does the light cast shadows?
		bool m_castShadow;

		//! Does this shadow alway updates
		bool m_dynamicShadow;

		//! Intensity of light
		float m_intensity;

		//! Number bounce of light on bake lighting
		u32 m_bakeBounce;

		//! Light type
		ELightType m_type;

	public:
		CLight();

		virtual ~CLight();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

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

		core::aabbox3df getBBBox();

		inline ELightType getLightType()
		{
			return m_type;
		}

		virtual void setLightType(ELightType type)
		{
			m_type = type;
		}

		void setBounce(u32 b)
		{
			m_bakeBounce = b;
		}

		u32 getBounce()
		{
			return m_bakeBounce;
		}

		void setRadius(float r)
		{
			r = core::max_(r, 0.01f);
			m_radius = r;
			m_attenuation = 1.0f / m_radius;
		}

		inline void setSpotAngle(float angle)
		{
			m_spotCutoff = angle;
		}

		inline float getSplotCutoff()
		{
			return m_spotCutoff;
		}

		inline void setSpotInnerAngle(float angle)
		{
			m_spotInnerCutoff = angle;
		}

		inline float getSpotInnerCutof()
		{
			return m_spotInnerCutoff;
		}

		inline void setSpotExponent(float e)
		{
			m_spotExponent = e;
		}

		inline float getSpotExponent()
		{
			return m_spotExponent;
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

		inline bool isDynamicShadow()
		{
			return m_dynamicShadow;
		}

		inline void setDynamicShadow(bool shadow)
		{
			m_dynamicShadow = shadow;
		}

		inline void setIntensity(float f)
		{
			m_intensity = f;
		}

		inline float getIntensity()
		{
			return m_intensity;
		}
	};
}