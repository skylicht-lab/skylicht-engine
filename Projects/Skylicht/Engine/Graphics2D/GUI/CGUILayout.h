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

#pragma once

#include "CGUIElement.h"
#include "Graphics2D/EntityData/CGUILayoutData.h"

namespace Skylicht
{
	/**
	 * @brief This is the object class that arranges other GUIs horizontally or vertically.
	 * @ingroup GUI
	 *
	 * @code
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf layoutRect(0.0f, 0.0f, 400.0f, 400.0f);
	 * CGUILayout* gui = canvas->createLayout(layoutRect);
	 * gui->setAlign(CGUILayoutData::Vertical);
	 *
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * canvas->createRect(gui, r, SColor(255,255,0,0));
	 * canvas->createRect(gui, r, SColor(255,0,255,0));
	 * canvas->createRect(gui, r, SColor(255,0,0,255));
	 * @endcode
	 */
	class SKYLICHT_API CGUILayout : public CGUIElement
	{
		friend class CCanvas;
	public:
		CGUILayoutData* m_layoutData;

	protected:
		/**
		 * @brief Construct a layout element for a canvas and optional parent.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 */
		CGUILayout(CCanvas* canvas, CGUIElement* parent);

		/**
		 * @brief Construct a layout element with an initial rectangle.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 */
		CGUILayout(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUILayout();

		/**
		 * @brief Render the layout element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Update layout calculation and child transforms before rendering.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void update(CCamera* camera);

		/**
		 * @brief Set whether children are arranged vertically or horizontally.
		 * @param type Layout alignment type.
		 */
		inline void setAlign(CGUILayoutData::EAlignType type)
		{
			m_layoutData->AlignType = type;
		}

		/**
		 * @brief Set spacing between consecutive children.
		 * @param space Spacing in GUI units.
		 */
		inline void setSpacing(float space)
		{
			m_layoutData->Spacing = space;
		}

		/**
		 * @brief Get spacing between consecutive children.
		 * @return Spacing in GUI units.
		 */
		inline float getSpacing()
		{
			return m_layoutData->Spacing;
		}

		/**
		 * @brief Enable or disable wrapping children to the next row or column.
		 * @param b True to enable wrapping.
		 */
		inline void setWrap(bool b)
		{
			m_layoutData->IsWrap = b;
		}

		/**
		 * @brief Set spacing between wrapped rows or columns.
		 * @param space Wrap spacing in GUI units.
		 */
		inline void setWrapSpacing(float space)
		{
			m_layoutData->WrapSpacing = space;
		}

		/**
		 * @brief Get spacing between wrapped rows or columns.
		 * @return Wrap spacing in GUI units.
		 */
		inline float getWrapSpacing()
		{
			return m_layoutData->WrapSpacing;
		}

		/**
		 * @brief Enable or disable resizing this layout to fit its children.
		 * @param b True to fit the layout size to its children.
		 */
		inline void setFitChildrenSize(bool b)
		{
			m_layoutData->FitChildrenSize = b;
		}

		/**
		 * @brief Check whether this layout fits its size to its children.
		 * @return True if fit-to-children is enabled.
		 */
		inline bool isFitChildrenSize()
		{
			return m_layoutData->FitChildrenSize;
		}

		DECLARE_GETTYPENAME(CGUILayout)

		/**
		 * @brief Create a serializable object that stores this layout state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load layout state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		/**
		 * @brief Refresh the internal child ordering used by layout calculation.
		 */
		void updateChildOrder();
	};
}
