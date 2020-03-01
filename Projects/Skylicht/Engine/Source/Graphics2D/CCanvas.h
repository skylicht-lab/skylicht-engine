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

#define MAX_CHILD_DEPTH 512

namespace Skylicht
{
	class CCanvas : public CComponentSystem
	{
	protected:
		std::list<CGUIElement*> m_entities;
		core::array<CGUIElement*> m_entitiesTree[MAX_CHILD_DEPTH];

		core::rectf m_rect;

		CGUIElement *m_root;
		int m_maxChildLevel;

		int m_sortDepth;

		bool m_enable3DBillboard;

	public:
		CCanvas();

		virtual ~CCanvas();

		virtual void initComponent();

		virtual void updateComponent();

		void render(CCamera *camera);

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

		inline int generateID()
		{
			return (int)m_entities.size();
		}

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

		CGUIElement* createElement();

		CGUIElement* createElement(const core::rectf& r);

		CGUIElement* createElement(CGUIElement *e, const core::rectf& r);

		CGUIImage* createImage();

		CGUIImage* createImage(const core::rectf& r);

		CGUIImage* createImage(CGUIElement *e, const core::rectf& r);

		CGUIText* createText();

		CGUIText* createText(const core::rectf& r);

		CGUIText* createText(CGUIElement *e, const core::rectf& r);

		void remove(CGUIElement *element);

	protected:

		void removeChildOfParent(CGUIElement *parent);
	};
}