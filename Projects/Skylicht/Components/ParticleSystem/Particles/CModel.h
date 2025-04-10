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

			const wchar_t* getName();

			inline EParticleParams getType()
			{
				return m_type;
			}

			inline CGroup* getGroup()
			{
				return m_group;
			}

			inline CModel* setStart(float f)
			{
				m_start1 = f;
				m_start2 = f;
				m_randomStart = true;
				return this;
			}

			inline CModel* setStart(float f1, float f2)
			{
				m_start1 = f1;
				m_start2 = f2;
				m_randomStart = true;
				return this;
			}

			inline CModel* setEnd(float f)
			{
				m_end1 = f;
				m_end2 = f;
				m_randomEnd = true;
				return this;
			}

			inline CModel* setEnd(float f1, float f2)
			{
				m_end1 = f1;
				m_end2 = f2;
				m_randomEnd = true;
				return this;
			}

			inline bool isEnableEndValue()
			{
				return m_enableEndValue;
			}

			inline void enableEndValue(bool b)
			{
				m_enableEndValue = b;
			}

			inline bool isRandomStart()
			{
				return m_randomStart;
			}

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

			float getRandomStart();

			float getRandomEnd();

			inline void setInterpolator(CInterpolator* interpolate)
			{
				m_interpolator = interpolate;
			}

			inline CInterpolator* getInterpolator()
			{
				return m_interpolator;
			}

			virtual std::string getTypeName();
		};
	}
}