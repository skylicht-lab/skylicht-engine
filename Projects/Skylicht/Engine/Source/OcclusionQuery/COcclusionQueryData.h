/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "RenderMesh/CMesh.h"
#include "COcclusionQuerySceneNode.h"

namespace Skylicht
{
	class SKYLICHT_API COcclusionQueryData : public IEntityData
	{
	protected:
		core::aabbox3df m_box;

		core::matrix4 m_localTransform;

		COcclusionQuerySceneNode* m_node;

		bool m_registerQuery;

	public:

		bool NeedValidate;

		u32 QueryResult;

		bool QueryVisible;

	public:
		COcclusionQueryData();

		virtual ~COcclusionQueryData();

		void setAABBox(const core::aabbox3df& box);

		void updateLocalTransform();

		inline const core::matrix4& getLocalTransform()
		{
			return m_localTransform;
		}

		inline COcclusionQuerySceneNode* getSceneNode()
		{
			return m_node;
		}

		inline bool isRegisterQuery()
		{
			return m_registerQuery;
		}

		void registerQuery(CMesh* mesh);

		DECLARE_GETTYPENAME(COcclusionQueryData)
	};

	DECLARE_PUBLIC_DATA_TYPE_INDEX(COcclusionQueryData);
}