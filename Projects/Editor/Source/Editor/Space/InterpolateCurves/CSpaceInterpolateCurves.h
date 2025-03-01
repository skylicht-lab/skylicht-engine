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

#pragma once

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"
#include "CInterpolateCurvesController.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceInterpolateCurves : public CSpace
		{
		public:

		protected:
			GUI::CBase* m_owner;

			CInterpolateCurvesController* m_controller;

		public:
			CSpaceInterpolateCurves(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceInterpolateCurves();

			virtual void onResize(float w, float h);

			virtual void onRender(GUI::CBase* base);

			inline CInterpolateCurvesController* getController()
			{
				return m_controller;
			}

			void setOwner(GUI::CBase* owner);

			inline GUI::CBase* getOwner()
			{
				return m_owner;
			}

			void onOwnerClosed();
		};
	}
}