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
#include "Editor/CEditor.h"
#include "Handles/CHandles.h"

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
		}

		void CGDirectionLight::updateComponent()
		{
			SColor lightColor = m_directionLight->getColor().toSColor();

			m_sprite->setColor(lightColor);

			CHandles::getInstance()->drawArrowInViewSpace(m_gameObject->getPosition(), m_directionLight->getDirection(), 0.5f, 0.05f, lightColor);
		}
	}
}