/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CInterpolateCurvesButton.h"
#include "CSpaceInterpolateCurves.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CInterpolateCurvesButton::CInterpolateCurvesButton(CBase* parent, CInterpolator interpolation) :
				CCustomDrawButton(parent),
				m_interpolation(interpolation)
			{
				OnCustomDraw = std::bind(&CInterpolateCurvesButton::renderUnderOverride, this, std::placeholders::_1, std::placeholders::_2);
				OnDown = BIND_LISTENER(&CInterpolateCurvesButton::onButtonDown, this);
			}

			CInterpolateCurvesButton::~CInterpolateCurvesButton()
			{
				const wchar_t* spaceName = L"Interpolate Curves";
				CEditor* editor = CEditor::getInstance();

				CSpaceInterpolateCurves* spaceInterpolate = dynamic_cast<CSpaceInterpolateCurves*>(editor->getWorkspaceByName(spaceName));
				if (spaceInterpolate && spaceInterpolate->getOwner() == this)
					spaceInterpolate->onOwnerClosed();
			}

			void CInterpolateCurvesButton::renderUnderOverride(CBase* base, const SRect& bounds)
			{
				CRenderer* renderer = CRenderer::getRenderer();

				if (m_interpolation.empty())
					return;

				float x = 0.0f;
				while (x < bounds.Width)
				{
					float fx1 = core::clamp(x / bounds.Width, 0.0f, 1.0f);
					float x1 = x;
					float y1 = core::clamp(m_interpolation.interpolate(fx1), 0.0f, 1.0f) * bounds.Height;

					x = x + 5.0f;
					if (x > bounds.Width)
						x = bounds.Width;

					float fx2 = core::clamp(x / bounds.Width, 0.0f, 1.0f);
					float x2 = x;
					float y2 = core::clamp(m_interpolation.interpolate(fx2), 0.0f, 1.0f) * bounds.Height;

					renderer->drawLine(
						bounds.X + x1,
						bounds.Y + bounds.Height - y1,
						bounds.X + x2,
						bounds.Y + bounds.Height - y2,
						SGUIColor(255, 0, 255, 0)
					);
				}
			}

			void CInterpolateCurvesButton::onButtonDown(CBase* base)
			{
				const wchar_t* spaceName = L"Interpolate Curves";
				CEditor* editor = CEditor::getInstance();

				CSpace* space = editor->getWorkspaceByName(spaceName);
				if (!space)
				{
					editor->showInterpolateCurves();
					space = editor->getWorkspaceByName(spaceName);
				}

				CSpaceInterpolateCurves* spaceInterpolate = dynamic_cast<CSpaceInterpolateCurves*>(space);
				spaceInterpolate->setOwner(this);

				CInterpolateCurvesController* controller = spaceInterpolate->getController();
				controller->setInterpolator(m_interpolation);
				controller->OnChanged = [&, controller]()
					{
						m_interpolation = controller->getInterpolator();

						if (OnChanged != nullptr)
							OnChanged(this);
					};
				controller->OnClose = [&, controller]()
					{
						m_interpolation = controller->getInterpolator();

						if (OnChanged != nullptr)
							OnChanged(this);
					};
			}
		}
	}
}