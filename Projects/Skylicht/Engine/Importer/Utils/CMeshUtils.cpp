/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CMeshUtils.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	void CMeshUtils::copyVertices(IVertexBuffer* srcBuffer, IVertexBuffer* dstBuffer)
	{
		int numVertex = srcBuffer->getVertexCount();
		unsigned char* src = (unsigned char*)srcBuffer->getVertices();
		u32 srcSize = srcBuffer->getVertexSize();

		dstBuffer->set_used(numVertex);
		unsigned char* dst = (unsigned char*)dstBuffer->getVertices();
		u32 dstSize = dstBuffer->getVertexSize();

		// cannot copy
		if (srcSize > dstSize)
			return;

		// copy vertex data
		for (int i = 0; i < numVertex; i++)
		{
			memset(dst, 0, dstSize);
			memcpy(dst, src, srcSize);
			dst += dstSize;
			src += srcSize;
		}
	}

	void CMeshUtils::convertToTangentVertices(IMeshBuffer* buffer, bool flipNormal)
	{
		// replace the buffer
		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			CVertexBuffer<video::S3DVertexTangents>* vertexBuffer = new CVertexBuffer<video::S3DVertexTangents>();

			// copy vertex data
			CMeshUtils::copyVertices(buffer->getVertexBuffer(j), vertexBuffer);

			// replace
			buffer->setVertexBuffer(vertexBuffer, j);

			vertexBuffer->drop();
		}

		// change Vertex Descriptor
		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_TANGENTS));

		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			mh->recalculateTangents(buffer);

			CVertexBuffer<video::S3DVertexTangents>* vertexBuffer = (CVertexBuffer<video::S3DVertexTangents>*)buffer->getVertexBuffer(j);
			const u32 vtxCnt = vertexBuffer->getVertexCount();
			video::S3DVertexTangents* v = (video::S3DVertexTangents*)vertexBuffer->getVertices();

			for (u32 i = 0; i != vtxCnt; ++i)
				v[i].VertexData.set(1.0f, (float)i);
		}

		updateTangentBinormal(buffer, flipNormal);
	}

	void CMeshUtils::convertToSkinTangentVertices(IMeshBuffer* buffer, bool flipNormal)
	{
		// replace the buffer
		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			CVertexBuffer<video::S3DVertexSkinTangents>* vertexBuffer = new CVertexBuffer<video::S3DVertexSkinTangents>();
			CMeshUtils::copyVertices(buffer->getVertexBuffer(j), vertexBuffer);
			buffer->setVertexBuffer(vertexBuffer, j);
			vertexBuffer->drop();
		}

		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_SKIN_TANGENTS));

		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			mh->recalculateTangents(buffer);

			CVertexBuffer<video::S3DVertexSkinTangents>* vertexBuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)buffer->getVertexBuffer(j);
			const u32 vtxCnt = vertexBuffer->getVertexCount();
			video::S3DVertexSkinTangents* v = (video::S3DVertexSkinTangents*)vertexBuffer->getVertices();

			for (u32 i = 0; i != vtxCnt; ++i)
				v[i].VertexData.set(1.0f, (float)i);
		}

		updateTangentBinormal(buffer);

		// assign skin material
		buffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("Skin");
	}

	void CMeshUtils::convertToSkinVertices(IMeshBuffer* buffer)
	{
		// change vertex descriptor
		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_SKIN));

		for (u32 i = 0; i < buffer->getVertexBufferCount(); ++i)
		{
			CVertexBuffer<video::S3DVertexSkin>* vertexBuffer = new CVertexBuffer<video::S3DVertexSkin>();

			// copy vertex data
			CMeshUtils::copyVertices(buffer->getVertexBuffer(i), vertexBuffer);

			// replace the buffer
			buffer->setVertexBuffer(vertexBuffer, i);

			// drop reference
			vertexBuffer->drop();
		}

		// assign skin material
		buffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("Skin");
	}

	void CMeshUtils::updateTangentBinormal(IMeshBuffer* buffer, bool flipNormal)
	{
		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			IVertexBuffer* vb = buffer->getVertexBuffer(j);
			video::E_VERTEX_TYPE vtType = buffer->getVertexType();

			bool haveUV2 = false;

			if (vtType != video::EVT_TANGENTS &&
				vtType != video::EVT_SKIN_TANGENTS &&
				vtType != video::EVT_2TCOORDS_TANGENTS &&
				vtType != video::EVT_SKIN_2TCOORDS_TANGENTS)
				continue;

			if (vtType == video::EVT_2TCOORDS_TANGENTS ||
				vtType == video::EVT_SKIN_2TCOORDS_TANGENTS)
				haveUV2 = true;

			updateTangentBinormal(vb, haveUV2, flipNormal);
		}
	}

	void CMeshUtils::updateTangentBinormal(IVertexBuffer* vb, bool haveUV2, bool flipNormal)
	{
		// todo calc tangent & binormal
		const u32 vtxCnt = vb->getVertexCount();

		u32 i;
		u32 vtxSize = vb->getVertexSize();
		unsigned char* vtx = (unsigned char*)vb->getVertices();

		core::vector3df* n;
		core::vector3df* tangent;
		core::vector3df* binormal;
		core::vector2df* vdata;

		for (i = 0; i < vtxCnt; ++i)
		{
			S3DVertexTangents* vt1 = (S3DVertexTangents*)vtx;
			S3DVertex2TCoordsTangents* vt2 = (S3DVertex2TCoordsTangents*)vtx;

			n = haveUV2 ? &vt2->Normal : &vt1->Normal;
			tangent = haveUV2 ? &vt2->Tangent : &vt1->Tangent;
			binormal = haveUV2 ? &vt2->Binormal : &vt1->Binormal;
			vdata = haveUV2 ? &vt2->VertexData : &vt1->VertexData;

			core::vector3df t = *tangent;
			core::vector3df t2 = *binormal;

			// hard code to fix some error on model
			if (t.getLengthSQ() == 0.0f)
			{
				if (t2.getLengthSQ() != 0.0f)
					t = n->crossProduct(t2);
				else
				{
					core::vector3df t1 = n->crossProduct(core::vector3df(0.0f, 0.0f, 1.0f));
					core::vector3df t2 = n->crossProduct(core::vector3df(0.0f, 1.0f, 0.0f));

					if (t1.getLength() > t2.getLength())
					{
						t = t1;
					}
					else
					{
						t = t2;
					}
				}
			}

			*tangent = t - (*n * n->dotProduct(t));

			float w = n->crossProduct(t).dotProduct(t2) < 0.0f ? -1.0f : 1.0f;
			*binormal = n->crossProduct(t);

			// need test this condition
			vdata->X = flipNormal ? -w : w;

			tangent->normalize();
			binormal->normalize();

			vtx += vtxSize;
		}
	}
}