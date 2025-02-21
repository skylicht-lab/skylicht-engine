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
			m_emitter(NULL)
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

		void CParticleGizmos::setZone(Particle::CPositionZone* zone, const core::matrix4& world)
		{
			m_zone = zone;
			m_state = Zone;
			m_parentWorld = world;

			m_position = zone->getPosition();
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
				core::quaternion newRot = handle->rotateHandle(*m_rotation, core::vector3df());
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

				// draw emitter direction
				handle->drawArrowInViewSpace(
					m_parentWorld.getTranslation(),
					m_emitter->getDirection(),
					0.4f, 0.1f,
					SColor(255, 255, 255, 255));
			}
			else if (m_state == Zone)
			{
				core::vector3df newPos = CHandles::getInstance()->positionHandle(*m_position, core::quaternion());
				core::vector3df delta;

				if (newPos != *m_position)
				{
					m_position = newPos;
					m_position.notify(this);

					m_zone->setPosition(newPos);

					m_changed = true;
					updateProperty();
				}

				if (handle->endCheck())
				{
					m_zone->setPosition(*m_position);
					handle->end();
				}
			}
		}

		void CParticleGizmos::updateProperty()
		{
			CParticleEditor* particleEditor = CPropertyController::getInstance()->getParticleEditor();
			CObjectSerializable* data = particleEditor->getData();

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
				CVector3Property* p = dynamic_cast<CVector3Property*>(data->getProperty("position"));
				if (p)
				{
					p->set(m_zone->getPosition());
					p->OnChanged();
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