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
#include "CGParticle.h"

#include "Editor/SpaceController/CSceneController.h"

#include "Culling/CCullingBBoxData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		using namespace Particle;

		ACTIVATOR_REGISTER(CGParticle);

		DEPENDENT_COMPONENT(CParticleComponent, CGParticle);

		CGParticle::CGParticle() :
			m_particle(NULL)
		{

		}

		CGParticle::~CGParticle()
		{

		}

		void CGParticle::initComponent()
		{
			m_particle = m_gameObject->getComponent<CParticleComponent>();
		}

		void CGParticle::updateComponent()
		{
			updateSelectBBox();

			CHandles* handle = CHandles::getInstance();
			CSceneController* sceneController = CSceneController::getInstance();

			CParticleGizmos* gizmos = sceneController->getParticleGizmos();

			CEntity* entity = m_gameObject->getEntity();
			CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);

			if (sceneController->getParticleGizmos() != sceneController->getGizmos())
			{
				selectObjectData->DrawSelectionBox = true;
				return;
			}

			CParticleGizmos::EState state = gizmos->getState();

			switch (state)
			{
			case CParticleGizmos::Emitter:
			{
				Particle::CDirectionEmitter* emitter = gizmos->getDirectionEmitter();

				core::vector3df direction = emitter->getDirection();
				core::matrix4 world = m_gameObject->calcWorldTransform();

				core::vector3df position(0.0f, 0.0f, 0.0f);

				Particle::CZone* zone = emitter->getZone();
				Particle::CPositionZone* positionZone = dynamic_cast<Particle::CPositionZone*>(zone);
				if (positionZone)
					position = positionZone->getPosition();

				world.transformVect(position);
				world.rotateVect(direction);
				direction.normalize();

				handle->drawArrowInViewSpace(
					position, direction,
					0.4f, 0.1f,
					SColor(255, 255, 255, 255));
				selectObjectData->DrawSelectionBox = false;
			}
			break;
			case CParticleGizmos::Zone:
			{
				renderZone(m_gameObject->calcWorldTransform(), gizmos->getZone());
				selectObjectData->DrawSelectionBox = false;
			}
			break;
			default:
				selectObjectData->DrawSelectionBox = true;
				break;
			}
		}

		void CGParticle::updateSelectBBox()
		{
			CEntity* entity = m_gameObject->getEntity();

			CSelectObjectData* selectObjectData = GET_ENTITY_DATA(entity, CSelectObjectData);
			if (selectObjectData == NULL)
				selectObjectData = entity->addData<CSelectObjectData>();

			CWorldInverseTransformData* worldInv = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
			if (worldInv == NULL)
				worldInv = entity->addData<CWorldInverseTransformData>();

			CCullingBBoxData* bbox = GET_ENTITY_DATA(entity, CCullingBBoxData);
			if (bbox == NULL)
				return;

			// select bbox data
			selectObjectData->GameObject = m_gameObject;
			selectObjectData->Entity = NULL;
			selectObjectData->BBox = bbox->BBox;
		}

		void CGParticle::renderZone(const core::matrix4& world, Particle::CZone* zone)
		{
			EZone type = zone->getType();

		}
	}
}