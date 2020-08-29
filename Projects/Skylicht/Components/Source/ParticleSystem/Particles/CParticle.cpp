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
#include "CParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticle::CParticle() :
			Age(0.0f),
			Life(0.0f),
			Immortal(false),
			Mass(1.0f)
		{

		}

		CParticle::~CParticle()
		{

		}

		void CParticle::swap(CParticle& p)
		{
			float age = Age;
			float life = Life;
			float mass = Mass;
			bool immortal = Immortal;
			core::vector3df position = Position;
			core::vector3df velocity = Velocity;
			video::SColor color = Color;

			Age = p.Age;
			Life = p.Life;
			Mass = p.Mass;
			Immortal = p.Immortal;
			Position = p.Position;
			Velocity = p.Velocity;
			Color = p.Color;

			p.Age = age;
			p.Life = life;
			p.Immortal = immortal;
			p.Position = position;
			p.Velocity = velocity;
			p.Color = color;
			p.Mass = mass;
		}
	}
}