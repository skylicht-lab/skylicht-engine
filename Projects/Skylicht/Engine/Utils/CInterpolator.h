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
	/**
	 * @brief One keyed interpolation value.
	 * @ingroup Utilities
	 *
	 * `X` is the key position and `Value` stores up to four channels depending on
	 * the interpolator type.
	 */
	struct SInterpolatorEntry
	{
		/** @brief Key position on the X axis. */
		float X;
		/** @brief Interpolated channel values. */
		float Value[4];

		/**
		 * @brief Construct an entry at a key position with zeroed values.
		 * @param x Key position.
		 */
		SInterpolatorEntry(float x)
		{
			X = x;
			for (int i = 0; i < 4; i++)
				Value[i] = 0.0f;
		}

		/**
		 * @brief Construct a zero entry.
		 */
		SInterpolatorEntry()
		{
			X = 0.0f;
			for (int i = 0; i < 4; i++)
				Value[i] = 0.0f;
		}

		/**
		 * @brief Copy another entry.
		 * @param entry Source entry.
		 */
		void operator=(const SInterpolatorEntry& entry)
		{
			X = entry.X;
			for (int i = 0; i < 4; i++)
				Value[i] = entry.Value[i];
		}
	};

	/**
	 * @brief Sort interpolation entries by their X key.
	 */
	inline bool operator<(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
	{
		return entry0.X < entry1.X;
	}

	/**
	 * @brief Compare interpolation entries by their X key.
	 */
	inline bool operator==(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
	{
		return entry0.X == entry1.X;
	}

	/**
	 * @brief Bezier control point used to generate interpolation graph entries.
	 * @ingroup Utilities
	 */
	struct SControlPoint
	{
		/**
		 * @brief Tangent behavior for a control point.
		 */
		enum EControlType
		{
			/** @brief Tangents are generated automatically. */
			Auto = 0,
			/** @brief Tangents remain smooth through the point. */
			Smooth,
			/** @brief Left and right tangents may move independently. */
			Broken,
			/** @brief Segment is treated as linear. */
			Linear
		};

		/** @brief Point position. */
		core::vector2df Position;
		/** @brief Left tangent offset. */
		core::vector2df Left;
		/** @brief Right tangent offset. */
		core::vector2df Right;
		/** @brief Control point tangent type. */
		EControlType Type;

		/**
		 * @brief Construct an automatic control point.
		 */
		SControlPoint()
		{
			Type = Auto;
		}
	};

	/**
	 * @brief Dynamic array of 2D points used for generated curve segments.
	 */
	typedef core::array<core::vector2df> ArrayPoint2df;

	/**
	 * @brief Keyframe interpolator for scalar, vector, and color values.
	 * @ingroup Utilities
	 *
	 * Values are stored as sorted entries and sampled with linear interpolation.
	 * Bezier control points can be converted into graph entries with `generateGraph`.
	 */
	class SKYLICHT_API CInterpolator
	{
	public:
		/**
		 * @brief Value layout used by the interpolator.
		 */
		enum EInterpolatorType
		{
			/** @brief One float channel. */
			Float,
			/** @brief Two float channels. */
			Vector2,
			/** @brief Three float channels. */
			Vector3,
			/** @brief Four float color channels. */
			Color
		};

	protected:
		std::set<SInterpolatorEntry> m_graph;

		std::vector<SControlPoint> m_controls[4];

		EInterpolatorType m_type;

	public:
		/**
		 * @brief Construct an empty float interpolator.
		 */
		CInterpolator();

		/**
		 * @brief Destroy the interpolator.
		 */
		virtual ~CInterpolator();

		/**
		 * @brief Get the current value layout.
		 * @return Interpolator type.
		 */
		inline EInterpolatorType getType()
		{
			return m_type;
		}

		/**
		 * @brief Set the value layout.
		 * @param type Interpolator type.
		 */
		inline void setType(EInterpolatorType type)
		{
			m_type = type;
		}

		/**
		 * @brief Copy another interpolator.
		 * @param other Source interpolator.
		 */
		void operator=(const CInterpolator& other)
		{
			m_type = other.m_type;
			m_graph = other.m_graph;
			for (int i = 0; i < 4; i++)
				m_controls[i] = other.m_controls[i];
		}

		/**
		 * @brief Check whether the interpolator has no graph entries.
		 * @return True if no entries are stored.
		 */
		inline bool empty()
		{
			return m_graph.empty();
		}

		/**
		 * @brief Compute the min and max X/Y bounds of the stored graph entries.
		 * @param min Receives minimum X and Y.
		 * @param max Receives maximum X and Y.
		 */
		void getMinMaxXY(core::vector2df& min, core::vector2df& max);

		/**
		 * @brief Sample the graph as a scalar value.
		 * @param x Key position.
		 * @return Interpolated first channel.
		 */
		float interpolate(float x);

		/**
		 * @brief Sample the graph as a 2D vector.
		 * @param x Key position.
		 * @return Interpolated first two channels.
		 */
		core::vector2df interpolateVec2(float x);

		/**
		 * @brief Sample the graph as a 3D vector.
		 * @param x Key position.
		 * @return Interpolated first three channels.
		 */
		core::vector3df interpolateVec3(float x);

		/**
		 * @brief Sample the graph as a floating-point color.
		 * @param x Key position.
		 * @return Interpolated four-channel color.
		 */
		SColorf interpolateColorf(float x);

		/**
		 * @brief Get mutable access to graph entries.
		 * @return Sorted graph entry set.
		 */
		inline std::set<SInterpolatorEntry>& getGraph()
		{
			return m_graph;
		}

		/**
		 * @brief Get read-only access to graph entries.
		 * @return Sorted graph entry set.
		 */
		inline const std::set<SInterpolatorEntry>& getGraph() const
		{
			return m_graph;
		}

		/**
		 * @brief Get mutable Bezier control points for a value layer.
		 * @param layer Channel index [0, 3].
		 * @return Control point list.
		 */
		inline std::vector<SControlPoint>& getControlPoints(int layer)
		{
			return m_controls[layer];
		}

		/**
		 * @brief Get read-only Bezier control points for a value layer.
		 * @param layer Channel index [0, 3].
		 * @return Control point list.
		 */
		inline const std::vector<SControlPoint>& getControlPoints(int layer) const
		{
			return m_controls[layer];
		}

		/**
		 * @brief Add a control point to a value layer.
		 * @param layer Channel index [0, 3].
		 * @return Newly added control point.
		 */
		SControlPoint& addControlPoint(int layer = 0);

		/**
		 * @brief Add a raw graph entry.
		 * @param entry Entry to insert.
		 * @return True when inserted, false when another entry already has the same X key.
		 */
		inline bool addEntry(const SInterpolatorEntry& entry)
		{
			return m_graph.insert(entry).second;
		}

		/**
		 * @brief Add a scalar graph entry.
		 * @param x Key position.
		 * @param y Scalar value.
		 * @return True when inserted.
		 */
		inline bool addEntry(float x, float y)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = y;
			return addEntry(entry);
		}

		/**
		 * @brief Add a single channel value to a graph entry.
		 * @param layer Channel index [0, 3].
		 * @param x Key position.
		 * @param y Channel value.
		 * @return True when inserted.
		 */
		inline bool addEntry(int layer, float x, float y)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[layer] = y;
			return addEntry(entry);
		}

		/**
		 * @brief Add a 2D vector graph entry.
		 * @param x Key position.
		 * @param v Vector value.
		 * @return True when inserted.
		 */
		inline bool addEntry(float x, const core::vector2df& v)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = v.X;
			entry.Value[1] = v.Y;
			return addEntry(entry);
		}

		/**
		 * @brief Add a 3D vector graph entry.
		 * @param x Key position.
		 * @param v Vector value.
		 * @return True when inserted.
		 */
		inline bool addEntry(float x, const core::vector3df& v)
		{
			SInterpolatorEntry entry;
			entry.X = x;
			entry.Value[0] = v.X;
			entry.Value[1] = v.Y;
			entry.Value[2] = v.Z;
			return addEntry(entry);
		}

		/**
		 * @brief Add a color graph entry.
		 * @param x Key position.
		 * @param c Color value.
		 * @return True when inserted.
		 */
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

		/**
		 * @brief Remove all graph entries and control points.
		 */
		inline void clearGraph()
		{
			m_graph.clear();

			for (int i = 0; i < 4; i++)
				m_controls[i].clear();
		}

		/**
		 * @brief Generate graph entries for a layer from its Bezier control points.
		 * @param layer Channel index [0, 3].
		 * @param bezierStep Number of subdivisions per Bezier segment.
		 */
		void generateGraph(int layer, int bezierStep);

		/**
		 * @brief Compute Bezier polyline segments for one layer.
		 * @param layer Channel index [0, 3].
		 * @param lines Receives generated line segments.
		 * @param bezierStep Number of subdivisions per Bezier segment.
		 */
		void computeLine(int layer, std::vector<ArrayPoint2df>& lines, int bezierStep);

		/**
		 * @brief Compute sampled points for the Bezier segment between two control points.
		 * @param p1 First control point.
		 * @param p2 Second control point.
		 * @param points Receives sampled points.
		 * @param bezierStep Number of subdivisions.
		 */
		void computeBezier(const SControlPoint& p1, const SControlPoint& p2, ArrayPoint2df& points, int bezierStep);
	};
}
