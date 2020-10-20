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

#include <functional>
#include "Type.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CBase
			{
			public:
				typedef std::list<CBase*> List;
				typedef std::function<void(void*)> Listener;
				typedef std::map<std::string, Listener> AccelMap;

				CBase(CBase* parent, const std::string& name = "");
				virtual ~CBase();

			public:
				virtual void setHidden(bool hidden);

				//!< Returns true only if this control is hidden.
				virtual bool isHidden() const;

				//!< Returns false if this control or its parents are hidden.
				virtual bool isVisible() const;

				virtual void hide() { setHidden(true); }

				virtual void show() { setHidden(false); }

				virtual void setDisabled(bool active);

				virtual bool isDisabled()
				{
					return m_disabled;
				}

				virtual void reDraw();

				const std::string& getName()
				{
					return m_name;
				}

			protected:

				virtual void addChild(CBase* child);
				virtual void removeChild(CBase* child);
				virtual void onChildAdded(CBase* child)
				{
					invalidate();
				}

				virtual void onChildRemoved(CBase* child)
				{
					invalidate();
				}

				virtual void onBoundsChanged(const core::rectf oldBounds);
				virtual void onChildBoundsChanged(const core::rectf oldChildBounds, CBase* child);

			public:
				List Children;

				virtual void removeAllChildren();
				virtual void sendToBack(void);
				virtual void bringToFront(void);
				virtual void bringNextToControl(CBase* child, bool bBehind);

				virtual CBase* findChildByName(const std::string& name, bool bRecursive);

				virtual bool isChild(CBase* child);

				virtual u32 numChildren();
				virtual CBase* getChild(u32 i);

				virtual void setParent(CBase* parent);
				virtual CBase* getParent() const
				{
					return m_parent;
				}

				virtual const List& getChildren()
				{
					return Children;
				}

			public:
				virtual void updateColors() {}

				virtual void invalidate()
				{
					m_needsLayout = true;
				}

				void invalidateParent()
				{
					if (m_parent)
						m_parent->invalidate();
				}

			public:

				inline float X() const { return m_bounds.UpperLeftCorner.X; }
				inline float Y() const { return m_bounds.UpperLeftCorner.Y; }

				inline float width() const { return m_bounds.getWidth(); }
				inline float height() const { return m_bounds.getHeight(); }

				inline const SMargin& getMargin() const { return m_margin; }
				inline const SPadding& getPadding() const { return m_padding; }

				inline void setPos(float x, float y) { setBounds(x, y, width(), height()); }
				inline void setPos(const core::vector2df& p) { return setPos(p.X, p.Y); }
				inline core::vector2df getPos() const { return core::vector2df(X(), Y()); }

				inline void setWidth(float w) { setSize(w, height()); }
				inline void setHeight(float h) { setSize(width(), h); }

				inline bool setSize(float w, float h) { setBounds(X(), Y(), w, h); }
				inline bool setSize(const core::dimension2df& p) { setSize(p.Width, p.Height); }
				inline core::dimension2df GetSize() const { return core::dimension2df(width(), height()); }

				inline bool setBounds(float x, float y, float w, float h) { return setBounds(core::rectf(x, y, x + w, y + h)); }
				virtual bool setBounds(const core::rectf& bounds);

				inline void setPadding(const SPadding& padding) { m_padding = padding; }
				inline void setMargin(const SMargin& margin) { m_margin = margin; }

				virtual void moveTo(float x, float y);
				inline void moveBy(float x, float y) { moveTo(X() + x, Y() + y); }

				inline const core::rectf& getBounds() const { return m_bounds; }

				virtual void updateRenderBounds();

			public:

				virtual void think() {}

				virtual void doRender();
				virtual void renderRecursive(const core::rectf& cliprect);

				virtual bool shouldClip() { return true; }

			protected:

				virtual void render();
				virtual void renderFocus();
				virtual void renderUnder() {}
				virtual void renderOver() {}

			protected:
				CBase *m_parent;

				std::string m_name;

				core::rectf m_bounds;
				core::rectf m_renderBounds;

				SPadding m_padding;
				SMargin m_margin;

				bool m_disabled;
				bool m_hidden;
				bool m_needsLayout;

				EPosition m_dock;
			};
		}
	}
}