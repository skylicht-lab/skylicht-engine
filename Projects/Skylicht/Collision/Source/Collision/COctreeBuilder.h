/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#pragma once

#include "CCollisionBuilder.h"

namespace Skylicht
{
	class COctreeBuilder : public CCollisionBuilder
	{
	protected:
		COctreeNode* m_root;

		u32 m_minimalPolysPerNode;

		core::array<core::triangle3df*> m_triangles;
		core::array<CCollisionNode*> m_collisions;

	public:
		COctreeBuilder();

		virtual ~COctreeBuilder();

		virtual void build();

		virtual void clear();

	public:

		virtual bool getCollisionPoint(
			const core::line3d<f32>& ray,
			f32& outBestDistanceSquared,
			core::vector3df& outIntersection,
			core::triangle3df& outTriangle,
			CCollisionNode*& outNode);



	protected:

		void constructOctree(COctreeNode* node);

		void getTrianglesFromOctree(
			core::array<core::triangle3df*>& listTriangle,
			core::array<CCollisionNode*>& listCollisions,
			COctreeNode* node,
			const core::vector3df& midLine,
			const core::vector3df& lineVect,
			float halfLength,
			const core::aabbox3df& box);
	};
}