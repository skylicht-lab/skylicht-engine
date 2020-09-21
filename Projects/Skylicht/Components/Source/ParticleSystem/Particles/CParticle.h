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

#include "Entity/IEntityData.h"

namespace Skylicht
{
	namespace Particle
	{
		enum EParticleParams
		{
			Scale = 0,
			ScaleX,
			ScaleY,
			ScaleZ,
			RotateX,
			RotateY,
			RotateZ,
			ColorR,
			ColorG,
			ColorB,
			ColorA,
			Mass,
			FrameIndex,
			RotateSpeedX,
			RotateSpeedY,
			RotateSpeedZ,
			NumParams
		};

		class CParticle
		{
		public:
			u32 Index;

			bool Immortal;

			float Params[NumParams];
			float StartValue[NumParams];
			float EndValue[NumParams];

			float Age;
			float Life;
			float LifeTime;

			bool HaveRotate;

			core::vector3df Position;
			core::vector3df Rotation;
			core::vector3df Velocity;

		public:
			CParticle(u32 index);

			virtual ~CParticle();

			void swap(CParticle& p);
		};
	}
}