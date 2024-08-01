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

#pragma once

#include "Components/CComponentSystem.h"

#include "Camera/CCamera.h"

#include "GUI/CGUIElement.h"
#include "GUI/CGUIImage.h"
#include "GUI/CGUIText.h"
#include "GUI/CGUISprite.h"
#include "GUI/CGUIRect.h"
#include "GUI/CGUIMask.h"
#include "GUI/CGUILayout.h"
#include "GUI/CGUIFitSprite.h"

#include "Entity/CEntityPrefab.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	class SKYLICHT_API CCanvas : public CComponentSystem
	{
	protected:
		core::rectf m_rect;

		core::rectf m_defaultRect;

		CGUIElement* m_root;

		int m_sortDepth;

		bool m_enable3DBillboard;

		core::matrix4 m_renderWorldTransform;

		CCamera* m_renderCamera;

		// ECS System
		CEntityPrefab* m_entityMgr;

		std::vector<IEntitySystem*> m_systems;

		CFastArray<CEntity*> m_depth[MAX_ENTITY_DEPTH];
		CFastArray<CEntity*> m_alives;

	public:

		bool IsInEditor;

	public:
		CCanvas();

		virtual ~CCanvas();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void onResize();

		void updateEntities();

		void render(CCamera* camera);

		CGUIElement* getHitTest(CCamera* camera, float x, float y, const core::recti& viewport);

		inline CEntityPrefab* getEntityManager()
		{
			return m_entityMgr;
		}

	public:

		inline void setSortDepth(int d)
		{
			m_sortDepth = d;
		}

		inline int getSortDepth()
		{
			return m_sortDepth;
		}

		inline void setRect(const core::rectf& r)
		{
			m_rect = r;
		}

		inline void setDefaultRect(const core::rectf& r)
		{
			m_defaultRect = r;
		}

		inline const core::rectf& getDefaultRect()
		{
			return m_defaultRect;
		}

		void applyScaleGUI(float widthOrHeight = 1.0f);

		inline CGUIElement* getRootElement()
		{
			return m_root;
		}

		inline void enable3DBillboard(bool b)
		{
			m_enable3DBillboard = b;
		}

		inline bool isEnable3DBillboard()
		{
			return m_enable3DBillboard;
		}

		void removeAllElement();

		CGUIElement* createElement();

		CGUIElement* createElement(const core::rectf& r);

		CGUIElement* createElement(CGUIElement* e, const core::rectf& r);

		CGUIImage* createImage();

		CGUIImage* createImage(const core::rectf& r);

		CGUIImage* createImage(CGUIElement* e, const core::rectf& r);

		CGUIText* createText(IFont* font);

		CGUIText* createText(const core::rectf& r, IFont* font);

		CGUIText* createText(CGUIElement* e, IFont* font);

		CGUIText* createText(CGUIElement* e, const core::rectf& r, IFont* font);

		CGUISprite* createSprite(SFrame* frame);

		CGUISprite* createSprite(const core::rectf& r, SFrame* frame);

		CGUISprite* createSprite(CGUIElement* e, SFrame* frame);

		CGUISprite* createSprite(CGUIElement* e, const core::rectf& r, SFrame* frame);

		CGUIMask* createMask(const core::rectf& r);

		CGUIMask* createMask(CGUIElement* e, const core::rectf& r);

		CGUIRect* createRect(const video::SColor& c);

		CGUIRect* createRect(const core::rectf& r, const video::SColor& c);

		CGUIRect* createRect(CGUIElement* e, const core::rectf& r, const video::SColor& c);

		CGUILayout* createLayout();

		CGUILayout* createLayout(const core::rectf& r);

		CGUILayout* createLayout(CGUIElement* e, const core::rectf& r);

		CGUIFitSprite* createFitSprite(SFrame* frame);

		CGUIFitSprite* createFitSprite(const core::rectf& r, SFrame* frame);

		CGUIFitSprite* createFitSprite(CGUIElement* e, SFrame* frame);

		CGUIFitSprite* createFitSprite(CGUIElement* e, const core::rectf& r, SFrame* frame);

		const core::matrix4& getRenderWorldTransform()
		{
			return m_renderWorldTransform;
		}

		inline void setRenderWorldTransform(core::matrix4& w)
		{
			m_renderWorldTransform = w;
		}

		inline CCamera* getRenderCamera()
		{
			return m_renderCamera;
		}

		CGUIElement* getGUIByID(const char* id);
	};
}