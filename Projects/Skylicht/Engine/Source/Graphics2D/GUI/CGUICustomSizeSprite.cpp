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
#include "CGUICustomSizeSprite.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/SpriteFrame/CSpriteManager.h"

namespace Skylicht
{
	CGUICustomSizeSprite::CGUICustomSizeSprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame) :
		CGUIElement(canvas, parent),
		m_frame(frame),
		m_anchorType(CGUICustomSizeSprite::AnchorAll),
		m_anchorLeft(15.0f),
		m_anchorRight(15.0f),
		m_anchorTop(15.0f),
		m_anchorBottom(15.0f)
	{
		m_enableMaterial = true;
	}

	CGUICustomSizeSprite::CGUICustomSizeSprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame) :
		CGUIElement(canvas, parent, rect),
		m_frame(frame),
		m_anchorType(CGUICustomSizeSprite::AnchorAll),
		m_anchorLeft(15.0f),
		m_anchorRight(15.0f),
		m_anchorTop(15.0f),
		m_anchorBottom(15.0f)
	{
		m_enableMaterial = true;
	}

	CGUICustomSizeSprite::~CGUICustomSizeSprite()
	{

	}

	void CGUICustomSizeSprite::update(CCamera* camera)
	{
		CGUIElement::update(camera);
	}

	void CGUICustomSizeSprite::render(CCamera* camera)
	{
		if (m_frame != NULL && m_frame->ModuleOffset.size() > 0)
		{
			CGraphics2D* g = CGraphics2D::getInstance();

			SFrame* frame = m_frame->ModuleOffset[0].Frame;
			float w = (float)frame->getWidth();
			float h = (float)frame->getHeight();

			switch (m_anchorType)
			{
			case CGUICustomSizeSprite::AnchorAll:
				g->addModuleBatch(&m_frame->ModuleOffset[0],
					getColor(),
					m_transform->World,
					getRect(),
					m_anchorLeft,
					w - m_anchorRight,
					m_anchorTop,
					h - m_anchorBottom,
					getShaderID(),
					getMaterial());
				break;
			case CGUICustomSizeSprite::AnchorLeftRight:
				g->addModuleBatchLR(&m_frame->ModuleOffset[0],
					getColor(),
					m_transform->World,
					getRect(),
					m_anchorLeft,
					w - m_anchorRight,
					getShaderID(),
					getMaterial());
				break;
			case CGUICustomSizeSprite::AnchorTopBottom:
				g->addModuleBatchTB(&m_frame->ModuleOffset[0],
					getColor(),
					m_transform->World,
					getRect(),
					m_anchorTop,
					h - m_anchorBottom,
					getShaderID(),
					getMaterial());
				break;
			default:
				break;
			}
			CGUIElement::render(camera);
		}

		CGUIElement::render(camera);
	}

	void CGUICustomSizeSprite::setFrame(SFrame* frame)
	{
		m_frame = frame;
	}

	void CGUICustomSizeSprite::setAnchor(AnchorType type, float left, float right, float top, float bottom)
	{
		m_anchorType = type;
		m_anchorLeft = core::max_(left, 0.0f);
		m_anchorRight = core::max_(right, 0.0f);
		m_anchorTop = core::max_(top, 0.0f);
		m_anchorBottom = core::max_(bottom, 0.0f);
	}

	CObjectSerializable* CGUICustomSizeSprite::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();

		CFrameSourceProperty* frame = new CFrameSourceProperty(object, "spriteSrc", m_frameName.c_str());
		frame->setGUID(m_guid.c_str());
		frame->setSprite(m_sprite.c_str());
		frame->setSpriteGUID(m_spriteId.c_str());
		object->autoRelease(frame);

		CEnumProperty<AnchorType>* anchorType = new CEnumProperty<AnchorType>(object, "anchorType", m_anchorType);
		anchorType->addEnumString("Anchor all", CGUICustomSizeSprite::AnchorAll);
		anchorType->addEnumString("Anchor left right", CGUICustomSizeSprite::AnchorLeftRight);
		anchorType->addEnumString("Anchor left right", CGUICustomSizeSprite::AnchorTopBottom);
		object->autoRelease(anchorType);

		object->autoRelease(new CFloatProperty(object, "anchorLeft", m_anchorLeft, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorRight", m_anchorRight, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorTop", m_anchorTop, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorBottom", m_anchorBottom, 0.0f));

		return object;
	}

	void CGUICustomSizeSprite::loadSerializable(CObjectSerializable* object)
	{
		CFrameSourceProperty* frame = dynamic_cast<CFrameSourceProperty*>(object->getProperty("spriteSrc"));

		m_anchorType = object->get<AnchorType>("anchorType", CGUICustomSizeSprite::AnchorAll);
		m_anchorLeft = object->get<float>("anchorLeft", 0.0f);
		m_anchorRight = object->get<float>("anchorRight", 0.0f);
		m_anchorTop = object->get<float>("anchorTop", 0.0f);
		m_anchorBottom = object->get<float>("anchorBottom", 0.0f);

		CGUIElement::loadSerializable(object);

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
	}
}