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
		SFrame* m_frame;

		bool m_autoRotate;

		float m_frameRotate;
		float m_frameSpeed;
		float m_animationTime;

		bool m_stretch;

		bool m_isCenter;
		float m_defaultOffsetX;
		float m_defaultOffsetY;

		std::string m_guid;
		std::string m_frameName;
		std::string m_sprite;
		std::string m_spriteId;

	protected:
		CGUISprite(CCanvas* canvas, CGUIElement* parent, SFrame* frame);
		CGUISprite(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, SFrame* frame);

	public:
		virtual ~CGUISprite();

		virtual void update(CCamera* camera);

		virtual void render(CCamera* camera);

		virtual const core::rectf getNativeRect();

		inline void setFrame(SFrame* frame)
		{
			m_frame = frame;
		}

		inline SFrame* getFrame()
		{
			return m_frame;
		}

		inline const char* getFrameName()
		{
			return m_frameName.c_str();
		}

		inline const char* getSpriteName()
		{
			return m_sprite.c_str();
		}

		inline const char* getSpriteId()
		{
			return m_spriteId.c_str();
		}

		inline const char* getFrameId()
		{
			return m_guid.c_str();
		}

		inline void setStretch(bool b)
		{
			m_stretch = b;
		}

		inline bool isStretch()
		{
			return m_stretch;
		}

		void setFrameSource(const char* spritePath, const char* frameName, const char* editorFileRef = NULL);

		void setAutoRotate(bool rotate, float rotateAngle, float framePerSec);

		void setAlignCenterModule();

		void setAlignModuleDefault();

		void setOffsetModule(float x, float y);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUISprite)
	};
}