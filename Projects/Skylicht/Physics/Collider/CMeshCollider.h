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

#include "CCollider.h"
#include "Entity/CEntityPrefab.h"
#include "RenderMesh/CRenderMeshData.h"

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief A collision shape generated from a 3D mesh prefab.
		 * @ingroup Physics
		 *
		 * This uses GImpact for moving triangle meshes.
		 * 
		 * Example:
		 * @code
		 * Physics::CMeshCollider* mesh = gameObject->addComponent<Physics::CMeshCollider>();
		 * mesh->setMeshSource("Assets/Models/MyMesh.fbx");
		 * @endcode
		 */
		class CMeshCollider : public CCollider
		{		protected:
			std::string m_source;

#ifdef USE_BULLET_PHYSIC_ENGINE
			btTriangleMesh* m_mesh;
#endif

		public:
			CMeshCollider();

			virtual ~CMeshCollider();

			/**
			 * @brief Releases the internal triangle mesh data.
			 */
			void releaseMesh();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Sets the source path for the mesh prefab.
			 * @param source Path to the .dae, .fbx, or .obj file.
			 */
			inline void setMeshSource(const char* source)
			{
				m_source = source;
			}

			/**
			 * @brief Gets the source path for the mesh prefab.
			 * @return Path string.
			 */
			inline const char* getMeshSource()
			{
				return m_source.c_str();
			}

			/**
			 * @brief Loads and returns the mesh prefab from the source path.
			 * @return Pointer to CEntityPrefab.
			 */
			CEntityPrefab* getMeshPrefab();

			/**
			 * @brief Gets the world transform matrix of the collider.
			 * @return 4x4 matrix.
			 */
			core::matrix4 getWorldTransform();

#ifdef USE_BULLET_PHYSIC_ENGINE
			virtual btCollisionShape* initCollisionShape();

			/**
			 * @brief Internal helper to initialize collision triangles from a prefab.
			 * @param prefab The mesh prefab.
			 * @param callback Callback to process each sub-mesh.
			 */
			void initFromPrefab(CEntityPrefab* prefab, std::function<void(const core::matrix4&, CMesh*)> callback);
#endif

			DECLARE_GETTYPENAME(CMeshCollider)
		};
	}
}