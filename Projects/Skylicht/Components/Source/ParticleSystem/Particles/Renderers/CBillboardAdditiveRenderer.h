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

#include "IRenderer.h"
#include "ParticleSystem/Particles/CParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		class CBillboardAdditiveRenderer : public IRenderer
		{
		protected:
			u32 m_atlasNx;
			u32 m_atlasNy;

			static const u32 NB_INDICES_PER_QUAD = 6;
			static const u32 NB_VERTICES_PER_QUAD = 4;

		public:
			CBillboardAdditiveRenderer();

			virtual ~CBillboardAdditiveRenderer();

			virtual void getParticleBuffer(IMeshBuffer *buffer);

			virtual void updateParticleBuffer(IMeshBuffer *buffer, CParticle *particles, int num);

			void setAtlas(u32 x, u32 y)
			{
				m_atlasNx = x;
				m_atlasNy = y;

				if (m_atlasNx < 1)
					m_atlasNx = 1;

				if (m_atlasNy < 1)
					m_atlasNy = 1;
			}

			inline u32 getAtlasX()
			{
				return m_atlasNx;
			}

			inline u32 getAtlasY()
			{
				return m_atlasNy;
			}

			void setTexture(int slot, ITexture *texture);

			virtual u32 getTotalFrames()
			{
				return m_atlasNx * m_atlasNy;
			}

			inline float getFrameWidth()
			{
				return 1.0f / m_atlasNx;
			}

			inline float getFrameHeight()
			{
				return 1.0f / m_atlasNy;
			}
		};
	}
}