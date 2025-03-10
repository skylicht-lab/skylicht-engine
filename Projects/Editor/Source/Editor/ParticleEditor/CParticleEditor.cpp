/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CParticleEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CParticleController.h"
#include "Editor/SpaceController/CSceneController.h"

#include "ParticleSystem/Particles/Emitters/CDirectionEmitter.h"

namespace Skylicht
{
	namespace Editor
	{
		CParticleEditor::CParticleEditor() :
			m_ps(NULL),
			m_data(NULL),
			m_isChanged(false)
		{

		}

		CParticleEditor::~CParticleEditor()
		{
			closeGUI();
		}

		void CParticleEditor::closeGUI()
		{
			if (m_data)
				delete m_data;

			m_data = NULL;
			m_ps = NULL;
		}

		void CParticleEditor::initGUI(Particle::CParticleSerializable* ps, CSpaceProperty* ui)
		{
			m_ps = ps;
			m_data = ps->createSerializable();

			std::string name = m_data->get("name", std::string());

			GUI::CCollapsibleGroup* group = ui->addGroup(name.c_str(), this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_data, ui, layout);
			group->setExpand(true);
		}

		void CParticleEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_isChanged = true;
			m_ps->loadSerializable(m_data);

			CParticleController::getInstance()->updateGroupName();

			CParticleGizmos* gizmos = CSceneController::getInstance()->getParticleGizmos();
			CParticleGizmos::EState state = gizmos->getState();

			switch (state)
			{
			case CParticleGizmos::Emitter:
			{
				Particle::CDirectionEmitter* directionEmitter = dynamic_cast<Particle::CDirectionEmitter*>(m_ps);
				if (directionEmitter)
					gizmos->setRotation(directionEmitter->getRotation());
			}
			break;
			case CParticleGizmos::Zone:
			{
				Particle::CPositionZone* positionZone = dynamic_cast<Particle::CPositionZone*>(m_ps);
				if (positionZone)
					gizmos->setPosition(positionZone->getPosition());
				else
					gizmos->refresh();
			}
			break;
			case CParticleGizmos::Gravity:
			case CParticleGizmos::Orientation:
			{
				Particle::CGroup* group = dynamic_cast<Particle::CGroup*>(m_ps);
				if (group)
				{
					if (state == CParticleGizmos::Gravity)
						gizmos->setRotation(core::quaternion(group->GravityRotation));
					else
						gizmos->setRotation(core::quaternion(group->ParticleRotation));
				}
				else
				{
					gizmos->refresh();
				}
			}
			break;
			default:
				break;
			}
		}
	}
}