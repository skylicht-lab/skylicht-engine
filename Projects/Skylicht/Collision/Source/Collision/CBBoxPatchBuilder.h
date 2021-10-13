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
	class CBBoxPatchBuilder : public CCollisionBuilder
	{
	protected:
		struct SPatch
		{
			core::aabbox3df BBox;
			int X;
			int Y;
			int Z;
			core::array<CCollisionNode*> Collisions;

			SPatch()
			{
				X = 0;
				Y = 0;
				Z = 0;
			}
		};

		SPatch m_global;
		core::array<SPatch*> m_collisionPatchs;
		core::array<SPatch*> m_patchs;
		core::aabbox3df m_size;
		float m_patchSize;
		int m_numX;
		int m_numY;
		int m_numZ;
	public:
		CBBoxPatchBuilder();

		virtual ~CBBoxPatchBuilder();

		virtual void clear();

		virtual void build();

		int getPatchID(int x, int y, int z);

		SPatch* getPatch(int x, int y, int z);

		void updateNode(CCollisionNode* collision);

		void addNodeToPatch(CCollisionNode* collision);

		void removeNode(CCollisionNode* collision);

	public:

		virtual CGameObject* getObjectWithRay(
			const core::line3d<f32>& ray,
			f32& outBestDistanceSquared,
			core::vector3df& outCollisionPoint,
			core::triangle3df& outTriangle,
			CCollisionNode*& outNode);

		virtual bool getCollisionPoint(
			const core::line3d<f32>& ray,
			f32& outBestDistanceSquared,
			core::vector3df& outIntersection,
			core::triangle3df& outTriangle,
			CCollisionNode*& outNode);

		virtual bool getCollisionPoint(
			const core::vector3df& target,
			const core::vector3df& pos,
			core::vector3df& outPos,
			core::triangle3df& outTri,
			CCollisionNode*& outNode);

	protected:

		void removeNode(CCollisionNode* collision, SPatch* patch);

	};
}