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
#include "CCanvas.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CCanvas::CCanvas() :
		m_sortDepth(0),
		m_enable3DBillboard(false),
		m_renderCamera(NULL)
	{
		CGraphics2D *g = CGraphics2D::getInstance();
		float w = (float)g->getScreenSize().Width;
		float h = (float)g->getScreenSize().Height;

		// default rect is fullscreen
		m_rect = core::rectf(0.0f, 0.0f, w, h);

		// add root
		m_root = new CGUIElement(this, m_rect);
		m_entities.push_back(m_root);

		// add this canvas
		CGraphics2D::getInstance()->addCanvas(this);
	}

	CCanvas::~CCanvas()
	{
		// remove all entity gui
		for (CGUIElement *entity : m_entities)
			delete entity;
		m_entities.clear();

		// remove this canvas
		CGraphics2D::getInstance()->removeCanvas(this);
	}

	void CCanvas::initComponent()
	{
	}

	void CCanvas::updateComponent()
	{
	}

	void CCanvas::render(CCamera *camera)
	{
		m_renderCamera = camera;

		// we update GUI element one last frame update
		for (int i = 0; i < MAX_CHILD_DEPTH; i++)
			m_entitiesTree[i].set_used(0);

		int maxLevel = 0;

		// update all entities
		for (CGUIElement *entity : m_entities)
		{
			int level = entity->getLevel();
			CGUIElement *parent = entity->getParent();

			// update entity
			entity->update(camera);

			// clear all child and build entity render by tree
			entity->m_childs.set_used(0);
			if (parent != NULL)
				parent->m_childs.push_back(entity);

			// pick transform changed to update
			if (entity->isTransformChanged() == true)
			{
				// me changed
				m_entitiesTree[level].push_back(entity);
				if (level > maxLevel)
					maxLevel = level;
			}
			else if (parent != NULL && parent->isTransformChanged() == true)
			{
				// parent changed
				m_entitiesTree[level].push_back(entity);
				if (level > maxLevel)
					maxLevel = level;
				entity->setTransformChanged(true);
			}

			// apply mask
			entity->applyParentMask(entity->getMask());
			if (parent != NULL)
			{
				CGUIMask *mask = parent->getMask();
				if (mask == NULL)
					mask = parent->getCurrentMask();

				if (mask != NULL)
					entity->applyParentMask(mask);
			}
		}

		// update transform
		for (int i = 0; i <= maxLevel; i++)
		{
			core::array<CGUIElement*>& elements = m_entitiesTree[i];

			for (u32 j = 0, m = elements.size(); j < m; j++)
			{
				elements[j]->calcAbsoluteTransform();
				elements[j]->setTransformChanged(false);
			}
		}

		m_maxChildLevel = maxLevel;

		// culling
		for (CGUIElement *entity : m_entities)
		{
			if (entity->getParent() != NULL && entity->getParent()->isCullingVisisble() == false)
			{
				// parent is hide
				entity->setCullingVisisble(false);
			}
			else
			{
				// test clip in screen
				entity->setCullingVisisble(true);
			}
		}

		// render
		std::stack<CGUIElement*> renderEntity;
		renderEntity.push(m_root);

		while (renderEntity.size() > 0)
		{
			CGUIElement *entity = renderEntity.top();
			renderEntity.pop();

			if (entity->isVisible() == false)
				continue;

			CGUIMask *mask = entity->getCurrentMask();
			if (mask != NULL)
				mask->beginMaskTest(camera);

			entity->render(camera);

			if (mask != NULL)
				mask->endMaskTest();

			// note
			// we use stack to render parent -> child
			// so we must inverse render position because stack = Last-In First-Out (LIFO)
			for (int i = (int)entity->m_childs.size() - 1; i >= 0; i--)
				renderEntity.push(entity->m_childs[i]);
		}
	}

	/*
	* Element constructor
	*/

	CGUIElement* CCanvas::createElement()
	{
		CGUIElement *element = new CGUIElement(this, m_root, m_rect);
		m_entities.push_back(element);
		return element;
	}

	CGUIElement* CCanvas::createElement(const core::rectf& r)
	{
		CGUIElement *element = new CGUIElement(this, m_root, r);
		m_entities.push_back(element);
		return element;
	}

	CGUIElement* CCanvas::createElement(CGUIElement *e, const core::rectf& r)
	{
		CGUIElement *element = new CGUIElement(this, e, r);
		m_entities.push_back(element);
		return element;
	}

	/*
	* Image constructor
	*/

	CGUIImage* CCanvas::createImage()
	{
		CGUIImage *element = new CGUIImage(this, m_root, m_rect);
		m_entities.push_back(element);
		return element;
	}

	CGUIImage* CCanvas::createImage(const core::rectf& r)
	{
		CGUIImage *element = new CGUIImage(this, m_root, r);
		m_entities.push_back(element);
		return element;
	}

	CGUIImage* CCanvas::createImage(CGUIElement *e, const core::rectf& r)
	{
		CGUIImage *element = new CGUIImage(this, e, r);
		m_entities.push_back(element);
		return element;
	}

	/*
	* Text constructor
	*/

	CGUIText* CCanvas::createText(IFont *font)
	{
		CGUIText *element = new CGUIText(this, m_root, m_rect, font);
		m_entities.push_back(element);
		return element;
	}

	CGUIText* CCanvas::createText(const core::rectf& r, IFont *font)
	{
		CGUIText *element = new CGUIText(this, m_root, r, font);
		m_entities.push_back(element);
		return element;
	}

	CGUIText* CCanvas::createText(CGUIElement *e, IFont *font)
	{
		CGUIText *element = new CGUIText(this, e, e->getRect(), font);
		m_entities.push_back(element);
		return element;
	}

	CGUIText* CCanvas::createText(CGUIElement *e, const core::rectf& r, IFont *font)
	{
		CGUIText *element = new CGUIText(this, e, r, font);
		m_entities.push_back(element);
		return element;
	}

	/*
	* Sprite constructor
	*/

	CGUISprite* CCanvas::createSprite(SFrame *frame)
	{
		CGUISprite *element = new CGUISprite(this, m_root, m_rect, frame);
		m_entities.push_back(element);
		return element;
	}

	CGUISprite* CCanvas::createSprite(const core::rectf& r, SFrame *frame)
	{
		CGUISprite *element = new CGUISprite(this, m_root, r, frame);
		m_entities.push_back(element);
		return element;
	}

	CGUISprite* CCanvas::createSprite(CGUIElement *e, SFrame *frame)
	{
		CGUISprite *element = new CGUISprite(this, e, e->getRect(), frame);
		m_entities.push_back(element);
		return element;
	}

	CGUISprite* CCanvas::createSprite(CGUIElement *e, const core::rectf& r, SFrame *frame)
	{
		CGUISprite *element = new CGUISprite(this, e, r, frame);
		m_entities.push_back(element);
		return element;
	}

	/*
	* Mask constructor
	*/

	CGUIMask* CCanvas::createMask(const core::rectf& r)
	{
		CGUIMask *element = new CGUIMask(this, r);
		m_entities.push_back(element);
		return element;
	}

	CGUIMask* CCanvas::createMask(CGUIElement *e, const core::rectf& r)
	{
		CGUIMask *element = new CGUIMask(this, e, r);
		m_entities.push_back(element);
		return element;
	}

	/*
	* Rect constructor
	*/

	CGUIRect* CCanvas::createRect(const video::SColor &c)
	{
		CGUIRect *element = new CGUIRect(this, m_root, m_rect);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}

	CGUIRect* CCanvas::createRect(const core::rectf& r, const video::SColor &c)
	{
		CGUIRect *element = new CGUIRect(this, m_root, r);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}

	CGUIRect* CCanvas::createRect(CGUIElement *e, const core::rectf& r, const video::SColor &c)
	{
		CGUIRect *element = new CGUIRect(this, e, r);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}


	/*
	* RoundedRect constructor
	*/

	CGUIRoundedRect* CCanvas::createRoundedRect(float radius, const video::SColor &c)
	{
		CGUIRoundedRect *element = new CGUIRoundedRect(this, m_root, m_rect, radius);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}

	CGUIRoundedRect* CCanvas::createRoundedRect(const core::rectf& r, float radius, const video::SColor &c)
	{
		CGUIRoundedRect *element = new CGUIRoundedRect(this, m_root, r, radius);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}

	CGUIRoundedRect* CCanvas::createRoundedRect(CGUIElement *e, const core::rectf& r, float radius, const video::SColor &c)
	{
		CGUIRoundedRect *element = new CGUIRoundedRect(this, e, r, radius);
		element->setColor(c);

		m_entities.push_back(element);
		return element;
	}

	/*
	* Element destructor
	*/

	void CCanvas::remove(CGUIElement *element)
	{
		// remove child firest
		removeChildOfParent(element);

		// delete
		delete element;
		m_entities.remove(element);
	}

	void CCanvas::removeChildOfParent(CGUIElement *parent)
	{
		std::list<CGUIElement*> removeList;

		for (CGUIElement *entity : m_entities)
		{
			if (entity->getParent() == parent)
			{
				removeList.push_back(entity);
			}
		}

		for (CGUIElement *entity : removeList)
		{
			entity->remove();
		}
	}
}