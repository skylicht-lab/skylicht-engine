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
#include "Graphics2D/SpriteFrame/CSpriteManager.h"

namespace Skylicht
{
	CGUISprite::CGUISprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame) :
		CGUIElement(canvas, parent),
		m_frame(frame),
		m_animationTime(0),
		m_autoRotate(false),
		m_frameSpeed(0.0f),
		m_frameRotate(0.0f),
		m_isCenter(false),
		m_defaultOffsetX(0.0f),
		m_defaultOffsetY(0.0f)
	{
		m_enableMaterial = true;
	}

	CGUISprite::CGUISprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame) :
		CGUIElement(canvas, parent, rect),
		m_frame(frame),
		m_animationTime(0),
		m_autoRotate(false),
		m_frameSpeed(0.0f),
		m_frameRotate(0.0f),
		m_isCenter(false),
		m_defaultOffsetX(0.0f),
		m_defaultOffsetY(0.0f)
	{
		m_enableMaterial = true;
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
			const SColor& color = getColor();
			if (color.getAlpha() > 0)
			{
				CGraphics2D::getInstance()->addFrameBatch(m_frame, color, m_transform->World, getShaderID(), getMaterial());
				CGUIElement::render(camera);
			}
		}

		CGUIElement::render(camera);
	}

	const core::rectf CGUISprite::getNativeRect()
	{
		core::rectf r = core::rectf(core::vector2df(0.0f, 0.0f), core::dimension2df(getWidth(), getHeight()));

		if (m_frame != NULL)
		{
			r.LowerRightCorner.X = r.UpperLeftCorner.X + m_frame->getWidth();
			r.LowerRightCorner.Y = r.UpperLeftCorner.Y + m_frame->getHeight();
		}
		return r;
	}

	void CGUISprite::setFrame(SFrame* frame)
	{
		m_frame = frame;
	}

	void CGUISprite::setFrameSource(const char* spritePath, const char* frameName, const char* editorRileRef)
	{
		CSpriteManager* spriteMgr = CSpriteManager::getInstance();

		// load by id first
		CSpriteFrame* sprite = spriteMgr->loadSprite(spritePath);
		if (sprite)
		{
			m_frame = sprite->getFrameByName(frameName);
			if (m_frame)
			{
				m_guid = m_frame->ID;
				if (editorRileRef)
					m_frameName = editorRileRef;
				else
					m_frameName = frameName;
				m_sprite = spritePath;
				m_spriteId = sprite->getId();
			}
		}
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
			if (!m_isCenter)
			{
				m_defaultOffsetX = m_frame->ModuleOffset[0].OffsetX;
				m_defaultOffsetY = m_frame->ModuleOffset[0].OffsetY;
				m_isCenter = true;
			}

			m_frame->ModuleOffset[0].OffsetX = -((float)m_frame->BoudingRect.getWidth() * 0.5f);
			m_frame->ModuleOffset[0].OffsetY = -((float)m_frame->BoudingRect.getHeight() * 0.5f);
		}
	}

	void CGUISprite::setAlignModuleDefault()
	{
		if (m_isCenter)
		{
			m_frame->ModuleOffset[0].OffsetX = m_defaultOffsetX;
			m_frame->ModuleOffset[0].OffsetY = m_defaultOffsetY;
			m_isCenter = false;
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

	CObjectSerializable* CGUISprite::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();

		CFrameSourceProperty* frame = new CFrameSourceProperty(object, "spriteSrc", m_frameName.c_str());
		frame->setGUID(m_guid.c_str());
		frame->setSprite(m_sprite.c_str());
		frame->setSpriteGUID(m_spriteId.c_str());
		object->autoRelease(frame);

		object->autoRelease(new CBoolProperty(object, "centerModule", m_isCenter));

		return object;
	}

	void CGUISprite::loadSerializable(CObjectSerializable* object)
	{
		CFrameSourceProperty* frame = dynamic_cast<CFrameSourceProperty*>(object->getProperty("spriteSrc"));
		CGUIElement::loadSerializable(object);

		bool isCenter = object->get("centerModule", false);

		if (frame != NULL)
		{
			if (m_guid != frame->getGUID())
			{
				m_guid = frame->getGUID();
				m_frameName = frame->get();
				m_sprite = frame->getSprite();
				m_spriteId = frame->getSpriteGUID();

				CSpriteManager* spriteMgr = CSpriteManager::getInstance();

				// load by id first
				CSpriteFrame* sprite = spriteMgr->getSpriteById(m_spriteId.c_str());
				if (!sprite)
					sprite = spriteMgr->loadSprite(m_sprite.c_str());

				if (sprite && sprite->getId() == m_spriteId)
				{
					m_frame = sprite->getFrameById(m_guid.c_str());
				}
			}
		}

		if (isCenter)
			setAlignCenterModule();
		else
			setAlignModuleDefault();
	}
}