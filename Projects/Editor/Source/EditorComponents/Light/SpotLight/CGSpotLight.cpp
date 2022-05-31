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
#include "CGSpotLight.h"

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
		ACTIVATOR_REGISTER(CGSpotLight);

		DEPENDENT_COMPONENT(CSpotLight, CGSpotLight);

		int CircleSegmentCount = 64;

		CGSpotLight::CGSpotLight() :
			m_spotLight(NULL)
		{
			m_circlePos = new core::vector3df[CircleSegmentCount + 1];
		}

		CGSpotLight::~CGSpotLight()
		{
			delete[]m_circlePos;
		}

		void CGSpotLight::initComponent()
		{
			m_spotLight = m_gameObject->getComponent<CSpotLight>();

			m_sprite = m_gameObject->addComponent<CSprite>();
			m_sprite->setFrame(CEditor::getInstance()->getSpriteIcon()->getFrame("light"), 1.0f, m_spotLight->getColor().toSColor());
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

		void CGSpotLight::updateComponent()
		{
			CCamera* camera = m_gameObject->getEntityManager()->getCamera();
			if (camera == NULL)
				return;

			core::vector3df cameraPosition = camera->getGameObject()->getPosition();

			// update color of light
			SColor lightColor = m_spotLight->getColor().toSColor();
			m_sprite->setColor(lightColor);

			// draw light direction arrow
			core::matrix4 world = m_gameObject->getTransformEuler()->calcWorldTransform();

			float radius = m_spotLight->getRadius();
			float outner = m_spotLight->getSplotCutoff() * 0.5f;
			float maxOuterEdge = tanf(outner * core::DEGTORAD) * radius;

			const core::vector3df& position = world.getTranslation();
			const core::vector3df& direction = m_spotLight->getDirection();
			core::vector3df end = position + direction * radius;

			CHandles::getInstance()->drawLine(position, end, lightColor);

			core::quaternion q;
			q.rotationFromTo(core::vector3df(0.0f, 1.0f, 0.0f), direction);

			core::vector3df viewVector = position - cameraPosition;
			viewVector.normalize();

			bool isBackFace = true;
			core::vector3df r1, r2;

			for (int i = 0; i < CircleSegmentCount; i++)
			{
				float ng = 2 * core::PI * ((float)i / (float)CircleSegmentCount);

				core::vector3df axisPos = core::vector3df(cosf(ng), sinf(ng), 0.f);
				float* p = &axisPos.X;

				int axis = 1;
				core::vector3df r = core::vector3df(p[axis], p[(axis + 1) % 3], p[(axis + 2) % 3]);

				core::vector3df worldR = q * r;
				worldR.normalize();

				if (viewVector.dotProduct(worldR) > FLT_EPSILON)
				{
					// back face
					if (i == 0)
						isBackFace = true;
					else if (!isBackFace)
					{
						r2 = worldR;
						isBackFace = true;
					}
				}
				else
				{
					// front face
					if (i == 0)
						isBackFace = false;
					else if (isBackFace)
					{
						r1 = worldR;
						isBackFace = false;
					}
				}

				m_circlePos[i] = end + worldR * maxOuterEdge;
			}

			// draw circle
			CHandles::getInstance()->drawPolyline(m_circlePos, CircleSegmentCount, true, lightColor);

			// draw border edge
			CHandles::getInstance()->drawLine(position, end + r1 * maxOuterEdge, lightColor);
			CHandles::getInstance()->drawLine(position, end + r2 * maxOuterEdge, lightColor);

			// update collision bbox
			float boxScale = m_sprite->getViewScale() * 10.0f;
			CSelectObjectData* selectObject = (CSelectObjectData*)m_gameObject->getEntity()->getDataByIndex(CSelectObjectData::DataTypeIndex);
			selectObject->BBox.MinEdge = m_defaultBBox.MinEdge * boxScale;
			selectObject->BBox.MaxEdge = m_defaultBBox.MaxEdge * boxScale;
		}
	}
}