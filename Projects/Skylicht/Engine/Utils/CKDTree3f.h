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
	/**
	 * @brief 3D KD-tree for nearest-neighbor lookup of user data.
	 * @ingroup Utilities
	 *
	 * The tree stores raw `void*` payload pointers and does not own or delete the payloads.
	 * Nodes are allocated in fixed-size blocks and released when the tree is cleared.
	 */
	class CKDTree3f
	{
	public:
		/**
		 * @brief Node stored in the KD-tree.
		 */
		struct SKDNode
		{
			/** @brief Position of the node in 3D space. */
			float Pos[3];
			/** @brief User payload associated with the node. */
			void* Data;
			/** @brief Split axis used by this node: 0 = X, 1 = Y, 2 = Z. */
			int Dir;

			/** @brief Child with smaller coordinate on `Dir`. */
			SKDNode* Left;
			/** @brief Child with greater or equal coordinate on `Dir`. */
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
		/**
		 * @brief Construct an empty KD-tree.
		 */
		CKDTree3f();

		/**
		 * @brief Destroy the KD-tree and release all node blocks.
		 */
		virtual ~CKDTree3f();

		/**
		 * @brief Insert a point and payload into the tree.
		 * @param pos Point position.
		 * @param data User payload pointer. Ownership remains with the caller.
		 */
		inline void insert(const core::vector3df& pos, void* data)
		{
			insert(pos.X, pos.Y, pos.Z, data);
		}

		/**
		 * @brief Insert a point and payload into the tree.
		 * @param x X coordinate.
		 * @param y Y coordinate.
		 * @param z Z coordinate.
		 * @param data User payload pointer. Ownership remains with the caller.
		 */
		void insert(float x, float y, float z, void* data);

		/**
		 * @brief Remove all nodes from the tree.
		 *
		 * Payload pointers are not deleted.
		 */
		void clear();

		/**
		 * @brief Find the node nearest to a point.
		 * @param pos Query position.
		 * @return Nearest node, or null when the tree is empty.
		 */
		inline SKDNode* nearest(const core::vector3df& pos)
		{
			return nearest(pos.X, pos.Y, pos.Z);
		}

		/**
		 * @brief Find the node nearest to a point.
		 * @param x Query X coordinate.
		 * @param y Query Y coordinate.
		 * @param z Query Z coordinate.
		 * @return Nearest node, or null when the tree is empty.
		 */
		SKDNode* nearest(float x, float y, float z);

		/**
		 * @brief Find the node nearest to a point.
		 * @param pos Pointer to three floats: X, Y, Z.
		 * @return Nearest node, or null when the tree is empty.
		 */
		SKDNode* nearest(const float* pos);

		/**
		 * @brief Find all nodes within a radius of a point.
		 * @param pos Query position.
		 * @param range Search radius.
		 * @param outResults Receives matching nodes sorted by distance.
		 * @return Number of results.
		 */
		inline int nearestRange(const core::vector3df& pos, float range, core::array<SKDNode*>& outResults)
		{
			return nearestRange(&pos.X, range, outResults);
		}

		/**
		 * @brief Find all nodes within a radius of a point.
		 * @param pos Pointer to three floats: X, Y, Z.
		 * @param range Search radius.
		 * @param outResults Receives matching nodes sorted by distance.
		 * @return Number of results.
		 */
		int nearestRange(const float* pos, float range, core::array<SKDNode*>& outResults);

	private:

		SKDNode* allocNode(float x, float y, float z, void* data, int dir);
	};
}
