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
#include "ufbx.h"

#include "CFBXMeshLoader.h"
#include "CFBXLoaderFunc.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityPrefab.h"

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"
#include "Importer/Utils/CMeshUtils.h"

#include "TextureManager/CTextureManager.h"

#include "Transform/CWorldTransformData.h"
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

		opts.load_external_files = false;
		opts.ignore_embedded = true;
		opts.ignore_animation = true;
		opts.evaluate_skinning = true;
		opts.generate_missing_normals = true;

		opts.target_axes.right = UFBX_COORDINATE_AXIS_NEGATIVE_X;
		opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
		opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;

		opts.target_unit_meters = 1.0f;
		opts.allow_nodes_out_of_root = true;

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_memory(buf, filesize, &opts, &error);
		if (!scene)
		{
			os::Printer::log("Failed to load scene");
			delete[]buf;
			file->drop();
			return false;
		}

		std::string modelName = CPath::getFileName(std::string(resource));
		std::map<ufbx_node*, CEntity*> mapNodes;

		// root node
		CEntity* root = NULL;

		core::matrix4 unitScaleMatrix;

		// import scene node
		for (int i = 0; i < scene->nodes.count; i++)
		{
			ufbx_node* node = scene->nodes[i];

			CEntity* parent = NULL;
			if (node->parent)
				parent = mapNodes[node->parent];

			const char* name = node->name.data;
			if (node->is_root)
			{
				name = modelName.c_str();
				unitScaleMatrix = convertFBXMatrix(node->node_to_parent);
			}

			CEntity* entity = output->createEntity();
			output->addTransformData(entity, parent, convertFBXMatrix(node->node_to_parent), name);
			mapNodes[node] = entity;

			if (node->is_root)
				root = entity;

			// Update world transform
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			if (parent)
			{
				CWorldTransformData* parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);
				transform->World.setbyproduct_nocheck(parentTransform->World, transform->Relative);
			}
			else
			{
				transform->World = transform->Relative;
			}
		}

		ufbx_vec2 default_uv = { 0 };
		ufbx_vec3 default_vec = { 0 };
		ufbx_vec4 default_color = { 1.0f, 1.0f, 1.0f, 1.0f };

		CShaderManager* shaderMgr = CShaderManager::getInstance();

		// import mesh data
		for (int i = 0; i < scene->meshes.count; i++)
		{
			ufbx_mesh* mesh = scene->meshes[i];

			bool haveTangent = mesh->vertex_tangent.values.count > 0;
			bool haveBitangent = mesh->vertex_bitangent.values.count > 0;
			bool haveUV = mesh->vertex_uv.values.count > 0;
			bool haveColor = mesh->vertex_color.values.count > 0;

			std::string meshName = mesh->name.data;

			// temp buffer
			size_t num_tri_indices = mesh->max_face_triangles * 3;
			uint32_t* tri_indices = new uint32_t[num_tri_indices];

			std::vector<IMeshBuffer*> meshBuffers;
			std::vector<std::string> materials;

			CMesh* resultMesh = NULL;
			bool isSkinnedMesh = false;
			bool haveBlendShape = false;
			S3DVertexSkin* mesh_skin_vertices = NULL;

			if (mesh->blend_deformers.count > 0)
				haveBlendShape = true;

			if (mesh->skin_deformers.count > 0)
			{
				isSkinnedMesh = true;
				ufbx_skin_deformer* skin = mesh->skin_deformers.data[0];

				mesh_skin_vertices = new S3DVertexSkin[mesh->num_vertices];

				for (size_t vi = 0; vi < mesh->num_vertices; vi++)
				{
					size_t num_weights = 0;
					float total_weight = 0.0f;
					float weights[4] = { 0.0f };
					uint8_t clusters[4] = { 0 };

					ufbx_skin_vertex vertex_weights = skin->vertices.data[vi];
					for (size_t wi = 0; wi < vertex_weights.num_weights && wi < 4; wi++)
					{
						ufbx_skin_weight weight = skin->weights.data[vertex_weights.weight_begin + wi];

						total_weight += (float)weight.weight;
						clusters[num_weights] = (uint8_t)weight.cluster_index;
						weights[num_weights] = (float)weight.weight;
						num_weights++;
					}

					if (total_weight > 0.0f)
					{
						S3DVertexSkin* skin_vert = &mesh_skin_vertices[vi];

						float* w = &skin_vert->BoneWeight.X;
						float* b = &skin_vert->BoneIndex.X;

						uint32_t quantized_sum = 0;
						for (size_t i = 0; i < 4; i++)
						{
							w[i] = weights[i] / total_weight;
							b[i] = (float)clusters[i];
						}
					}
				}
			}

			// init mesh
			for (int j = 0; j < mesh->material_parts.count; j++)
			{
				ufbx_mesh_part* mesh_part = &mesh->material_parts.data[j];

				if (mesh_part->num_triangles == 0)
					continue;

				IMeshBuffer* mb = NULL;
				video::E_INDEX_TYPE indexType = video::EIT_16BIT;

				size_t numVertex = mesh_part->num_triangles * 3;
				if (numVertex >= USHRT_MAX - 1)
					indexType = video::EIT_32BIT;

				if (isSkinnedMesh)
				{
					if (normalMap || haveBlendShape)
					{
						mb = new CMeshBuffer<video::S3DVertexSkinTangents>(
							getVideoDriver()->getVertexDescriptor(EVT_SKIN_TANGENTS),
							indexType);
					}
					else
					{
						mb = new CMeshBuffer<video::S3DVertexSkin>(
							getVideoDriver()->getVertexDescriptor(EVT_SKIN),
							indexType);
					}
				}
				else
				{
					if (normalMap || haveBlendShape)
					{
						mb = new CMeshBuffer<video::S3DVertexTangents>(
							getVideoDriver()->getVertexDescriptor(EVT_TANGENTS),
							indexType);
					}
					else
					{
						mb = new CMeshBuffer<video::S3DVertex>(
							getVideoDriver()->getVertexDescriptor(EVT_STANDARD),
							indexType);
					}
				}

				// add mesh buffer
				char materialName[512];

				ufbx_material* mesh_mat = NULL;
				if (j < mesh->materials.count)
					mesh_mat = mesh->materials.data[j];

				if (mesh_mat && mesh_mat->name.data && CStringImp::length(mesh_mat->name.data))
				{
					CStringImp::copy(materialName, mesh_mat->name.data);
				}
				else
				{
					sprintf(materialName, "material_%d", j);
				}

				meshBuffers.push_back(mb);
				materials.push_back(materialName);

				IVertexBuffer* vertexBuffer = mb->getVertexBuffer();
				IIndexBuffer* indexBuffer = mb->getIndexBuffer();

				core::map<uint32_t, u32> vertMap;

				for (size_t fi = 0; fi < mesh_part->num_faces; fi++)
				{
					int32_t faceID = mesh_part->face_indices[fi];
					ufbx_face face = mesh->faces[faceID];

					size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

					u32 trisIndices[3];
					int indexCount = 0;

					for (size_t vi = 0; vi < num_tris * 3; vi++)
					{
						uint32_t ix = tri_indices[vi];

						ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
						ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
						ufbx_vec4 color = haveColor ? ufbx_get_vertex_vec4(&mesh->vertex_color, ix) : default_color;
						ufbx_vec2 uv = haveUV ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;
						ufbx_vec3 tangent = haveTangent ? ufbx_get_vertex_vec3(&mesh->vertex_tangent, ix) : default_vec;
						ufbx_vec3 binormal = haveBitangent ? ufbx_get_vertex_vec3(&mesh->vertex_bitangent, ix) : default_vec;

						SColor c(255, (u32)(color.x * 255), (u32)(color.y * 255), (u32)(color.z * 255));

						// [vid] for morph, blendshape data as vertex index
						int vid = mesh->vertex_indices[ix];

						u32 vertLocation;
						core::map<uint32_t, u32>::Node* node = vertMap.find(ix);

						if (node)
							vertLocation = node->getValue();
						else
						{
							if (isSkinnedMesh)
							{
								if (normalMap || haveBlendShape)
								{
									S3DVertexSkinTangents v;
									v.Pos = convertFBXVec3(pos);
									v.Normal = convertFBXVec3(normal);
									v.Normal.normalize();
									v.Color = c;
									v.TCoords = convertFBXUVVec2(uv);
									v.Tangent = convertFBXVec3(tangent);
									v.Binormal = convertFBXVec3(binormal);
									v.VertexData.set(1.f, (float)vid);
									v.BoneIndex = mesh_skin_vertices[vid].BoneIndex;
									v.BoneWeight = mesh_skin_vertices[vid].BoneWeight;

									vertexBuffer->addVertex(&v);
								}
								else
								{
									S3DVertexSkin v;
									v.Pos = convertFBXVec3(pos);
									v.Normal = convertFBXVec3(normal);
									v.Normal.normalize();
									v.Color = c;
									v.TCoords = convertFBXUVVec2(uv);
									v.BoneIndex = mesh_skin_vertices[vid].BoneIndex;
									v.BoneWeight = mesh_skin_vertices[vid].BoneWeight;

									vertexBuffer->addVertex(&v);
								}
							}
							else
							{
								S3DVertexTangents v;
								v.Pos = convertFBXVec3(pos);
								v.Normal = convertFBXVec3(normal);
								v.Normal.normalize();
								v.Color = c;
								v.TCoords = convertFBXUVVec2(uv);
								v.Tangent = convertFBXVec3(tangent);
								v.Binormal = convertFBXVec3(binormal);
								v.VertexData.set(1.f, (float)vid);

								vertexBuffer->addVertex(&v);
							}

							vertLocation = vertexBuffer->getVertexCount() - 1;
							vertMap.insert(ix, vertLocation);
						}

						trisIndices[indexCount++] = vertLocation;

						if (indexCount >= 3)
						{
							if (opts.target_axes.right == UFBX_COORDINATE_AXIS_NEGATIVE_X)
							{
								indexBuffer->addIndex(trisIndices[2]);
								indexBuffer->addIndex(trisIndices[1]);
								indexBuffer->addIndex(trisIndices[0]);
							}
							else
							{
								indexBuffer->addIndex(trisIndices[0]);
								indexBuffer->addIndex(trisIndices[1]);
								indexBuffer->addIndex(trisIndices[2]);
							}

							indexCount = 0;
						}
					}
				}

				// need load texture
				if (mesh_mat)
				{
					SMaterial& mat = mb->getMaterial();
					if (mesh_mat->textures.count == 0)
					{
						// no texture
						if (isSkinnedMesh)
							mat.MaterialType = shaderMgr->getShaderIDByName("SkinVertexColor");
						else
							mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
					}
					else
					{
						bool foundTexture = false;

						if (mesh_mat->textures.count > 0)
						{
							ufbx_material_texture* materialTexture = &mesh_mat->textures.data[0];
							if (materialTexture->texture)
							{
								std::string fileName = CPath::getFileName(std::string(materialTexture->texture->filename.data));
								ITexture* texture = CTextureManager::getInstance()->getTexture(fileName.c_str(), m_textureFolder);
								if (texture == NULL)
									texture = CTextureManager::getInstance()->getTexture(materialTexture->texture->relative_filename.data, m_textureFolder);

								if (texture)
								{
									foundTexture = true;
									mat.UseMipMaps = true;
									mat.setTexture(0, texture);
									mat.setFlag(video::EMF_BILINEAR_FILTER, false);
									mat.setFlag(video::EMF_TRILINEAR_FILTER, false);
									mat.setFlag(video::EMF_ANISOTROPIC_FILTER, true, 2);
								}
							}
						}

						if (isSkinnedMesh)
						{
							if (foundTexture)
								mat.MaterialType = shaderMgr->getShaderIDByName("Skin");
							else
								mat.MaterialType = shaderMgr->getShaderIDByName("SkinVertexColor");
						}
						else
						{
							if (foundTexture)
								mat.MaterialType = shaderMgr->getShaderIDByName("TextureColor");
							else
								mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
						}
					}
				}
				else
				{
					// default vertex color if model dont have material
					SMaterial& mat = mb->getMaterial();
					if (isSkinnedMesh)
						mat.MaterialType = shaderMgr->getShaderIDByName("SkinVertexColor");
					else
						mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
				}

				// need calculate tangent & binormal
				if (!haveTangent && normalMap)
				{
					IMeshManipulator* mh = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
					mh->recalculateTangents(mb);
				}

				mb->recalculateBoundingBox();

				// apply unit scale for culling
				if (isSkinnedMesh)
				{
					core::aabbox3df box = mb->getBoundingBox();
					unitScaleMatrix.transformBox(box);
					mb->getBoundingBox() = box;
				}
			}

			if (isSkinnedMesh)
			{
				// init skinned mesh
				CSkinnedMesh* skinnedMesh = new CSkinnedMesh();

				ufbx_skin_deformer* skin = mesh->skin_deformers.data[0];

				// init joint
				for (size_t ci = 0; ci < skin->clusters.count; ci++)
				{
					ufbx_skin_cluster* cluster = skin->clusters.data[ci];

					ufbx_node* bone_node = cluster->bone_node;
					CEntity* boneEntity = mapNodes[bone_node];

					CJointData* jointData = GET_ENTITY_DATA(boneEntity, CJointData);
					if (!jointData)
					{
						jointData = boneEntity->addData<CJointData>();

						jointData->SID = bone_node->name.data;
						jointData->BoneName = bone_node->name.data;

						if (bone_node->parent)
						{
							CEntity* boneParentEntity = mapNodes[bone_node->parent];
							if (boneParentEntity)
							{
								CJointData* parentJoint = GET_ENTITY_DATA(boneParentEntity, CJointData);
								if (parentJoint == NULL)
									jointData->BoneRoot = true;
							}
						}
					}

					skinnedMesh->Joints.push_back(CSkinnedMesh::SJoint());

					CSkinnedMesh::SJoint& joint = skinnedMesh->Joints.getLast();
					joint.Name = jointData->SID;
					joint.JointData = jointData;
					joint.BindPoseMatrix = convertFBXMatrix(cluster->geometry_to_bone);
					joint.EntityIndex = boneEntity->getIndex();
				}

				resultMesh = skinnedMesh;
			}
			else
			{
				// init static mesh
				resultMesh = new CMesh();
			}

			if (haveBlendShape)
			{
				for (size_t di = 0; di < mesh->blend_deformers.count; di++)
				{
					ufbx_blend_deformer* deformer = mesh->blend_deformers.data[di];

					for (size_t ci = 0; ci < deformer->channels.count; ci++)
					{
						ufbx_blend_channel* chan = deformer->channels.data[ci];
						if (chan->keyframes.count == 0)
							continue;

						ufbx_blend_shape* shape = chan->keyframes.data[chan->keyframes.count - 1].shape;

						// alloc blendshape data
						CBlendShape* blendShape = new CBlendShape();
						blendShape->Name = chan->name.data;
						blendShape->Weight = ci == 0 ? 1.0f : 0.0f;

						u32 vtxCount = (u32)mesh->num_vertices;
						u32 offsetCount = (u32)shape->num_offsets + 1;

						blendShape->VtxId.set_used(vtxCount);
						blendShape->Offset.set_used(offsetCount);
						blendShape->NormalOffset.set_used(offsetCount);

						for (u32 i = 0; i < vtxCount; i++)
							blendShape->VtxId[i] = offsetCount;

						for (u32 i = 0; i < offsetCount; i++)
						{
							blendShape->Offset[i].set(0.0f, 0.0f, 0.0f);
							blendShape->NormalOffset[i].set(0.0f, 0.0f, 0.0f);
						}

						// read blend shape data
						for (size_t oi = 0, n = (u32)shape->num_offsets; oi < n; oi++)
						{
							uint32_t ix = (uint32_t)shape->offset_vertices[oi];
							if (ix < vtxCount)
							{
								blendShape->VtxId[ix] = oi;
								blendShape->Offset[oi] = convertFBXVec3(shape->position_offsets[oi]);
								blendShape->NormalOffset[oi] = convertFBXVec3(shape->normal_offsets[oi]);
							}
						}

						resultMesh->addBlendShape(blendShape);
						blendShape->drop();
					}
				}
			}

			// add meshbuffer to mesh
			for (int i = 0, n = (int)meshBuffers.size(); i < n; i++)
			{
				resultMesh->addMeshBuffer(meshBuffers[i], materials[i].c_str());
				meshBuffers[i]->drop();
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

					if (isSkinnedMesh)
					{
						CSkinnedMesh* skinnedMesh = (CSkinnedMesh*)resultMesh;

#if defined(IOS_SIMULATOR)
						meshData->setSoftwareSkinning(true);
#else
						if (skinnedMesh->Joints.size() >= GPU_BONES_COUNT)
							meshData->setSoftwareSkinning(true);
#endif

						meshData->setSkinnedMesh(true);

						// need mode this mesh to to root
						output->changeParent(entity, NULL);
					}

					// drop this mesh
					resultMesh->drop();

					// add culling data
					entity->addData<CCullingData>();
				}
			}

			delete[]tri_indices;

			if (mesh_skin_vertices)
				delete[]mesh_skin_vertices;
		}

		// free data
		delete[]buf;
		file->drop();

		ufbx_free_scene(scene);
		return true;
	}
}