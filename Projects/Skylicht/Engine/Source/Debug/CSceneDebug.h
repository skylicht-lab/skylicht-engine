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

#include "Utils/CGameSingleton.h"

#define MAX_DEBUGGEOMETRY	2000

namespace Skylicht
{
	class CSceneDebug : public CGameSingleton<CSceneDebug>
	{
	public:
		struct SLineDebug
		{
			core::line3df line;
			SColor color;
		};

		struct SLineStripDebug
		{
			core::array<core::vector3df> point;
			SColor color;
		};

		struct S2DRectDebug
		{
			core::rectf rect;
			bool outLine;
			SColor color;
		};

		struct SBoxDebug
		{
			core::aabbox3df box;
			SColor color;
		};

		struct STriDebug
		{
			core::triangle3df tri;
			SColor color;
		};


	protected:
		SLineDebug		m_lines[MAX_DEBUGGEOMETRY];
		SLineStripDebug	m_linestrip[MAX_DEBUGGEOMETRY];

		SBoxDebug		m_boxs[MAX_DEBUGGEOMETRY];
		STriDebug		m_tri[MAX_DEBUGGEOMETRY];

		int			m_nLine;
		int			m_nLineStrip;
		int			m_nBox;
		int			m_nTri;

	public:

		CSceneDebug();
		virtual ~CSceneDebug();

		void addCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color);
		void addEclipse(const core::vector3df& pos, float radiusZ, float radiusX, const core::vector3df& normal, const SColor& color);
		void addTri(const core::triangle3df& tri, const SColor& color);
		void addLine(const core::line3df& line, const SColor& color);
		void addLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color);
		void addLinestrip(const core::array<core::vector3df>& point, const SColor& color);
		void addLinestrip(const std::vector<core::vector3df>& point, const SColor& color);
		void addBoudingBox(const core::aabbox3df& box, const SColor& color);
		void addTransformBBox(const core::aabbox3df& box, const SColor& color, const core::matrix4& mat);
		void addSphere(const core::vector3df& pos, float radius, const SColor& color);
		void addTransform(const core::matrix4& mat, float vectorLength);

		void clear()
		{
			clearLines();
			clearLineStrip();
			clearBoxs();
			clearTri();
		}

		inline void clearLines()
		{
			m_nLine = 0;
		}

		inline void clearLineStrip()
		{
			m_nLineStrip = 0;
		}

		inline void clearBoxs()
		{
			m_nBox = 0;
		}

		inline void clearTri()
		{
			m_nTri = 0;
		}

		inline int getLinesCount()
		{
			return m_nLine;
		}

		inline int getLineStripCount()
		{
			return m_nLineStrip;
		}

		inline int getBoxCount()
		{
			return m_nBox;
		}

		inline int getTriCount()
		{
			return m_nTri;
		}

		inline const SLineDebug& getLine(int i)
		{
			return m_lines[i];
		}

		inline const SLineStripDebug& getLineStrip(int i)
		{
			return m_linestrip[i];
		}

		inline const SBoxDebug& getBox(int i)
		{
			return m_boxs[i];
		}

		inline const STriDebug& getTri(int i)
		{
			return m_tri[i];
		}
	};
}