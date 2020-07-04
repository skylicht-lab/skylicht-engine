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
	void CMeshUtils::copyVertices(IVertexBuffer *srcBuffer, IVertexBuffer *dstBuffer)
	{
		int numVertex = srcBuffer->getVertexCount();
		unsigned char *src = (unsigned char*)srcBuffer->getVertices();
		u32 srcSize = srcBuffer->getVertexSize();

		dstBuffer->set_used(numVertex);
		unsigned char *dst = (unsigned char*)dstBuffer->getVertices();
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
		}

		// change Vertex Descriptor
		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_TANGENTS));

		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			// todo calc tangent & binormal
			CVertexBuffer<video::S3DVertexTangents>* vertexBuffer = (CVertexBuffer<video::S3DVertexTangents>*)buffer->getVertexBuffer(j);

			const u32 vtxCnt = vertexBuffer->getVertexCount();

			u32 i;
			video::S3DVertexTangents* v = (video::S3DVertexTangents*)vertexBuffer->getVertices();

			// (1)
			// Use irrlicht compute			
			IMeshManipulator *mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			mh->recalculateTangents(buffer);

			for (i = 0; i != vtxCnt; ++i)
				v[i].TangentW.set(1.f, 1.f);

			/*
			// Use skylicht compute
			// https://www.marti.works/calculating-tangents-for-your-mesh/
			// https://answers.unity.com/questions/7789/calculating-tangents-vector4.html
			const u32 idxCnt = buffer->getIndexBuffer()->getIndexCount();

			u16* idx16 = NULL;
			u32* idx32 = NULL;

			if (buffer->getIndexBuffer()->getType() == video::EIT_16BIT)
				idx16 = (u16*)buffer->getIndexBuffer()->getIndices();
			else
				idx32 = (u32*)buffer->getIndexBuffer()->getIndices();

			for (i = 0; i != vtxCnt; ++i)
			{
				v[i].Tangent.set(0.f, 0.f, 0.f);
				v[i].Binormal.set(0.f, 0.f, 0.f);
				v[i].TangentW.set(1.f, 1.f);
			}

			// (1)
			for (i = 0; i < idxCnt; i += 3)
			{
				int i1 = 0;
				int i2 = 0;
				int i3 = 0;

				if (idx16 != NULL)
				{
					i1 = idx16[i + 0];
					i2 = idx16[i + 1];
					i3 = idx16[i + 2];
				}
				else
				{
					i1 = idx32[i + 0];
					i2 = idx32[i + 1];
					i3 = idx32[i + 2];
				}

				// if this triangle is degenerate, skip it!
				if (v[i1].Pos == v[i2].Pos ||
					v[i1].Pos == v[i3].Pos ||
					v[i2].Pos == v[i3].Pos)
				{
					continue;
				}

				core::vector3df v1 = v[i1].Pos;
				core::vector3df v2 = v[i2].Pos;
				core::vector3df v3 = v[i3].Pos;

				core::vector2df w1 = v[i1].TCoords;
				core::vector2df w2 = v[i2].TCoords;
				core::vector2df w3 = v[i3].TCoords;

				float x1 = v2.X - v1.X;
				float x2 = v3.X - v1.X;
				float y1 = v2.Y - v1.Y;
				float y2 = v3.Y - v1.Y;
				float z1 = v2.Z - v1.Z;
				float z2 = v3.Z - v1.Z;

				float s1 = w2.X - w1.X;
				float s2 = w3.X - w1.X;
				float t1 = w2.Y - w1.Y;
				float t2 = w3.Y - w1.Y;

				float div = s1 * t2 - s2 * t1;
				float r = div == 0.0f ? 0.0f : 1.0f / div;

				// float r = 1.0f / (s1 * t2 - s2 * t1);	// bug if: s1 * t2 - s2 * t1 == 0

				core::vector3df sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				core::vector3df tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				v[i1].Tangent += sdir;
				v[i2].Tangent += sdir;
				v[i3].Tangent += sdir;

				v[i1].Binormal += tdir;
				v[i2].Binormal += tdir;
				v[i3].Binormal += tdir;
			}
			*/

			// (2)
			for (i = 0; i != vtxCnt; ++i)
			{
				core::vector3df n = v[i].Normal;
				core::vector3df t = v[i].Tangent;
				core::vector3df t2 = v[i].Binormal;

				// hard code to fix some error on model
				if (t.getLengthSQ() == 0)
				{
					// char log[512];
					// sprintf(log, "[CMeshUtils] Error compute TBN space at: %d", i);
					// os::Printer::log(log);

					if (t2.getLengthSQ() != 0)
						t = n.crossProduct(t2);
					else
					{
						core::vector3df t1 = n.crossProduct(core::vector3df(0.0f, 0.0f, 1.0f));
						core::vector3df t2 = n.crossProduct(core::vector3df(0.0f, 1.0f, 0.0f));

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

				v[i].Tangent = t - (n * n.dotProduct(t));

				float w = n.crossProduct(t).dotProduct(t2) < 0.0f ? -1.0f : 1.0f;
				v[i].Binormal = n.crossProduct(t);

				// need test this condition				
				if (flipNormal == true)
					v[i].TangentW.X = -w;
				else
					v[i].TangentW.X = w;

				v[i].Tangent.normalize();
				v[i].Binormal.normalize();
				v[i].Normal.normalize();
			}

			// drop reference
			vertexBuffer->drop();
		}
	}

	void CMeshUtils::convertToSkinTangentVertices(IMeshBuffer* buffer, bool flipNormal)
	{
		// replace the buffer
		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			CVertexBuffer<video::S3DVertexSkinTangents>* vertexBuffer = new CVertexBuffer<video::S3DVertexSkinTangents>();

			// copy vertex data
			CMeshUtils::copyVertices(buffer->getVertexBuffer(j), vertexBuffer);

			// replace
			buffer->setVertexBuffer(vertexBuffer, j);
		}

		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_SKIN_TANGENTS));

		for (u32 j = 0; j < buffer->getVertexBufferCount(); ++j)
		{
			// todo calc tangent & binormal
			CVertexBuffer<video::S3DVertexSkinTangents>* vertexBuffer = (CVertexBuffer<video::S3DVertexSkinTangents>*)buffer->getVertexBuffer(j);

			const u32 vtxCnt = vertexBuffer->getVertexCount();

			u32 i;
			video::S3DVertexSkinTangents* v = (video::S3DVertexSkinTangents*)vertexBuffer->getVertices();

			u16* idx16 = NULL;
			u32* idx32 = NULL;

			if (buffer->getIndexBuffer()->getType() == video::EIT_16BIT)
				idx16 = (u16*)buffer->getIndexBuffer()->getIndices();
			else
				idx32 = (u32*)buffer->getIndexBuffer()->getIndices();

			// (1)
			// Use irrlicht compute			
			IMeshManipulator *mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			mh->recalculateTangents(buffer);

			for (i = 0; i != vtxCnt; ++i)
				v[i].TangentW.set(1.f, 1.f);

			/*
			// Use skylicht compute
			// https://www.marti.works/calculating-tangents-for-your-mesh/
			// https://answers.unity.com/questions/7789/calculating-tangents-vector4.html
			const u32 idxCnt = buffer->getIndexBuffer()->getIndexCount();
			for (i = 0; i != vtxCnt; ++i)
			{
				v[i].Tangent.set(0.f, 0.f, 0.f);
				v[i].Binormal.set(0.f, 0.f, 0.f);
				v[i].TangentW.set(1.f, 1.f);
			}

			// https://www.marti.works/calculating-tangents-for-your-mesh/
			// https://answers.unity.com/questions/7789/calculating-tangents-vector4.html
			// (1)
			for (i = 0; i < idxCnt; i += 3)
			{
				int i1 = 0;
				int i2 = 0;
				int i3 = 0;

				if (idx16 != NULL)
				{
					i1 = idx16[i + 0];
					i2 = idx16[i + 1];
					i3 = idx16[i + 2];
				}
				else
				{
					i1 = idx32[i + 0];
					i2 = idx32[i + 1];
					i3 = idx32[i + 2];
				}

				// if this triangle is degenerate, skip it!
				if (v[i1].Pos == v[i2].Pos ||
					v[i1].Pos == v[i3].Pos ||
					v[i2].Pos == v[i3].Pos)
				{
					continue;
				}

				core::vector3df v1 = v[i1].Pos;
				core::vector3df v2 = v[i2].Pos;
				core::vector3df v3 = v[i3].Pos;

				core::vector2df w1 = v[i1].TCoords;
				core::vector2df w2 = v[i2].TCoords;
				core::vector2df w3 = v[i3].TCoords;

				float x1 = v2.X - v1.X;
				float x2 = v3.X - v1.X;
				float y1 = v2.Y - v1.Y;
				float y2 = v3.Y - v1.Y;
				float z1 = v2.Z - v1.Z;
				float z2 = v3.Z - v1.Z;

				float s1 = w2.X - w1.X;
				float s2 = w3.X - w1.X;
				float t1 = w2.Y - w1.Y;
				float t2 = w3.Y - w1.Y;

				float r = 1.0f / (s1 * t2 - s2 * t1);

				core::vector3df sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				core::vector3df tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				v[i1].Tangent += sdir;
				v[i2].Tangent += sdir;
				v[i3].Tangent += sdir;

				v[i1].Binormal += tdir;
				v[i2].Binormal += tdir;
				v[i3].Binormal += tdir;
			}
			*/

			// (2)
			for (i = 0; i != vtxCnt; ++i)
			{
				core::vector3df n = v[i].Normal;
				core::vector3df t = v[i].Tangent;
				core::vector3df t2 = v[i].Binormal;

				v[i].Tangent = (t - n * n.dotProduct(t));

				float w = n.crossProduct(t).dotProduct(t2) < 0.0f ? -1.0f : 1.0f;
				v[i].Binormal = n.crossProduct(t);

				// need test this condition
				if (flipNormal == true)
					v[i].TangentW.X = -w;
				else
					v[i].TangentW.X = w;

				v[i].Tangent.normalize();
				v[i].Binormal.normalize();
				v[i].Normal.normalize();
			}

			// replace the buffer
			buffer->setVertexBuffer(vertexBuffer, j);

			// drop reference
			vertexBuffer->drop();
		}

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
}