/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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

#include "Entity/IEntityData.h"

namespace Skylicht
{
	enum class EGUIVerticalAlign
	{
		Top,
		Middle,
		Bottom
	};

	enum class EGUIHorizontalAlign
	{
		Left,
		Center,
		Right
	};

	enum class EGUIDock
	{
		NoDock,
		DockLeft,
		DockRight,
		DockTop,
		DockBottom,
		DockFill
	};

	struct SMargin
	{
		float Left;
		float Top;
		float Right;
		float Bottom;

		SMargin()
		{
			Left = 0.0f;
			Top = 0.0f;
			Right = 0.0f;
			Bottom = 0.0f;
		}

		SMargin(float l, float t, float r, float b)
		{
			Left = l;
			Top = t;
			Right = r;
			Bottom = b;
		}
	};

	class SKYLICHT_API CGUIAlignData : public IEntityData
	{
		friend class CGUILayoutSystem;

	public:

		EGUIDock Dock;

		SMargin Margin;

		EGUIVerticalAlign Vertical;

		EGUIHorizontalAlign Horizontal;

	public:
		CGUIAlignData();

		virtual ~CGUIAlignData();

		DECLARE_GETTYPENAME(CGUIAlignData)
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(CGUIAlignData);
}