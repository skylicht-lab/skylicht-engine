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

#include "CGUIElement.h"

namespace Skylicht
{
	/**
	 * @brief This is the object class for displaying a texture image.
	 * @ingroup GUI
	 *
	 * @code
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * CGUIImage* gui = canvas->createImage(r);
	 * gui->setImage(CTextureManager::getInstance()->getTexture("BuiltIn/Textures/Skylicht.png"));
	 * @endcode
	 */
	class SKYLICHT_API CGUIImage : public CGUIElement
	{
		friend class CCanvas;

	protected:
		ITexture* m_image;

		std::string m_resource;
		std::string m_id;

		core::rectf m_sourceRect;
		core::position2df m_pivot;

	protected:
		/**
		 * @brief Construct an image element for a canvas and optional parent.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 */
		CGUIImage(CCanvas* canvas, CGUIElement* parent);

		/**
		 * @brief Construct an image element with an initial rectangle.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 */
		CGUIImage(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIImage();

		/**
		 * @brief Render the image element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Get the native rectangle of the assigned texture.
		 * @return Rectangle matching the texture size in local coordinates.
		 */
		virtual const core::rectf getNativeRect();

		/**
		 * @brief Set the texture rendered by this image element.
		 * @param texture Texture to render.
		 */
		void setImage(ITexture* texture);

		/**
		 * @brief Load and assign a texture from a resource path.
		 * @param path Texture resource path.
		 * @param editorRefId Optional editor reference ID.
		 */
		void setImageResource(const char* path, const char* editorRefId);

		/**
		 * @brief Get the serialized texture resource ID.
		 * @return Resource ID string.
		 */
		const char* getResourceId()
		{
			return m_id.c_str();
		}

		/**
		 * @brief Get the assigned texture.
		 * @return Texture pointer.
		 */
		inline ITexture* getImage()
		{
			return m_image;
		}

		/**
		 * @brief Set the source rectangle sampled from the texture.
		 * @param x Source X coordinate.
		 * @param y Source Y coordinate.
		 * @param w Source width.
		 * @param h Source height.
		 */
		inline void setSourceRect(float x, float y, float w, float h)
		{
			m_sourceRect.UpperLeftCorner.X = x;
			m_sourceRect.UpperLeftCorner.Y = y;
			m_sourceRect.LowerRightCorner.X = x + w;
			m_sourceRect.LowerRightCorner.Y = y + h;
		}

		/**
		 * @brief Set the normalized pivot point used when rendering the image.
		 * @param x Pivot X value.
		 * @param y Pivot Y value.
		 */
		void setPivot(float x, float y)
		{
			m_pivot.set(x, y);
		}

		/**
		 * @brief Get the image pivot point.
		 * @return Current pivot point.
		 */
		const core::position2df& getPivot()
		{
			return m_pivot;
		}

		/**
		 * @brief Create a serializable object that stores this image state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load image state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUIImage)
	};
}
