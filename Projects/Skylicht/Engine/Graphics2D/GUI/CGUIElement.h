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

	/**
	 * @brief This is the base object class from which other GUIs inherit. It's an empty GUI and can contain child GUIs in a tree structure.
	 * @ingroup GUI
	 *
	 * @code
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * CGUIElement* gui = canvas->createElement(r);
	 * @endcode
	 */
	class SKYLICHT_API CGUIElement
	{
		friend class CCanvas;

	protected:
		CGUIElement* m_parent;
		std::vector<CGUIElement*> m_childs;

		CCanvas* m_canvas;

		CGUIMask* m_mask;
		CGUIMask* m_applyCurrentMask;

		bool m_drawBorder;
		bool m_enableMaterial;

		int m_renderOrder;

		ArrayMaterial m_materials;

		int m_materialId;

		CEntity* m_entity;

		CWorldTransformData* m_transform;
		CGUITransformData* m_guiTransform;
		CGUIAlignData* m_guiAlign;
		CGUIRenderData* m_renderData;

		std::string m_materialFile;

		int m_tagInt;
		std::string m_tagString;

		std::string m_maskId;

	public:

		std::function<void(CGUIElement*)> OnRender;

	protected:
		/**
		 * @brief Construct a GUI element for a canvas and optional parent.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 */
		CGUIElement(CCanvas* canvas, CGUIElement* parent);

		/**
		 * @brief Construct a GUI element with an initial rectangle.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 */
		CGUIElement(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

		/**
		 * @brief Store the mask currently active during hierarchy rendering.
		 * @param mask Active mask pointer.
		 */
		inline void applyCurrentMask(CGUIMask* mask)
		{
			m_applyCurrentMask = mask;
		}

		/**
		 * @brief Get the mask currently active during hierarchy rendering.
		 * @return Active mask pointer.
		 */
		inline CGUIMask* getCurrentMask()
		{
			return m_applyCurrentMask;
		}

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIElement();

		/**
		 * @brief Remove this GUI element from its canvas and parent hierarchy.
		 */
		void remove();

		/**
		 * @brief Remove all child GUI elements owned by this element.
		 */
		void removeAllChilds();

	public:

		/**
		 * @brief Get the canvas that owns this GUI element.
		 * @return Pointer to the owner canvas.
		 */
		inline CCanvas* getCanvas()
		{
			return m_canvas;
		}

		/**
		 * @brief Get the parent GUI element.
		 * @return Pointer to the parent element, or NULL if this is a root element.
		 */
		inline CGUIElement* getParent()
		{
			return m_parent;
		}

		/**
		 * @brief Set the element name.
		 * @param name UTF-8 element name.
		 */
		inline void setName(const char* name)
		{
			m_transform->Name = name;
		}

		/**
		 * @brief Get the element name.
		 * @return UTF-8 element name.
		 */
		inline const char* getName()
		{
			return m_transform->Name.c_str();
		}

		/**
		 * @brief Get the unique entity ID associated with this element.
		 * @return Entity ID string.
		 */
		inline const char* getID()
		{
			return m_entity->getID().c_str();
		}

		/**
		 * @brief Store an integer tag on this element.
		 * @param i Tag value.
		 */
		inline void setTagInt(int i)
		{
			m_tagInt = i;
		}

		/**
		 * @brief Store a string tag on this element.
		 * @param s Tag string.
		 */
		inline void setTagString(const char* s)
		{
			m_tagString = s;
		}

		/**
		 * @brief Get the integer tag value.
		 * @return Stored integer tag.
		 */
		inline int getTagInt()
		{
			return m_tagInt;
		}

		/**
		 * @brief Get the string tag value.
		 * @return Stored string tag.
		 */
		inline const std::string& getTagString()
		{
			return m_tagString;
		}

		/**
		 * @brief Set the element name from a wide string.
		 * @param name Wide string element name.
		 */
		void setName(const wchar_t* name);

		/**
		 * @brief Get the element name as a wide string.
		 * @return Wide string element name.
		 */
		std::wstring getNameW();

		/**
		 * @brief Get the hierarchical path of this element.
		 * @return Path built from parent names to this element.
		 */
		std::wstring getPath();

		/**
		 * @brief Reparent this element to another GUI element.
		 * @param parent New parent element.
		 */
		void setParent(CGUIElement* parent);

		/**
		 * @brief Get the direct child list.
		 * @return Mutable list of child elements.
		 */
		inline std::vector<CGUIElement*>& getChilds()
		{
			return m_childs;
		}

		/**
		 * @brief Append this element's descendants to a list.
		 * @param childs Output list receiving all descendant elements.
		 */
		void getAllChilds(std::vector<CGUIElement*>& childs);

		/**
		 * @brief Set the render color tint.
		 * @param c Color applied to this element.
		 */
		inline void setColor(const SColor& c)
		{
			m_renderData->Color = c;
		}

		/**
		 * @brief Get the current render color tint.
		 * @return Current color.
		 */
		const SColor& getColor()
		{
			return m_renderData->getColor();
		}

		/**
		 * @brief Get the GUI rectangle height.
		 * @return Height in local GUI units.
		 */
		inline float getHeight()
		{
			return m_guiTransform->getHeight();
		}

		/**
		 * @brief Get the GUI rectangle width.
		 * @return Width in local GUI units.
		 */
		inline float getWidth()
		{
			return m_guiTransform->getWidth();
		}

		/**
		 * @brief Set the GUI rectangle height.
		 * @param h Height in local GUI units.
		 */
		inline void setHeight(float h)
		{
			m_guiTransform->setHeight(h);
		}

		/**
		 * @brief Set the GUI rectangle width.
		 * @param w Width in local GUI units.
		 */
		inline void setWidth(float w)
		{
			m_guiTransform->setWidth(w);
		}

		/**
		 * @brief Get the GUI rectangle.
		 * @return Rectangle in local GUI coordinates.
		 */
		inline const core::rectf& getRect()
		{
			return m_guiTransform->getRect();
		}

		/**
		 * @brief Get the native content rectangle for this element.
		 * @return Native rectangle in local coordinates.
		 */
		virtual const core::rectf getNativeRect();

		/**
		 * @brief Get the transform depth used for GUI sorting.
		 * @return Depth value.
		 */
		inline int getDepth()
		{
			return m_transform->Depth;
		}

		/**
		 * @brief Get the render order assigned by the canvas.
		 * @return Render order index.
		 */
		inline int getRenderOrder()
		{
			return m_renderOrder;
		}

		/**
		 * @brief Set the docking mode used by layout alignment.
		 * @param dock Docking mode.
		 */
		inline void setDock(EGUIDock dock)
		{
			m_guiAlign->Dock = dock;
		}

		/**
		 * @brief Get the docking mode.
		 * @return Current docking mode.
		 */
		inline EGUIDock getDock()
		{
			return m_guiAlign->Dock;
		}

		/**
		 * @brief Get the element margin used by docking and layout.
		 * @return Current margin.
		 */
		inline const SMargin& getMargin()
		{
			return m_guiAlign->Margin;
		}

		/**
		 * @brief Set the element margin.
		 * @param m Margin values.
		 */
		inline void setMargin(const SMargin& m)
		{
			m_guiAlign->Margin = m;
		}

		/**
		 * @brief Set the element margin.
		 * @param l Left margin.
		 * @param t Top margin.
		 * @param r Right margin.
		 * @param b Bottom margin.
		 */
		inline void setMargin(float l, float t, float r, float b)
		{
			m_guiAlign->Margin.Left = l;
			m_guiAlign->Margin.Top = t;
			m_guiAlign->Margin.Right = r;
			m_guiAlign->Margin.Bottom = b;
		}

		/**
		 * @brief Set the GUI rectangle.
		 * @param r Rectangle in local GUI coordinates.
		 */
		inline void setRect(const core::rectf& r)
		{
			m_guiTransform->setRect(r);
		}

		/**
		 * @brief Set the absolute world transform matrix.
		 * @param world World transform matrix.
		 */
		void setWorldTransform(const core::matrix4& world);

		/**
		 * @brief Set the transform relative to the parent element.
		 * @param relative Relative transform matrix.
		 */
		void setRelativeTransform(const core::matrix4& relative);

		/**
		 * @brief Get the aligned position computed from the current alignment settings.
		 * @return Aligned local position.
		 */
		inline const core::vector3df getAlignPosition()
		{
			return m_guiTransform->getAlignPosition();
		}

		/**
		 * @brief Get the local position.
		 * @return Current local position.
		 */
		inline const core::vector3df& getPosition()
		{
			return m_guiTransform->getPosition();
		}

		/**
		 * @brief Mark this element's transform data as changed.
		 */
		inline void invalidate()
		{
			m_transform->HasChanged = true;
			m_guiTransform->HasChanged = true;
		}

		/**
		 * @brief Set the local position.
		 * @param v Local position.
		 */
		inline void setPosition(const core::vector3df& v)
		{
			m_guiTransform->setPosition(v);
		}

		/**
		 * @brief Get the local scale.
		 * @return Current local scale.
		 */
		inline const core::vector3df& getScale()
		{
			return m_guiTransform->getScale();
		}

		/**
		 * @brief Set the local scale.
		 * @param v Local scale.
		 */
		inline void setScale(const core::vector3df& v)
		{
			m_guiTransform->setScale(v);
		}

		/**
		 * @brief Get the local Euler rotation in degrees.
		 * @return Current local rotation.
		 */
		inline const core::vector3df& getRotation()
		{
			return m_guiTransform->getRotation();
		}

		/**
		 * @brief Set the local Euler rotation in degrees.
		 * @param v Local rotation.
		 */
		inline void setRotation(const core::vector3df& v)
		{
			m_guiTransform->setRotation(v);
		}

		/**
		 * @brief Get the local rotation as a quaternion.
		 * @return Quaternion converted from the Euler rotation.
		 */
		inline core::quaternion getRotationQuaternion()
		{
			return core::quaternion(getRotation() * core::DEGTORAD);
		}

		/**
		 * @brief Get the vertical alignment mode.
		 * @return Current vertical alignment.
		 */
		inline EGUIVerticalAlign getVerticalAlign()
		{
			return m_guiAlign->Vertical;
		}

		/**
		 * @brief Get the horizontal alignment mode.
		 * @return Current horizontal alignment.
		 */
		inline EGUIHorizontalAlign getHorizontalAlign()
		{
			return m_guiAlign->Horizontal;
		}

		/**
		 * @brief Set the vertical alignment mode.
		 * @param a Vertical alignment.
		 */
		inline void setVerticalAlign(EGUIVerticalAlign a)
		{
			m_guiAlign->Vertical = a;
		}

		/**
		 * @brief Set the horizontal alignment mode.
		 * @param a Horizontal alignment.
		 */
		inline void setHorizontalAlign(EGUIHorizontalAlign a)
		{
			m_guiAlign->Horizontal = a;
		}

		/**
		 * @brief Set horizontal and vertical alignment modes.
		 * @param h Horizontal alignment.
		 * @param v Vertical alignment.
		 */
		inline void setAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			m_guiAlign->Vertical = v;
			m_guiAlign->Horizontal = h;
		}

		/**
		 * @brief Set the shader ID used by the render data.
		 * @param id Shader ID.
		 */
		inline void setShaderID(int id)
		{
			m_renderData->ShaderID = id;
		}

		/**
		 * @brief Get the shader ID used by the render data.
		 * @return Shader ID.
		 */
		inline int getShaderID()
		{
			return m_renderData->ShaderID;
		}

		/**
		 * @brief Get the active material slot ID.
		 * @return Material slot ID.
		 */
		inline int getMaterialId()
		{
			return m_materialId;
		}

		/**
		 * @brief Set the active material slot ID.
		 * @param id Material slot ID.
		 */
		void setMaterialId(int id);

		/**
		 * @brief Set the render material directly.
		 * @param material Material instance.
		 */
		inline void setMaterial(CMaterial* material)
		{
			m_renderData->Material = material;
		}

		/**
		 * @brief Load and set the render material from a material file.
		 * @param materialFile Path to the material file.
		 */
		void setMaterialSource(const char* materialFile);

		/**
		 * @brief Get the active render material.
		 * @return Material instance.
		 */
		inline CMaterial* getMaterial()
		{
			return m_renderData->Material;
		}

		/**
		 * @brief Get the mask assigned directly to this element.
		 * @return Mask pointer, or NULL if no mask is assigned.
		 */
		inline CGUIMask* getMask()
		{
			return m_mask;
		}

		/**
		 * @brief Find the nearest mask inherited from this element's parents.
		 * @return Parent mask pointer, or NULL if none exists.
		 */
		CGUIMask* getParentMask();

		/**
		 * @brief Set the serialized mask reference ID.
		 * @param id Mask ID string.
		 */
		void setMaskId(const char* id);

		/**
		 * @brief Assign a mask to clip this element.
		 * @param mask Mask element.
		 */
		inline void setMask(CGUIMask* mask)
		{
			m_mask = mask;
		}

		/**
		 * @brief Enable or disable drawing a debug border for this element.
		 * @param b True to draw the border.
		 */
		inline void setDrawBorder(bool b)
		{
			m_drawBorder = b;
		}

		/**
		 * @brief Check whether material rendering is enabled.
		 * @return True if material rendering is enabled.
		 */
		inline bool isEnableMaterial()
		{
			return m_enableMaterial;
		}

		/**
		 * @brief Update this element before rendering.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void update(CCamera* camera);

		/**
		 * @brief Render this element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Get the relative transform matrix.
		 * @return Transform relative to the parent.
		 */
		const core::matrix4& getRelativeTransform()
		{
			return m_transform->Relative;
		}

		/**
		 * @brief Get the absolute world transform matrix.
		 * @return World transform.
		 */
		const core::matrix4& getAbsoluteTransform()
		{
			return m_transform->World;
		}

		/**
		 * @brief Set this element's visibility flag.
		 * @param b True to make the element visible.
		 */
		inline void setVisible(bool b)
		{
			m_entity->setVisible(b);
		}

		/**
		 * @brief Check this element's own visibility flag.
		 * @return True if the element itself is visible.
		 */
		inline bool isVisible()
		{
			return m_entity->isVisible();
		}

		/**
		 * @brief Check whether this element is visible through its parent hierarchy.
		 * @return True if this element and all ancestors are visible.
		 */
		bool isVisibleInHierarchy();

		/**
		 * @brief Get the entity associated with this GUI element.
		 * @return Entity pointer.
		 */
		inline CEntity* getEntity()
		{
			return m_entity;
		}

		/**
		 * @brief Notify the GUI system that this element has changed.
		 */
		void notifyChanged();

		/**
		 * @brief Get the child immediately before another child.
		 * @param object Child element to search from.
		 * @return Previous child element, or NULL if there is none.
		 */
		CGUIElement* getChildBefore(CGUIElement* object);

		/**
		 * @brief Move an object next to a target child in the hierarchy.
		 * @param object Element to move.
		 * @param target Target child element.
		 * @param behind True to place object behind target, false to place it in front.
		 */
		void bringToNext(CGUIElement* object, CGUIElement* target, bool behind);

		/**
		 * @brief Bring a child element to the front of this element's child order.
		 * @param object Child element to move.
		 */
		void bringToChild(CGUIElement* object);

		/**
		 * @brief Check whether an element is a descendant of this element.
		 * @param e Element to test.
		 * @return True if e is this element's child or nested descendant.
		 */
		bool isChild(CGUIElement* e);

		/**
		 * @brief Find a GUI element by hierarchy path.
		 * @param path Path string to resolve.
		 * @return Matching GUI element, or NULL if not found.
		 */
		CGUIElement* getGUIByPath(const char* path);

		/**
		 * @brief Create a serializable object that stores this element's state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load this element's state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		/**
		 * @brief Generate and assign a new entity ID for this element.
		 */
		void generateNewId();

		DECLARE_GETTYPENAME(CGUIElement)

	protected:

		/**
		 * @brief Remove a direct child from this element without deleting unrelated hierarchy state.
		 * @param child Child element to remove.
		 * @return True if the child was found and removed.
		 */
		bool removeChild(CGUIElement* child);
	};
}
