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
			/** @brief The parameter type being animated. */
			EParticleParams m_type;

			/** @brief If true, picks a random start value between start1 and start2. */
			bool m_randomStart;
			float m_start1;
			float m_start2;

			/** @brief Whether to interpolate towards an end value. */
			bool m_enableEndValue;
			/** @brief If true, picks a random end value between end1 and end2. */
			bool m_randomEnd;
			float m_end1;
			float m_end2;

			/** @brief Parent group. */
			CGroup* m_group;

			/** @brief Optional custom curve for interpolation. */
			CInterpolator* m_interpolator;

			std::string m_typeName;

		public:
			CModel(CGroup* group, EParticleParams type);

			virtual ~CModel();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Gets the display name of the model. */
			const wchar_t* getName();

			/** @brief Gets the parameter type. */
			inline EParticleParams getType()
			{
				return m_type;
			}

			/** @brief Gets parent group. */
			inline CGroup* getGroup()
			{
				return m_group;
			}

			/** @brief Sets fixed start value. */
			inline CModel* setStart(float f)
			{
				m_start1 = f;
				m_start2 = f;
				m_randomStart = true;
				return this;
			}

			/** @brief Sets random start range. */
			inline CModel* setStart(float f1, float f2)
			{
				m_start1 = f1;
				m_start2 = f2;
				m_randomStart = true;
				return this;
			}

			/** @brief Sets fixed end value. */
			inline CModel* setEnd(float f)
			{
				m_end1 = f;
				m_end2 = f;
				m_randomEnd = true;
				return this;
			}

			/** @brief Sets random end range. */
			inline CModel* setEnd(float f1, float f2)
			{
				m_end1 = f1;
				m_end2 = f2;
				m_randomEnd = true;
				return this;
			}

			/** @brief Checks if end value is enabled. */
			inline bool isEnableEndValue()
			{
				return m_enableEndValue;
			}

			/** @brief Enables or disables interpolation to end value. */
			inline void enableEndValue(bool b)
			{
				m_enableEndValue = b;
			}

			/** @brief Checks if start is random. */
			inline bool isRandomStart()
			{
				return m_randomStart;
			}

			/** @brief Checks if end is random. */
			inline bool isRandomEnd()
			{
				return m_randomEnd;
			}

			float getStartValue1()
			{
				return m_start1;
			}

			void setStartValue1(float f)
			{
				m_start1 = f;
			}

			float getStartValue2()
			{
				return m_start2;
			}

			void setStartValue2(float f)
			{
				m_start2 = f;
			}

			float getEndValue1()
			{
				return m_end1;
			}

			void setEndValue1(float f)
			{
				m_end1 = f;
			}

			float getEndValue2()
			{
				return m_end2;
			}

			void setEndValue2(float f)
			{
				m_end2 = f;
			}

			/** @brief Generates a random start value from range. */
			float getRandomStart();

			/** @brief Generates a random end value from range. */
			float getRandomEnd();

			/** @brief Sets a custom curve for parameter animation. */
			inline void setInterpolator(CInterpolator* interpolate)
			{
				m_interpolator = interpolate;
			}

			/** @brief Gets current interpolator. */
			inline CInterpolator* getInterpolator()
			{
				return m_interpolator;
			}

			virtual const char* getTypeName();
		};
	}
}