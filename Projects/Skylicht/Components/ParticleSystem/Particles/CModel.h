/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "Utils/CInterpolator.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CParticleSerializable.h"

namespace Skylicht
{
	namespace Particle
	{
		class CGroup;

		/**
		 * @class CModel
		 * @ingroup ParticleSystem
		 * @brief Data model for animating a particle parameter (EParticleParams) over its lifetime.
		 * @details Supports linear interpolation between start/end values or custom curves via CInterpolator.
		 */
		class COMPONENT_API CModel : public CParticleSerializable
		{
		protected:
			EParticleParams m_type;

			bool m_randomStart;
			float m_start1;
			float m_start2;

			bool m_enableEndValue;
			bool m_randomEnd;
			float m_end1;
			float m_end2;

			CGroup* m_group;

			CInterpolator* m_interpolator;

		public:
			CModel(CGroup* group, EParticleParams type);

			virtual ~CModel();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Get the display name of the animated parameter.
			 * @return Wstring identifier.
			 */
			const wchar_t* getName();

			/**
			 * @brief Get the enum ID of the parameter being animated.
			 * @return EParticleParams enum.
			 */
			inline EParticleParams getType()
			{
				return m_type;
			}

			/**
			 * @brief Get the group that owns this model.
			 * @return Pointer to CGroup.
			 */
			inline CGroup* getGroup()
			{
				return m_group;
			}

			/**
			 * @brief Set a fixed starting value for the parameter.
			 * @param f The start value.
			 * @return Pointer to this model (for chaining).
			 */
			inline CModel* setStart(float f)
			{
				m_start1 = f;
				m_start2 = f;
				m_randomStart = true;
				return this;
			}

			/**
			 * @brief Set a random starting range for the parameter.
			 * @param f1 Minimum start value.
			 * @param f2 Maximum start value.
			 * @return Pointer to this model (for chaining).
			 */
			inline CModel* setStart(float f1, float f2)
			{
				m_start1 = f1;
				m_start2 = f2;
				m_randomStart = true;
				return this;
			}

			/**
			 * @brief Set a fixed end value for the parameter.
			 * @param f The end value.
			 * @return Pointer to this model (for chaining).
			 */
			inline CModel* setEnd(float f)
			{
				m_end1 = f;
				m_end2 = f;
				m_randomEnd = true;
				return this;
			}

			/**
			 * @brief Set a random end range for the parameter.
			 * @param f1 Minimum end value.
			 * @param f2 Maximum end value.
			 * @return Pointer to this model (for chaining).
			 */
			inline CModel* setEnd(float f1, float f2)
			{
				m_end1 = f1;
				m_end2 = f2;
				m_randomEnd = true;
				return this;
			}

			/**
			 * @brief Check if interpolation to an end value is enabled.
			 * @return True if interpolation is active.
			 */
			inline bool isEnableEndValue()
			{
				return m_enableEndValue;
			}

			/**
			 * @brief Enable or disable interpolation to an end value.
			 * @param b True to enable.
			 */
			inline void enableEndValue(bool b)
			{
				m_enableEndValue = b;
			}

			/**
			 * @brief Check if the start value is randomized.
			 * @return True if random.
			 */
			inline bool isRandomStart()
			{
				return m_randomStart;
			}

			/**
			 * @brief Check if the end value is randomized.
			 * @return True if random.
			 */
			inline bool isRandomEnd()
			{
				return m_randomEnd;
			}

			/**
			 * @brief Set a custom curve for interpolation over particle life.
			 * @param interpolate Pointer to CInterpolator.
			 */
			inline void setInterpolator(CInterpolator* interpolate)
			{
				m_interpolator = interpolate;
			}

			/**
			 * @brief Get the assigned custom curve.
			 * @return Pointer to CInterpolator, or NULL if linear.
			 */
			inline CInterpolator* getInterpolator()
			{
				return m_interpolator;
			}

			virtual std::string getTypeName();
		};
	}
}