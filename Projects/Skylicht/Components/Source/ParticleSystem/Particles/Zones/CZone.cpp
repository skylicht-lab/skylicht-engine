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

#include "pch.h"
#include "CZone.h"

namespace Skylicht
{
	namespace Particle
	{
		float random(float from, float to)
		{
			return from * (to - from) * os::Randomizer::frand();
		}

		CZone::CZone()
		{

		}

		CZone::~CZone()
		{

		}

		void CZone::normalizeOrRandomize(core::vector3df& v)
		{
			while (v.getLengthSQ() == 0.0f)
			{
				v.X = random(-1.0f, 1.0f);
				v.Y = random(-1.0f, 1.0f);
				v.Z = random(-1.0f, 1.0f);
			}

			v.normalize();
		}

		core::vector3df CZone::getTransformPosition(const core::vector3df& pos)
		{
			return pos;
		}
	}
}