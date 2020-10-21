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
#include "CCanvas.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CBase::CBase(CBase* parent, const std::string& name) :
				m_parent(NULL),
				m_name(name),
				m_disabled(false),
				m_hidden(false),
				m_needsLayout(true),
				m_dock(EPosition::None),
				m_bounds(0.0f, 0.0f, 18.0f, 30.0f),
				m_padding(0.0f, 0.0f, 0.0f, 0.0f),
				m_margin(0.0f, 0.0f, 0.0f, 0.0f),
				m_mouseInputEnabled(true),
				m_keyboardInputEnabled(true)
			{
				setParent(parent);
			}

			CBase::~CBase()
			{
				CCanvas *canvas = getCanvas();

				if (canvas != NULL)
					canvas->removeDelayDelete(this);

				setParent(NULL);

				releaseChildren();

				if (CGUIContext::HoveredControl == this)
					CGUIContext::HoveredControl = NULL;

				if (CGUIContext::KeyboardFocus == this)
					CGUIContext::KeyboardFocus = NULL;

				if (CGUIContext::MouseFocus == this)
					CGUIContext::MouseFocus = NULL;
			}

			void CBase::setHidden(bool hidden)
			{
				if (m_hidden == hidden)
					return;

				m_hidden = hidden;
				invalidate();
				reDraw();
			}

			void CBase::remove()
			{
				CCanvas* canvas = getCanvas();
				canvas->addDelayedDelete(this);
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

			void CBase::releaseChildren()
			{
				List::iterator iter = Children.begin();

				while (iter != Children.end())
				{
					CBase* child = *iter;
					iter = Children.erase(iter);
					delete child;
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

			CCanvas* CBase::getCanvas()
			{
				if (!m_parent)
					return NULL;

				return m_parent->getCanvas();
			}

			bool CBase::setBounds(const SRect& bounds)
			{
				if (m_bounds == bounds)
					return false;

				const SRect oldBounds = m_bounds;
				m_bounds = bounds;
				onBoundsChanged(oldBounds);
				return true;
			}

			void CBase::onBoundsChanged(const SRect oldBounds)
			{
				if (m_parent)
					m_parent->onChildBoundsChanged(oldBounds, this);

				invalidate();
				reDraw();
				updateRenderBounds();
			}

			void CBase::onChildBoundsChanged(const SRect oldChildBounds, CBase* child)
			{

			}

			void CBase::updateRenderBounds()
			{
				m_renderBounds.X = 0;
				m_renderBounds.Y = 0;
				m_renderBounds.Width = m_bounds.Width;
				m_renderBounds.Height = m_bounds.Height;
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

			void CBase::renderRecursive(const SRect& cliprect)
			{
				CRenderer *render = CRenderer::getRenderer();

				SPoint oldRenderOffset = render->getRenderOffset();

				render->addRenderOffset(cliprect);

				renderUnder();

				SRect oldRegion = render->clipRegion();

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

			bool CBase::isHovered() const
			{
				return CGUIContext::HoveredControl == this;
			}

			bool CBase::shouldDrawHover() const
			{
				return CGUIContext::MouseFocus == this || CGUIContext::MouseFocus == NULL;
			}

			void CBase::touch()
			{
				if (m_parent)
					m_parent->onChildTouched(this);
			}

			void CBase::onChildTouched(CBase* child)
			{
				touch();
			}

			bool CBase::isOnTop()
			{
				if (!m_parent)
					return false;

				List::iterator iter = m_parent->Children.begin();
				CBase* child = *iter;

				if (child == this)
					return true;

				return false;
			}

			bool CBase::isFocussed()
			{
				return CGUIContext::KeyboardFocus == this;
			}

			void CBase::focus()
			{
				if (CGUIContext::KeyboardFocus == this)
					return;

				if (CGUIContext::KeyboardFocus)
					CGUIContext::KeyboardFocus->onLostKeyboardFocus();

				CGUIContext::KeyboardFocus = this;
				onKeyboardFocus();
				reDraw();
			}

			void CBase::unfocus()
			{
				if (CGUIContext::KeyboardFocus != this)
					return;

				CGUIContext::KeyboardFocus = NULL;
				onLostKeyboardFocus();
				reDraw();
			}

			void CBase::onMouseEnter()
			{
				OnHoverEnter(this);

				reDraw();
			}

			void CBase::onMouseLeave()
			{
				OnHoverLeave(this);

				reDraw();
			}

			bool CBase::onMouseWheeled(int iDelta)
			{
				if (m_parent)
					return m_parent->onMouseWheeled(iDelta);

				return false;
			}

			bool CBase::onKeyPress(int iKey, bool bPress)
			{
				bool handled = false;

				switch (iKey)
				{
				case EKey::KEY_TAB:
					handled = onKeyTab(bPress);
					break;

				case EKey::KEY_SPACE:
					handled = onKeySpace(bPress);
					break;

				case EKey::KEY_HOME:
					handled = onKeyHome(bPress);
					break;

				case EKey::KEY_END:
					handled = onKeyEnd(bPress);
					break;

				case EKey::KEY_RETURN:
					handled = onKeyReturn(bPress);
					break;

				case EKey::KEY_BACK:
					handled = onKeyBackspace(bPress);
					break;

				case EKey::KEY_DELETE:
					handled = onKeyDelete(bPress);
					break;

				case EKey::KEY_RIGHT:
					handled = onKeyRight(bPress);
					break;

				case EKey::KEY_LEFT:
					handled = onKeyLeft(bPress);
					break;

				case EKey::KEY_UP:
					handled = onKeyUp(bPress);
					break;

				case EKey::KEY_DOWN:
					handled = onKeyDown(bPress);
					break;

				case EKey::KEY_ESCAPE:
					handled = onKeyEscape(bPress);
					break;

				default:
					break;
				}

				if (!handled && m_parent)
					m_parent->onKeyPress(iKey, bPress);

				return handled;
			}

			bool CBase::onKeyRelease(int iKey)
			{
				return onKeyPress(iKey, false);
			}

			bool CBase::onKeyTab(bool bDown)
			{
				if (!bDown)
					return true;

				if (getCanvas()->NextTab)
				{
					getCanvas()->NextTab->focus();
					reDraw();
				}

				return true;
			}

			CBase* CBase::getControlAt(float x, float y, bool bOnlyIfMouseEnabled)
			{
				if (isHidden())
					return NULL;

				if (x < 0 || y < 0 || x >= width() || y >= height())
					return NULL;

				for (List::reverse_iterator iter = Children.rbegin(); iter != Children.rend(); ++iter)
				{
					CBase* child = *iter;
					CBase* found = NULL;
					found = child->getControlAt(x - child->X(), y - child->Y(), bOnlyIfMouseEnabled);

					if (found)
						return found;
				}

				if (bOnlyIfMouseEnabled && !getMouseInputEnabled())
					return NULL;

				return this;
			}

			SPoint CBase::localPosToCanvas(const SPoint& pnt)
			{
				if (m_parent)
				{
					float x = pnt.X + X();
					float y = pnt.Y + Y();

					return m_parent->localPosToCanvas(SPoint(x, y));
				}

				return pnt;
			}

			SPoint CBase::canvasPosToLocal(const SPoint& pnt)
			{
				if (m_parent)
				{
					float x = pnt.X - X();
					float y = pnt.Y - Y();

					return m_parent->canvasPosToLocal(SPoint(x, y));
				}

				return pnt;
			}

			void CBase::dock(EPosition dock)
			{
				if (m_dock == dock)
					return;

				m_dock = dock;

				invalidate();
				invalidateParent();
			}

			void CBase::recurseLayout()
			{
				if (isHidden())
					return;

				if (m_needsLayout)
				{
					m_needsLayout = false;
					layout();
				}

				SRect rBounds = getRenderBounds();

				// Adjust bounds for padding
				rBounds.X = rBounds.X + m_padding.Left;
				rBounds.Width = rBounds.Width - m_padding.Left + m_padding.Right;
				rBounds.Y = rBounds.Y + m_padding.Top;
				rBounds.Height = rBounds.Height - m_padding.Top + m_padding.Bottom;

				for (auto&& child : Children)
				{
					if (child->isHidden())
						continue;

					EPosition dock = child->m_dock;

					if (dock & EPosition::Fill)
						continue;

					if (dock & EPosition::Top)
					{
						const SMargin& margin = child->getMargin();

						child->setBounds(
							rBounds.X + margin.Left,
							rBounds.Y + margin.Top,
							rBounds.Width - margin.Left - margin.Right,
							child->height()
						);

						float iHeight = margin.Top + margin.Bottom + child->height();

						rBounds.Y = rBounds.Y + iHeight;
						rBounds.Height = rBounds.Height - iHeight;
					}

					if (dock & EPosition::Left)
					{
						const SMargin& margin = child->getMargin();

						child->setBounds(
							rBounds.X + margin.Left,
							rBounds.Y + margin.Top,
							child->width(),
							rBounds.Height - margin.Top - margin.Bottom
						);

						float iWidth = margin.Left + margin.Right + child->width();
						rBounds.X = rBounds.X + iWidth;
						rBounds.Width = rBounds.Width - iWidth;
					}

					if (dock & EPosition::Right)
					{
						// TODO: THIS MARGIN CODE MIGHT NOT BE FULLY FUNCTIONAL
						const SMargin& margin = child->getMargin();

						child->setBounds(
							(rBounds.X + rBounds.Width) - child->width() - margin.Right,
							rBounds.Y + margin.Top,
							child->width(),
							rBounds.Height - margin.Top - margin.Bottom
						);

						float iWidth = margin.Left + margin.Right + child->width();
						rBounds.Width = rBounds.Width - iWidth;
					}

					if (dock & EPosition::Bottom)
					{
						// TODO: THIS MARGIN CODE MIGHT NOT BE FULLY FUNCTIONAL
						const SMargin& margin = child->getMargin();

						child->setBounds(
							rBounds.X + margin.Left,
							(rBounds.Y + rBounds.Height) - child->height() - margin.Bottom,
							rBounds.Width - margin.Left - margin.Right,
							child->height()
						);

						rBounds.Height = rBounds.Height - child->height() + margin.Bottom + margin.Top;
					}

					child->recurseLayout();
				}

				m_innerBounds = rBounds;

				//
				// Fill uses the left over space, so do that now.
				//
				for (auto&& child : Children)
				{
					EPosition dock = child->getDock();

					if (dock & EPosition::Fill)
					{
						const SMargin& margin = child->getMargin();

						child->setBounds(
							rBounds.X + margin.Left,
							rBounds.Y + margin.Top,
							rBounds.Width - margin.Left - margin.Right,
							rBounds.Height - margin.Top - margin.Bottom
						);

						child->recurseLayout();
					}
				}

				postLayout();

				/*
				if (isTabable() && !isDisabled())
				{
					if (!getCanvas()->FirstTab)
						getCanvas()->FirstTab = this;

					if (!getCanvas()->NextTab)
						getCanvas()->NextTab = this;
				}
				*/

				// if (CGUIContext::KeyboardFocus == this)
				//	getCanvas()->NextTab = nullptr;
			}

			void CBase::layout()
			{

			}
		}
	}
}