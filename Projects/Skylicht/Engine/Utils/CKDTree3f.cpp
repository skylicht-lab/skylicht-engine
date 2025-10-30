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

#include "pch.h"
#include "CKDTree3f.h"

#include <vector>
#include <limits>
#include <algorithm>

namespace Skylicht
{
	CKDTree3f::CKDTree3f() :
		m_root(NULL),
		m_currentBlock(nullptr),
		m_blockIndex(0)
	{

	}

	CKDTree3f::~CKDTree3f()
	{
		clear();
	}

	CKDTree3f::SKDNode* CKDTree3f::allocNode(float x, float y, float z, void* data, int dir)
	{
		if (m_currentBlock == nullptr || m_blockIndex >= NODE_BLOCK_SIZE)
		{
			SKDNode* block = new SKDNode[NODE_BLOCK_SIZE];
			m_blocks.push_back(block);
			m_currentBlock = block;
			m_blockIndex = 0;
		}

		SKDNode* node = &m_currentBlock[m_blockIndex++];
		node->Pos[0] = x;
		node->Pos[1] = y;
		node->Pos[2] = z;
		node->Data = data;
		node->Dir = dir;
		node->Left = nullptr;
		node->Right = nullptr;
		return node;
	}

	void CKDTree3f::insert(float x, float y, float z, void* data)
	{
		if (!m_root)
		{
			m_root = allocNode(x, y, z, data, 0);
			return;
		}

		float pos[3] = { x, y, z };
		SKDNode* node = m_root;
		SKDNode* parent = nullptr;

		while (node)
		{
			parent = node;
			if (pos[node->Dir] < node->Pos[node->Dir])
				node = node->Left;
			else
				node = node->Right;
		}

		int childDir = (parent->Dir + 1) % 3;
		SKDNode* newNode = allocNode(x, y, z, data, childDir);
		if (pos[parent->Dir] < parent->Pos[parent->Dir])
			parent->Left = newNode;
		else
			parent->Right = newNode;
	}

	void CKDTree3f::clear()
	{
		// release blocks
		for (SKDNode* b : m_blocks)
			delete[] b;
		m_blocks.clear();
		m_currentBlock = nullptr;
		m_blockIndex = 0;
		m_root = NULL;
	}

	CKDTree3f::SKDNode* CKDTree3f::nearest(float x, float y, float z)
	{
		float pos[3] = { x, y, z };
		return nearest(pos);
	}

	CKDTree3f::SKDNode* CKDTree3f::nearest(const float* pos)
	{
		if (!m_root)
			return NULL;

		// Start with root as best so far
		SKDNode* best = m_root;
		float dx = best->Pos[0] - pos[0];
		float dy = best->Pos[1] - pos[1];
		float dz = best->Pos[2] - pos[2];
		float bestDistSq = dx * dx + dy * dy + dz * dz;

		// stack for farther branches
		m_stack.set_used(0);

		SKDNode* node = m_root;
		while (node)
		{
			// visit node
			float d0 = node->Pos[0] - pos[0];
			float d1 = node->Pos[1] - pos[1];
			float d2 = node->Pos[2] - pos[2];
			float distSq = d0 * d0 + d1 * d1 + d2 * d2;
			if (distSq < bestDistSq)
			{
				bestDistSq = distSq;
				best = node;
			}

			int dir = node->Dir;
			float delta = pos[dir] - node->Pos[dir];

			SKDNode* nearer = nullptr;
			SKDNode* farther = nullptr;
			if (delta < 0.0f)
			{
				nearer = node->Left;
				farther = node->Right;
			}
			else
			{
				nearer = node->Right;
				farther = node->Left;
			}

			// Push farther branch if sphere intersects splitting plane
			if (farther && (delta * delta) < bestDistSq)
				m_stack.push_back(farther);

			// go down nearer subtree
			node = nearer;

			// if reached leaf, pop next branch
			if (!node && !m_stack.empty())
			{
				node = m_stack.getLast();
				m_stack.set_used(m_stack.size() - 1);
			}
		}

		return best;
	}

	int CKDTree3f::nearestRange(const float* pos, float range, core::array<SKDNode*>& outResults)
	{
		outResults.clear();
		if (!m_root)
			return 0;

		float rangeSq = range * range;

		m_stack.set_used(0);
		m_stack.push_back(m_root);

		// collect nodes
		while (!m_stack.empty())
		{
			SKDNode* node = m_stack.getLast();
			m_stack.set_used(m_stack.size() - 1);

			float d0 = node->Pos[0] - pos[0];
			float d1 = node->Pos[1] - pos[1];
			float d2 = node->Pos[2] - pos[2];
			float distSq = d0 * d0 + d1 * d1 + d2 * d2;

			if (distSq <= rangeSq)
			{
				outResults.push_back(node);
			}

			int dir = node->Dir;
			float delta = pos[dir] - node->Pos[dir];

			SKDNode* nearNode = (delta < 0.0f) ? node->Left : node->Right;
			SKDNode* farNode = (delta < 0.0f) ? node->Right : node->Left;

			if (nearNode)
				m_stack.push_back(nearNode);

			// far subtree only needed if splitting plane is within range
			if (farNode && (delta * delta) <= rangeSq)
				m_stack.push_back(farNode);
		}

		if (outResults.empty())
			return 0;

		CKDTree3f::SKDNode** begin = outResults.pointer();
		CKDTree3f::SKDNode** end = begin + outResults.size();

		// sort by distance ascending (computed relative to pos)
		std::sort(begin, end, [pos](SKDNode* a, SKDNode* b) {
			float ad0 = a->Pos[0] - pos[0]; float ad1 = a->Pos[1] - pos[1]; float ad2 = a->Pos[2] - pos[2];
			float bd0 = b->Pos[0] - pos[0]; float bd1 = b->Pos[1] - pos[1]; float bd2 = b->Pos[2] - pos[2];
			float da = ad0 * ad0 + ad1 * ad1 + ad2 * ad2;
			float db = bd0 * bd0 + bd1 * bd1 + bd2 * bd2;
			return da < db;
			});

		return outResults.size();
	}
}