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
#include "CInterpolateCurvesController.h"
#include "CSpaceInterpolateCurves.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CInterpolateCurvesController::CInterpolateCurvesController(CSpaceInterpolateCurves* space) :
			m_space(space)
		{

		}

		CInterpolateCurvesController::~CInterpolateCurvesController()
		{

		}

		void CInterpolateCurvesController::onClosed()
		{
			m_interpolation.clearGraph();
			OnChanged = nullptr;
			OnClose = nullptr;
		}

		void CInterpolateCurvesController::getRangleMinMax(core::vector2df& min, core::vector2df& max)
		{
			min.set(0.0f, 0.0f);
			max.set(0.0f, 0.0f);

			int id = 0;
			const std::vector<SControlPoint>& graph = m_interpolation.getControlPoints();
			for (const auto& it : graph)
			{
				if (id++ == 0)
				{
					min.X = it.Position.X;
					min.Y = it.Position.Y;

					max.X = it.Position.X;
					max.Y = it.Position.Y;
				}

				min.X = core::min_(min.X, it.Position.X);
				max.X = core::max_(max.X, it.Position.X);

				min.Y = core::min_(min.Y, it.Position.Y);
				max.Y = core::max_(max.Y, it.Position.Y);
			}
		}

		void CInterpolateCurvesController::setDefaultLinear()
		{
			m_interpolation.clearGraph();

			SControlPoint& start = m_interpolation.addControlPoint();
			start.Position.set(0.0f, 0.0f);
			start.Type = SControlPoint::Linear;

			SControlPoint& end = m_interpolation.addControlPoint();
			end.Position.set(1.0f, 1.0f);
			end.Type = SControlPoint::Linear;
		}

		void CInterpolateCurvesController::setDefaultInOutCubic()
		{
			SControlPoint& start = m_interpolation.addControlPoint();
			start.Position.set(0.0f, 0.0f);

			SControlPoint& end = m_interpolation.addControlPoint();
			end.Position.set(1.0f, 1.0f);
		}
	}
}