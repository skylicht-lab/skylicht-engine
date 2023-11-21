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

#include "CGUIElement.h"
#include "Graphics2D/EntityData/CGUILayoutData.h"

namespace Skylicht
{
	class CGUILayout : public CGUIElement
	{
		friend class CCanvas;
	public:
		CGUILayoutData* m_layoutData;

	protected:
		CGUILayout(CCanvas* canvas, CGUIElement* parent);
		CGUILayout(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

	public:
		virtual ~CGUILayout();

		virtual void render(CCamera* camera);

		inline void setAlign(CGUILayoutData::EAlignType type)
		{
			m_layoutData->AlignType = type;
		}

		inline void setSpacing(float space)
		{
			m_layoutData->Spacing = space;
		}

		inline float getSpacing()
		{
			return m_layoutData->Spacing;
		}

		inline void setFitChildrenSize(bool b)
		{
			m_layoutData->FitChildrenSize = b;
		}

		inline bool isFitChildrenSize()
		{
			return m_layoutData->FitChildrenSize;
		}

		DECLARE_GETTYPENAME(CGUILayout);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);
	};
}