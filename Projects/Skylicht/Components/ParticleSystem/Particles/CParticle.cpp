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
			HaveRotate(false),
			SubEmitterDirection(0.0f, 1.0f, 0.0f),
			UserData(NULL)
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
			for (int i = 0; i < NumParams; i++)
			{
				float temp = Params[i];
				Params[i] = p.Params[i];
				p.Params[i] = temp;

				temp = StartValue[i];
				StartValue[i] = p.StartValue[i];
				p.StartValue[i] = temp;

				temp = EndValue[i];
				EndValue[i] = p.EndValue[i];
				p.EndValue[i] = temp;
			}

			int parentIndex = ParentIndex;
			bool immortal = Immortal;

			float age = Age;
			float life = Life;
			float lifeTime = LifeTime;

			bool haveRotate = HaveRotate;

			core::vector3df position = Position;
			core::vector3df rotation = Rotation;
			core::vector3df velocity = Velocity;
			core::vector3df lastPosition = LastPosition;
			core::vector3df subEmitterDirection = SubEmitterDirection;
			void* userData = UserData;

			// note: dont swap [p.Index], just swap data
			ParentIndex = p.ParentIndex;
			Age = p.Age;
			Life = p.Life;
			LifeTime = p.LifeTime;

			HaveRotate = p.HaveRotate;

			Position = p.Position;
			Rotation = p.Rotation;
			Velocity = p.Velocity;
			LastPosition = p.LastPosition;

			SubEmitterDirection = p.SubEmitterDirection;
			UserData = p.UserData;

			p.ParentIndex = parentIndex;
			p.Immortal = immortal;

			p.Age = age;
			p.Life = life;
			p.LifeTime = lifeTime;

			p.HaveRotate = haveRotate;

			p.Position = position;
			p.Rotation = rotation;
			p.Velocity = velocity;
			p.LastPosition = lastPosition;

			p.SubEmitterDirection = subEmitterDirection;
			p.UserData = userData;
		}
	}
}