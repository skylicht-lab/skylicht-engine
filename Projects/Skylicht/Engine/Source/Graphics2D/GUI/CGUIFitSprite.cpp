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
#include "CGUIFitSprite.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/SpriteFrame/CSpriteManager.h"

namespace Skylicht
{
	CGUIFitSprite::CGUIFitSprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame) :
		CGUIElement(canvas, parent),
		m_frame(frame),
		m_anchorType(CGUIFitSprite::AnchorAll),
		m_anchorLeft(15.0f),
		m_anchorRight(15.0f),
		m_anchorTop(15.0f),
		m_anchorBottom(15.0f)
	{
		m_enableMaterial = true;
	}

	CGUIFitSprite::CGUIFitSprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame) :
		CGUIElement(canvas, parent, rect),
		m_frame(frame),
		m_anchorType(CGUIFitSprite::AnchorAll),
		m_anchorLeft(15.0f),
		m_anchorRight(15.0f),
		m_anchorTop(15.0f),
		m_anchorBottom(15.0f)
	{
		m_enableMaterial = true;
	}

	CGUIFitSprite::~CGUIFitSprite()
	{

	}

	void CGUIFitSprite::update(CCamera* camera)
	{
		CGUIElement::update(camera);
	}

	void CGUIFitSprite::render(CCamera* camera)
	{
		if (m_frame != NULL && m_frame->ModuleOffset.size() > 0)
		{
			const SColor& color = getColor();
			if (color.getAlpha() > 0)
			{
				CGraphics2D* g = CGraphics2D::getInstance();

				SFrame* frame = m_frame->ModuleOffset[0].Frame;
				float w = (float)frame->getWidth();
				float h = (float)frame->getHeight();

				switch (m_anchorType)
				{
				case CGUIFitSprite::AnchorAll:
					g->addModuleBatch(&m_frame->ModuleOffset[0],
						color,
						m_transform->World,
						getRect(),
						m_anchorLeft,
						w - m_anchorRight,
						m_anchorTop,
						h - m_anchorBottom,
						getShaderID(),
						getMaterial());
					break;
				case CGUIFitSprite::AnchorLeftRight:
					g->addModuleBatchLR(&m_frame->ModuleOffset[0],
						color,
						m_transform->World,
						getRect(),
						m_anchorLeft,
						w - m_anchorRight,
						getShaderID(),
						getMaterial());
					break;
				case CGUIFitSprite::AnchorTopBottom:
					g->addModuleBatchTB(&m_frame->ModuleOffset[0],
						color,
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
			}
		}

		CGUIElement::render(camera);

		if (m_drawBorder)
		{
			core::rectf r = getRect();

			float z = 0.0f;

			core::vector3df topLeft(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, z);
			core::vector3df topRight(r.LowerRightCorner.X, r.UpperLeftCorner.Y, z);
			core::vector3df bottomLeft(r.UpperLeftCorner.X, r.LowerRightCorner.Y, z);
			core::vector3df bottomRight(r.LowerRightCorner.X, r.LowerRightCorner.Y, z);

			m_transform->World.transformVect(topLeft);
			m_transform->World.transformVect(topRight);
			m_transform->World.transformVect(bottomLeft);
			m_transform->World.transformVect(bottomRight);

			std::vector<core::vector2df> lines;
			lines.push_back(core::vector2df(topLeft.X + m_anchorLeft, topLeft.Y + m_anchorTop));
			lines.push_back(core::vector2df(topRight.X - m_anchorRight, topRight.Y + m_anchorTop));
			lines.push_back(core::vector2df(bottomRight.X - m_anchorRight, bottomRight.Y - m_anchorBottom));
			lines.push_back(core::vector2df(bottomLeft.X + m_anchorLeft, bottomLeft.Y - m_anchorBottom));
			lines.push_back(core::vector2df(topLeft.X + m_anchorLeft, topLeft.Y + m_anchorTop));

			SColor borderColor(100, 255, 255, 255);
			CGraphics2D::getInstance()->draw2DLines(lines, borderColor);
		}
	}

	void CGUIFitSprite::setFrame(SFrame* frame)
	{
		m_frame = frame;
	}

	void CGUIFitSprite::setFrameSource(const char* spritePath, const char* frameName, const char* editorFileRef)
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
				if (editorFileRef)
					m_frameName = editorFileRef;
				else
					m_frameName = frameName;
				m_sprite = spritePath;
				m_spriteId = sprite->getId();
			}
		}
	}

	void CGUIFitSprite::setAnchor(AnchorType type, float left, float right, float top, float bottom)
	{
		m_anchorType = type;
		m_anchorLeft = core::max_(left, 0.0f);
		m_anchorRight = core::max_(right, 0.0f);
		m_anchorTop = core::max_(top, 0.0f);
		m_anchorBottom = core::max_(bottom, 0.0f);
	}

	CObjectSerializable* CGUIFitSprite::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();

		CFrameSourceProperty* frame = new CFrameSourceProperty(object, "spriteSrc", m_frameName.c_str());
		frame->setGUID(m_guid.c_str());
		frame->setSprite(m_sprite.c_str());
		frame->setSpriteGUID(m_spriteId.c_str());
		object->autoRelease(frame);

		CEnumProperty<AnchorType>* anchorType = new CEnumProperty<AnchorType>(object, "anchorType", m_anchorType);
		anchorType->addEnumString("Anchor all", CGUIFitSprite::AnchorAll);
		anchorType->addEnumString("Anchor left right", CGUIFitSprite::AnchorLeftRight);
		anchorType->addEnumString("Anchor top bottom", CGUIFitSprite::AnchorTopBottom);
		object->autoRelease(anchorType);

		object->autoRelease(new CFloatProperty(object, "anchorLeft", m_anchorLeft, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorRight", m_anchorRight, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorTop", m_anchorTop, 0.0f));
		object->autoRelease(new CFloatProperty(object, "anchorBottom", m_anchorBottom, 0.0f));

		return object;
	}

	void CGUIFitSprite::loadSerializable(CObjectSerializable* object)
	{
		CFrameSourceProperty* frame = dynamic_cast<CFrameSourceProperty*>(object->getProperty("spriteSrc"));

		m_anchorType = object->get<AnchorType>("anchorType", CGUIFitSprite::AnchorAll);
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