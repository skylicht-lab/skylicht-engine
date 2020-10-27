/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CCanvas::CCanvas(float width, float height) :
				CBase(NULL),
				FirstTab(NULL),
				NextTab(NULL)
			{
				setBounds(0.0, 0.0f, width, height);
				initialize();
			}

			CCanvas::~CCanvas()
			{

			}

			void CCanvas::initialize()
			{
			}

			CCanvas* CCanvas::getCanvas()
			{
				return this;
			}

			void CCanvas::addDelayedDelete(CBase* control)
			{
				if (m_deleteSet.find(control) == m_deleteSet.end())
				{
					m_deleteSet.insert(control);
					m_deleteList.push_back(control);
				}
			}

			void CCanvas::removeDelayDelete(CBase *control)
			{
				std::set<CBase*>::iterator itFind;

				if ((itFind = m_deleteSet.find(control)) != m_deleteSet.end())
				{
					m_deleteList.remove(control);
					m_deleteSet.erase(control);
				}
			}

			void CCanvas::processDelayedDeletes()
			{
				CBase::List deleteList = m_deleteList;
				m_deleteList.clear();
				m_deleteSet.clear();

				for (auto&& control : deleteList)
				{
					delete control;
				}
			}

			void CCanvas::update()
			{
				processDelayedDeletes();

				if (isHidden())
					return;

				recurseLayout();

				if (NextTab == NULL)
					NextTab = FirstTab;
			}

			void CCanvas::doRender()
			{
				CRenderer *render = CRenderer::getRenderer();

				render->begin();

				render->setClipRegion(m_bounds);

				render->setRenderOffset(SPoint(0.0f, 0.0f));

				CBase::doRender();

				render->end();
			}

			void CCanvas::render()
			{
				// do nothing
			}
		}
	}
}