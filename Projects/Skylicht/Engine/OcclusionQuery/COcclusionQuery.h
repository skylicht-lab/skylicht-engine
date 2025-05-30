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

#include "Components/CComponentSystem.h"
#include "COcclusionQueryData.h"

namespace Skylicht
{
	class SKYLICHT_API COcclusionQuery : public CComponentSystem
	{
	protected:
		COcclusionQueryData* m_queryData;

	public:
		COcclusionQuery();

		virtual ~COcclusionQuery();

		virtual void initComponent();

		virtual void updateComponent();

		virtual void onEnable(bool b);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		inline void setAABBox(const core::aabbox3df& box)
		{
			m_queryData->setAABBox(box);
		}

		inline const core::aabbox3df& getAABBox()
		{
			return m_queryData->getAABBox();
		}

		inline u32 getResult()
		{
			return m_queryData->QueryResult;
		}

		inline bool getVisible()
		{
			return m_queryData->QueryVisible;
		}

		DECLARE_GETTYPENAME(COcclusionQuery)
	};
}