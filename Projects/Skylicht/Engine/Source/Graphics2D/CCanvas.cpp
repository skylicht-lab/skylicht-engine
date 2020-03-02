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
		m_enable3DBillboard(false)
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
		// we update GUI element one last frame update
		for (int i = 0; i < MAX_CHILD_DEPTH; i++)
			m_entitiesTree[i].set_used(0);

		int maxLevel = 0;

		// pick transform changed to update
		for (CGUIElement *entity : m_entities)
		{
			int level = entity->getLevel();

			if (entity->isTransformChanged() == true)
			{
				// me changed
				m_entitiesTree[level].push_back(entity);
				if (level > maxLevel)
					maxLevel = level;
			}
			else if (entity->getParent() != NULL && entity->getParent()->isTransformChanged() == true)
			{
				// parent changed
				m_entitiesTree[level].push_back(entity);
				if (level > maxLevel)
					maxLevel = level;
				entity->setTransformChanged(true);
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
		for (CGUIElement *entity : m_entities)
		{
			if (entity->isCullingVisisble() == true)
				entity->render(camera);
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
		CGUIText *element = new CGUIText(this, m_root, m_rect, font);
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