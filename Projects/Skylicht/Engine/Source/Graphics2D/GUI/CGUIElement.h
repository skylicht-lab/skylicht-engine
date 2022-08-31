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

#include "Camera/CCamera.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class CCanvas;
	class CGUIMask;

	class CGUIElement
	{
		friend class CCanvas;

	public:
		enum EGUIVerticalAlign
		{
			Top,
			Middle,
			Bottom
		};

		enum EGUIHorizontalAlign
		{
			Left,
			Center,
			Right
		};

		enum EGUIDock
		{
			NoDock,
			DockLeft,
			DockRight,
			DockTop,
			DockBottom,
			DockFill
		};

		struct SRect
		{
			float Left;
			float Top;
			float Right;
			float Bottom;

			SRect()
			{
				Left = 0.0f;
				Top = 0.0f;
				Right = 0.0f;
				Bottom = 0.0f;
			}
		};

	protected:
		int m_level;

		CGUIElement* m_parent;
		std::vector<CGUIElement*> m_childs;

		CCanvas* m_canvas;

		CGUIMask* m_mask;
		CGUIMask* m_applyCurrentMask;

		core::rectf m_rect;

		EGUIDock m_dock;
		SRect m_margin;

		bool m_visible;

		core::vector3df m_transformPosition;

		core::vector3df	m_position;
		core::vector3df	m_scale;
		core::vector3df	m_rotation;

		core::matrix4 m_relativeTransform;
		core::matrix4 m_absoluteTransform;

		EGUIVerticalAlign m_vertical;
		EGUIHorizontalAlign m_horizontal;

		bool m_cullingVisible;

		SColor m_color;
		int m_shaderID;
		CMaterial* m_material;

	protected:
		CGUIElement(CCanvas* canvas, CGUIElement* parent);
		CGUIElement(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

		inline void applyCurrentMask(CGUIMask* mask)
		{
			m_applyCurrentMask = mask;
		}

		inline CGUIMask* getCurrentMask()
		{
			return m_applyCurrentMask;
		}

	public:
		virtual ~CGUIElement();

		void remove();

		void removeAllChilds();

	public:

		inline CCanvas* getCanvas()
		{
			return m_canvas;
		}

		inline CGUIElement* getParent()
		{
			return m_parent;
		}

		void setParent(CGUIElement* parent);

		inline std::vector<CGUIElement*>& getChilds()
		{
			return m_childs;
		}

		inline int getLevel()
		{
			return m_level;
		}

		inline void setColor(const SColor& c)
		{
			m_color = c;
		}

		const SColor& getColor()
		{
			return m_color;
		}

		inline float getHeight()
		{
			return m_rect.getHeight();
		}

		inline float getWidth()
		{
			return m_rect.getWidth();
		}

		inline void setDock(EGUIDock dock)
		{
			m_dock = dock;
		}

		inline EGUIDock getDock()
		{
			return m_dock;
		}

		inline core::rectf& getRect()
		{
			return m_rect;
		}

		inline const SRect& getMargin()
		{
			return m_margin;
		}

		inline void setMargin(const SRect& m)
		{
			m_margin = m;
		}

		inline void setMargin(float l, float t, float r, float b)
		{
			m_margin.Left = l;
			m_margin.Top = t;
			m_margin.Right = r;
			m_margin.Bottom = b;
		}

		inline void setRect(const core::rectf& r)
		{
			m_rect = r;
		}

		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		inline void setPosition(const core::vector3df& v)
		{
			m_position = v;
		}

		inline const core::vector3df& getScale()
		{
			return m_scale;
		}

		inline void setScale(const core::vector3df& v)
		{
			m_scale = v;
		}

		inline const core::vector3df& getRotation()
		{
			return m_rotation;
		}

		inline void setRotation(const core::vector3df& v)
		{
			m_rotation = v;
		}

		inline EGUIVerticalAlign getVerticalAlign()
		{
			return m_vertical;
		}

		inline EGUIHorizontalAlign getHorizontalAlign()
		{
			return m_horizontal;
		}

		inline void setVerticalAlign(EGUIVerticalAlign a)
		{
			m_vertical = a;
		}

		inline void setHorizontalAlign(EGUIHorizontalAlign a)
		{
			m_horizontal = a;
		}

		inline void setAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			m_vertical = v;
			m_horizontal = h;
		}

		inline void setCullingVisisble(bool b)
		{
			m_cullingVisible = b;
		}

		inline bool isCullingVisisble()
		{
			return m_cullingVisible;
		}

		inline void setShaderID(int id)
		{
			m_shaderID = id;
		}

		inline int getShaderID()
		{
			return m_shaderID;
		}

		inline void setMaterial(CMaterial* material)
		{
			m_material = material;
		}

		inline CMaterial* getMaterial()
		{
			return m_material;
		}

		inline CGUIMask* getMask()
		{
			return m_mask;
		}

		inline void setMask(CGUIMask* mask)
		{
			m_mask = mask;
		}

		virtual void update(CCamera* camera);

		virtual void render(CCamera* camera);

		virtual void layout(const core::rectf& parentRect);

		const core::matrix4& getRelativeTransform(bool forceRecalc = false);

		const core::matrix4& getAbsoluteTransform()
		{
			return m_absoluteTransform;
		}

		void calcAbsoluteTransform();

		inline void setVisible(bool b)
		{
			m_visible = b;
		}

		inline bool isVisible()
		{
			return m_visible;
		}

	protected:

		void layoutNoDock(const core::rectf& parentRect);

		void layoutDock(const core::rectf& parentRect);

		bool removeChild(CGUIElement* child);
	};
}