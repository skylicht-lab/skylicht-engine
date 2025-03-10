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

		void CInterpolateCurvesController::updateValue()
		{
			m_interpolation.generateGraph(0, 5);
			if (OnChanged != nullptr)
				OnChanged();
		}

		void CInterpolateCurvesController::getRangleMinMax(int layer, core::vector2df& min, core::vector2df& max)
		{
			min.set(0.0f, 0.0f);
			max.set(0.0f, 0.0f);

			int id = 0;
			const std::vector<SControlPoint>& graph = m_interpolation.getControlPoints(layer);
			for (const auto& it : graph)
			{
				const core::vector2df& pos = it.Position;
				core::vector2df left = pos + it.Left;
				core::vector2df right = pos + it.Right;

				if (id++ == 0)
				{
					min.X = pos.X;
					min.Y = pos.Y;

					max.X = pos.X;
					max.Y = pos.Y;
				}

				min.X = core::min_(min.X, pos.X);
				min.X = core::min_(min.X, left.X);
				min.X = core::min_(min.X, right.X);

				max.X = core::max_(max.X, pos.X);
				max.X = core::max_(max.X, left.X);
				max.X = core::max_(max.X, right.X);

				min.Y = core::min_(min.Y, pos.Y);
				min.Y = core::min_(min.Y, left.Y);
				min.Y = core::min_(min.Y, right.Y);

				max.Y = core::max_(max.Y, pos.Y);
				max.Y = core::max_(max.Y, left.Y);
				max.Y = core::max_(max.Y, right.Y);
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

			updateValue();
		}

		void CInterpolateCurvesController::setDefaultInOutCubic()
		{
			m_interpolation.clearGraph();

			SControlPoint& start = m_interpolation.addControlPoint();
			start.Position.set(0.0f, 0.0f);
			start.Right.set(0.6f, 0.0f);
			start.Type = SControlPoint::Auto;

			SControlPoint& end = m_interpolation.addControlPoint();
			end.Position.set(1.0f, 1.0f);
			end.Left.set(-0.6f, 0.0f);
			end.Type = SControlPoint::Auto;

			updateValue();
		}

		void CInterpolateCurvesController::onPointChangeType(int layer, SControlPoint* point)
		{
			const std::vector<SControlPoint>& graph = m_interpolation.getControlPoints(layer);

			int id = -1;
			for (int i = 0, n = (int)graph.size(); i < n; i++)
			{
				if (point == &graph[i])
				{
					id = i;
					break;
				}
			}

			if (id == -1)
				return;

			if (point->Type != SControlPoint::Linear)
				convertPointAuto(layer, id, point->Type);

			updateValue();
		}

		void CInterpolateCurvesController::convertPointAuto(int layer, int pointId, SControlPoint::EControlType type)
		{
			std::vector<SControlPoint>& graph = m_interpolation.getControlPoints(layer);
			SControlPoint& point = graph[pointId];

			if (pointId == 0)
			{
				SControlPoint& next = graph[pointId + 1];

				if (point.Right.getLengthSQ() < 0.01f)
				{
					float length = next.Position.getDistanceFrom(point.Position) * 0.2f;
					point.Right.set(length, 0.0f);
				}
			}
			else if (pointId == (int)graph.size() - 1)
			{
				SControlPoint& prev = graph[pointId - 1];

				if (point.Left.getLengthSQ() < 0.01f)
				{
					float length = prev.Position.getDistanceFrom(point.Position) * 0.2f;
					point.Left.set(-length, 0.0f);
				}
			}
			else
			{
				SControlPoint& prev = graph[pointId - 1];
				SControlPoint& next = graph[pointId + 1];

				core::vector2df v = next.Position - prev.Position;
				v.normalize();

				float leftLength = point.Left.getLength();
				float rightLength = point.Right.getLength();

				if (leftLength < 0.01f && rightLength < 0.01f)
				{
					float length = prev.Position.getDistanceFrom(point.Position) * 0.2f;
					point.Right = v * length;
					point.Left = -v * length;
				}
				else
				{
					if (type == SControlPoint::Auto)
					{
						float length = core::max_(leftLength, rightLength);
						point.Right = v * length;
						point.Left = -v * length;
					}
					else
					{
						point.Right = v * rightLength;
						point.Left = -v * leftLength;
					}
				}
			}
		}

		void CInterpolateCurvesController::insertPoint(int layer, SControlPoint* after, core::vector2df& position)
		{
			std::vector<SControlPoint>& graph = m_interpolation.getControlPoints(layer);

			int id = -1;
			for (int i = 0, n = (int)graph.size(); i < n; i++)
			{
				if (after == &graph[i])
				{
					id = i;
					break;
				}
			}

			if (id == -1)
				return;

			SControlPoint::EControlType type = after->Type;

			graph.insert(graph.begin() + (id + 1), SControlPoint());

			SControlPoint& point = graph[id + 1];
			point.Position = position;
			point.Left.set(0.0f, 0.0f);
			point.Right.set(0.0f, 0.0f);
			point.Type = type;

			if (type != SControlPoint::Linear)
				convertPointAuto(layer, id + 1, type);

			updateValue();
		}

		void CInterpolateCurvesController::deletePoint(int layer, SControlPoint* point)
		{
			std::vector<SControlPoint>& graph = m_interpolation.getControlPoints(layer);

			int id = -1;
			for (int i = 0, n = (int)graph.size(); i < n; i++)
			{
				if (point == &graph[i])
				{
					id = i;
					break;
				}
			}

			if (id == -1)
				return;

			if (graph.size() <= 2)
				clearGraph();
			else
			{
				graph.erase(graph.begin() + id);
				updateValue();
			}
		}
	}
}