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

#include "CRecastMesh.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"
#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	namespace Graph
	{
		class CRecastBuilder
		{
		protected:
			float m_cellSize;
			float m_cellHeight;
			float m_agentHeight;
			float m_agentRadius;
			float m_agentMaxClimb;
			float m_agentMaxSlope;
			float m_regionMinSize;
			float m_regionMergeSize;
			float m_edgeMaxLen;
			float m_edgeMaxError;
			float m_vertsPerPoly;
			float m_detailSampleDist;
			float m_detailSampleMaxError;

		public:
			CRecastBuilder();

			virtual ~CRecastBuilder();

			bool build(CRecastMesh* mesh, CMesh* output, CObstacleAvoidance* obstacle);
		};
	}
}