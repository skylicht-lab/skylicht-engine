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
#include "Projective/CProjective.h"

#include "GUISystem/CGUILayoutSystem.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CCanvas::CCanvas() :
		m_sortDepth(0),
		m_enable3DBillboard(false),
		m_renderCamera(NULL),
		IsInEditor(false)
	{
		CGraphics2D* g = CGraphics2D::getInstance();
		float w = (float)g->getScreenSize().Width;
		float h = (float)g->getScreenSize().Height;

		// default rect is fullscreen
		m_rect = core::rectf(0.0f, 0.0f, w, h);
		m_defaultRect = m_rect;

		m_entityMgr = new CEntityPrefab();

		// add root
		m_root = new CGUIElement(this, NULL, m_rect);
		m_root->setDock(EGUIDock::DockFill);
		m_root->setName("Root GUI");

		m_systems.push_back(new CGUILayoutSystem());
		for (IEntitySystem* system : m_systems)
		{
			system->init(NULL);
		}

		// add this canvas
		CGraphics2D::getInstance()->addCanvas(this);
	}

	CCanvas::~CCanvas()
	{
		for (IEntitySystem* system : m_systems)
		{
			delete system;
		}
		m_systems.clear();

		// remove all entity gui
		delete m_root;

		delete m_entityMgr;

		// remove this canvas
		CGraphics2D::getInstance()->removeCanvas(this);
	}

	void CCanvas::initComponent()
	{
	}

	void CCanvas::updateComponent()
	{
	}

	void CCanvas::onResize()
	{
		// layout on 2d ui
		if (m_renderCamera && m_renderCamera->getProjectionType() == CCamera::OrthoUI)
		{
			// get current screen size
			CGraphics2D* g = CGraphics2D::getInstance();
			core::dimension2du s = g->getScreenSize();

			float w = (float)s.Width;
			float h = (float)s.Height;

			// update the root rect
			m_rect = core::rectf(0.0f, 0.0f, w, h);
		}
	}

	void CCanvas::updateEntities()
	{
		for (u32 i = 0; i < MAX_ENTITY_DEPTH; i++)
		{
			m_depth[i].reset();
		}

		CEntity** entities = m_entityMgr->getEntities();
		int numEntity = m_entityMgr->getNumEntities();

		int maxDepth = 0;

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			if (entity->isAlive())
			{
				CWorldTransformData* world = GET_ENTITY_DATA(entity, CWorldTransformData);

				m_depth[world->Depth].push(entity);

				if (maxDepth < world->Depth)
					maxDepth = world->Depth;

				if (world->ParentIndex != -1)
					world->Parent = GET_ENTITY_DATA(entities[world->ParentIndex], CWorldTransformData);
				else
					world->Parent = NULL;
			}
		}

		m_alives.reset();

		// copy and sort the alives by depth
		int count = 0;
		for (int i = 0; i <= maxDepth; i++)
		{
			CEntity** entitiesPtr = m_depth[i].pointer();

			for (int j = 0, n = m_depth[i].count(); j < n; j++)
				m_alives.push(entitiesPtr[j]);
		}

		// update systems
		for (IEntitySystem* system : m_systems)
		{
			system->beginQuery(NULL);
			system->onQuery(NULL, m_alives.pointer(), m_alives.count());
			system->update(NULL);
		}
	}

	void CCanvas::render(CCamera* camera)
	{
		m_renderCamera = camera;

		// render
		std::stack<CGUIElement*> renderEntity;
		renderEntity.push(m_root);

		while (renderEntity.size() > 0)
		{
			CGUIElement* entity = renderEntity.top();
			renderEntity.pop();

			// skip the invisible
			if (entity->isVisible() == false)
				continue;

			// update the entity
			entity->update(camera);

			// apply mask
			entity->applyCurrentMask(entity->getMask());

			// try set the parrent mask
			CGUIElement* parent = entity->getParent();
			if (parent != NULL)
			{
				CGUIMask* mask = parent->getMask();
				if (mask == NULL)
					mask = parent->getCurrentMask();

				if (mask != NULL)
					entity->applyCurrentMask(mask);
			}

			CGUIMask* mask = entity->getCurrentMask();
			if (mask != NULL)
				mask->beginMaskTest(camera);

			entity->render(camera);

			if (mask != NULL)
				mask->endMaskTest();

			// we use stack to render parent -> child
			// so we must inverse render position because stack = Last-In First-Out (LIFO)
			for (int i = (int)entity->m_childs.size() - 1; i >= 0; i--)
				renderEntity.push(entity->m_childs[i]);
		}
	}

	CGUIElement* CCanvas::getHitTest(CCamera* camera, float x, float y, const core::recti& viewport)
	{
		core::matrix4 billboardMatrix;
		core::matrix4 world;

		if (camera->getProjectionType() != CCamera::OrthoUI)
		{
			// Calculate billboard matrix
			// this is invert view camera
			const core::matrix4& cameraTransform = camera->getGameObject()->getTransform()->getRelativeTransform();

			// look vector
			core::vector3df cameraPosition = cameraTransform.getTranslation();
			core::vector3df look(0.0f, 0.0f, -1.0f);
			cameraTransform.rotateVect(look);
			look.normalize();

			// up vector
			core::vector3df up(0.0f, 1.0f, 0.0f);
			cameraTransform.rotateVect(up);
			up.normalize();

			// right vector
			core::vector3df right = up.crossProduct(look);
			up = look.crossProduct(right);
			up.normalize();

			// billboard matrix		
			f32* matData = billboardMatrix.pointer();

			matData[0] = right.X;
			matData[1] = right.Y;
			matData[2] = right.Z;
			matData[3] = 0.0f;

			matData[4] = up.X;
			matData[5] = up.Y;
			matData[6] = up.Z;
			matData[7] = 0.0f;

			matData[8] = look.X;
			matData[9] = look.Y;
			matData[10] = look.Z;
			matData[11] = 0.0f;

			matData[12] = 0.0f;
			matData[13] = 0.0f;
			matData[14] = 0.0f;
			matData[15] = 1.0f;
		}

		if (m_enable3DBillboard == true && camera->getProjectionType() != CCamera::OrthoUI)
		{
			// rotation canvas to billboard
			world = billboardMatrix;

			// scale canvas
			core::matrix4 scale;
			scale.setScale(m_root->getScale());
			world *= scale;

			// move to canvas position
			world.setTranslation(m_root->getPosition());
		}
		else
		{
			// world is relative of root
			world = m_root->getRelativeTransform();
		}

		core::array<CGUIElement*> visits;
		visits.push_back(m_root);

		CGUIElement* result = NULL;

		while (visits.size() > 0)
		{
			CGUIElement* gui = visits.getLast();
			visits.erase(visits.size() - 1);

			core::matrix4 worldElementTransform;
			if (gui == m_root)
				worldElementTransform = world;
			else
				worldElementTransform = world * gui->getAbsoluteTransform();

			const core::rectf& r = gui->getRect();

			core::vector3df p[4];
			core::vector3df p2d[4];

			p[0].set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 0.0f);
			p[1].set(r.LowerRightCorner.X, r.UpperLeftCorner.Y, 0.0f);
			p[2].set(r.UpperLeftCorner.X, r.LowerRightCorner.Y, 0.0f);
			p[3].set(r.LowerRightCorner.X, r.LowerRightCorner.Y, 0.0f);

			for (int i = 0; i < 4; i++)
			{
				// get real 3d position
				worldElementTransform.transformVect(p[i]);

				// project to 2d screen
				CProjective::getScreenCoordinatesFrom3DPosition(camera, p[i], p2d[i].X, p2d[i].Y, viewport.getWidth(), viewport.getHeight());
				p2d[i].Z = 0.0f;
			}

			// hit test
			core::triangle3df t1(p2d[0], p2d[1], p2d[2]);
			core::triangle3df t2(p2d[2], p2d[1], p2d[3]);
			core::vector3df mousePos(x, y, 0.0f);

			bool hit = false;
			if (t1.isPointInside(mousePos) == true || t2.isPointInside(mousePos) == true)
				hit = true;

			if (hit)
			{
				result = gui;

				// add child to test child
				visits.clear();
				std::vector<CGUIElement*>& childs = gui->getChilds();
				for (CGUIElement* e : childs)
					visits.push_back(e);
			}
		}

		return result;
	}

	void CCanvas::removeAllElement()
	{
		m_root->removeAllChilds();
	}

	void CCanvas::applyScaleGUI(float widthOrHeight)
	{
		if (m_defaultRect.getWidth() == 0.0f || m_defaultRect.getHeight() == 0.0f)
			return;

		float s = core::clamp(widthOrHeight, 0.0f, 1.0f);

		CGraphics2D* g = CGraphics2D::getInstance();
		float screenW = (float)g->getScreenSize().Width;
		float screenH = (float)g->getScreenSize().Height;

		float sw = screenW / m_defaultRect.getWidth();
		float sh = screenH / m_defaultRect.getHeight();

		float scale = sw + (sh - sw) * widthOrHeight;
		float w = screenW / scale;
		float h = screenH / scale;

		// apply new rect
		core::rectf r = m_root->getRect();
		r.LowerRightCorner.X = r.UpperLeftCorner.X + w;
		r.LowerRightCorner.Y = r.UpperLeftCorner.Y + h;
		m_root->setRect(r);

		// apply new scale
		m_root->setScale(core::vector3df(scale, scale, 1.0f));
	}

	/*
	* Element constructor
	*/

	CGUIElement* CCanvas::createElement()
	{
		CGUIElement* element = new CGUIElement(this, m_root, m_rect);
		return element;
	}

	CGUIElement* CCanvas::createElement(const core::rectf& r)
	{
		CGUIElement* element = new CGUIElement(this, m_root, r);
		return element;
	}

	CGUIElement* CCanvas::createElement(CGUIElement* e, const core::rectf& r)
	{
		CGUIElement* element = new CGUIElement(this, e, r);
		return element;
	}

	/*
	* Image constructor
	*/

	CGUIImage* CCanvas::createImage()
	{
		CGUIImage* element = new CGUIImage(this, m_root, m_rect);
		return element;
	}

	CGUIImage* CCanvas::createImage(const core::rectf& r)
	{
		CGUIImage* element = new CGUIImage(this, m_root, r);
		return element;
	}

	CGUIImage* CCanvas::createImage(CGUIElement* e, const core::rectf& r)
	{
		CGUIImage* element = new CGUIImage(this, e, r);
		return element;
	}

	/*
	* Text constructor
	*/

	CGUIText* CCanvas::createText(IFont* font)
	{
		CGUIText* element = new CGUIText(this, m_root, m_rect, font);
		return element;
	}

	CGUIText* CCanvas::createText(const core::rectf& r, IFont* font)
	{
		CGUIText* element = new CGUIText(this, m_root, r, font);
		return element;
	}

	CGUIText* CCanvas::createText(CGUIElement* e, IFont* font)
	{
		CGUIText* element = new CGUIText(this, e, e->getRect(), font);
		return element;
	}

	CGUIText* CCanvas::createText(CGUIElement* e, const core::rectf& r, IFont* font)
	{
		CGUIText* element = new CGUIText(this, e, r, font);
		return element;
	}

	/*
	* Sprite constructor
	*/

	CGUISprite* CCanvas::createSprite(SFrame* frame)
	{
		CGUISprite* element = new CGUISprite(this, m_root, m_rect, frame);
		return element;
	}

	CGUISprite* CCanvas::createSprite(const core::rectf& r, SFrame* frame)
	{
		CGUISprite* element = new CGUISprite(this, m_root, r, frame);
		return element;
	}

	CGUISprite* CCanvas::createSprite(CGUIElement* e, SFrame* frame)
	{
		CGUISprite* element = new CGUISprite(this, e, e->getRect(), frame);
		return element;
	}

	CGUISprite* CCanvas::createSprite(CGUIElement* e, const core::rectf& r, SFrame* frame)
	{
		CGUISprite* element = new CGUISprite(this, e, r, frame);
		return element;
	}

	/*
	* Mask constructor
	*/

	CGUIMask* CCanvas::createMask(const core::rectf& r)
	{
		CGUIMask* element = new CGUIMask(this, m_root, r);
		return element;
	}

	CGUIMask* CCanvas::createMask(CGUIElement* e, const core::rectf& r)
	{
		CGUIMask* element = new CGUIMask(this, e, r);
		return element;
	}

	/*
	* Rect constructor
	*/

	CGUIRect* CCanvas::createRect(const video::SColor& c)
	{
		CGUIRect* element = new CGUIRect(this, m_root, m_rect);
		element->setColor(c);
		return element;
	}

	CGUIRect* CCanvas::createRect(const core::rectf& r, const video::SColor& c)
	{
		CGUIRect* element = new CGUIRect(this, m_root, r);
		element->setColor(c);
		return element;
	}

	CGUIRect* CCanvas::createRect(CGUIElement* e, const core::rectf& r, const video::SColor& c)
	{
		CGUIRect* element = new CGUIRect(this, e, r);
		element->setColor(c);
		return element;
	}

	/*
	* Layout constructor
	*/

	CGUILayout* CCanvas::createLayout()
	{
		CGUILayout* element = new CGUILayout(this, m_root, m_rect);
		return element;
	}

	CGUILayout* CCanvas::createLayout(const core::rectf& r)
	{
		CGUILayout* element = new CGUILayout(this, m_root, r);
		return element;
	}

	CGUILayout* CCanvas::createLayout(CGUIElement* e, const core::rectf& r)
	{
		CGUILayout* element = new CGUILayout(this, e, r);
		return element;
	}

	/*
	* Custom Size Sprite constructor
	*/

	CGUIFitSprite* CCanvas::createFitSprite(SFrame* frame)
	{
		CGUIFitSprite* element = new CGUIFitSprite(this, m_root, m_rect, frame);
		return element;
	}

	CGUIFitSprite* CCanvas::createFitSprite(const core::rectf& r, SFrame* frame)
	{
		CGUIFitSprite* element = new CGUIFitSprite(this, m_root, r, frame);
		return element;
	}

	CGUIFitSprite* CCanvas::createFitSprite(CGUIElement* e, SFrame* frame)
	{
		CGUIFitSprite* element = new CGUIFitSprite(this, e, e->getRect(), frame);
		return element;
	}

	CGUIFitSprite* CCanvas::createFitSprite(CGUIElement* e, const core::rectf& r, SFrame* frame)
	{
		CGUIFitSprite* element = new CGUIFitSprite(this, e, r, frame);
		return element;
	}

	CGUIElement* CCanvas::getGUIByID(const char* id)
	{
		std::queue<CGUIElement*> queue;
		queue.push(m_root);

		while (queue.size() > 0)
		{
			CGUIElement* gui = queue.front();
			queue.pop();

			if (CStringImp::comp(gui->getID(), id) == 0)
			{
				return gui;
			}

			std::vector<CGUIElement*>& childs = gui->getChilds();
			for (CGUIElement* e : childs)
			{
				queue.push(e);
			}
		}

		return NULL;
	}
}