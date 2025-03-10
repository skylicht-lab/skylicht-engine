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
			case CParticleGizmos::Gravity:
			{
				Particle::CGroup* group = gizmos->getGroup();

				core::vector3df direction(0.0f, -1.0f, 0.0f);

				core::matrix4 world = m_gameObject->calcWorldTransform();
				core::vector3df position = world.getTranslation();

				direction = core::quaternion(group->GravityRotation) * direction;
				direction.normalize();

				handle->drawArrowInViewSpace(
					position, direction,
					0.3f, 0.1f,
					SColor(255, 255, 255, 255));
				selectObjectData->DrawSelectionBox = false;
			}
			break;
			case CParticleGizmos::Orientation:
			{
				Particle::CGroup* group = gizmos->getGroup();

				core::matrix4 world = m_gameObject->calcWorldTransform();
				core::vector3df position = world.getTranslation();

				core::quaternion r(group->ParticleRotation);

				core::vector3df ox = r * Transform::Ox;
				core::vector3df oy = r * Transform::Oy;
				core::vector3df oz = r * Transform::Oz;
				ox.normalize();
				oy.normalize();
				oz.normalize();

				handle->drawLine(position, position + ox, SColor(255, 255, 0, 0));
				handle->drawLine(position, position + oy, SColor(255, 0, 255, 0));
				handle->drawLine(position, position + oz, SColor(255, 0, 0, 255));

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

			selectObjectData->GameObject = m_gameObject;
			selectObjectData->Entity = NULL;
			selectObjectData->BBox = bbox->BBox;
		}

		void CGParticle::renderZone(const core::matrix4& world, Particle::CZone* zone)
		{
			CHandles* handle = CHandles::getInstance();

			SColor color(255, 255, 255, 255);

			EZone type = zone->getType();
			switch (type)
			{
			case Particle::AABox:
			{
				Particle::CAABox* box = dynamic_cast<Particle::CAABox*>(zone);
				const core::vector3df& d = box->getDimension();
				const core::vector3df& p = box->getPosition();

				core::aabbox3df bbox(p - d * 0.5f, p + d * 0.5f);
				world.transformBox(bbox);
				handle->draw3DBox(bbox, color);
			}
			break;
			case Particle::Cylinder:
			{
				Particle::CCylinder* cylinder = dynamic_cast<Particle::CCylinder*>(zone);
				core::vector3df n = cylinder->getDirection();
				core::vector3df p = cylinder->getPosition();

				world.transformVect(p);
				world.rotateVect(n);
				handle->drawCylinder(p, n.normalize(), cylinder->getLength(), cylinder->getRadius(), color);
			}
			break;
			case Particle::Sphere:
			{
				Particle::CSphere* sphere = dynamic_cast<Particle::CSphere*>(zone);
				core::vector3df p = sphere->getPosition();

				world.transformVect(p);
				handle->drawSphere(p, sphere->getRadius(), color);
			}
			break;
			case Particle::Ring:
			{
				Particle::CRing* ring = dynamic_cast<Particle::CRing*>(zone);
				core::vector3df p = ring->getPosition();
				core::vector3df n = ring->getNormal();

				world.transformVect(p);
				world.rotateVect(n);
				n.normalize();

				handle->drawCircle(p, ring->getMinRadius(), n, color);
				handle->drawCircle(p, ring->getMaxRadius(), n, color);
			}
			break;
			case Particle::Line:
			{
				Particle::CLine* line = dynamic_cast<Particle::CLine*>(zone);
				core::line3df l = line->getLine();

				world.transformVect(l.start);
				world.transformVect(l.end);

				handle->drawLine(l.start, l.end, color);
			}
			break;
			case Particle::PolyLine:
			{
				Particle::CPolyLine* line = dynamic_cast<Particle::CPolyLine*>(zone);
				core::array<core::vector3df> points = line->getPoints();

				for (u32 i = 0, n = points.size(); i < n; i++)
					world.transformVect(points[i]);

				handle->drawPolyline(points.const_pointer(), points.size(), false, color);
			}
			break;
			default:
				break;
			};
		}
	}
}