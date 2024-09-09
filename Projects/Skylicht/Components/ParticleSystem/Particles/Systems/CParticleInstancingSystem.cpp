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
#include "CParticleInstancingSystem.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

#include "ParticleSystem/Particles/Renderers/CQuadRenderer.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleInstancingSystem::CParticleInstancingSystem()
		{

		}

		CParticleInstancingSystem::~CParticleInstancingSystem()
		{

		}

		void CParticleInstancingSystem::update(CParticle* particles, int num, CGroup* group, float dt)
		{
			CVertexBuffer<SParticleInstance>* buffer = group->getIntancing()->getInstanceBuffer();

			buffer->set_used(num);

			if (num == 0)
				return;

			SParticleInstance* vtx = (SParticleInstance*)buffer->getVertices();

			CParticle* p;
			float* params;
			SParticleInstance* data;

			u32 frameX = 1;
			u32 frameY = 1;

			IRenderer* renderer = group->getRenderer();
			float sx = 1.0f;
			float sy = 1.0f;
			float sz = 1.0f;

			if (renderer != NULL)
			{
				sx = renderer->SizeX;
				sy = renderer->SizeY;
				sz = renderer->SizeZ;

				if (renderer->getType() == Particle::Quad)
				{
					CQuadRenderer* quadRenderer = (CQuadRenderer*)renderer;
					frameX = quadRenderer->getAtlasX();
					frameY = quadRenderer->getAtlasY();
				}
			}

			u32 totalFrames = frameX * frameY;
			float frameW = 1.0f / frameX;
			float frameH = 1.0f / frameY;
			u32 frame, row, col;

#pragma omp parallel for private(p, params, data, frame, row, col)
			for (int i = 0; i < num; i++)
			{
				p = particles + i;
				params = p->Params;
				data = vtx + i;

				data->Pos.X = p->Position.X;
				data->Pos.Y = p->Position.Y;
				data->Pos.Z = p->Position.Z;

				data->Color.set(
					(u32)(params[ColorA] * 255.0f),
					(u32)(params[ColorR] * 255.0f),
					(u32)(params[ColorG] * 255.0f),
					(u32)(params[ColorB] * 255.0f)
				);

				data->Size.X = sx * params[ScaleX];
				data->Size.Y = sy * params[ScaleY];
				data->Size.Z = sz * params[ScaleZ];

				data->Rotation.X = p->Rotation.X;
				data->Rotation.Y = p->Rotation.Y;
				data->Rotation.Z = p->Rotation.Z;

				data->Velocity.X = p->Velocity.X;
				data->Velocity.Y = p->Velocity.Y;
				data->Velocity.Z = p->Velocity.Z;

				frame = (u32)params[FrameIndex];
				frame = frame < 0 ? 0 : frame;
				frame = frame >= totalFrames ? totalFrames - 1 : frame;

				row = frame / frameX;
				col = frame - (row * frameX);

				data->UVScale.set(frameW, frameH);
				data->UVOffset.set(col * frameW, row * frameH);
			}

			buffer->setDirty();
		}
	}
}