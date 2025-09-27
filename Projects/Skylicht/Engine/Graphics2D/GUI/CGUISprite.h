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
	/// @brief This is the object class for displaying a frame image in sprite.
	/// @ingroup GUI
	/// 
	/// @see CSpriteFrame
	/// 
	/// CGUISprite is used to render a frame from a sprite sheet in the GUI. It supports
	/// features such as auto - rotation, stretching to fit its rectangle, alignment, offset,
	/// and serialization. Managed by CCanvas, this element is useful for icons, buttons,
	/// and other image - based GUI components.
	/// 
	/// Example usage:
	/// @code
	/// CSpriteFrame* sprite = CSpriteManager::getInstance()->loadSprite("Assets/SampleGUI/SampleGUI.spritedata")
	/// SFrame* frame = sprite->getFrame("skylicht-icon");
	/// 
	/// CCanvas *canvas = gameobject->addComponent<CCanvas>();
	/// core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	/// CGUISprite* gui = canvas->createSprite(r, frame);
	/// @endcode
	class SKYLICHT_API CGUISprite : public CGUIElement
	{
		friend class CCanvas;
	protected:
		/// Pointer to the current sprite frame
		SFrame* m_frame;

		/// Enables auto - rotation of the sprite.
		bool m_autoRotate;

		/// Rotation angle per frame(degrees).
		float m_frameRotate;

		/// Interval between rotations(milliseconds).
		float m_frameSpeed;

		/// Time accumulator for animation.
		float m_animationTime;

		/// Enables stretching the sprite to fill the rectangle.
		bool m_stretch;

		/// Flag the frame is centered.
		bool m_isCenter;

		/// Default offsets for module alignment.
		float m_defaultOffsetX;
		float m_defaultOffsetY;

		/// Identifiers for serialization and sprite management.
		std::string m_guid;
		std::string m_frameName;
		std::string m_sprite;
		std::string m_spriteId;

	protected:
		/**
		 * @brief Constructor with frame.
		 * @param canvas Pointer to the parent canvas.
		 * @param parent Pointer to the parent element.
		 * @param frame Pointer to the sprite frame.
		 */
		CGUISprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame);

		/**
		 * @brief Constructor with rectangle and frame.
		 * @param canvas Pointer to the parent canvas.
		 * @param parent Pointer to the parent element.
		 * @param rect Rectangle for the sprite.
		 * @param frame Pointer to the sprite frame.
		 */
		CGUISprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame);

	public:
		/**
		* @brief Destructor.
		*/
		virtual ~CGUISprite();

		/**
		 * @brief Update element state (auto-rotation, etc.).
		 * @param camera Pointer to the camera.
		 */
		virtual void update(CCamera* camera);

		/**
		 * @brief Render the sprite frame.
		 * @param camera Pointer to the camera.
		 */
		virtual void render(CCamera* camera);

		/**
		 * @brief Get the native rectangle of the frame.
		 * The native rectangle is a rectangle whose size matches the size of the frame.
		 * It is useful for positioning or aligning the sprite in the GUI according to its actual image dimensions.
		 *
		 * @return Rectangle in local coordinates with width and height equal to the frame's size.
		 */
		virtual const core::rectf getNativeRect();

		/**
		 * @brief Set the sprite frame to display.
		 * @param frame Pointer to the new frame.
		 */
		inline void setFrame(SFrame* frame)
		{
			m_frame = frame;
		}

		/**
		 * @brief Get the current sprite frame.
		 * @return Pointer to the frame.
		 */
		inline SFrame* getFrame()
		{
			return m_frame;
		}

		/**
		 * @brief Get the frame name.
		 * @return Frame name string.
		 */
		inline const char* getFrameName()
		{
			return m_frameName.c_str();
		}

		/**
		 * @brief Get the sprite name (path).
		 * @return Sprite name string.
		 */
		inline const char* getSpriteName()
		{
			return m_sprite.c_str();
		}

		/**
		 * @brief Get the sprite ID.
		 * @return Sprite ID string.
		 */
		inline const char* getSpriteId()
		{
			return m_spriteId.c_str();
		}

		/**
		 * @brief Get the frame ID (GUID).
		 * @return Frame ID string.
		 */
		inline const char* getFrameId()
		{
			return m_guid.c_str();
		}

		/**
		* @brief Set stretching mode.
		* @param b Enable/disable stretching.
		*/
		inline void setStretch(bool b)
		{
			m_stretch = b;
		}

		/**
		* @brief Check whether stretching is enabled.
		* @return True if stretching, false otherwise.
		*/
		inline bool isStretch()
		{
			return m_stretch;
		}

		/**
		 * @brief Reload the sprite frame by stored identifiers.
		 */
		void reloadSpriteFrame();

		/**
		* @brief Set the frame source by sprite path and frame name.
		* @param spritePath Path to the sprite.
		* @param frameName Name of the frame.
		* @param editorFileRef Optional editor reference name.
		*/
		void setFrameSource(const char* spritePath, const char* frameName, const char* editorFileRef = NULL);

		/**
		* @brief Enable auto-rotation with parameters.
		* @param rotate Enable/disable auto-rotation.
		* @param rotateAngle Rotation angle per frame.
		* @param framePerSec Rotation speed (frames per second).
		*/
		void setAutoRotate(bool rotate, float rotateAngle, float framePerSec);

		/**
		 * @brief Align the frame to the center.
		 */
		void setAlignCenterModule();

		/**
		* @brief Restore the default module alignment.
		*/
		void setAlignModuleDefault();

		/**
		 * @brief Set manual offset for the module.
		 * @param x Offset X.
		 * @param y Offset Y.
		 */
		void setOffsetModule(float x, float y);

		/**
		* @brief Create a serializable object for saving state.
		* @return Pointer to the serializable object.
		*/
		virtual CObjectSerializable* createSerializable();

		/**
		 * @brief Load state from a serializable object.
		 * @param object Pointer to the serializable object.
		 */
		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUISprite)
	};
}