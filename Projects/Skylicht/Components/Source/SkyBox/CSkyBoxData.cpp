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
#include "CSkyBoxData.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CSkyBoxData::CSkyBoxData() :
		Front(NULL),
		Left(NULL),
		Back(NULL),
		Right(NULL),
		Top(NULL),
		Bottom(NULL)
	{
		initMesh();
	}

	CSkyBoxData::~CSkyBoxData()
	{
		drop();
	}

	void CSkyBoxData::drop()
	{
		for (int i = 0; i < 6; i++)
		{
			if (MeshBuffer[i])
			{
				MeshBuffer[i]->drop();
				MeshBuffer[i] = NULL;
			}

			if (Material[i])
			{
				delete Material[i];
				Material[i] = NULL;
			}
		}
	}

	void CSkyBoxData::updateTexture()
	{
		Material[0]->setTexture(0, Front);
		Material[1]->setTexture(0, Left);
		Material[2]->setTexture(0, Back);
		Material[3]->setTexture(0, Right);
		Material[4]->setTexture(0, Top);
		Material[5]->setTexture(0, Bottom);

		for (int i = 0; i < 6; i++)
			Material[i]->applyMaterial();
	}

	void CSkyBoxData::initMesh()
	{
		for (int i = 0; i < 6; i++)
		{
			MeshBuffer[i] = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));
			MeshBuffer[i]->setHardwareMappingHint(scene::EHM_NEVER);

			IIndexBuffer* indexBuffer = MeshBuffer[i]->getIndexBuffer();
			indexBuffer->addIndex(3);
			indexBuffer->addIndex(2);
			indexBuffer->addIndex(0);
			indexBuffer->addIndex(1);
			indexBuffer->addIndex(3);
			indexBuffer->addIndex(0);

			Material[i] = new CMaterial("Skybox", "BuiltIn/Shader/Basic/TextureSRGB.xml");
			Material[i]->setZWrite(false);
			Material[i]->addAffectMesh(MeshBuffer[i]);
			Material[i]->applyMaterial();

			SMaterial& mat = MeshBuffer[i]->getMaterial();
			mat.TextureLayer[0].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
			mat.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
		}

		// Ref: Irrlicht source
		// https://sourceforge.net/p/irrlicht/code/HEAD/tree/branches/shader-pipeline/source/Irrlicht/CSkyBoxSceneNode.cpp
		video::ITexture* tex = Front;
		if (!tex) tex = Left;
		if (!tex) tex = Back;
		if (!tex) tex = Right;
		if (!tex) tex = Top;
		if (!tex) tex = Bottom;

		const f32 onepixel = tex ? (1.0f / (tex->getSize().Width * 1.5f)) : 0.0f;
		const f32 t = 1.0f - onepixel;
		const f32 o = 0.0f + onepixel;

		// create front side		
		IVertexBuffer* VertexBuffer = MeshBuffer[0]->getVertexBuffer(0);

		video::S3DVertex Vertex = video::S3DVertex(-1, -1, -1, 0, 0, 1, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, -1, -1, 0, 0, 1, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, -1, 0, 0, 1, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, 1, -1, 0, 0, 1, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		// create left side		
		VertexBuffer = MeshBuffer[1]->getVertexBuffer(0);

		Vertex = video::S3DVertex(1, -1, -1, -1, 0, 0, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, -1, 1, -1, 0, 0, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, 1, -1, -1, 0, 0, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, 1, 1, -1, 0, 0, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		// create back side		
		VertexBuffer = MeshBuffer[2]->getVertexBuffer(0);

		Vertex = video::S3DVertex(1, -1, 1, 0, 0, -1, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, -1, 1, 0, 0, -1, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, 1, 1, 0, 0, -1, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, 1, 0, 0, -1, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		// create right side		
		VertexBuffer = MeshBuffer[3]->getVertexBuffer(0);

		Vertex = video::S3DVertex(-1, -1, 1, 1, 0, 0, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, -1, -1, 1, 0, 0, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, 1, 1, 0, 0, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, -1, 1, 0, 0, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		// create top side		
		VertexBuffer = MeshBuffer[4]->getVertexBuffer(0);

		Vertex = video::S3DVertex(1, 1, 1, 0, -1, 0, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, 1, 0, -1, 0, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, 1, -1, 0, -1, 0, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, 1, -1, 0, -1, 0, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		// create bottom side		
		VertexBuffer = MeshBuffer[5]->getVertexBuffer(0);

		Vertex = video::S3DVertex(-1, -1, 1, 0, 1, 0, video::SColor(255, 255, 255, 255), o, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, -1, 1, 0, 1, 0, video::SColor(255, 255, 255, 255), t, o);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(-1, -1, -1, 0, 1, 0, video::SColor(255, 255, 255, 255), o, t);
		VertexBuffer->addVertex(&Vertex);

		Vertex = video::S3DVertex(1, -1, -1, 0, 1, 0, video::SColor(255, 255, 255, 255), t, t);
		VertexBuffer->addVertex(&Vertex);
	}
}