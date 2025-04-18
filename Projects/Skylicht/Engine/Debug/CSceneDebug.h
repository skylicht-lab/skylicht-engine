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

#include "Utils/CSingleton.h"
#include "TextBillboard/CTextBillboardManager.h"

namespace Skylicht
{
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

	class SKYLICHT_API CSceneDebug
	{
	public:
		DECLARE_SINGLETON(CSceneDebug)

	protected:
		core::array<SLineDebug*> m_lines;
		core::array<SLineStripDebug*> m_linestrip;
		core::array<SBoxDebug*> m_boxs;
		core::array<STriDebug*> m_tri;
		core::array<CRenderTextData*> m_texts;

		CSceneDebug* m_noZDebug;

	public:

		CSceneDebug();
		virtual ~CSceneDebug();

		CSceneDebug* getNoZDebug();

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
		void addPosition(const core::vector3df& pos, float length, const SColor& color);
		void addTransform(const core::matrix4& mat, float vectorLength);
		void addText(const core::vector3df& pos, const char* string, const SColor& color);
		void clearText();

		void clear()
		{
			clearLines();
			clearLineStrip();
			clearBoxs();
			clearTri();
			clearText();
		}

		void clearLines();

		void clearLineStrip();

		void clearBoxs();

		void clearTri();

		inline u32 getLinesCount()
		{
			return m_lines.size();
		}

		inline u32 getLineStripCount()
		{
			return m_linestrip.size();
		}

		inline u32 getBoxCount()
		{
			return m_boxs.size();
		}

		inline u32 getTriCount()
		{
			return m_tri.size();
		}

		inline const SLineDebug& getLine(int i)
		{
			return *m_lines[i];
		}

		inline const SLineStripDebug& getLineStrip(int i)
		{
			return *m_linestrip[i];
		}

		inline const SBoxDebug& getBox(int i)
		{
			return *m_boxs[i];
		}

		inline const STriDebug& getTri(int i)
		{
			return *m_tri[i];
		}
	};
}