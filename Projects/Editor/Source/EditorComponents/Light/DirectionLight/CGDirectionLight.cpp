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
#include "CGDirectionLight.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Transform/CWorldInverseTransformData.h"

#include "Editor/CEditor.h"
#include "Handles/CHandles.h"

#include "EditorComponents/SelectObject/CSelectObjectData.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGDirectionLight);

		DEPENDENT_COMPONENT(CDirectionalLight, CGDirectionLight);

		CGDirectionLight::CGDirectionLight() :
			m_directionLight(NULL)
		{

		}

		CGDirectionLight::~CGDirectionLight()
		{

		}

		void CGDirectionLight::initComponent()
		{
			m_directionLight = m_gameObject->getComponent<CDirectionalLight>();

			m_sprite = m_gameObject->addComponent<CSprite>();
			m_sprite->setFrame(CEditor::getInstance()->getSpriteIcon()->getFrame("light"), 1.0f, m_directionLight->getColor().toSColor());
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

		void CGDirectionLight::updateComponent()
		{
			// update color of light
			SColor lightColor = m_directionLight->getColor().toSColor();
			m_sprite->setColor(lightColor);

			// draw light direction arrow
			core::matrix4 world = m_gameObject->getTransformEuler()->calcWorldTransform();
			CHandles::getInstance()->drawArrowInViewSpace(world.getTranslation(), m_directionLight->getDirection(), 0.5f, 0.05f, lightColor);

			// update collision bbox
			float boxScale = m_sprite->getViewScale() * 10.0f;
			CSelectObjectData* selectObject = (CSelectObjectData*)m_gameObject->getEntity()->getDataByIndex(CSelectObjectData::DataTypeIndex);
			selectObject->BBox.MinEdge = m_defaultBBox.MinEdge * boxScale;
			selectObject->BBox.MaxEdge = m_defaultBBox.MaxEdge * boxScale;
		}
	}
}