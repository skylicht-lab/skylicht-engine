/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include <vector>

namespace Skylicht
{
	class CKDTree3f
	{
	public:
		struct SKDNode
		{
			float Pos[3];
			void* Data;
			int Dir;

			SKDNode* Left;
			SKDNode* Right;
		};

	protected:
		SKDNode* m_root;

	private:
		// Simple block allocator to reduce per-node new/delete cost
		std::vector<SKDNode*> m_blocks;
		SKDNode* m_currentBlock;
		size_t m_blockIndex;
		static constexpr size_t NODE_BLOCK_SIZE = 1024;

		core::array<SKDNode*> m_stack;
	public:
		CKDTree3f();

		virtual ~CKDTree3f();

		inline void insert(const core::vector3df& pos, void* data)
		{
			insert(pos.X, pos.Y, pos.Z, data);
		}

		void insert(float x, float y, float z, void* data);

		void clear();

		inline SKDNode* nearest(const core::vector3df& pos)
		{
			return nearest(pos.X, pos.Y, pos.Z);
		}

		SKDNode* nearest(float x, float y, float z);

		SKDNode* nearest(const float* pos);

		inline int nearestRange(const core::vector3df& pos, float range, core::array<SKDNode*>& outResults)
		{
			return nearestRange(&pos.X, range, outResults);
		}

		int nearestRange(const float* pos, float range, core::array<SKDNode*>& outResults);

	private:

		SKDNode* allocNode(float x, float y, float z, void* data, int dir);
	};
}