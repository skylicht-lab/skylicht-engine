// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __T_MESH_BUFFER_H_INCLUDED__
#define __T_MESH_BUFFER_H_INCLUDED__

#include "IMeshBuffer.h"
#include "IVertexDescriptor.h"

namespace irr
{
namespace scene
{
	//! Implementation of the IMeshBuffer interface
	template <class T>
	class CMeshBuffer : public IMeshBuffer
	{
	public:
		//! Constructor
		CMeshBuffer(video::IVertexDescriptor* vertexDescriptor, video::E_INDEX_TYPE type = video::EIT_16BIT) : IMeshBuffer(vertexDescriptor)
		{
			#ifdef _DEBUG
			setDebugName("CMeshBuffer");
			#endif

			if (VertexDescriptor)
			{
				VertexDescriptor->grab();

				VertexBuffer.push_back(new CVertexBuffer<T>());

				if (VertexDescriptor->getVertexSize(0) != VertexBuffer[0]->getVertexSize())
					VertexBufferCompatible = false;
			}
			else
			{
				VertexBufferCompatible = false;
			}

			IndexBuffer = new CIndexBuffer(type);
		}

		// Copy constructor (due to problems with detect types a vertex and index buffers aren't duplicated, they are assigned like in assignment operator)
		CMeshBuffer(const CMeshBuffer& meshBuffer) : IMeshBuffer(meshBuffer.VertexDescriptor)
		{
			if (VertexDescriptor)
				VertexDescriptor->grab();

			Material = meshBuffer.Material;

			const u32 vbCount = meshBuffer.VertexBuffer.size();

			for (u32 i = 0; i < vbCount; ++i)
			{
				VertexBuffer.push_back(meshBuffer.VertexBuffer[i]);

				if (VertexBuffer[i])
					VertexBuffer[i]->grab();
			}

			IndexBuffer = meshBuffer.IndexBuffer;

			if (IndexBuffer)
				IndexBuffer->grab();

			BoundingBox = meshBuffer.BoundingBox;
			Transformation = meshBuffer.Transformation;
			BoundingBoxNeedsRecalculated = meshBuffer.BoundingBoxNeedsRecalculated;
			VertexBufferCompatible = meshBuffer.VertexBufferCompatible;
		}

		//! Destructor
		virtual ~CMeshBuffer()
		{
			const u32 vbCount = VertexBuffer.size();

			for (u32 i = 0; i < vbCount; ++i)
			{
				if (VertexBuffer[i])
					VertexBuffer[i]->drop();
			}

			if (IndexBuffer)
				IndexBuffer->drop();

			if (VertexDescriptor)
				VertexDescriptor->drop();
		}

		virtual video::IVertexDescriptor* getVertexDescriptor() const
		{
			return VertexDescriptor;
		}

		virtual bool setVertexDescriptor(video::IVertexDescriptor* vertexDescriptor)
		{
			if (vertexDescriptor && vertexDescriptor != VertexDescriptor)
			{
				if (VertexDescriptor)
					VertexDescriptor->drop();

				VertexDescriptor = vertexDescriptor;
				VertexDescriptor->grab();

				bool vertexBufferCompatible = true;

				for (u32 i = 0; i < VertexBuffer.size(); ++i)
				{
					if (VertexDescriptor->getVertexSize(i) != VertexBuffer[i]->getVertexSize())
					{
						vertexBufferCompatible = false;
						break;
					}
				}

				VertexBufferCompatible = vertexBufferCompatible;

				return true;
			}

			return false;
		}

		virtual bool addVertexBuffer(IVertexBuffer* vertexBuffer)
		{
			bool status = false;

			if (vertexBuffer && VertexDescriptor)
			{
				if (VertexDescriptor->getVertexSize(VertexBuffer.size()) != vertexBuffer->getVertexSize())
					VertexBufferCompatible = false;

				vertexBuffer->grab();
				VertexBuffer.push_back(vertexBuffer);

				status = true;
			}

			return status;
		}

		virtual IVertexBuffer* getVertexBuffer(u32 id = 0) const
		{
			IVertexBuffer* vb = 0;

			if (id < VertexBuffer.size())
				vb = VertexBuffer[id];

			return vb;
		}

		virtual u32 getVertexBufferCount() const
		{
			return VertexBuffer.size();
		}

		virtual void removeVertexBuffer(u32 id)
		{
			if (id > 0 && id < VertexBuffer.size())
			{
				VertexBuffer[id]->drop();
				VertexBuffer.erase(id);

				bool vertexBufferCompatible = true;

				if (VertexDescriptor)
				{
					for (u32 i = 0; i < VertexBuffer.size(); ++i)
					{
						if (VertexDescriptor->getVertexSize(i) != VertexBuffer[i]->getVertexSize())
						{
							vertexBufferCompatible = false;
							break;
						}
					}
				}
				else
				{
					vertexBufferCompatible = false;
				}

				VertexBufferCompatible = vertexBufferCompatible;
			}
		}

		virtual bool setVertexBuffer(IVertexBuffer* vertexBuffer, u32 id = 0)
		{
			if (id >= VertexBuffer.size() || !vertexBuffer || VertexBuffer[id] == vertexBuffer)
				return false;

			VertexBuffer[id]->drop();
			vertexBuffer->grab();

			VertexBuffer[id] = vertexBuffer;

			bool vertexBufferCompatible = true;

			if (VertexDescriptor)
			{
				for (u32 i = 0; i < VertexBuffer.size(); ++i)
				{
					if (VertexDescriptor->getVertexSize(i) != VertexBuffer[i]->getVertexSize())
					{
						vertexBufferCompatible = false;
						break;
					}
				}
			}
			else
			{
				vertexBufferCompatible = false;
			}

			VertexBufferCompatible = vertexBufferCompatible;

			return true;
		}

		virtual IIndexBuffer* getIndexBuffer() const
		{
			return IndexBuffer;
		}

		virtual bool setIndexBuffer(IIndexBuffer* indexBuffer)
		{
			if (!indexBuffer || IndexBuffer == indexBuffer)
				return false;

			IndexBuffer->drop();
			indexBuffer->grab();

			IndexBuffer = indexBuffer;

			return true;
		}

		//! Get material of this meshbuffer
		/** \return Material of this buffer */
		virtual const video::SMaterial& getMaterial() const
		{
			return Material;
		}

		//! Get material of this meshbuffer
		/** \return Material of this buffer */
		virtual video::SMaterial& getMaterial()
		{
			return Material;
		}

		virtual video::E_VERTEX_TYPE getVertexType() const
		{
			return VertexDescriptor ? (video::E_VERTEX_TYPE)VertexDescriptor->getID() : (video::E_VERTEX_TYPE) - 1;
		}

		//! Get the axis aligned bounding box
		/** \return Axis aligned bounding box of this buffer. */
		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{
			return BoundingBox;
		}

		//! Get the axis aligned bounding box
		/** \return Axis aligned bounding box of this buffer. */
		virtual core::aabbox3d<f32>& getBoundingBox()
		{
			return BoundingBox;
		}

		//! Call this after changing the positions of any vertex.
		virtual void boundingBoxNeedsRecalculated()
		{
			BoundingBoxNeedsRecalculated = true;
		}

		//! Recalculate the bounding box.
		/** should be called if the mesh changed. */
		virtual void recalculateBoundingBox()
		{
			/*if(!BoundingBoxNeedsRecalculated)
				return;*/

			BoundingBoxNeedsRecalculated = false;

			const video::IVertexAttribute* attribute = (VertexDescriptor && VertexBufferCompatible) ? VertexDescriptor->getAttributeBySemantic(video::EVAS_POSITION) : 0;

			if (!attribute || attribute->getBufferID() >= VertexBuffer.size() || VertexBuffer[attribute->getBufferID()]->getVertexCount() == 0)
				BoundingBox.reset(0,0,0);
			else
			{
				const u32 bufferID = attribute->getBufferID();

				u8* offset = static_cast<u8*>(VertexBuffer[bufferID]->getVertices());
				offset += attribute->getOffset();

				core::vector3df position(0.0f);

				memcpy(&position, offset, sizeof(core::vector3df));

				BoundingBox.reset(position);

				const u32 vertexCount = VertexBuffer[bufferID]->getVertexCount();
				const u32 vertexSize = VertexBuffer[bufferID]->getVertexSize();

				for(u32 j = 1; j < vertexCount; ++j)
				{
					offset += vertexSize;
					memcpy(&position, offset, sizeof(core::vector3df));
					BoundingBox.addInternalPoint(position);
				}
			}
		}

		//! Append the vertices and indices to the current buffer
		/** Only works for compatible types, i.e. either the same type
		or the main buffer is of standard type. Otherwise, behavior is
		undefined.
		*/
		virtual void append(IVertexBuffer* vertexBuffer, u32 vertexBufferID, IIndexBuffer* indexBuffer)
		{
			if (vertexBufferID < VertexBuffer.size() && VertexDescriptor && vertexBuffer &&
				vertexBuffer->getVertexSize() == VertexDescriptor->getVertexSize(vertexBufferID) &&
				vertexBuffer->getVertexSize() == VertexBuffer[vertexBufferID]->getVertexSize())
			{
				const u32 vertexCount = vertexBuffer->getVertexCount();
				const u32 vertexSize = vertexBuffer->getVertexSize();

				VertexBuffer[vertexBufferID]->reallocate(VertexBuffer[vertexBufferID]->getVertexCount() + vertexCount);

				video::IVertexAttribute* attribute = VertexDescriptor->getAttributeBySemantic(video::EVAS_POSITION);

				u8* offset = static_cast<u8*>(vertexBuffer->getVertices());

				const u32 attributeOffset = attribute ? attribute->getOffset() : 0;

				for (u32 i = 0; i < vertexCount; ++i)
				{
					VertexBuffer[vertexBufferID]->addVertex(offset);

					if (attribute)
					{
						u8* positionBuffer = offset + attributeOffset;

						core::vector3df position(0.0f);
						memcpy(&position, positionBuffer, sizeof(core::vector3df));
						BoundingBox.addInternalPoint(position);
					}

					offset += vertexSize;
				}
			}

			if (indexBuffer)
			{
				const u32 indexCount = indexBuffer->getIndexCount();

				IndexBuffer->reallocate(IndexBuffer->getIndexCount() + indexCount);

				for (u32 i = 0; i < indexCount; ++i)
					IndexBuffer->addIndex(indexBuffer->getIndex(i));
			}
		}

		//! Append the meshbuffer to the current buffer
		/** Only works for compatible types, i.e. either the same type
		or the main buffer is of standard type. Otherwise, behavior is
		undefined.
		\param other Meshbuffer to be appended to this one.
		*/
		virtual void append(IMeshBuffer* meshBuffer)
		{
			if (!meshBuffer || meshBuffer == this)
				return;

			append(meshBuffer->getVertexBuffer(0), 0, meshBuffer->getIndexBuffer());

			const u32 bufferCount = meshBuffer->getVertexBufferCount();

			for (u32 i = 1; i < bufferCount && i < VertexBuffer.size(); ++i)
				append(meshBuffer->getVertexBuffer(i), i, 0);
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex(u32 id = 0) const
		{
			return (id < VertexBuffer.size()) ? VertexBuffer[id]->getHardwareMappingHint() : EHM_NEVER;
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const
		{
			return IndexBuffer->getHardwareMappingHint();
		}

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING pMappingHint, E_BUFFER_TYPE type = EBT_VERTEX_AND_INDEX, u32 id = 0)
		{
			if((type == EBT_VERTEX_AND_INDEX || type == EBT_VERTEX) && id < VertexBuffer.size())
				VertexBuffer[id]->setHardwareMappingHint(pMappingHint);

			if(type == EBT_VERTEX_AND_INDEX || type == EBT_INDEX)
				IndexBuffer->setHardwareMappingHint(pMappingHint);
		}

		//! flags the mesh as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE type = EBT_VERTEX_AND_INDEX, u32 id = 0)
		{
			if((type == EBT_VERTEX_AND_INDEX || type == EBT_VERTEX) && id < VertexBuffer.size())
				VertexBuffer[id]->setDirty();

			if(type == EBT_VERTEX_AND_INDEX || type == EBT_INDEX)
				IndexBuffer->setDirty();
		}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Vertex(u32 id = 0) const
		{
			return (id < VertexBuffer.size()) ? VertexBuffer[id]->getChangedID() : 0;
		}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Index() const
		{
			return IndexBuffer->getChangedID();
		}

		virtual core::matrix4& getTransformation()
		{
			return Transformation;
		}

		virtual void setUseForHardwareInstancing(bool b)
		{
			UseForHardwareInstancing = b;
		}

		virtual bool useForHardwareInstancing()
		{
			return UseForHardwareInstancing;
		}
	};
} // end namespace scene
} // end namespace irr

#endif


