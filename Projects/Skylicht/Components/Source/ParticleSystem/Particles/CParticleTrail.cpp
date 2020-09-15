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
#include "CParticleTrail.h"
#include "Camera/CCamera.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleTrail::CParticleTrail(CGroup *group) :
			m_group(group),
			m_segmentLength(0.2f),
			m_width(0.1f),
			m_alpha(1.0f),
			m_trailCount(0),
			m_maxSegmentCount(10)
		{
			group->setCallback(this);

			m_meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), EIT_32BIT);
			m_meshBuffer->setHardwareMappingHint(EHM_STREAM);
		}

		CParticleTrail::~CParticleTrail()
		{
			if (m_group != NULL)
				m_group->setCallback(NULL);

			for (u32 i = 0, n = m_trails.size(); i < n; i++)
			{
				m_trails[i].DeleteData();
				// printf("Delete %d\n", i);
			}

			m_trails.clear();

			if (m_meshBuffer != NULL)
				m_meshBuffer->drop();
		}

		void CParticleTrail::update(CCamera *camera)
		{
			if (m_group == NULL)
				return;

			IVertexBuffer *buffer = m_meshBuffer->getVertexBuffer(0);
			IIndexBuffer *index = m_meshBuffer->getIndexBuffer();

			int indexID = 0;
			int vertexID = 0;

			// camera
			core::vector3df campos = camera->getGameObject()->getPosition();
			core::vector3df up = camera->getUpVector();

			int numVertex = m_trails.size() * 4 * m_maxSegmentCount;
			int numIndex = m_trails.size() * 6 * m_maxSegmentCount;

			buffer->set_used(numVertex);
			index->set_used(numIndex);

			S3DVertex* vertices = (S3DVertex*)buffer->getVertices();
			u32 *indices = (u32*)index->getIndices();

			u32 totalSegDraw = 0;

			for (u32 trailID = 0, numTrail = m_trails.size(); trailID < numTrail; trailID++)
			{
				STrailInfo& trail = m_trails[trailID];

				// todo setup buffer
				u32 numSeg = trail.Position->size() - 1;

				int endSeg = 0;
				if (trail.Position->size() > m_maxSegmentCount)
					endSeg = (int)(numSeg - m_maxSegmentCount + 1);

				u32 numSegDraw = 0;
				for (int i = numSeg; i >= endSeg; i--)
				{
					core::vector3df pos1;
					core::vector3df pos2;

					float thickness = 0.0f;
					float alpha = 1.0f;

					if (i == numSeg)
					{
						// first segment
						SParticlePosition &p = (*trail.Position)[i];
						pos2 = p.Position;
						thickness = p.Width;

						pos1 = trail.CurrentPosition;
					}
					else
					{
						// get nearest point
						SParticlePosition& p1 = (*trail.Position)[i + 1];
						SParticlePosition& p2 = (*trail.Position)[i];

						pos2 = p2.Position;
						pos1 = p1.Position;
						thickness = p1.Width;
					}

					// line direction
					core::vector3df direction = pos1 - pos2;
					direction.normalize();

					// look
					core::vector3df lookdir = pos1 - campos;
					if (lookdir.getLength() < 0.2f)
						continue;
					lookdir.normalize();

					// view angle
					f32 angle = lookdir.dotProduct(direction);

					if (angle < 0.9999f && angle > -0.9999f)
					{
						core::vector3df updown = direction.crossProduct(lookdir);
						updown.normalize();

						core::vector3df normal = direction.crossProduct(updown);

						int vertex = totalSegDraw * 4;
						int index = totalSegDraw * 6;

						SColor c((int)(255.0f * alpha), 255, 255, 255);

						// vertex buffer
						vertices[vertex + 0].Pos = pos1 - updown * thickness*0.5f;
						vertices[vertex + 0].Normal = normal;
						vertices[vertex + 0].Color = c;

						vertices[vertex + 1].Pos = pos1 + updown * thickness*0.5f;
						vertices[vertex + 1].Normal = normal;
						vertices[vertex + 1].Color = c;

						vertices[vertex + 2].Pos = pos2 - updown * thickness*0.5f;
						vertices[vertex + 2].Normal = normal;
						vertices[vertex + 2].Color = c;

						vertices[vertex + 3].Pos = pos2 + updown * thickness*0.5f;
						vertices[vertex + 3].Normal = normal;
						vertices[vertex + 3].Color = c;

						// index buffer
						indices[index + 0] = vertex + 0;
						indices[index + 1] = vertex + 1;
						indices[index + 2] = vertex + 2;

						indices[index + 3] = vertex + 1;
						indices[index + 4] = vertex + 3;
						indices[index + 5] = vertex + 2;

						// todo modify to link 2 segment (fix seam)
						if (numSegDraw >= 1)
						{
							vertex = (totalSegDraw - 1) * 4;
							vertices[vertex + 2].Pos = pos1 - updown * thickness*0.5f;
							vertices[vertex + 2].Color = c;

							vertices[vertex + 3].Pos = pos1 + updown * thickness*0.5f;
							vertices[vertex + 3].Color = c;
						}

						totalSegDraw++;
						numSegDraw++;
					}
				}
			}

			m_meshBuffer->setDirty(EBT_VERTEX_AND_INDEX);

			index->set_used(totalSegDraw * 6);
		}

		void CParticleTrail::OnParticleUpdate(CParticle *particles, int num, CGroup *group, float dt)
		{
			float seg2 = m_segmentLength * m_segmentLength;

			// delete dead particle data
			if (m_trails.size() >= (u32)num)
			{
				for (u32 i = (u32)num, n = m_trails.size(); i < n; i++)
				{
					// printf("Delete %d\n", i);
					m_trails[i].DeleteData();
				}

				m_trailCount = num;
			}

			m_trails.set_used(num);

			for (int i = 0; i < num; i++)
			{
				CParticle& p = particles[i];

				STrailInfo &particlePos = m_trails[p.Index];

				if (m_trailCount <= i)
				{
					// init new particle data
					particlePos.InitData();
					m_trailCount = i + 1;

					// printf("New %d\n", i);
				}

				particlePos.CurrentPosition = p.Position;

				if (particlePos.Position->size() == 0)
				{
					SParticlePosition pos;
					pos.Alpha = m_alpha;
					pos.Width = m_width;
					pos.Position = p.Position;

					particlePos.Position->push_back(pos);

					particlePos.LastPosition = p.Position;
				}
				else
				{
					if (p.Position.getDistanceFromSQ(particlePos.LastPosition) >= seg2)
					{
						particlePos.Position->push_back(SParticlePosition());

						SParticlePosition& pos = particlePos.Position->getLast();
						pos.Alpha = m_alpha;
						pos.Width = m_width;
						pos.Position = p.Position;

						particlePos.LastPosition = p.Position;
					}
				}
			}
		}

		void CParticleTrail::OnParticleBorn(CParticle &p)
		{

		}

		void CParticleTrail::OnParticleDead(CParticle &p)
		{

		}

		void CParticleTrail::OnSwapParticleData(CParticle &p1, CParticle &p2)
		{
			int index1 = p1.Index;
			int index2 = p2.Index;

			STrailInfo t = m_trails[index1];

			m_trails[index1] = m_trails[index2];
			m_trails[index2] = t;
		}

		void CParticleTrail::OnGroupDestroy()
		{
			m_group = NULL;
		}
	}
}