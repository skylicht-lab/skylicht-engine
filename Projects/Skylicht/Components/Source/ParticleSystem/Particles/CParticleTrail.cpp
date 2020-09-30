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
			m_segmentLength(0.05f),
			m_width(0.1f),
			m_trailCount(0),
			m_maxSegmentCount(0),
			m_destroyWhenParticleDead(false),
			m_deadAlphaReduction(0.01f)
		{
			setLength(1.0f);

			group->setCallback(this);

			m_meshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), EIT_32BIT);
			m_meshBuffer->setHardwareMappingHint(EHM_STREAM);

			m_material = new CMaterial("TrailMaterial", "BuiltIn/Shader/Particle/ParticleTrailTurbulenceAdditive.xml");
			m_material->setBackfaceCulling(false);
		}

		CParticleTrail::~CParticleTrail()
		{
			if (m_group != NULL)
				m_group->setCallback(NULL);

			for (u32 i = 0, n = m_trails.size(); i < n; i++)
			{
				m_trails[i].DeleteData();
				// printf("Delete trail %d\n", i);
			}

			for (u32 i = 0, n = m_deadTrails.size(); i < n; i++)
			{
				m_deadTrails[i].DeleteData();
				// printf("Delete dead trail %d\n", i);
			}

			m_trails.clear();
			m_deadTrails.clear();

			if (m_meshBuffer != NULL)
				m_meshBuffer->drop();

			delete m_material;
		}

		void CParticleTrail::setLength(float l)
		{
			m_maxSegmentCount = (int)(l / m_segmentLength + 1.0f);
			m_length = l;
		}

		void CParticleTrail::applyMaterial()
		{
			m_material->applyMaterial(m_meshBuffer->getMaterial());
		}

		void CParticleTrail::updateDeadTrail()
		{
			float dt = getTimeStep();

			u32 numTrail = m_deadTrails.size();

			for (u32 i = 0; i < numTrail; i++)
			{
				STrailInfo& trail = m_deadTrails[i];

				u32 numSeg = trail.Position->size() - 1;

				int endSeg = 0;
				if (trail.Position->size() > m_maxSegmentCount)
					endSeg = (int)(numSeg - m_maxSegmentCount + 1);

				trail.Flag = 1;

				for (u32 j = endSeg; j <= numSeg; j++)
				{
					SParticlePosition& p = (*trail.Position)[j];
					if (p.Alpha == 0.0f)
						continue;
					else if (p.Alpha > 0.0f)
					{
						p.Alpha = p.Alpha - m_deadAlphaReduction * dt;
						if (p.Alpha < 0.0f)
							p.Alpha = 0.0f;

						trail.Flag = 0;

						// > 0.5f to keep smooth fade out
						if (p.Alpha > 0.5f)
							break;
					}
				}
			}

			for (u32 i = 0; i < numTrail; i++)
			{
				if (m_deadTrails[i].Flag == 1)
				{
					m_deadTrails[i].DeleteData();
					// printf("Delete dead %d\n", i);

					if (numTrail > 1)
					{
						STrailInfo t = m_deadTrails[i];
						m_deadTrails[i] = m_deadTrails[numTrail - 1];
						m_deadTrails[numTrail - 1] = m_deadTrails[i];

						i--;
						numTrail--;
					}
					else
					{
						numTrail = 0;
						break;
					}
				}
			}
			m_deadTrails.set_used(numTrail);
		}

		void CParticleTrail::update(CCamera *camera)
		{
			if (m_group == NULL)
				return;

			// on dead trail
			// we reduction alpha to zero and kill
			updateDeadTrail();

			IVertexBuffer *buffer = m_meshBuffer->getVertexBuffer(0);
			IIndexBuffer *index = m_meshBuffer->getIndexBuffer();

			int indexID = 0;
			int vertexID = 0;

			// camera
			core::vector3df campos = camera->getGameObject()->getPosition();
			core::vector3df up = camera->getUpVector();

			u32 numTrail = m_trails.size() + m_deadTrails.size();
			u32 liveTrail = m_trails.size();

			int numVertex = numTrail * 4 * m_maxSegmentCount;
			int numIndex = numTrail * 6 * m_maxSegmentCount;

			buffer->set_used(numVertex);
			index->set_used(numIndex);

			S3DVertex* vertices = (S3DVertex*)buffer->getVertices();
			u32 *indices = (u32*)index->getIndices();

			u32 totalSegDraw = 0;

			for (u32 trailID = 0; trailID < numTrail; trailID++)
			{
				STrailInfo* pTrail = NULL;

				if (trailID < liveTrail)
					pTrail = &m_trails[trailID];
				else
					pTrail = &m_deadTrails[trailID - liveTrail];

				STrailInfo& trail = *pTrail;

				u32 numSeg = trail.Position->size() - 1;
				int endSeg = 0;
				if (trail.Position->size() > m_maxSegmentCount)
					endSeg = (int)(numSeg - m_maxSegmentCount + 1);

				u32 numSegDraw = 0;
				float currentLength = 0.0f;
				float trailLength = 0.0f;

				// compute trail length
				for (int i = numSeg; i >= endSeg; i--)
				{
					core::vector3df *pos1;
					core::vector3df *pos2;

					if (i == numSeg)
					{
						// first segment
						SParticlePosition &p = (*trail.Position)[i];
						pos2 = &p.Position;
						pos1 = &trail.CurrentPosition;
					}
					else
					{
						// get nearest point
						SParticlePosition& p1 = (*trail.Position)[i + 1];
						SParticlePosition& p2 = (*trail.Position)[i];

						pos2 = &p2.Position;
						pos1 = &p1.Position;
					}

					trailLength = trailLength + pos2->getDistanceFrom(*pos1);
				}

				// build trail mesh buffer
				for (int i = numSeg; i >= endSeg; i--)
				{
					core::vector3df pos1;
					core::vector3df pos2;

					float thickness = 0.0f;
					float alpha1 = 1.0f;
					float alpha2 = 1.0f;

					const SColor &c = trail.CurrentColor;
					float currentAlpha = c.getAlpha() / 255.0f;

					if (i == numSeg)
					{
						// first segment
						SParticlePosition &p = (*trail.Position)[i];
						pos2 = p.Position;
						thickness = p.Width;
						alpha1 = p.Alpha;
						alpha2 = p.Alpha;

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
						alpha1 = p1.Alpha;
						alpha2 = p2.Alpha;
					}

					SColor c1(c);
					SColor c2(c);

					c1.setAlpha((u32)(alpha1 * currentAlpha * 255.0f));
					c2.setAlpha((u32)(alpha2 * currentAlpha * 255.0f));

					// direction
					core::vector3df direction = pos1 - pos2;

					float uv1 = currentLength / m_length;

					// length
					currentLength = currentLength + direction.getLength();

					direction.normalize();

					float uv2 = currentLength / trailLength;

					if (currentLength > m_length)
					{
						float cutLength = currentLength - m_length;
						pos2 = pos2 + cutLength * direction;
						uv2 = 1.0f;
					}

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

						// note: we use updown as normal on billboard
						// core::vector3df normal = direction.crossProduct(updown);

						u32 vertex = totalSegDraw * 4;
						u32 index = totalSegDraw * 6;

						// vertex buffer
						vertices[vertex + 0].Pos = pos1 - updown * thickness*0.5f;
						vertices[vertex + 0].Normal = updown;
						vertices[vertex + 0].Color = c1;
						vertices[vertex + 0].TCoords.set(0.0f, uv1);

						vertices[vertex + 1].Pos = pos1 + updown * thickness*0.5f;
						vertices[vertex + 1].Normal = updown;
						vertices[vertex + 1].Color = c1;
						vertices[vertex + 1].TCoords.set(1.0f, uv1);

						vertices[vertex + 2].Pos = pos2 - updown * thickness*0.5f;
						vertices[vertex + 2].Normal = updown;
						vertices[vertex + 2].Color = c2;
						vertices[vertex + 2].TCoords.set(0.0f, uv2);

						vertices[vertex + 3].Pos = pos2 + updown * thickness*0.5f;
						vertices[vertex + 3].Normal = updown;
						vertices[vertex + 3].Color = c2;
						vertices[vertex + 3].TCoords.set(1.0f, uv2);

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
							int copyVertex = vertex;

							vertex = (totalSegDraw - 1) * 4;
							vertices[vertex + 2].Pos = vertices[copyVertex + 0].Pos;
							vertices[vertex + 2].TCoords = vertices[copyVertex + 0].TCoords;
							vertices[vertex + 2].Normal = vertices[copyVertex + 0].Normal;

							vertices[vertex + 3].Pos = vertices[copyVertex + 1].Pos;
							vertices[vertex + 3].TCoords = vertices[copyVertex + 1].TCoords;
							vertices[vertex + 3].Normal = vertices[copyVertex + 1].Normal;
						}

						totalSegDraw++;
						numSegDraw++;

						// skip
						if (currentLength >= m_length)
							break;
					}
				}
			}

			index->set_used(totalSegDraw * 6);

			m_meshBuffer->setDirty(EBT_VERTEX_AND_INDEX);
		}

		void CParticleTrail::OnParticleUpdate(CParticle *particles, int num, CGroup *group, float dt)
		{
			float seg2 = m_segmentLength * m_segmentLength;

			for (u32 i = 0; i < (u32)num; i++)
			{
				CParticle& p = particles[i];

				STrailInfo &trail = m_trails[p.Index];

				trail.CurrentPosition = p.Position;
				trail.CurrentColor.set(
					(u32)(p.Params[ColorA] * 255.0f),
					(u32)(p.Params[ColorR] * 255.0f),
					(u32)(p.Params[ColorG] * 255.0f),
					(u32)(p.Params[ColorB] * 255.0f)
				);

				if (trail.Position->size() == 0)
				{
					SParticlePosition pos;
					pos.Width = m_width;
					pos.Alpha = 1.0f;
					pos.Position = p.Position;

					trail.Position->push_back(pos);

					trail.LastPosition = p.Position;
				}
				else
				{
					if (p.Position.getDistanceFromSQ(trail.LastPosition) >= seg2)
					{
						trail.Position->push_back(SParticlePosition());

						SParticlePosition& pos = trail.Position->getLast();
						pos.Width = m_width;
						pos.Alpha = 1.0f;
						pos.Position = p.Position;

						trail.LastPosition = p.Position;
					}
				}
			}
		}

		void CParticleTrail::OnParticleBorn(CParticle &p)
		{
			m_trailCount++;

			m_trails.push_back(STrailInfo());

			// init new particle data
			m_trails.getLast().InitData();
			// printf("New %d\n", p.Index);
		}

		void CParticleTrail::OnParticleDead(CParticle &p)
		{
			if (m_destroyWhenParticleDead == false)
			{
				// add to list dead trail to update alpha reduction
				m_deadTrails.push_back(STrailInfo());
				STrailInfo &t = m_deadTrails.getLast();

				t.InitData();
				t.Copy(m_trails[p.Index]);
				// printf("New dead %d\n", m_deadTrails.size());
			}

			// delete this trail data
			m_trails[p.Index].DeleteData();
			// printf("Delete %d\n", p.Index);

			// remove list
			m_trailCount = m_trails.size() - 1;
			m_trails.set_used(m_trailCount);
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