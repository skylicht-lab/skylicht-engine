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

#pragma once

#include "Entity/CEntity.h"
#include "VertexInstancingType.h"
#include "Transform/CWorldTransformData.h"
#include "IndirectLighting/CIndirectLightingData.h"

namespace Skylicht
{
	class CMaterial;

	/**
	 * @class IShaderInstancing
	 * @ingroup Materials
	 * @brief Abstract base class for GPU instancing logic in Skylicht-Engine.
	 *
	 * Provides an interface for describing how vertex buffers and mesh buffers should be configured for hardware instancing,
	 * including batching transforms and lighting, descriptor setup, compatibility checks, and linking buffers.
	 */
	class SKYLICHT_API IShaderInstancing
	{
	protected:
		/// The base vertex descriptor(original mesh format)
		video::IVertexDescriptor* m_baseVtxDescriptor;

		/// Vertex descriptor for instanced rendering
		video::IVertexDescriptor* m_vtxDescriptor;

		/// Vertex descriptor for instanced rendering with lighting data
		video::IVertexDescriptor* m_vtxDescriptorForRenderLighting;

	public:
		/**
		 * @brief Constructor. Initializes descriptors to null.
		 */
		IShaderInstancing();

		/**
		 * @brief Destructor.
		 */
		virtual ~IShaderInstancing();

		/**
		 * @brief Set up vertex descriptor for instanced rendering with lighting data.
		 *        Adds necessary attributes for color, UV scaling, and world matrix.
		 * @param name Name of the vertex descriptor.
		 */
		virtual void setupDescriptorForRenderLighting(const char* name);

		/**
		 * @brief Check if instancing is supported for the given mesh buffer.
		 *        Compares vertex descriptors to base and instanced formats.
		 * @param mb Mesh buffer to check.
		 * @return True if supported, false otherwise.
		 */
		virtual bool isSupport(IMeshBuffer* mb);

		/**
		 * @brief Check if instancing is supported for the given mesh.
		 *        Compares vertex descriptors for all mesh buffers.
		 * @param mesh Mesh to check.
		 * @return True if supported, false otherwise.
		 */
		virtual bool isSupport(IMesh* mesh);

		/**
		 * @brief Create a mesh buffer that links to the vertex and index buffers of another mesh buffer.
		 *        Used for instanced rendering.
		 * @param mb Source mesh buffer to link.
		 * @return Pointer to new linked mesh buffer.
		 */
		virtual IMeshBuffer* createLinkMeshBuffer(IMeshBuffer* mb);

		/**
		 * @brief Create a vertex buffer for instancing data (must be implemented by subclasses).
		 * @return Pointer to newly created instancing vertex buffer.
		 */
		virtual IVertexBuffer* createInstancingVertexBuffer() = 0;

		/**
		 * @brief Create a mesh buffer for a specific index type (must be implemented by subclasses).
		 * @param type Index buffer type.
		 * @return Pointer to newly created mesh buffer.
		 */
		virtual IMeshBuffer* createMeshBuffer(video::E_INDEX_TYPE type) = 0;

		/**
		 * @brief Create a vertex buffer for transform matrices (static method).
		 * @return Pointer to transform vertex buffer.
		 */
		static IVertexBuffer* createTransformVertexBuffer();

		/**
		 * @brief Create a vertex buffer for indirect lighting data (static method).
		 * @return Pointer to indirect lighting vertex buffer.
		 */
		static IVertexBuffer* createIndirectLightingVertexBuffer();

		/**
		 * @brief Apply instancing setup to a mesh buffer, binding instancing and transform buffers.
		 * @param meshbuffer Target mesh buffer.
		 * @param instancingBuffer Instancing vertex buffer.
		 * @param transformBuffer Transform vertex buffer.
		 * @return True if successful.
		 */
		virtual bool applyInstancing(IMeshBuffer* meshbuffer, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		/**
		 * @brief Apply instancing setup for render lighting to a mesh buffer, binding buffers and using lighting descriptor.
		 * @param meshbuffer Target mesh buffer.
		 * @param instancingBuffer Instancing vertex buffer.
		 * @param transformBuffer Transform vertex buffer.
		 * @return True if successful.
		 */
		virtual bool applyInstancingForRenderLighting(IMeshBuffer* meshbuffer, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		/**
		 * @brief Remove instancing buffers from a mesh buffer, restoring the base descriptor.
		 * @param meshbuffer Target mesh buffer.
		 * @return True if successful.
		 */
		virtual bool removeInstancing(IMeshBuffer* meshbuffer);

		/**
		 * @brief Apply instancing setup to all mesh buffers in a mesh, binding instancing and transform buffers.
		 * @param mesh Target mesh.
		 * @param instancingBuffer Instancing vertex buffer.
		 * @param transformBuffer Transform vertex buffer.
		 * @return True if successful.
		 */
		virtual bool applyInstancing(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		/**
		 * @brief Apply instancing setup for render lighting to all mesh buffers in a mesh.
		 * @param mesh Target mesh.
		 * @param instancingBuffer Instancing vertex buffer.
		 * @param transformBuffer Transform vertex buffer.
		 * @return True if successful.
		 */
		virtual bool applyInstancingForRenderLighting(IMesh* mesh, IVertexBuffer* instancingBuffer, IVertexBuffer* transformBuffer);

		/**
		 * @brief Remove instancing buffers from all mesh buffers in a mesh, restoring the base descriptor.
		 * @param mesh Target mesh.
		 * @return True if successful.
		 */
		virtual bool removeInstancing(IMesh* mesh);

		/**
		 * @brief Batch update instancing vertex buffer with transform and material data for multiple entities (must be implemented by subclasses).
		 * @param vtxBuffer Instancing vertex buffer to fill.
		 * @param materials Array of materials per entity.
		 * @param entities Array of entities.
		 * @param count Number of instances.
		 */
		virtual void batchIntancing(IVertexBuffer* vtxBuffer,
			CMaterial** materials,
			CEntity** entities,
			int count) = 0;

		/**
		 * @brief Batch update transform and lighting vertex buffers for multiple entities (static method).
		 * @param tBuffer Transform vertex buffer.
		 * @param lBuffer Indirect lighting vertex buffer.
		 * @param entities Array of entities.
		 * @param count Number of instances.
		 */
		static void batchTransformAndLighting(
			IVertexBuffer* tBuffer,
			IVertexBuffer* lBuffer,
			CEntity** entities,
			int count);

		/**
		 * @brief Batch update transform vertex buffer for multiple entities (static method).
		 * @param tBuffer Transform vertex buffer.
		 * @param entities Array of entities.
		 * @param count Number of instances.
		 */
		static void batchTransform(
			IVertexBuffer* tBuffer,
			CEntity** entities,
			int count);

		/**
		 * @brief Get the base vertex descriptor used for original mesh format.
		 * @return Pointer to base vertex descriptor.
		 */
		video::IVertexDescriptor* getBaseVertexDescriptor()
		{
			return m_baseVtxDescriptor;
		}

		/**
		 * @brief Get the instancing vertex descriptor used for instanced rendering format.
		 * @return Pointer to instancing vertex descriptor.
		 */
		video::IVertexDescriptor* getInstancingVertexDescriptor()
		{
			return m_vtxDescriptor;
		}
	};
}