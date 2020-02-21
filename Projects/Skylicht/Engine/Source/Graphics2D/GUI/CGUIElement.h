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

namespace Skylicht
{
	class CCanvas;

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

	protected:
		int m_level;

		CGUIElement* m_parent;
		CCanvas *m_canvas;

		core::rectf m_rect;

		bool m_visible;
		bool m_transformChanged;

		core::vector3df	m_position;
		core::vector3df	m_scale;
		core::vector3df	m_rotation;

		core::matrix4 m_relativeTransform;
		core::matrix4 m_absoluteTransform;

		EGUIVerticalAlign m_vertical;
		EGUIHorizontalAlign m_horizontal;

		bool m_cullingVisible;

	protected:
		CGUIElement(CCanvas *canvas, const core::rectf& rect);
		CGUIElement(CCanvas *canvas, CGUIElement *parent);
		CGUIElement(CCanvas *canvas, CGUIElement *parent, const core::rectf& rect);

	public:
		virtual ~CGUIElement();

		void remove();

	public:

		inline CGUIElement* getParent()
		{
			return m_parent;
		}

		inline int getLevel()
		{
			return m_level;
		}

		inline core::rectf& getRect()
		{
			return m_rect;
		}

		inline bool isTransformChanged()
		{
			return m_transformChanged;
		}

		inline void setTransformChanged(bool b)
		{
			m_transformChanged = b;
		}

		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		inline void setPosition(const core::vector3df& v)
		{
			m_position = v;
			m_transformChanged = true;
		}

		inline const core::vector3df& getScale()
		{
			return m_scale;
		}

		inline void setScale(const core::vector3df& v)
		{
			m_scale = v;
			m_transformChanged = true;
		}

		inline const core::vector3df& getRotation()
		{
			return m_rotation;
		}

		inline void setRotation(const core::vector3df& v)
		{
			m_rotation = v;
			m_transformChanged = true;
		}

		inline EGUIVerticalAlign getVerticlaAlign()
		{
			return m_vertical;
		}

		inline EGUIHorizontalAlign getHorizontalAlign()
		{
			return m_horizontal;
		}

		inline void setVerticlaAlign(EGUIVerticalAlign a)
		{
			m_vertical = a;
		}

		inline void setHorizontalAlign(EGUIHorizontalAlign a)
		{
			m_horizontal = a;
		}

		inline void setCullingVisisble(bool b)
		{
			m_cullingVisible = b;
		}

		inline bool isCullingVisisble()
		{
			return m_cullingVisible;
		}

		virtual void render(CCamera *camera);

		const core::matrix4& getRelativeTransform();

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
	};
}