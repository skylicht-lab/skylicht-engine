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

#include "pch.h"
#include "COcclusionQueryData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(COcclusionQueryData);

	COcclusionQueryData::COcclusionQueryData() :
		m_node(NULL),
		m_registerQuery(false),
		NeedValidate(true),
		QueryResult(0),
		QueryVisible(false)
	{
		float scale = 0.5f;
		m_box.MinEdge.set(-scale, -scale, -scale);
		m_box.MaxEdge.set(scale, scale, scale);

		// Note:
		// Use scene-node for compatibility with some functions OcclusionQuery from Irrlicht
		ISceneManager* smgr = getIrrlichtDevice()->getSceneManager();
		m_node = new COcclusionQuerySceneNode(smgr->getRootSceneNode(), smgr, this);

		updateLocalTransform();
	}

	COcclusionQueryData::~COcclusionQueryData()
	{
		if (m_registerQuery)
			getVideoDriver()->removeOcclusionQuery(m_node);

		m_node->remove();
		m_node->drop();
	}

	void COcclusionQueryData::setAABBox(const core::aabbox3df& box)
	{
		m_box = box;
		updateLocalTransform();
	}

	void COcclusionQueryData::updateLocalTransform()
	{
		m_node->BBox = m_box;

		m_localTransform.makeIdentity();
		m_localTransform.setScale(m_box.getExtent());
		m_localTransform.setTranslation(m_box.getCenter());

		NeedValidate = true;
	}

	void COcclusionQueryData::registerQuery(CMesh* mesh)
	{
		getVideoDriver()->addOcclusionQuery(m_node, mesh);
		m_registerQuery = true;
	}
}