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

namespace Skylicht
{
	namespace Particle
	{
		class CParticle;
		class CGroup;

		int random(int from, int to);

		float random(float from, float to);

		void random_reset(s32 seed);

		enum EZone
		{
			Sphere,
			Point,
			AABox,
			Plane,
			Line,
			Ring,
			Cylinder
		};

		class CZone
		{
		protected:
			core::vector3df m_position;

			EZone m_type;

		public:
			CZone(EZone type);

			virtual ~CZone();

			inline void setPosition(const core::vector3df& pos)
			{
				m_position = pos;
			}

			const core::vector3df& getPosition()
			{
				return m_position;
			}

			inline EZone getType()
			{
				return m_type;
			}

			void normalizeOrRandomize(core::vector3df& v);

			virtual void generatePosition(CParticle& particle, bool full, CGroup* group) = 0;

			virtual core::vector3df computeNormal(const core::vector3df& point, CGroup* group) = 0;
		};
	}
}