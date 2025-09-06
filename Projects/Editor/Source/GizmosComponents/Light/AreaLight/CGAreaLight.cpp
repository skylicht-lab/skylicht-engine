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
#include "CGAreaLight.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Transform/CWorldInverseTransformData.h"

#include "Editor/CEditor.h"
#include "Handles/CHandles.h"

#include "GizmosComponents/SelectObject/CSelectObjectData.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGAreaLight);

		DEPENDENT_COMPONENT(CAreaLight, CGAreaLight);

		CGAreaLight::CGAreaLight() :
			m_areaLight(NULL),
			m_sprite(NULL)
		{

		}

		CGAreaLight::~CGAreaLight()
		{
			if (m_gameObject)
				m_gameObject->removeComponent<CSprite>();
		}

		void CGAreaLight::initComponent()
		{
			m_areaLight = m_gameObject->getComponent<CAreaLight>();

			m_sprite = m_gameObject->addComponent<CSprite>();
			m_sprite->setFrame(CEditor::getInstance()->getSpriteIcon()->getFrameByName("light"), 1.0f, m_areaLight->getColor().toSColor());
			m_sprite->setCenter(true);
			m_sprite->setBillboard(true);
			m_sprite->setAutoScaleInViewSpace(true);
			m_sprite->setEnableSerializable(false);

			addLinkComponent(m_sprite);

			m_defaultBBox = core::aabbox3df(core::vector3df(-1.0f, -1.0f, -1.0f), core::vector3df(1.0f, 1.0f, 1.0f));

			// pick collision
			CEntity* entity = m_gameObject->getEntity();
			entity->addData<CWorldInverseTransformData>();

			// select bbox data
			CSelectObjectData* selectObject = entity->addData<CSelectObjectData>();
			selectObject->GameObject = m_gameObject;
			selectObject->BBox = m_defaultBBox;
		}

		void CGAreaLight::updateComponent()
		{
			// update color of light
			SColor lightColor = m_areaLight->getColor().toSColor();
			m_sprite->setColor(lightColor);

			if (CSelection::getInstance()->getSelected(m_gameObject))
			{
				// draw light direction arrow
				core::matrix4 world = m_gameObject->getTransformEuler()->calcWorldTransform();
				CHandles* handles = CHandles::getInstance();
				handles->drawArrowInViewSpace(world.getTranslation(), m_areaLight->getDirection(), 0.3f, 0.05f, lightColor);

				float x = m_areaLight->getSizeX() * 0.5f;
				float y = m_areaLight->getSizeY() * 0.5f;
				float r = 0.0f;

				core::vector3df points[4]
				{
					core::vector3df(-x, -y, r),
					core::vector3df(-x, y, r),
					core::vector3df(x, y, r),
					core::vector3df(x, -y, r)
				};
				for (int i = 0; i < 4; i++)
					world.transformVect(points[i]);

				handles->drawLine(points[0], points[1], lightColor);
				handles->drawLine(points[1], points[2], lightColor);
				handles->drawLine(points[2], points[3], lightColor);
				handles->drawLine(points[3], points[0], lightColor);
			}

			// update collision bbox
			float boxScale = m_sprite->getViewScale() * 10.0f;
			CSelectObjectData* selectObject = GET_ENTITY_DATA(m_gameObject->getEntity(), CSelectObjectData);
			selectObject->BBox.MinEdge = m_defaultBBox.MinEdge * boxScale;
			selectObject->BBox.MaxEdge = m_defaultBBox.MaxEdge * boxScale;
			selectObject->TransformChanged = true;
		}
	}
}