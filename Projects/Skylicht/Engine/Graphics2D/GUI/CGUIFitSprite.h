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
#include "Graphics2D/SpriteFrame/CSpriteFrame.h"

namespace Skylicht
{
	/**
	 * @brief This is the object class for displaying a frame image in sprite. But it will scale the image size while minimizing image distortion.
	 * @ingroup GUI
	 *
	 * @see CSpriteFrame
	 *
	 * @code
	 * CSpriteFrame* sprite = CSpriteManager::getInstance()->loadSprite("Assets/SampleGUI/SampleGUI.spritedata")
	 * SFrame* frame = sprite->getFrame("dialog");
	 *
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * CGUIFitSprite* gui = canvas->createFitSprite(r, frame);
	 * gui->setAnchor(CGUIFitSprite::AnchorAll, 20.0f, 20.0f, 20.0f, 20.0f);
	 * @endcode
	 */
	class SKYLICHT_API CGUIFitSprite : public CGUIElement
	{
		friend class CCanvas;
	public:
		enum AnchorType
		{
			AnchorAll,
			AnchorLeftRight,
			AnchorTopBottom
		};

	protected:
		SFrame* m_frame;

		std::string m_guid;
		std::string m_frameName;
		std::string m_sprite;
		std::string m_spriteId;

		AnchorType m_anchorType;

		float m_anchorLeft;
		float m_anchorRight;
		float m_anchorTop;
		float m_anchorBottom;

	protected:
		/**
		 * @brief Construct a fitted sprite element with an initial frame.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param frame Sprite frame to render.
		 */
		CGUIFitSprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame);

		/**
		 * @brief Construct a fitted sprite element with an initial rectangle and frame.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 * @param frame Sprite frame to render.
		 */
		CGUIFitSprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame);

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIFitSprite();

		/**
		 * @brief Update the fitted sprite element before rendering.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void update(CCamera* camera);

		/**
		 * @brief Render the fitted sprite element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Reload the sprite frame from the stored sprite and frame identifiers.
		 */
		void reloadSpriteFrame();

		/**
		 * @brief Set the sprite asset and frame used by this element.
		 * @param spritePath Path to the sprite data file.
		 * @param frameName Name of the frame in the sprite.
		 * @param editorFileRef Optional editor reference ID.
		 */
		void setFrameSource(const char* spritePath, const char* frameName, const char* editorFileRef = NULL);

		/**
		 * @brief Set the sprite frame directly.
		 * @param frame Sprite frame to render.
		 */
		inline void setFrame(SFrame* frame)
		{
			m_frame = frame;
		}

		/**
		 * @brief Get the current sprite frame.
		 * @return Current sprite frame.
		 */
		inline SFrame* getFrame()
		{
			return m_frame;
		}

		/**
		 * @brief Get the current frame name.
		 * @return Frame name string.
		 */
		inline const char* getFrameName()
		{
			return m_frameName.c_str();
		}

		/**
		 * @brief Get the sprite asset path or name.
		 * @return Sprite asset string.
		 */
		inline const char* getSpriteName()
		{
			return m_sprite.c_str();
		}

		/**
		 * @brief Get the serialized sprite ID.
		 * @return Sprite ID string.
		 */
		inline const char* getSpriteId()
		{
			return m_spriteId.c_str();
		}

		/**
		 * @brief Get the serialized frame ID.
		 * @return Frame ID string.
		 */
		inline const char* getFrameId()
		{
			return m_guid.c_str();
		}

		/**
		 * @brief Configure the fixed borders used when fitting the sprite.
		 * @param type Anchor mode.
		 * @param left Left border size.
		 * @param right Right border size.
		 * @param top Top border size.
		 * @param bottom Bottom border size.
		 */
		void setAnchor(AnchorType type, float left, float right, float top, float bottom);

		/**
		 * @brief Create a serializable object that stores this fitted sprite state.
		 * @return Serializable object.
		 */
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load fitted sprite state from a serializable object.
		 * @param object Serializable object to read from.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUIFitSprite)
	};
}
