/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#include "Tween/CTweenManager.h"
#include "GizmosComponents/Viewpoint/CViewpoint.h"

namespace Skylicht
{
	namespace Editor
	{
		class CViewpointController
		{
		protected:
			CCamera* m_editorCamera;
			CCamera* m_viewpointCamera;

		public:
			CViewpointController();

			virtual ~CViewpointController();

			void update();

			void setCameraLook(CViewpointData::EAxis axis);

			inline void setCamera(CCamera* editorCamera, CCamera* viewpointCamera)
			{
				m_editorCamera = editorCamera;
				m_viewpointCamera = viewpointCamera;
			}

		protected:

			void updateViewpoint();

			void setupCameraTween(CTweenVector3df* look, CTweenVector3df* up);
		};
	}
}