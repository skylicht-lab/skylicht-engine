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

#include "CFBXAnimLoader.h"
#include "CFBXLoaderFunc.h"
#include "Animation/CAnimationManager.h"

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CFBXAnimLoader::CFBXAnimLoader()
	{

	}

	CFBXAnimLoader::~CFBXAnimLoader()
	{

	}

	bool CFBXAnimLoader::loadAnimation(const char* resource, CAnimationClip* output)
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
		opts.ignore_geometry = true;
		opts.ignore_animation = false;

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

		std::map<ufbx_node*, SEntityAnim*> m_nodeAnim;

		for (int i = 0; i < scene->nodes.count; i++)
		{
			ufbx_node* node = scene->nodes[i];

			const char* name = node->name.data;
			if (node->parent == NULL)
				name = modelName.c_str();

			core::matrix4 relativeTransform = convertFBXMatrix(node->node_to_parent);

			SEntityAnim* nodeAnim = new SEntityAnim();
			nodeAnim->Name = name;

			// default value
			core::matrix4 mat;
			nodeAnim->Data.Rotations.Default = core::quaternion(
				(f32)node->euler_rotation.x,
				(f32)node->euler_rotation.y,
				(f32)node->euler_rotation.z
			);
			nodeAnim->Data.Positions.Default = relativeTransform.getTranslation();
			nodeAnim->Data.Scales.Default = relativeTransform.getScale();

			m_nodeAnim[node] = nodeAnim;
		}

		// only load 1 clip
		for (size_t i = 0; i < scene->anim_stacks.count && i < 1; i++)
		{
			ufbx_anim_stack* stack = scene->anim_stacks.data[i];

			output->Duration = (float)stack->time_end - (float)stack->time_begin;
			output->AnimName = CPath::getFileNameNoExt(modelName);

			float targetFPS = 30.0f;
			int totalFrame = (int)(output->Duration * targetFPS);

			for (auto i : m_nodeAnim)
			{
				ufbx_node* node = i.first;
				SEntityAnim* entityAnim = i.second;

				// check animation data in this node
				ufbx_anim_prop_list listAnim = ufbx_find_anim_props(stack->layers.data[0], &node->element);
				if (listAnim.count == 0)
				{
					delete entityAnim;
					continue;
				}

				// add this node animation
				output->addAnim(entityAnim);

				for (int i = 0; i < totalFrame; i++)
				{
					double time = stack->time_begin + (double)i / (double)targetFPS;

					ufbx_transform transform = ufbx_evaluate_transform(stack->anim, node, time);

					CPositionKey pos;
					CScaleKey scale;
					CRotationKey rot;

					pos.Frame = scale.Frame = rot.Frame = (float)time;

					pos.Value.set(
						(f32)transform.translation.x,
						(f32)transform.translation.y,
						(f32)transform.translation.z);

					scale.Value.set(
						(f32)transform.scale.x,
						(f32)transform.scale.y,
						(f32)transform.scale.z);

					rot.Value.set(
						(f32)transform.rotation.x,
						(f32)transform.rotation.y,
						(f32)transform.rotation.z,
						(f32)transform.rotation.w);

					bool addFrame = true;

					if (i > 0)
					{
						const CRotationKey& lastFrameRot = entityAnim->Data.Rotations.Data.getLast();
						const CPositionKey& lastFramePos = entityAnim->Data.Positions.Data.getLast();
						const CScaleKey& lastFrameScale = entityAnim->Data.Scales.Data.getLast();

						// Negated quaternions are equivalent, but interpolating between ones of different
						// polarity takes a the longer path, so flip the quaternion if necessary.
						float dot = lastFrameRot.Value.dotProduct(rot.Value);
						if (dot < 0.0f)
						{
							rot.Value *= -1.0f;
						}

						if (lastFrameRot.Value == rot.Value &&
							lastFramePos.Value == pos.Value &&
							lastFrameScale.Value == scale.Value &&
							i != totalFrame - 1)
						{
							// still use old frame time
							addFrame = false;
						}
					}

					if (addFrame)
					{
						entityAnim->Data.Positions.Data.push_back(pos);
						entityAnim->Data.Scales.Data.push_back(scale);
						entityAnim->Data.Rotations.Data.push_back(rot);
					}
				}
			}
		}

		// free data
		delete[]buf;
		file->drop();

		ufbx_free_scene(scene);
		return true;
	}
}