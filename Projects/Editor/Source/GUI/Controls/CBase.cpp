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
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CBase::CBase(CBase* parent, const std::string& name) :
				m_parent(NULL),
				m_innerPanel(NULL),
				m_name(name),
				m_disabled(false),
				m_hidden(false),
				m_needsLayout(true),
				m_dock(EPosition::None),
				m_bounds(0.0f, 0.0f, 0.0f, 0.0f),
				m_padding(0.0f, 0.0f, 0.0f, 0.0f),
				m_margin(0.0f, 0.0f, 0.0f, 0.0f),
				m_transparentMouseInput(false),
				m_mouseInputEnabled(true),
				m_keyboardInputEnabled(false),
				m_shouldClip(false),
				m_cursor(ECursorType::Normal),
				m_renderFillRect(false),
				m_fillRectColor(CThemeConfig::WindowInnerColor),
				m_accelOnlyFocus(true),
				m_keyboardFocus(false),
				m_debugValue(0),
				m_tagInt(0),
				m_tagFloat(0.0f),
				m_tagBool(false),
				m_tagData(NULL),
				m_isTabable(false)
			{
				if (parent != NULL)
					setParent(parent->getInnerPanel());
			}

			CBase::~CBase()
			{
				if (OnDestroy != nullptr)
					OnDestroy(this);

				CCanvas* canvas = getCanvas();
				if (canvas != NULL)
					canvas->removeDelayDelete(this);

				releaseChildren();

				setParent(NULL);

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
			}

			void CBase::remove()
			{
				setHidden(true);
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

			void CBase::setDisabled(bool disable)
			{
				if (m_disabled == disable)
					return;

				m_disabled = disable;
			}

			void CBase::updateCursor()
			{
				if (m_disabled)
					return;

				CInput::getInput()->setCursor(m_cursor);
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
			}

			void CBase::bringNextToControl(CBase* child, bool behind)
			{
				if (!m_parent)
					return;

				m_parent->Children.remove(this);
				List::iterator it = std::find(m_parent->Children.begin(), m_parent->Children.end(), child);

				if (it == m_parent->Children.end())
					return bringToFront();

				if (behind)
				{
					++it;
					if (it == m_parent->Children.end())
						return bringToFront();
				}

				m_parent->Children.insert(it, this);
				invalidateParent();
			}

			void CBase::bringSwapChildControl(CBase* control1, CBase* control2)
			{
				List::iterator begin = Children.begin(), end = Children.end();
				List::iterator it1 = std::find(begin, end, control1);
				List::iterator it2 = std::find(begin, end, control2);

				if (it1 != end && it2 != end)
				{
					std::swap(*it1, *it2);
					invalidate();
				}
			}

			CBase* CBase::findChildByName(const std::string& name, bool recursive)
			{
				for (auto&& child : Children)
				{
					if (!child->getName().empty() && child->getName() == name)
						return child;

					if (recursive)
					{
						CBase* subChild = child->findChildByName(name, true);

						if (subChild)
							return subChild;
					}
				}

				return nullptr;
			}

			bool CBase::isChild(CBase* child, bool recursive)
			{
				for (auto&& c : Children)
				{
					if (c == child)
						return true;

					if (recursive)
					{
						bool ret = c->isChild(child, true);
						if (ret == true)
							return true;
					}
				}

				return false;
			}

			u32 CBase::numChildren()
			{
				return (u32)Children.size();
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

			SDimension CBase::getChildrenSize()
			{
				SDimension size;

				for (auto&& child : Children)
				{
					if (child->isHidden())
						continue;

					size.Width = core::max_<float>(size.Width, child->right());
					size.Height = core::max_<float>(size.Height, child->bottom());
				}

				return size;
			}

			void CBase::sizeToChildren(bool w, bool h)
			{
				SDimension size = getChildrenSize();
				size.Height = size.Height + m_padding.Bottom;
				size.Width = size.Width - m_padding.Right;
				setSize(w ? size.Width : width(), h ? size.Height : height());
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

			void CBase::onBoundsChanged(const SRect& oldBounds)
			{
				if (m_parent)
					m_parent->onChildBoundsChanged(oldBounds, this);

				invalidate();
				updateRenderBounds();

				if (OnResize != nullptr)
					OnResize(this);
			}

			void CBase::onChildBoundsChanged(const SRect& oldChildBounds, CBase* child)
			{
				if (m_parent)
					m_parent->onChildBoundsChanged(oldChildBounds, child);
			}

			void CBase::updateRenderBounds()
			{
				m_renderBounds.X = 0;
				m_renderBounds.Y = 0;
				m_renderBounds.Width = m_bounds.Width;
				m_renderBounds.Height = m_bounds.Height;
			}

			void CBase::dragTo(float x, float y, float dragPosX, float dragPosY, float paddingBottom)
			{
				float testX = x + dragPosX;
				float testY = y;

				if (m_parent)
				{
					if (testX - m_padding.Left < m_parent->m_margin.Left)
						testX = m_parent->m_margin.Left + m_padding.Left;

					if (testY - m_padding.Top < m_parent->m_margin.Top)
						testY = m_parent->m_margin.Top + m_padding.Top;

					if (testX + m_padding.Right > m_parent->width() - m_parent->m_margin.Right)
						testX = m_parent->width() - m_parent->m_margin.Right - m_padding.Right;

					if (testY + m_padding.Bottom > m_parent->height() - m_parent->m_margin.Bottom - paddingBottom)
						testY = m_parent->height() - m_parent->m_margin.Bottom - m_padding.Bottom - paddingBottom;
				}

				x = testX - dragPosX;
				y = testY;

				setBounds(x, y, width(), height());
			}

			void CBase::moveTo(float x, float y)
			{
				if (m_parent && m_parent != CGUIContext::getRoot())
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

			void CBase::doRenderOverlay()
			{
				renderOverlay();
			}

			void CBase::renderRecursive(const SRect& cliprect)
			{
				CRenderer* render = CRenderer::getRenderer();

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
				if (shouldClip())
				{
					render->startClip();
				}

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

				if (shouldClip())
				{
					render->endClip();
				}

				// Render overlay/focus
				{
					render->setClipRegion(oldRegion);
					if (shouldClip())
					{
						render->startClip();
					}

					{
						renderFocus();
					}

					if (shouldClip())
					{
						render->endClip();
					}

					render->setRenderOffset(oldRenderOffset);
				}
			}

			void CBase::renderOverlay()
			{
				renderOver();

				if (!Children.empty())
				{
					// Now render my kids
					for (auto&& child : Children)
					{
						if (child->isHidden())
							continue;

						child->doRenderOverlay();
					}
				}
			}

			void CBase::render()
			{
				if (m_renderFillRect)
				{
					CRenderer::getRenderer()->drawFillRect(getRenderBounds(), m_fillRectColor);
				}

				if (OnRender != nullptr)
					OnRender(this);
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
			}

			void CBase::unfocus()
			{
				if (CGUIContext::KeyboardFocus != this)
					return;

				CGUIContext::KeyboardFocus = NULL;
				onLostKeyboardFocus();
			}

			void CBase::onMouseEnter()
			{
				if (OnHoverEnter != nullptr)
					OnHoverEnter(this);
			}

			void CBase::onMouseLeave()
			{
				if (OnHoverLeave != nullptr)
					OnHoverLeave(this);
			}

			bool CBase::onMouseWheeled(int delta)
			{
				if (OnMouseWheeled != nullptr)
					OnMouseWheeled(this, delta);

				if (m_parent)
					return m_parent->onMouseWheeled(delta);

				return false;
			}

			bool CBase::onKeyPress(EKey key, bool press)
			{
				bool handled = false;

				switch (key)
				{
				case EKey::KEY_TAB:
					handled = onKeyTab(press);
					break;

				case EKey::KEY_SPACE:
					handled = onKeySpace(press);
					break;

				case EKey::KEY_HOME:
					handled = onKeyHome(press);
					break;

				case EKey::KEY_END:
					handled = onKeyEnd(press);
					break;

				case EKey::KEY_RETURN:
					handled = onKeyReturn(press);
					break;

				case EKey::KEY_BACK:
					handled = onKeyBackspace(press);
					break;

				case EKey::KEY_DELETE:
					handled = onKeyDelete(press);
					break;

				case EKey::KEY_RIGHT:
					handled = onKeyRight(press);
					break;

				case EKey::KEY_LEFT:
					handled = onKeyLeft(press);
					break;

				case EKey::KEY_UP:
					handled = onKeyUp(press);
					break;

				case EKey::KEY_DOWN:
					handled = onKeyDown(press);
					break;

				case EKey::KEY_ESCAPE:
					handled = onKeyEscape(press);
					break;

				default:
					break;
				}

				if (!handled && m_parent)
					m_parent->onKeyPress(key, press);

				if (OnKeyPress != nullptr)
					OnKeyPress(this, key, press);

				return handled;
			}

			bool CBase::onKeyRelease(EKey key)
			{
				return onKeyPress(key, false);
			}

			bool CBase::onKeyTab(bool down)
			{
				if (!down)
					return true;

				CTabableGroup& tabableGroup = getCanvas()->TabableGroup;
				if (tabableGroup.CurrentTab == this)
				{
					tabableGroup.next();

					if (tabableGroup.CurrentTab != NULL)
					{
						tabableGroup.CurrentTab->focus();
						tabableGroup.CurrentTab->onTabableFocus();

						return true;
					}
				}

				return false;
			}

			void CBase::setCenterPosition()
			{
				if (m_parent)
				{
					float x = floorf(m_parent->width() * 0.5f - width() * 0.5f);
					float y = floorf(m_parent->height() * 0.5f - height() * 0.5f);

					setPos(x, y);
				}
			}

			CBase* CBase::getControlAt(float x, float y, bool onlyIfMouseEnabled)
			{
				if (isHidden())
					return NULL;

				if (m_transparentMouseInput == false)
				{
					if (x < 0 || y < 0 || x >= width() || y >= height())
						return NULL;
				}

				for (List::reverse_iterator iter = Children.rbegin(); iter != Children.rend(); ++iter)
				{
					CBase* child = *iter;
					CBase* found = NULL;

					found = child->getControlAt(x - child->X(), y - child->Y(), onlyIfMouseEnabled);

					if (found)
						return found;
				}

				if (onlyIfMouseEnabled && !getMouseInputEnabled())
					return NULL;

				if (m_transparentMouseInput)
					return NULL;

				return this;
			}

			CBase* CBase::getControlAt(float x, float y, const std::type_info& type, bool onlyIfMouseEnabled)
			{
				if (isHidden())
					return NULL;

				if (m_transparentMouseInput == false)
				{
					if (x < 0 || y < 0 || x >= width() || y >= height())
						return NULL;
				}

				for (List::reverse_iterator iter = Children.rbegin(); iter != Children.rend(); ++iter)
				{
					CBase* child = *iter;
					CBase* found = NULL;
					found = child->getControlAt(x - child->X(), y - child->Y(), type, onlyIfMouseEnabled);

					if (found)
						return found;
				}

				if (onlyIfMouseEnabled && !getMouseInputEnabled())
					return NULL;

				if (m_transparentMouseInput)
					return NULL;

				if (typeid(*this) == type)
				{
					return this;
				}

				return NULL;
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
				rBounds.Width = rBounds.Width - m_padding.Left - m_padding.Right;
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

				// Fill uses the left over space, so do that now.
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

				if (m_isTabable)
				{
					getCanvas()->TabableGroup.setEnableControl(this);

					if (CGUIContext::KeyboardFocus == this)
						getCanvas()->TabableGroup.CurrentTab = this;
				}
			}

			void CBase::layout()
			{

			}

			bool CBase::isMenuComponent()
			{
				if (!m_parent)
					return false;

				return m_parent->isMenuComponent();
			}

			void CBase::addAccelerator(const std::string& accelerator, const Listener& function)
			{
				std::string upper = accelerator;
				std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
				m_accelerators[upper] = function;
			}

			bool CBase::handleAccelerator(const std::string& accelerator)
			{
				if (CGUIContext::KeyboardFocus == this || !accelOnlyFocus())
				{
					AccelMap::iterator iter = m_accelerators.find(accelerator);

					if (iter != m_accelerators.end())
					{
						iter->second(this);
						return true;
					}
				}

				for (auto&& child : Children)
				{
					if (child->handleAccelerator(accelerator))
						return true;
				}

				return false;
			}
		}
	}
}