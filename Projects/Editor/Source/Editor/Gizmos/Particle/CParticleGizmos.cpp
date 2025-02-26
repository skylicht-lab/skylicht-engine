/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CParticleGizmos.h"
#include "Handles/CHandles.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "Editor/CEditor.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleGizmos::CParticleGizmos() :
			m_position(core::vector3df()),
			m_rotation(core::quaternion()),
			m_changed(false),
			m_state(None),
			m_emitter(NULL),
			m_zone(NULL),
			m_positionZone(NULL)
		{

		}

		CParticleGizmos::~CParticleGizmos()
		{

		}

		void CParticleGizmos::setEmitter(Particle::CDirectionEmitter* emitter, const core::matrix4& world)
		{
			m_emitter = emitter;
			m_state = Emitter;

			m_parentWorld = world;

			m_rotation = emitter->getRotation();
			m_changed = false;

			CHandles* handle = CHandles::getInstance();
			handle->setWorld(m_parentWorld);
			handle->end();
			handle->reset();
		}

		void CParticleGizmos::setZone(Particle::CZone* zone, const core::matrix4& world)
		{
			m_zone = zone;
			m_state = Zone;
			m_parentWorld = world;

			m_positionZone = dynamic_cast<Particle::CPositionZone*>(zone);
			if (m_positionZone)
				m_position = m_positionZone->getPosition();
			else
				m_position.set(core::vector3df());

			m_selectPointId = 0;
			m_changed = false;

			CHandles* handle = CHandles::getInstance();
			handle->setWorld(m_parentWorld);
			handle->end();
			handle->reset();
		}

		void CParticleGizmos::onGizmos()
		{
			CHandles* handle = CHandles::getInstance();

			if (m_state == None)
			{
				m_parentWorld.makeIdentity();
				handle->setWorld(m_parentWorld);
				handle->end();
				return;
			}
			else if (m_state == Emitter)
			{
				core::vector3df position(0.0f, 0.0f, 0.0f);

				Particle::CZone* zone = m_emitter->getZone();
				Particle::CPositionZone* positionZone = dynamic_cast<Particle::CPositionZone*>(zone);
				if (positionZone)
					position = positionZone->getPosition();

				core::quaternion newRot = handle->rotateHandle(*m_rotation, position);
				if (newRot != *m_rotation)
				{
					m_rotation = newRot;
					m_rotation.notify(this);

					m_emitter->setRotation(newRot);
					m_changed = true;
					updateProperty();
				}

				if (handle->endCheck())
				{
					m_emitter->setRotation(*m_rotation);
					handle->end();
				}
			}
			else if (m_state == Zone)
			{
				if (m_positionZone)
				{
					core::vector3df newPos = CHandles::getInstance()->positionHandle(*m_position, core::quaternion());
					core::vector3df delta;

					if (newPos != *m_position)
					{
						m_position = newPos;
						m_position.notify(this);

						m_positionZone->setPosition(newPos);

						m_changed = true;
						updateProperty();
					}

					if (handle->endCheck())
					{
						m_positionZone->setPosition(*m_position);
						handle->end();
					}
				}
				else
				{
					Particle::CPolyLine* polyline = dynamic_cast<Particle::CPolyLine*>(m_zone);
					if (polyline)
					{
						std::vector<core::vector3df> points;
						core::array<core::vector3df>& polyPoints = polyline->getPoints();

						for (u32 i = 0, n = polyline->getNumPoint(); i < n; i++)
							points.push_back(polyPoints[i]);

						CHandles* handles = CHandles::getInstance();
						handles->listPositionHandle(points, m_selectPointId, core::quaternion());
						m_selectPointId = handles->getSelectId();

						if (polyPoints[m_selectPointId] != handles->getHandlePosition())
						{
							polyPoints[m_selectPointId] = handles->getHandlePosition();
							m_changed = true;
							updateProperty();
						}

						if (handle->endCheck())
						{
							polyPoints[m_selectPointId] = handles->getHandlePosition();
							handle->end();
						}
					}
					else
					{
						Particle::CLine* line = dynamic_cast<Particle::CLine*>(m_zone);
						if (line)
						{
							std::vector<core::vector3df> points;
							points.push_back(line->getLine().start);
							points.push_back(line->getLine().end);

							CHandles* handles = CHandles::getInstance();
							handles->listPositionHandle(points, m_selectPointId, core::quaternion());
							m_selectPointId = handles->getSelectId();

							std::vector<core::vector3df>& handlePoint = handles->getListPosition();

							if (points[m_selectPointId] != handles->getHandlePosition())
							{
								line->setLine(handlePoint[0], handlePoint[1]);
								m_changed = true;
								updateProperty();
							}

							if (handle->endCheck())
							{
								line->setLine(handlePoint[0], handlePoint[1]);
								handle->end();
							}
						}
					}
				}
			}
		}

		void CParticleGizmos::updateProperty()
		{
			CParticleEditor* particleEditor = CPropertyController::getInstance()->getParticleEditor();
			CObjectSerializable* data = particleEditor->getData();

			if (!data)
				return;

			if (m_state == Emitter)
			{
				CVector3Property* p = dynamic_cast<CVector3Property*>(data->getProperty("direction"));
				if (p)
				{
					p->set(m_emitter->getDirection());
					p->OnChanged();
				}
			}
			else if (m_state == Zone)
			{
				if (m_positionZone)
				{
					CVector3Property* p = dynamic_cast<CVector3Property*>(data->getProperty("position"));
					if (p)
					{
						p->set(m_positionZone->getPosition());
						p->OnChanged();
					}
				}
				else
				{
					Particle::CLine* line = dynamic_cast<Particle::CLine*>(m_zone);
					if (line)
					{
						CVector3Property* p1 = dynamic_cast<CVector3Property*>(data->getProperty("p1"));
						CVector3Property* p2 = dynamic_cast<CVector3Property*>(data->getProperty("p2"));

						core::line3df l = line->getLine();
						if (p1)
						{
							p1->set(l.start);
							p1->OnChanged();
						}
						if (p2)
						{
							p2->set(l.end);
							p2->OnChanged();
						}
					}
					else
					{
						Particle::CPolyLine* polyline = dynamic_cast<Particle::CPolyLine*>(m_zone);
						if (polyline)
						{
							CArraySerializable* points = (CArraySerializable*)data->getProperty("Points");
							if (points)
							{
								core::array<core::vector3df>& listPoints = polyline->getPoints();
								int numPoint = points->getElementCount();
								for (int i = 0; i < numPoint; i++)
								{
									CVector3Property* pointData = (CVector3Property*)points->getElement(i);
									if (pointData)
									{
										pointData->set(listPoints[i]);
										pointData->OnChanged();
									}
								}
							}
						}
					}
				}
			}
		}


		void CParticleGizmos::onEnable()
		{

		}

		void CParticleGizmos::onRemove()
		{
			CHandles::getInstance()->end();
		}

		void CParticleGizmos::refresh()
		{
			CHandles::getInstance()->end();
		}

		void CParticleGizmos::reset()
		{
			CHandles::getInstance()->end();
			m_state = None;
			m_emitter = NULL;
			m_zone = NULL;
		}

		void CParticleGizmos::onNotify(ISubject* subject, IObserver* from)
		{

		}

		void CParticleGizmos::setPosition(const core::vector3df& pos)
		{
			m_position = pos;
			CHandles::getInstance()->end();
			m_position.notify(this);
		}

		void CParticleGizmos::setRotation(const core::quaternion& rot)
		{
			m_rotation = rot;
			CHandles::getInstance()->end();
			m_rotation.notify(this);
		}
	}
}