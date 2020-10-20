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
#include "CBase.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CBase::CBase(CBase* parent, const std::string& name) :
				m_parent(parent),
				m_name(name),
				m_disabled(false),
				m_hidden(false),
				m_needsLayout(true),
				m_dock(EPosition::None),
				m_bounds(0.0f, 0.0f, 18.0f, 30.0f),
				m_padding(0.0f, 0.0f, 0.0f, 0.0f),
				m_margin(0.0f, 0.0f, 0.0f, 0.0f)
			{

			}

			CBase::~CBase()
			{

			}

			void CBase::setHidden(bool hidden)
			{
				if (m_hidden == hidden)
					return;

				m_hidden = hidden;
				invalidate();
				reDraw();
			}

			bool CBase::isHidden() const
			{
				return m_hidden;
			}

			bool CBase::isVisible() const
			{
				if (isHidden())
					return false;

				if (m_parent)
					return m_parent->isVisible();

				return true;
			}

			void CBase::setDisabled(bool active)
			{
				if (m_disabled == active)
					return;

				m_disabled = active;
				reDraw();
			}

			void CBase::reDraw()
			{
				updateColors();
				if (m_parent)
					m_parent->reDraw();
			}

			void CBase::addChild(CBase* child)
			{
				Children.push_back(child);

				onChildAdded(child);
				child->m_parent = this;
			}

			void CBase::removeChild(CBase* child)
			{
				child->m_parent = NULL;
				Children.remove(child);
				onChildRemoved(child);
			}

			void CBase::removeAllChildren()
			{
				while (Children.size() > 0)
				{
					removeChild(*Children.begin());
				}
			}

			void CBase::sendToBack(void)
			{
				if (!m_parent)
					return;

				if (m_parent->Children.front() == this)
					return;

				m_parent->Children.remove(this);
				m_parent->Children.push_front(this);
				invalidateParent();
			}

			void CBase::bringToFront(void)
			{
				if (!m_parent)
					return;

				if (m_parent->Children.back() == this)
					return;

				m_parent->Children.remove(this);
				m_parent->Children.push_back(this);
				invalidateParent();
				reDraw();
			}

			void CBase::bringNextToControl(CBase* child, bool bBehind)
			{
				if (!m_parent)
					return;

				m_parent->Children.remove(this);
				List::iterator it = std::find(m_parent->Children.begin(), m_parent->Children.end(), child);

				if (it == m_parent->Children.end())
					return bringToFront();

				if (bBehind)
				{
					++it;
					if (it == m_parent->Children.end())
						return bringToFront();
				}

				m_parent->Children.insert(it, this);
				invalidateParent();
			}

			CBase* CBase::findChildByName(const std::string& name, bool bRecursive)
			{
				for (auto&& child : Children)
				{
					if (!child->getName().empty() && child->getName() == name)
						return child;

					if (bRecursive)
					{
						CBase* subChild = child->findChildByName(name, true);

						if (subChild)
							return subChild;
					}
				}

				return nullptr;
			}

			bool CBase::isChild(CBase* child)
			{
				for (auto&& c : Children)
				{
					if (c == child)
						return true;
				}

				return false;
			}

			u32 CBase::numChildren()
			{
				return Children.size();
			}

			CBase* CBase::getChild(u32 i)
			{
				if (i >= numChildren())
					return nullptr;

				for (auto&& child : Children)
				{
					if (i == 0)
						return child;

					i--;
				}

				// Should never happen
				return NULL;
			}

			void CBase::setParent(CBase* parent)
			{
				if (m_parent == parent)
					return;

				if (m_parent)
					m_parent->removeChild(this);

				m_parent = parent;

				if (m_parent)
					m_parent->addChild(this);
			}

			bool CBase::setBounds(const core::rectf& bounds)
			{
				if (m_bounds == bounds)
					return false;

				const core::rectf oldBounds = m_bounds;
				m_bounds = bounds;
				onBoundsChanged(oldBounds);
				return true;
			}

			void CBase::onBoundsChanged(const core::rectf oldBounds)
			{
				if (m_parent)
					m_parent->onChildBoundsChanged(oldBounds, this);

				invalidate();
				reDraw();
				updateRenderBounds();
			}

			void CBase::onChildBoundsChanged(const core::rectf oldChildBounds, CBase* child)
			{

			}

			void CBase::updateRenderBounds()
			{
				m_renderBounds.UpperLeftCorner.X = 0;
				m_renderBounds.UpperLeftCorner.Y = 0;
				m_renderBounds.LowerRightCorner.X = m_bounds.getWidth();
				m_renderBounds.LowerRightCorner.Y = m_bounds.getHeight();
			}

			void CBase::moveTo(float x, float y)
			{
				if (m_parent)
				{
					if (x - m_padding.Left < m_parent->m_margin.Left)
						x = m_parent->m_margin.Left + m_padding.Left;

					if (y - m_padding.Top < m_parent->m_margin.Top)
						y = m_parent->m_margin.Top + m_padding.Top;

					if (x + width() + m_padding.Right > m_parent->width() - m_parent->m_margin.Right)
						x = m_parent->width() - m_parent->m_margin.Right - width() - m_padding.Right;

					if (y + height() + m_padding.Bottom > m_parent->height() - m_parent->m_margin.Bottom)
						y = m_parent->height() - m_parent->m_margin.Bottom - height() - m_padding.Bottom;
				}

				setBounds(x, y, width(), height());
			}

			void CBase::doRender()
			{
				think();
				renderRecursive(m_bounds);
			}

			void CBase::renderRecursive(const core::rectf& cliprect)
			{
				CRenderer *render = CRenderer::getRenderer();

				core::vector2df oldRenderOffset = render->getRenderOffset();

				render->addRenderOffset(cliprect);

				renderUnder();

				core::rectf oldRegion = render->clipRegion();

				// If this control is clipping, change the clip rect to ourselves
				// else clip using our parents clip rect.
				if (shouldClip())
				{
					render->addClipRegion(cliprect);

					if (!render->clipRegionVisible())
					{
						render->setRenderOffset(oldRenderOffset);
						render->setClipRegion(oldRegion);
						return;
					}
				}

				// Render this control and children controls
				render->startClip();
				{
					this->render();

					if (!Children.empty())
					{
						// Now render my kids
						for (auto&& child : Children)
						{
							if (child->isHidden())
								continue;

							child->doRender();
						}
					}
				}
				render->endClip();

				// Render overlay/focus
				{
					render->setClipRegion(oldRegion);
					render->startClip();
					{
						renderOver();
						renderFocus();
					}
					render->endClip();
					render->setRenderOffset(oldRenderOffset);
				}
			}

			void CBase::render()
			{

			}

			void CBase::renderFocus()
			{

			}
		}
	}
}