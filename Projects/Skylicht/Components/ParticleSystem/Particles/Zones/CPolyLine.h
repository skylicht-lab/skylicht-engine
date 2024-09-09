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

#include "CZone.h"

namespace Skylicht
{
	namespace Particle
	{
		class COMPONENT_API CPolyLine : public CZone
		{
		protected:
			core::array<core::vector3df> m_point;
			core::array<float> m_segments;
			core::array<float> m_ratios;
			float m_length;
			core::vector3df m_normal;

		public:
			CPolyLine(const core::array<core::vector3df>& point);

			virtual ~CPolyLine();

			void setLine(const core::array<core::vector3df>& point);

			inline void setNormal(const core::vector3df& normal)
			{
				m_normal = normal;
			}

			virtual void generatePosition(CParticle& particle, bool full, CGroup* group);

			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group);
		};
	}
}