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

#include "Graphics2D/EntityData/CGUITransformData.h"
#include "Graphics2D/EntityData/CGUIAlignData.h"
#include "Graphics2D/EntityData/CGUIRenderData.h"
#include "Graphics2D/EntityData/CGUILayoutData.h"

namespace Skylicht
{
	class CCanvas;
	class CGUIMask;

	class CGUIElement
	{
		friend class CCanvas;

	protected:
		CGUIElement* m_parent;
		std::vector<CGUIElement*> m_childs;

		CCanvas* m_canvas;

		CGUIMask* m_mask;
		CGUIMask* m_applyCurrentMask;

		bool m_drawBorder;
		bool m_visible;

		CEntity* m_entity;

		CWorldTransformData* m_transform;
		CGUITransformData* m_guiTransform;
		CGUIAlignData* m_guiAlign;
		CGUIRenderData* m_renderData;

		std::string m_materialFile;

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

		inline void setName(const char* name)
		{
			m_transform->Name = name;
		}

		inline const char* getName()
		{
			return m_transform->Name.c_str();
		}

		inline const char* getID()
		{
			return m_entity->getID().c_str();
		}

		void setName(const wchar_t* name);

		std::wstring getNameW();

		void setParent(CGUIElement* parent);

		inline std::vector<CGUIElement*>& getChilds()
		{
			return m_childs;
		}

		inline void setColor(const SColor& c)
		{
			m_renderData->Color = c;
		}

		const SColor& getColor()
		{
			return m_renderData->Color;
		}

		inline float getHeight()
		{
			return m_guiTransform->getHeight();
		}

		inline float getWidth()
		{
			return m_guiTransform->getWidth();
		}

		inline const core::rectf& getRect()
		{
			return m_guiTransform->getRect();
		}

		inline int getDepth()
		{
			return m_transform->Depth;
		}

		inline void setDock(EGUIDock dock)
		{
			m_guiAlign->Dock = dock;
		}

		inline EGUIDock getDock()
		{
			return m_guiAlign->Dock;
		}

		inline const SMargin& getMargin()
		{
			return m_guiAlign->Margin;
		}

		inline void setMargin(const SMargin& m)
		{
			m_guiAlign->Margin = m;
		}

		inline void setMargin(float l, float t, float r, float b)
		{
			m_guiAlign->Margin.Left = l;
			m_guiAlign->Margin.Top = t;
			m_guiAlign->Margin.Right = r;
			m_guiAlign->Margin.Bottom = b;
		}

		inline void setRect(const core::rectf& r)
		{
			m_guiTransform->setRect(r);
		}

		void setWorldTransform(const core::matrix4& world);

		void setRelativeTransform(const core::matrix4& relative);

		inline const core::vector3df getAlignPosition()
		{
			return m_guiTransform->getAlignPosition();
		}

		inline const core::vector3df& getPosition()
		{
			return m_guiTransform->getPosition();
		}

		inline void invalidate()
		{
			m_transform->HasChanged = true;
			m_guiTransform->HasChanged = true;
		}

		inline void setPosition(const core::vector3df& v)
		{
			m_guiTransform->setPosition(v);
		}

		inline const core::vector3df& getScale()
		{
			return m_guiTransform->getScale();
		}

		inline void setScale(const core::vector3df& v)
		{
			m_guiTransform->setScale(v);
		}

		inline const core::vector3df& getRotation()
		{
			return m_guiTransform->getRotation();
		}

		inline void setRotation(const core::vector3df& v)
		{
			m_guiTransform->setRotation(v);
		}

		inline core::quaternion getRotationQuaternion()
		{
			return core::quaternion(getRotation() * core::DEGTORAD);
		}

		inline EGUIVerticalAlign getVerticalAlign()
		{
			return m_guiAlign->Vertical;
		}

		inline EGUIHorizontalAlign getHorizontalAlign()
		{
			return m_guiAlign->Horizontal;
		}

		inline void setVerticalAlign(EGUIVerticalAlign a)
		{
			m_guiAlign->Vertical = a;
		}

		inline void setHorizontalAlign(EGUIHorizontalAlign a)
		{
			m_guiAlign->Horizontal = a;
		}

		inline void setAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			m_guiAlign->Vertical = v;
			m_guiAlign->Horizontal = h;
		}

		inline void setShaderID(int id)
		{
			m_renderData->ShaderID = id;
		}

		inline int getShaderID()
		{
			return m_renderData->ShaderID;
		}

		inline void setMaterial(CMaterial* material)
		{
			m_renderData->Material = material;
		}

		inline CMaterial* getMaterial()
		{
			return m_renderData->Material;
		}

		inline CGUIMask* getMask()
		{
			return m_mask;
		}

		inline void setMask(CGUIMask* mask)
		{
			m_mask = mask;
		}

		inline void setDrawBorder(bool b)
		{
			m_drawBorder = b;
		}

		virtual void update(CCamera* camera);

		virtual void render(CCamera* camera);

		const core::matrix4& getRelativeTransform()
		{
			return m_transform->Relative;
		}

		const core::matrix4& getAbsoluteTransform()
		{
			return m_transform->World;
		}
		inline void setVisible(bool b)
		{
			m_visible = b;
		}

		inline bool isVisible()
		{
			return m_visible;
		}

		void notifyChanged();

		void bringToNext(CGUIElement* object, CGUIElement* target, bool behind);

		void bringToChild(CGUIElement* object);

		bool isChild(CGUIElement* e);

		DECLARE_GETTYPENAME(CGUIElement);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

	protected:

		bool removeChild(CGUIElement* child);
	};
}