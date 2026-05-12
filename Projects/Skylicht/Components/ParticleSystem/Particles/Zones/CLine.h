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
		/**
		 * @class CLine
		 * @ingroup ParticleSystem
		 * @brief A zone that spawns particles along a line segment.
		 */
		class COMPONENT_API CLine : public CZone
		{
		protected:
			/** @brief Starting point of the segment. */
			core::vector3df m_p1;
			/** @brief Ending point of the segment. */
			core::vector3df m_p2;

		public:
			CLine(const core::vector3df& p1, const core::vector3df& p2);

			virtual ~CLine();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/** @brief Updates the line segment coordinates. */
			void setLine(const core::vector3df& p1, const core::vector3df& p2);

			/** @brief Updates the line segment using an Irrlicht line. */
			void setLine(const core::line3df& line)
			{
				setLine(line.start, line.end);
			}

			/** @brief Gets the segment as an Irrlicht line. */
			core::line3df getLine()
			{
				return core::line3df(m_p1, m_p2);
			}

			/** @brief Implementation: spawns at a random point along the segment. */
			virtual void generatePosition(CParticle& particle, bool full, CGroup* group);

			/** @brief Implementation: computes normal relative to the nearest point on segment. */
			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group);

			DECLARE_GETTYPENAME(CLine)
		};
	}
}