/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CFBXMeshLoader.h"
#include "CFBXLoaderFunc.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityPrefab.h"
#include "Exporter/ExportResources.h"

#include "Utils/CMemoryStream.h"
#include "Utils/CActivator.h"
#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

#include "Transform/CWorldTransformData.h"
#include "ufbx.h"

#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CCullingData.h"

namespace Skylicht
{
	CFBXMeshLoader::CFBXMeshLoader()
	{

	}

	CFBXMeshLoader::~CFBXMeshLoader()
	{

	}

	bool CFBXMeshLoader::loadModel(const char* resource, CEntityPrefab* output, bool normalMap, bool flipNormalMap, bool texcoord2, bool batching)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IReadFile* file = fs->createAndOpenFile(resource);
		if (file == NULL)
		{
			char log[64];
			sprintf(log, "Can not load: %s\n", resource);
			os::Printer::log(log);
			return false;
		}

		const long filesize = file->getSize();
		if (!filesize)
			return 0;

		c8* buf = new c8[filesize];
		memset(buf, 0, filesize);
		file->read((void*)buf, filesize);

		ufbx_load_opts opts;
		memset(&opts, 0, sizeof(ufbx_load_opts));

		opts.load_external_files = true;
		opts.allow_null_material = true;
		opts.evaluate_skinning = true;

		opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
		opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
		opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;

		opts.target_unit_meters = 1.0f;

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_memory(buf, filesize, &opts, &error);
		if (!scene)
		{
			os::Printer::log("Failed to load scene");
			delete buf;
			file->drop();
			return false;
		}

		std::map<ufbx_node*, CEntity*> mapNodes;

		CEntity* root = output->createEntity();
		std::string modelName = CPath::getFileName(std::string(resource));

		output->addTransformData(root, NULL, core::IdentityMatrix, modelName.c_str());

		// import scene
		for (int i = 0; i < scene->nodes.count; i++)
		{
			ufbx_node* node = scene->nodes[i];

			CEntity* parent = root;
			if (node->parent)
				parent = mapNodes[node->parent];

			CEntity* entity = output->createEntity();

			output->addTransformData(entity, parent, convertFBXMatrix(node->node_to_parent), node->name.data);

			mapNodes[node] = entity;
		}

		ufbx_vec2 default_uv = { 0 };
		ufbx_vec3 default_vec = { 0 };

		CShaderManager* shaderMgr = CShaderManager::getInstance();

		for (int i = 0; i < scene->meshes.count; i++)
		{
			CMesh* resultMesh = new CMesh();

			ufbx_mesh* mesh = scene->meshes[i];

			bool haveTangent = mesh->vertex_tangent.num_values > 0;
			bool haveBitangent = mesh->vertex_bitangent.num_values > 0;
			bool haveUV = mesh->vertex_uv.num_values > 0;

			std::string meshName = mesh->name.data;
			bool isSkinnedMesh = false;

			if (mesh->skin_deformers.count > 0)
				isSkinnedMesh = true;

			// temp buffer
			size_t num_tri_indices = mesh->max_face_triangles * 3;
			uint32_t* tri_indices = new uint32_t[num_tri_indices];

			for (int j = 0; j < mesh->materials.count; j++)
			{
				ufbx_mesh_material* mesh_mat = &mesh->materials.data[j];
				if (mesh_mat->num_triangles == 0)
					continue;

				IMeshBuffer* mb = NULL;
				video::E_INDEX_TYPE indexType = video::EIT_16BIT;

				u32 numVertex = mesh_mat->num_triangles * 3;
				if (numVertex >= USHRT_MAX - 1)
					indexType = video::EIT_32BIT;

				if (normalMap)
				{
					mb = new CMeshBuffer<video::S3DVertexTangents>(
						getVideoDriver()->getVertexDescriptor(EVT_TANGENTS),
						indexType
						);
				}
				else
				{
					mb = new CMeshBuffer<video::S3DVertex>(
						getVideoDriver()->getVertexDescriptor(EVT_STANDARD),
						indexType
						);
				}

				// add mesh buffer
				char materialName[512];
				if (mesh_mat->material &&
					mesh_mat->material->name.data &&
					CStringImp::length(mesh_mat->material->name.data))
				{
					CStringImp::copy(materialName, mesh_mat->material->name.data);
				}
				else
				{
					sprintf(materialName, "material_%d", j);
				}

				resultMesh->addMeshBuffer(mb, materialName);

				IVertexBuffer* vertexBuffer = mb->getVertexBuffer();
				IIndexBuffer* indexBuffer = mb->getIndexBuffer();

				core::map<uint32_t, u32> vertMap;

				for (size_t fi = 0; fi < mesh_mat->num_faces; fi++)
				{
					int32_t faceID = mesh_mat->face_indices[fi];
					ufbx_face face = mesh->faces[faceID];

					size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

					for (size_t vi = 0; vi < num_tris * 3; vi++)
					{
						uint32_t ix = tri_indices[vi];

						ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
						ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
						ufbx_vec2 uv = haveUV ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;
						ufbx_vec3 tangent = haveTangent ? ufbx_get_vertex_vec3(&mesh->vertex_tangent, ix) : default_vec;
						ufbx_vec3 binormal = haveBitangent ? ufbx_get_vertex_vec3(&mesh->vertex_bitangent, ix) : default_vec;

						// create mesh buffer here
						S3DVertexTangents v;
						v.Pos = convertFBXVec3(pos);
						v.Normal = convertFBXVec3(normal);
						v.Normal.normalize();
						v.TCoords = convertFBXVec2(uv);
						v.Tangent = convertFBXVec3(tangent);
						v.Binormal = convertFBXVec3(binormal);
						v.TangentW.set(1.f, 1.f);

						u32 vertLocation;
						core::map<uint32_t, u32>::Node* node = vertMap.find(ix);
						if (node)
							vertLocation = node->getValue();
						else
						{
							vertexBuffer->addVertex(&v);
							vertLocation = vertexBuffer->getVertexCount() - 1;
							vertMap.insert(ix, vertLocation);
						}

						indexBuffer->addIndex(vertLocation);
					}
				}

				// need calculate tangent & binormal
				if (!haveTangent && normalMap)
				{
					IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
					mh->recalculateTangents(mb);
				}

				// need load texture
				if (mesh_mat->material)
				{
					SMaterial& mat = mb->getMaterial();
					if (mesh_mat->material->textures.count == 0)
					{
						// no texture
						mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
					}
					else
					{
						bool foundTexture = false;

						for (int i = 0; i < mesh_mat->material->textures.count && i < MATERIAL_MAX_TEXTURES; i++)
						{
							ufbx_material_texture* materialTexture = &mesh_mat->material->textures.data[i];
							if (materialTexture->texture)
							{
								std::string fileName = CPath::getFileName(std::string(materialTexture->texture->filename.data));
							}
						}

						if (foundTexture)
							mat.MaterialType = shaderMgr->getShaderIDByName("TextureColor");
						else
							mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
					}
				}

				mb->recalculateBoundingBox();
				mb->drop();
			}

			for (int j = 0; j < mesh->instances.count; j++)
			{
				ufbx_node* node = mesh->instances[j];
				CEntity* entity = mapNodes[node];
				if (entity)
				{
					// add mesh renderer
					// add & clone new render mesh
					resultMesh->recalculateBoundingBox();
					resultMesh->setHardwareMappingHint(EHM_STATIC);

					CRenderMeshData* meshData = entity->addData<CRenderMeshData>();
					meshData->setMesh(resultMesh);

					// drop this mesh
					resultMesh->drop();

					// add culling data
					entity->addData<CCullingData>();
				}
			}

			delete[]tri_indices;
		}

		// free data
		delete[]buf;
		file->drop();

		ufbx_free_scene(scene);

		return true;
	}
}