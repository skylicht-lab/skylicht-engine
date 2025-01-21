/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "Entity/CEntity.h"
#include "Exporter/ExportResources.h"

#include "COBJMeshFileExporter.h"

namespace Skylicht
{
	COBJMeshFileExporter::COBJMeshFileExporter()
	{

	}

	COBJMeshFileExporter::~COBJMeshFileExporter()
	{

	}

	bool COBJMeshFileExporter::exportModel(CEntity** entities, u32 count, const char* output)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IWriteFile* file = fs->createAndWriteFile(output);
		if (file == NULL)
			return false;

		os::Printer::log("[COBJMeshFileExporter] Writing mesh", file->getFileName());

		// write OBJ MESH header
		core::stringc header = "# exported by Skylicht Engine\n# https://github.com/skylicht-lab/skylicht-engine\n\n\n";
		file->write(header.c_str(), header.size());

		u32 allVertexCount = 1;

		for (u32 i = 0; i < count; i++)
		{
			CEntity* entity = entities[i];
			if (entity->isAlive() == false)
				continue;

			CRenderMeshData* meshData = entity->getData<CRenderMeshData>();
			if (meshData == NULL)
				continue;

			CWorldTransformData* transform = entity->getData<CWorldTransformData>();

			CMesh* mesh = meshData->getMesh();
			if (mesh)
			{
				core::stringc objName = "o ";
				objName += transform->Name.c_str();
				objName += "\n";
				file->write(objName.c_str(), objName.size());

				allVertexCount = writeMesh(file, mesh, transform->World, allVertexCount);
			}
		}

		file->drop();
		return true;
	}

	u32 COBJMeshFileExporter::writeMesh(io::IWriteFile* file, CMesh* mesh, core::matrix4& transform, u32 allVertexCount)
	{
		// write mesh buffers
		// count vertices over the whole file
		for (u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
		{
			core::stringc num(i + 1);

			IMeshBuffer* buffer = mesh->getMeshBuffer(i);
			IVertexBuffer* vertexBuffer = buffer->getVertexBuffer();
			IIndexBuffer* indexBuffer = buffer->getIndexBuffer();

			core::vector3df p, n;

			if (buffer && vertexBuffer->getVertexCount())
			{
				file->write("g grp", 5);
				file->write(num.c_str(), num.size());
				file->write("\n", 1);

				u32 j;
				const u32 vertexCount = vertexBuffer->getVertexCount();
				for (j = 0; j < vertexCount; ++j)
				{
					video::S3DVertex* vtx = (video::S3DVertex*)vertexBuffer->getVertex(j);

					p = vtx->Pos;
					transform.transformVect(p);

					file->write("v ", 2);
					getVectorAsStringLine(p, num);
					file->write(num.c_str(), num.size());
				}

				for (j = 0; j < vertexCount; ++j)
				{
					video::S3DVertex* vtx = (video::S3DVertex*)vertexBuffer->getVertex(j);

					file->write("vt ", 3);
					getVectorAsStringLine(vtx->TCoords, num);
					file->write(num.c_str(), num.size());
				}

				for (j = 0; j < vertexCount; ++j)
				{
					video::S3DVertex* vtx = (video::S3DVertex*)vertexBuffer->getVertex(j);

					n = vtx->Normal;
					transform.rotateVect(n);
					n.normalize();

					file->write("vn ", 3);
					getVectorAsStringLine(n, num);
					file->write(num.c_str(), num.size());
				}

				if (!mesh->MaterialName[i].empty())
				{
					core::stringc materialName = "usemtl ";
					materialName += mesh->MaterialName[i].c_str();
					materialName += "\n";
					file->write(materialName.c_str(), materialName.size());
				}

				const u32 indexCount = indexBuffer->getIndexCount();

				for (j = 0; j < indexCount; j += 3)
				{
					file->write("f ", 2);
					num = core::stringc(indexBuffer->getIndex(j + 2) + allVertexCount);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write(" ", 1);

					num = core::stringc(indexBuffer->getIndex(j + 1) + allVertexCount);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write(" ", 1);

					num = core::stringc(indexBuffer->getIndex(j + 0) + allVertexCount);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write("/", 1);
					file->write(num.c_str(), num.size());
					file->write(" ", 1);

					file->write("\n", 1);
				}
				file->write("\n", 1);
				allVertexCount += vertexCount;
			}
		}

		return allVertexCount;
	}


	void COBJMeshFileExporter::getVectorAsStringLine(const core::vector3df& v, core::stringc& s) const
	{
		s = core::stringc(-v.X);
		s += " ";
		s += core::stringc(v.Y);
		s += " ";
		s += core::stringc(v.Z);
		s += "\n";
	}


	void COBJMeshFileExporter::getVectorAsStringLine(const core::vector2df& v, core::stringc& s) const
	{
		s = core::stringc(v.X);
		s += " ";
		s += core::stringc(1 - v.Y);
		s += "\n";
	}


	void COBJMeshFileExporter::getColorAsStringLine(const video::SColor& color, const c8* const prefix, core::stringc& s) const
	{
		s = prefix;
		s += " ";
		s += core::stringc((double)(color.getRed() / 255.f));
		s += " ";
		s += core::stringc((double)(color.getGreen() / 255.f));
		s += " ";
		s += core::stringc((double)(color.getBlue() / 255.f));
		s += "\n";
	}
}