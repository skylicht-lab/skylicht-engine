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

#include "CInterpolator.h"
#include "ParticleSystem/Particles/CParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		class COMPONENT_API CModel
		{
		protected:
			EParticleParams m_type;

			bool m_haveStart;
			float m_start1;
			float m_start2;

			bool m_haveEnd;
			float m_end1;
			float m_end2;

			CInterpolator *m_interpolator;

		public:
			CModel(EParticleParams type);

			virtual ~CModel();

			inline EParticleParams getType()
			{
				return m_type;
			}

			inline CModel* setStart(float f)
			{
				m_start1 = f;
				m_start2 = f;
				m_haveStart = true;
				return this;
			}

			inline CModel* setStart(float f1, float f2)
			{
				m_start1 = f1;
				m_start2 = f2;

				m_haveStart = true;
				return this;
			}

			inline CModel* setEnd(float f)
			{
				m_end1 = f;
				m_end2 = f;
				m_haveEnd = true;
				return this;
			}

			inline CModel* setEnd(float f1, float f2)
			{
				m_end1 = f1;
				m_end2 = f2;

				m_haveEnd = true;
				return this;
			}

			inline bool haveStart()
			{
				return m_haveStart;
			}

			inline bool haveEnd()
			{
				return m_haveEnd;
			}

			float getStartValue1()
			{
				return m_start1;
			}

			float getStartValue2()
			{
				return m_start2;
			}

			float getEndValue1()
			{
				return m_end1;
			}

			float getEndValue2()
			{
				return m_end2;
			}

			float getRandomStart();

			float getRandomEnd();

			inline void setInterpolator(CInterpolator *interpolate)
			{
				m_interpolator = interpolate;
			}

			inline CInterpolator* getInterpolator()
			{
				return m_interpolator;
			}
		};
	}
}