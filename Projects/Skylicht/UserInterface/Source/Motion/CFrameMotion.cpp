/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CFrameMotion.h"

#include "Graphics2D/GUI/CGUISprite.h"
#include "Graphics2D/GUI/CGUIFitSprite.h"

namespace Skylicht
{
	namespace UI
	{
		CFrameMotion::CFrameMotion(SFrame* frame)
		{
			m_frames.push_back(frame);
		}

		CFrameMotion::CFrameMotion(std::vector<SFrame*>& frames)
		{
			m_frames = frames;
		}

		CFrameMotion::CFrameMotion()
		{
			m_toDefault = true;
		}

		CFrameMotion::~CFrameMotion()
		{

		}

		void CFrameMotion::init(CGUIElement* gui)
		{
			CMotion::init(gui);

			CGUISprite* sprite = dynamic_cast<CGUISprite*>(gui);
			CGUIFitSprite* fitSprite = dynamic_cast<CGUIFitSprite*>(gui);

			if (sprite)
				m_defaultFrame = sprite->getFrame();
			else if (fitSprite)
				m_defaultFrame = fitSprite->getFrame();
		}

		void CFrameMotion::setFrame(SFrame* frame)
		{
			CGUISprite* sprite = dynamic_cast<CGUISprite*>(m_gui);
			CGUIFitSprite* fitSprite = dynamic_cast<CGUIFitSprite*>(m_gui);

			if (sprite)
				sprite->setFrame(frame);
			else if (fitSprite)
				fitSprite->setFrame(frame);
		}

		void CFrameMotion::setFrame(float f)
		{
			if (m_frames.size() == 0)
			{
				setFrame(m_defaultFrame);
				return;
			}

			std::vector<SFrame*> allFrames;

			allFrames.push_back(m_defaultFrame);
			for (SFrame* f : m_frames)
				allFrames.push_back(f);

			int totalFrameId = (int)allFrames.size() - 1;
			int frameId = (int)(f / totalFrameId);

			setFrame(allFrames[frameId]);
		}

		void CFrameMotion::start()
		{
			if (m_toDefault)
				m_tween = new CTweenFloat(1.0f, 0.0f, m_duration);
			else
			{
				if (m_inverseMotion)
					m_tween = new CTweenFloat(1.0f, 0.0f, m_duration);
				else
					m_tween = new CTweenFloat(0.0f, 1.0f, m_duration);
			}

			m_tween->setDelay(m_delay);
			m_tween->setEase(m_ease);
			m_tween->OnUpdate = [&](CTween* t)
				{
					setFrame(((CTweenFloat*)t)->getValue());
				};
			m_tween->OnFinish = [&](CTween* t)
				{
					m_tween = NULL;
				};

			CTweenManager::getInstance()->addTween(m_tween);
		}
	}
}