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
		CParticle::CParticle(u32 index) :
			Index(index),
			ParentIndex(-1),
			Immortal(false),
			Age(0.0f),
			Life(0.0f),
			LifeTime(0.0f),
			HaveRotate(false)
		{
			memset(StartValue, 0, sizeof(float) * NumParams);
			memset(EndValue, 0, sizeof(float) * NumParams);

			Params[ColorR] = 1.0f;
			Params[ColorG] = 1.0f;
			Params[ColorB] = 1.0f;
			Params[ColorA] = 1.0f;

			Params[ScaleX] = 1.0f;
			Params[ScaleY] = 1.0f;
			Params[ScaleZ] = 1.0f;

			Params[Mass] = 1.0f;
			Params[FrameIndex] = 0.0f;

			Params[RotateSpeedX] = 0.0f;
			Params[RotateSpeedY] = 0.0f;
			Params[RotateSpeedZ] = 0.0f;
		}

		CParticle::~CParticle()
		{

		}

		void CParticle::swap(CParticle& p)
		{
			float temp[NumParams];
			u32 size = sizeof(float) * NumParams;

			memcpy(temp, Params, size);
			memcpy(Params, p.Params, size);
			memcpy(p.Params, temp, size);

			memcpy(temp, StartValue, size);
			memcpy(StartValue, p.StartValue, size);
			memcpy(p.StartValue, temp, size);

			memcpy(temp, EndValue, size);
			memcpy(EndValue, p.EndValue, size);
			memcpy(p.EndValue, temp, size);

			float age = Age;
			float life = Life;
			float lifeTime = LifeTime;
			bool haveRotate = HaveRotate;
			int parentIndex = ParentIndex;

			core::vector3df lastPosition = LastPosition;
			core::vector3df position = Position;
			core::vector3df rotation = Rotation;
			core::vector3df velocity = Velocity;

			// note: 
			// Dont swap [p.Index], just swap data
			Age = p.Age;
			Life = p.Life;
			LifeTime = p.LifeTime;
			LastPosition = p.LastPosition;
			Position = p.Position;
			Rotation = p.Rotation;
			Velocity = p.Velocity;
			HaveRotate = p.HaveRotate;
			ParentIndex = p.ParentIndex;

			p.Age = age;
			p.Life = life;
			p.LifeTime = lifeTime;
			p.LastPosition = lastPosition;
			p.Position = position;
			p.Rotation = rotation;
			p.Velocity = velocity;
			p.HaveRotate = haveRotate;
			p.ParentIndex = parentIndex;
		}
	}
}