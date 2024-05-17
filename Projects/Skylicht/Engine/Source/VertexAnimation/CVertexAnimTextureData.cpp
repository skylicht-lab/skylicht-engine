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
#include "CVertexAnimTextureData.h"
#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CVertexAnimTextureData);

	CVertexAnimTextureData::CVertexAnimTextureData() :
		PositionTexture(NULL),
		NormalTexture(NULL),
		FrameCount(0),
		VertexCount(0),
		PositionData(NULL),
		NormalData(NULL)
	{

	}

	CVertexAnimTextureData::~CVertexAnimTextureData()
	{
		freeTextureData();
	}

	void CVertexAnimTextureData::allocFrames(u32 numVertex, u32 frames)
	{
		if (numVertex == 0 || frames == 0 || numVertex > 4096 || frames > 4096)
		{
			char log[512];
			sprintf(log, "[CVertexAnimTextureData] allocFrames failed - %d - %d!", numVertex, frames);
			os::Printer::log(log);
			return;
		}

		// fix: OpenGL texture size
		u32 tw = 0;
		while (tw < numVertex)
		{
			tw = tw + 64;
		}
		numVertex = tw;

		core::vector3df* newPositionData = new core::vector3df[numVertex * frames];
		core::vector3df* newNormalData = new core::vector3df[numVertex * frames];

		if (PositionData && VertexCount == numVertex)
		{
			u32 frameMin = core::min_(frames, FrameCount);
			for (u32 i = 0; i < frameMin; i++)
			{
				for (u32 j = 0; j < numVertex; j++)
				{
					u32 id = i * numVertex + j;
					newPositionData[id] = PositionData[id];
					newNormalData[id] = NormalData[id];
				}
			}
		}

		freeTextureData();
		FrameCount = frames;
		VertexCount = numVertex;
		PositionData = newPositionData;
		NormalData = newNormalData;
	}

	void CVertexAnimTextureData::freeTextureData()
	{
		if (PositionData)
		{
			delete[]PositionData;
			PositionData = NULL;
		}

		if (NormalData)
		{
			delete[]NormalData;
			NormalData = NULL;
		}
	}

	void CVertexAnimTextureData::addFrame(u32 frame, CMesh* mesh)
	{
		if (frame >= FrameCount)
			return;

		IMeshBuffer* mb = mesh->getMeshBuffer(0);

		video::IVertexAttribute* attributePos = mb->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_POSITION);
		video::IVertexAttribute* attributeNorm = mb->getVertexDescriptor()->getAttributeBySemantic(video::EVAS_NORMAL);

		IVertexBuffer* vb = mb->getVertexBuffer(0);
		u32 vtxCount = vb->getVertexCount();
		u32 vtxSize = vb->getVertexSize();

		u8* vtx = static_cast<u8*>(vb->getVertices());
		u8* offsetPos = vtx + attributePos->getOffset();
		u8* offsetNorm = vtx + attributeNorm->getOffset();

		core::vector3df* positionData = &PositionData[frame * VertexCount];
		core::vector3df* normalData = &NormalData[frame * VertexCount];

		for (u32 i = 0; i < vtxCount; ++i)
		{
			core::vector3df* pPos = (core::vector3df*)offsetPos;
			core::vector3df* pNorm = (core::vector3df*)offsetNorm;

			// position
			*positionData = *pPos;
			positionData++;

			// normal
			*normalData = *pNorm;
			normalData++;

			offsetPos += vtxSize;
			offsetNorm += vtxSize;
		}
	}

	void CVertexAnimTextureData::buildTexture()
	{
		CTextureManager* textureMgr = CTextureManager::getInstance();
		PositionTexture = textureMgr->createVectorTexture2D("VATPosition", PositionData, VertexCount, FrameCount);
		NormalTexture = textureMgr->createVectorTexture2D("VATNormal", NormalData, VertexCount, FrameCount);
		freeTextureData();
	}
}