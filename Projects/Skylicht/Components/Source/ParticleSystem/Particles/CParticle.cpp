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
			Immortal(false)
		{
			memset(Params, 0, sizeof(float) * NumParams);

			Params[ColorR] = 255;
			Params[ColorG] = 255;
			Params[ColorB] = 255;
			Params[ColorA] = 255;

			Params[Age] = 0.0f;
			Params[Life] = 0.0f;
			Params[Mass] = 1.0f;
			Params[FrameIndex] = 0.0f;
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
		}
	}
}