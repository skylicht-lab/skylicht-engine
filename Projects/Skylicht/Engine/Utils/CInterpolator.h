/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include <set>

namespace Skylicht
{
	struct SInterpolatorEntry
	{
		float X;
		float Value[4];

		SInterpolatorEntry(float x)
		{
			X = x;
			for (int i = 0; i < 4; i++)
				Value[i] = 0.0f;
		}

		SInterpolatorEntry()
		{
			X = 0.0f;
			for (int i = 0; i < 4; i++)
				Value[i] = 0.0f;
		}

		void operator=(const SInterpolatorEntry& entry)
		{
			X = entry.X;
			for (int i = 0; i < 4; i++)
				Value[i] = entry.Value[i];
		}
	};

	inline bool operator<(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
	{
		return entry0.X < entry1.X;
	}

	inline bool operator==(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
	{
		return entry0.X == entry1.X;
	}

	struct SControlPoint
	{
		enum EControlType
		{
			Auto = 0,
			Smooth,
			Broken,
			Linear
		};

		core::vector2df Position;
		core::vector2df Left;
		core::vector2df Right;
		EControlType Type;

		SControlPoint()
		{
			Type = Auto;
		}
	};

	typedef core::array<core::vector2df> ArrayPoint2df;

	class SKYLICHT_API CInterpolator
	{
	public:
		enum EInterpolatorType
		{
			Float,
			Vector2,
			Vector3,
			Color
		};

	protected:
		std::set<SInterpolatorEntry> m_graph;

		std::vector<SControlPoint> m_controls[4];

		EInterpolatorType m_type;

	public:
		CInterpolator();

		virtual ~CInterpolator();

		inline EInterpolatorType getType()
		{
			return m_type;
		}

		inline void setType(EInterpolatorType type)
		{
			m_type = type;
		}

		void operator=(const CInterpolator& other)
		{
			m_type = other.m_type;
			m_graph = other.m_graph;
			for (int i = 0; i < 4; i++)
				m_controls[i] = other.m_controls[i];
		}

		inline bool empty()
		{
			return m_graph.empty();
		}

		void getMinMaxXY(core::vector2df& min, core::vector2df& max);

		float interpolate(float x);

		core::vector2df interpolateVec2(float x);

		core::vector3df interpolateVec3(float x);

		SColorf interpolateColorf(float x);

		inline std::set<SInterpolatorEntry>& getGraph()
		{
			return m_graph;
		}

		inline const std::set<SInterpolatorEntry>& getGraph() const
		{
			return m_graph;
		}

		inline std::vector<SControlPoint>& getControlPoints(int layer)
		{
			return m_controls[layer];
		}

		inline const std::vector<SControlPoint>& getControlPoints(int layer) const
		{
			return m_controls[layer];
		}

		SControlPoint& addControlPoint(int layer = 0);

		inline bool addEntry(const SInterpolatorEntry& entry)
		{
			return m_graph.insert(entry).second;
		}

		inline bool addEntry(float x, float y)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = y;
			return addEntry(entry);
		}

		inline bool addEntry(int layer, float x, float y)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[layer] = y;
			return addEntry(entry);
		}

		inline bool addEntry(float x, const core::vector2df& v)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = v.X;
			entry.Value[1] = v.Y;
			return addEntry(entry);
		}

		inline bool addEntry(float x, const core::vector3df& v)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = v.X;
			entry.Value[1] = v.Y;
			entry.Value[2] = v.Z;
			return addEntry(entry);
		}

		inline bool addEntry(float x, const video::SColorf& c)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = c.r;
			entry.Value[1] = c.g;
			entry.Value[2] = c.b;
			entry.Value[3] = c.a;
			return addEntry(entry);
		}

		inline void clearGraph()
		{
			m_graph.clear();

			for (int i = 0; i < 4; i++)
				m_controls[i].clear();
		}

		void generateGraph(int layer, int bezierStep);

		void computeLine(int layer, std::vector<ArrayPoint2df>& lines, int bezierStep);

		void computeBezier(const SControlPoint& p1, const SControlPoint& p2, ArrayPoint2df& points, int bezierStep);
	};
}