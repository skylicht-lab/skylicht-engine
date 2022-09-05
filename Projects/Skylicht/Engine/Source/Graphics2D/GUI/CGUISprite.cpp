/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CGUISprite.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGUISprite::CGUISprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame) :
		CGUIElement(canvas, parent),
		m_frame(frame),
		m_animationTime(0),
		m_autoRotate(false),
		m_frameSpeed(0.0f),
		m_frameRotate(0.0f)
	{

	}

	CGUISprite::CGUISprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame) :
		CGUIElement(canvas, parent, rect),
		m_frame(frame),
		m_animationTime(0),
		m_autoRotate(false),
		m_frameSpeed(0.0f),
		m_frameRotate(0.0f)
	{

	}

	CGUISprite::~CGUISprite()
	{

	}

	void CGUISprite::update(CCamera* camera)
	{
		CGUIElement::update(camera);

		if (m_autoRotate)
		{
			m_animationTime = m_animationTime - getTimeStep();

			if (m_animationTime <= 0.0f)
			{
				core::vector3df rot = getRotation();
				rot.Z = rot.Z + m_frameRotate;
				rot.Z = fmodf(rot.Z, 360.0f);

				setRotation(rot);
				m_animationTime = m_frameSpeed;

				setAlignCenterModule();
			}
		}
	}

	void CGUISprite::render(CCamera* camera)
	{
		if (m_frame != NULL)
		{
			CGraphics2D::getInstance()->addFrameBatch(m_frame, m_color, m_transform->World, m_shaderID, m_material);
			CGUIElement::render(camera);
		}
	}

	void CGUISprite::setFrame(SFrame* frame)
	{
		m_frame = frame;
	}

	void CGUISprite::setAutoRotate(bool rotate, float rotateAngle, float framePerSec)
	{
		m_autoRotate = rotate;
		m_frameRotate = rotateAngle;

		if (framePerSec > 0)
			m_frameSpeed = 1000.0f / framePerSec;
	}

	void CGUISprite::setAlignCenterModule()
	{
		if (m_frame)
		{
			m_frame->ModuleOffset[0].OffsetX = -((float)m_frame->BoudingRect.getWidth() * 0.5f);
			m_frame->ModuleOffset[0].OffsetY = -((float)m_frame->BoudingRect.getHeight() * 0.5f);
		}
	}

	void CGUISprite::setOffsetModule(float x, float y)
	{
		if (m_frame)
		{
			m_frame->ModuleOffset[0].OffsetX = x;
			m_frame->ModuleOffset[0].OffsetY = y;
		}
	}
}