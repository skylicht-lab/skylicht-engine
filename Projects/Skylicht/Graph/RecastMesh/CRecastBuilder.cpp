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
#include "CRecastBuilder.h"

// Recast library
#include "Recast.h"

namespace Skylicht
{
	namespace Graph
	{
		CRecastBuilder::CRecastBuilder()
		{

		}

		CRecastBuilder::~CRecastBuilder()
		{

		}

		bool CRecastBuilder::build(CRecastMesh* mesh, CMesh* output, CObstacleAvoidance* obstacle)
		{
			rcContext ctx;

			rcContext* m_ctx = &ctx;
			rcCompactHeightfield* m_chf;
			rcContourSet* m_cset;
			rcPolyMesh* m_pmesh;

			// Step 1. Initialize build config.
			rcConfig m_cfg;
			memset(&m_cfg, 0, sizeof(m_cfg));
			m_cfg.cs = m_config.CellSize;
			m_cfg.ch = m_config.CellHeight;
			m_cfg.walkableSlopeAngle = m_config.AgentMaxSlope;

			m_cfg.walkableHeight = (int)ceilf(m_config.AgentHeight / m_cfg.ch);
			m_cfg.walkableClimb = (int)floorf(m_config.AgentMaxClimb / m_cfg.ch);
			m_cfg.walkableRadius = (int)ceilf(m_config.AgentRadius / m_cfg.cs);

			m_cfg.maxEdgeLen = (int)(m_config.EdgeMaxLen / m_config.CellSize);
			m_cfg.maxSimplificationError = m_config.EdgeMaxError;

			m_cfg.minRegionArea = (int)rcSqr(m_config.RegionMinSize); // Note: area = size*size
			m_cfg.mergeRegionArea = (int)rcSqr(m_config.RegionMergeSize); // Note: area = size*size

			m_cfg.maxVertsPerPoly = m_config.VertsPerPoly;

			m_cfg.detailSampleDist = m_config.DetailSampleDist < 0.9f ? 0 : m_config.CellSize * m_config.DetailSampleDist;
			m_cfg.detailSampleMaxError = m_config.CellHeight * m_config.DetailSampleMaxError;

			const core::aabbox3df& box = mesh->getBBox();
			m_cfg.bmin[0] = box.MinEdge.X;
			m_cfg.bmin[1] = box.MinEdge.Y - 0.2f;
			m_cfg.bmin[2] = box.MinEdge.Z;
			m_cfg.bmax[0] = box.MaxEdge.X;
			m_cfg.bmax[1] = box.MaxEdge.Y + 0.2f;
			m_cfg.bmax[2] = box.MaxEdge.Z;
			rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

			// model data
			const float* verts = mesh->getVerts();
			const int nverts = mesh->getVertCount();
			const int* tris = mesh->getTris();
			const int ntris = mesh->getTriCount();

			// Step 2. Rasterize input polygon soup.
			// Allocate voxel heightfield where we rasterize our input data to.
			rcHeightfield* m_solid = rcAllocHeightfield();
			if (!m_solid)
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Out of memory 'solid'.");
				return false;
			}
			if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not create solid heightfield.");
				return false;
			}

			// Allocate array that can hold triangle area types.
			// If you have multiple meshes you need to process, allocate
			// and array which can hold the max number of triangles you need to process.
			unsigned char* m_triareas = new unsigned char[ntris];
			if (!m_triareas)
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Out of memory 'm_triareas'.");
				return false;
			}

			// Find triangles which are walkable based on their slope and rasterize them.
			// If your input data is multiple meshes, you can transform them here, calculate
			// the are type for each of the meshes and rasterize them.
			memset(m_triareas, 0, ntris * sizeof(unsigned char));
			rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
			if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not rasterize triangles.");
				return false;
			}
			delete[] m_triareas;
			m_triareas = 0;

			//
			// Step 3. Filter walkable surfaces.
			//

			// Once all geometry is rasterized, we do initial pass of filtering to
			// remove unwanted overhangs caused by the conservative rasterization
			// as well as filter spans where the character cannot possibly stand.

			// if (m_filterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
			// if (m_filterLedgeSpans)
			rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
			// if (m_filterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

			//
			// Step 4. Partition walkable surface to simple regions.
			//

			// Compact the heightfield so that it is faster to handle from now on.
			// This will result more cache coherent data as well as the neighbours
			// between walkable cells will be calculated.
			m_chf = rcAllocCompactHeightfield();
			if (!m_chf)
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Out of memory 'chf'.");
				return false;
			}
			if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not build compact data.");
				return false;
			}
			rcFreeHeightField(m_solid);
			m_solid = 0;

			// Erode the walkable area by agent radius.
			if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not erode.");
				return false;
			}

			// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
			{
				// Prepare for region partitioning, by calculating distance field along the walkable surface.
				if (!rcBuildDistanceField(m_ctx, *m_chf))
				{
					os::Printer::log("[CRecastBuilder] buildNavigation: Could not build distance field.");
					return false;
				}

				// Partition the walkable surface into simple regions without holes.
				if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
				{
					os::Printer::log("[CRecastBuilder] buildNavigation: Could not build watershed regions.");
					return false;
				}
			}

			// Step 5. Trace and simplify region contours.
			// Create contours.
			m_cset = rcAllocContourSet();
			if (!m_cset)
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Out of memory 'cset'.");
				return false;
			}
			if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not create contours.");
				return false;
			}

			//
			// Step 6. Build polygons mesh from contours.
			//

			// Build polygon navmesh from the contours.
			m_pmesh = rcAllocPolyMesh();
			if (!m_pmesh)
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Out of memory 'pmesh'.");
				return false;
			}
			if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
			{
				os::Printer::log("[CRecastBuilder] buildNavigation: Could not triangulate contours.");
				return false;
			}

			// Write output
			output->removeAllMeshBuffer();
			IVideoDriver* driver = getVideoDriver();

			CMeshBuffer<S3DVertex>* buffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), video::EIT_16BIT);

			IIndexBuffer* ib = buffer->getIndexBuffer();
			IVertexBuffer* vb = buffer->getVertexBuffer();

			const int nvp = m_pmesh->nvp;
			const float cs = m_pmesh->cs;
			const float ch = m_pmesh->ch;
			const float* orig = m_pmesh->bmin;

			S3DVertex vtx;
			for (int i = 0; i < m_pmesh->nverts; ++i)
			{
				const unsigned short* v = &m_pmesh->verts[i * 3];
				vtx.Pos.X = orig[0] + v[0] * cs;
				vtx.Pos.Y = orig[1] + v[1] * ch;
				vtx.Pos.Z = orig[2] + v[2] * cs;
				vb->addVertex(&vtx);
			}

			for (int i = 0; i < m_pmesh->npolys; ++i)
			{
				const unsigned short* p = &m_pmesh->polys[i * nvp * 2];

				unsigned short vi[3];
				for (int j = 2; j < nvp; ++j)
				{
					if (p[j] == RC_MESH_NULL_IDX)
						break;

					vi[0] = p[0];
					vi[1] = p[j - 1];
					vi[2] = p[j];

					for (int k = 0; k < 3; ++k)
					{
						ib->addIndex(vi[k]);
					}
				}
			}

			IMeshManipulator* meshManipulator = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			meshManipulator->recalculateNormals(buffer, true);

			buffer->recalculateBoundingBox();

			output->addMeshBuffer(buffer, "default");
			output->recalculateBoundingBox();

			buffer->drop();

			obstacle->clear();
			core::vector3df seg[2];

			// Get boundary edges
			for (int i = 0; i < m_pmesh->npolys; ++i)
			{
				const unsigned short* p = &m_pmesh->polys[i * nvp * 2];
				for (int j = 0; j < nvp; ++j)
				{
					if (p[j] == RC_MESH_NULL_IDX)
						break;

					if ((p[nvp + j] & 0x8000) == 0)
						continue;

					const int nj = (j + 1 >= nvp || p[j + 1] == RC_MESH_NULL_IDX) ? 0 : j + 1;
					const int vi[2] = { p[j], p[nj] };

					for (int k = 0; k < 2; ++k)
					{
						const unsigned short* v = &m_pmesh->verts[vi[k] * 3];
						seg[k].X = orig[0] + v[0] * cs;
						seg[k].Y = orig[1] + v[1] * ch;
						seg[k].Z = orig[2] + v[2] * cs;
					}

					obstacle->addSegment(seg[0], seg[1]);
				}
			}

			// Clean data
			rcFreeCompactHeightfield(m_chf);
			m_chf = 0;
			rcFreeContourSet(m_cset);
			m_cset = 0;
			rcFreePolyMesh(m_pmesh);
			m_pmesh = 0;
			return true;
		}
	}
}
