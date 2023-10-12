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
	class CGUISprite : public CGUIElement
	{
		friend class CCanvas;
	protected:
		SFrame* m_frame;

		bool m_autoRotate;

		float m_frameRotate;
		float m_frameSpeed;
		float m_animationTime;

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

		void setFrame(SFrame* frame);

		void setAutoRotate(bool rotate, float rotateAngle, float framePerSec);

		void setAlignCenterModule();

		void setOffsetModule(float x, float y);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUISprite);
	};
}