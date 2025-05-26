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
#include "CGOcclusionQuery.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGOcclusionQuery);

		DEPENDENT_COMPONENT(COcclusionQuery, CGOcclusionQuery);

		CGOcclusionQuery::CGOcclusionQuery() :
			m_occlusionQuery(NULL)
		{

		}

		CGOcclusionQuery::~CGOcclusionQuery()
		{

		}

		void CGOcclusionQuery::initComponent()
		{
			m_occlusionQuery = m_gameObject->getComponent<COcclusionQuery>();
		}

		void CGOcclusionQuery::updateComponent()
		{
			if (m_occlusionQuery)
			{
				core::matrix4 world = m_gameObject->getTransformEuler()->calcWorldTransform();

				core::aabbox3df bbox = m_occlusionQuery->getAABBox();
				world.transformBoxEx(bbox);

				CHandles::getInstance()->draw3DBox(bbox, SColor(255, 255, 255, 0));
			}
		}
	}
}