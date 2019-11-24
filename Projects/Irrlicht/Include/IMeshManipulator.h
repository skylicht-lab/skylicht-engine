// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_MANIPULATOR_H_INCLUDED__
#define __I_MESH_MANIPULATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "vector3d.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "CMeshBuffer.h"
#include "SMesh.h"

namespace irr
{
namespace scene
{
	//! An interface for easy manipulation of meshes.
	/** Scale, set alpha value, flip surfaces, and so on. This exists for
	fixing problems with wrong imported or exported meshes quickly after
	loading. It is not intended for doing mesh modifications and/or
	animations during runtime.
	*/
	class IMeshManipulator : public virtual IReferenceCounted
	{
	public:

		//! Flips the direction of surfaces.
		/** Changes backfacing triangles to frontfacing
		triangles and vice versa.
		\param mesh Mesh on which the operation is performed. */
		virtual void flipSurfaces(IMesh* mesh) const = 0;

		//! Sets the alpha vertex color value of the whole mesh to a new value.
		/** \param buffer Meshbuffer on which the operation is performed.
		\param alpha New alpha value. Must be a value between 0 and 255. */
		virtual void setVertexColorAlpha(IMeshBuffer* meshBuffer, s32 alpha) const = 0;

		//! Sets the alpha vertex color value of the whole mesh to a new value.
		/** \param mesh Mesh on which the operation is performed.
		\param alpha New alpha value. Must be a value between 0 and 255. */
		void setVertexColorAlpha(IMesh* mesh, s32 alpha) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				setVertexColorAlpha(mesh->getMeshBuffer(i), alpha);
		}

		//! Sets the colors of all vertices to one color
		/** \param buffer Meshbuffer on which the operation is performed.
		\param color New color. */
		virtual void setVertexColors(IMeshBuffer* meshBuffer, video::SColor color) const = 0;

		//! Sets the colors of all vertices to one color
		/** \param mesh Mesh on which the operation is performed.
		\param color New color. */
		void setVertexColors(IMesh* mesh, video::SColor color) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				setVertexColors(mesh->getMeshBuffer(i), color);
		}

		//! Scales the actual meshbuffer, not a scene node.
		/** \param buffer Meshbuffer on which the operation is performed.
		\param factor Scale factor for each axis. */
		virtual void scale(IMeshBuffer* meshBuffer, const core::vector3df& factor) const = 0;

		//! Scales the actual mesh, not a scene node.
		/** \param mesh Mesh on which the operation is performed.
		\param factor Scale factor for each axis. */
		void scale(IMesh* mesh, const core::vector3df& factor) const
		{
			core::aabbox3df BufferBox;

			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
			{
				scale(mesh->getMeshBuffer(i), factor);

				if (0 == i)
					BufferBox.reset(mesh->getMeshBuffer(i)->getBoundingBox());
				else
					BufferBox.addInternalBox(mesh->getMeshBuffer(i)->getBoundingBox());
			}

			mesh->setBoundingBox(BufferBox);
		}

		//! Scales the actual mesh, not a scene node.
		/** \deprecated Use scale() instead. This method may be removed by Irrlicht 1.9
		\param mesh Mesh on which the operation is performed.
		\param factor Scale factor for each axis. */
		_IRR_DEPRECATED_ void scaleMesh(IMesh* mesh, const core::vector3df& factor) const {return scale(mesh,factor);}

		//! Scale the texture coords of a meshbuffer.
		/** \param buffer Meshbuffer on which the operation is performed.
		\param factor Vector which defines the scale for each axis.
		\param level Number of texture coord. */
		virtual void scaleTCoords(IMeshBuffer* meshBuffer, const core::vector2df& factor, u32 level = 0) const = 0;

		//! Scale the texture coords of a mesh.
		/** \param mesh Mesh on which the operation is performed.
		\param factor Vector which defines the scale for each axis.
		\param level Number of texture coord. */
		void scaleTCoords(scene::IMesh* mesh, const core::vector2df& factor, u32 level=0) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				scaleTCoords(mesh->getMeshBuffer(i), factor, level);
		}

		//! Applies a transformation to a meshbuffer
		/** \param buffer Meshbuffer on which the operation is performed.
		\param m transformation matrix. */
		virtual void transform(IMeshBuffer* meshBuffer, const core::matrix4& mat) const = 0;

		//! Applies a transformation to a mesh
		/** \param mesh Mesh on which the operation is performed.
		\param m transformation matrix. */
		void transform(IMesh* mesh, const core::matrix4& m) const
		{
			core::aabbox3df BufferBox;

			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
			{
				transform(mesh->getMeshBuffer(i), m);

				if (0 == i)
					BufferBox.reset(mesh->getMeshBuffer(i)->getBoundingBox());
				else
					BufferBox.addInternalBox(mesh->getMeshBuffer(i)->getBoundingBox());
			}

			mesh->setBoundingBox(BufferBox);
		}

		//! Applies a transformation to a mesh
		/** \deprecated Use transform() instead. This method may be removed by Irrlicht 1.9
		\param mesh Mesh on which the operation is performed.
		\param m transformation matrix. */
		_IRR_DEPRECATED_ virtual void transformMesh(IMesh* mesh, const core::matrix4& m) const {return transform(mesh,m);}

		//! Recalculates all normals of the mesh buffer.
		/** \param buffer: Mesh buffer on which the operation is performed.
		\param smooth: If the normals shall be smoothed.
		\param angleWeighted: If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision. */
		virtual void recalculateNormals(IMeshBuffer* meshBuffer, bool smooth = false, bool angleWeighted = false) const = 0;

		//! Recalculates all normals of the mesh.
		/** \param mesh: Mesh on which the operation is performed.
		\param smooth: If the normals shall be smoothed.
		\param angleWeighted: If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision. */
		void recalculateNormals(IMesh* mesh, bool smooth = false, bool angleWeighted = false) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				recalculateNormals(mesh->getMeshBuffer(i), smooth, angleWeighted);
		}

		virtual void recalculateTangents(IMeshBuffer* meshBuffer, bool recalculateNormals = false, bool smooth = false, bool angleWeighted = false) const = 0;

		void recalculateTangents(IMesh* mesh, bool recNormals = false, bool smooth = false, bool angleWeighted = false) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				recalculateTangents(mesh->getMeshBuffer(i), recNormals, smooth, angleWeighted);
		}

		//! Creates a planar texture mapping on the meshbuffer
		/** \param meshbuffer: Buffer on which the operation is performed.
		\param resolution: resolution of the planar mapping. This is
		the value specifying which is the relation between world space
		and texture coordinate space. */
		virtual void makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolution = 0.001f) const = 0;

		//! Creates a planar texture mapping on the mesh
		/** \param mesh: Mesh on which the operation is performed.
		\param resolution: resolution of the planar mapping. This is
		the value specifying which is the relation between world space
		and texture coordinate space. */
		void makePlanarTextureMapping(IMesh* mesh, f32 resolution=0.001f) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				makePlanarTextureMapping(mesh->getMeshBuffer(i), resolution);
		}

		//! Creates a planar texture mapping on the meshbuffer
		/** This method is currently implemented towards the LWO planar mapping. A more general biasing might be required.
		\param buffer Buffer on which the operation is performed.
		\param resolutionS Resolution of the planar mapping in horizontal direction. This is the ratio between object space and texture space.
		\param resolutionT Resolution of the planar mapping in vertical direction. This is the ratio between object space and texture space.
		\param axis The axis along which the texture is projected. The allowed values are 0 (X), 1(Y), and 2(Z).
		\param offset Vector added to the vertex positions (in object coordinates).
		*/
		virtual void makePlanarTextureMapping(IMeshBuffer* meshBuffer, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset) const = 0;

		//! Creates a planar texture mapping on the buffer
		/** This method is currently implemented towards the LWO planar mapping. A more general biasing might be required.
		\param mesh Mesh on which the operation is performed.
		\param resolutionS Resolution of the planar mapping in horizontal direction. This is the ratio between object space and texture space.
		\param resolutionT Resolution of the planar mapping in vertical direction. This is the ratio between object space and texture space.
		\param axis The axis along which the texture is projected. The allowed values are 0 (X), 1(Y), and 2(Z).
		\param offset Vector added to the vertex positions (in object coordinates).
		*/
		void makePlanarTextureMapping(scene::IMesh* mesh,
				f32 resolutionS, f32 resolutionT,
				u8 axis, const core::vector3df& offset) const
		{
			for(u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
				makePlanarTextureMapping(mesh->getMeshBuffer(i), resolutionS, resolutionT, axis, offset);
		}

		virtual bool copyIndices(IIndexBuffer* srcBuffer, IIndexBuffer* dstBuffer) const = 0;

		virtual bool copyVertices(IVertexBuffer* srcBuffer, u32 srcDescriptionBufferID, video::IVertexDescriptor* srcDescriptor,
			IVertexBuffer* dstBuffer, u32 dstDescriptionBufferID, video::IVertexDescriptor* dstDescriptor, bool copyCustomAttribute) const = 0;

		virtual bool createTangents(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, bool copyCustomAttribute,
			bool recalculateNormals = false, bool smooth = false, bool angleWeighted= false) = 0;

		virtual bool createUniquePrimitives(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer) const = 0;

		virtual bool createWelded(IMeshBuffer* srcBuffer, IMeshBuffer* dstBuffer, f32 tolerance = core::ROUNDING_ERROR_f32,
			bool check4Component = true, bool check3Component = true, bool check2Component = true, bool check1Component = true) const = 0;

		// Support mesh buffers with only one vertex buffer per mesh buffer.
		template <class T>
		bool convertVertices(IMeshBuffer* buffer, video::IVertexDescriptor* descriptor, bool copyCustomAttribute = false)
		{
			if (!buffer || !buffer->isVertexBufferCompatible() || buffer->getVertexBufferCount() > 1 ||
				!descriptor || descriptor->getVertexSize(0) != sizeof(T))
				return false;

			scene::CVertexBuffer<T>* vb = new scene::CVertexBuffer<T>();

			bool status = false;
			
			if (copyVertices(buffer->getVertexBuffer(0), 0, buffer->getVertexDescriptor(), vb, 0, descriptor, false))
			{
				buffer->setVertexDescriptor(descriptor);
				buffer->setVertexBuffer(vb, 0);
				status = true;
			}

			vb->drop();

			return status;
		}

		// Support mesh buffers with only one vertex buffer per mesh buffer.
		template <class T>
		bool createTangents(IMeshBuffer* buffer, video::IVertexDescriptor* descriptor, bool copyCustomAttribute = false)
		{
			if (!convertVertices<T>(buffer, descriptor, copyCustomAttribute))
				return false;

			recalculateTangents(buffer, false, false, false);

			return true;
		}

		// Support mesh buffers with only one vertex buffer per mesh buffer.
		template <class T>
		SMesh* createMeshCopy(IMesh* mesh, video::IVertexDescriptor* descriptor, bool copyCustomAttribute = false)
		{
			if (!mesh || mesh->getMeshBufferCount() == 0 || !descriptor || descriptor->getVertexSize(0) != sizeof(T))
				return NULL;

			const u32 mbCount = mesh->getMeshBufferCount();

			for (u32 i = 0; i < mbCount; ++i)
			{
				const IMeshBuffer* const mb = mesh->getMeshBuffer(i);

				if (!mb->isVertexBufferCompatible() || mb->getVertexBufferCount() > 1)
					return NULL;
			}

			SMesh* dstMesh = new SMesh();

			for (u32 i = 0; i < mbCount; ++i)
			{
				IMeshBuffer* srcBuffer = mesh->getMeshBuffer(i);
				IIndexBuffer* srcIndexBuffer = srcBuffer->getIndexBuffer();
				CMeshBuffer<T>* dstBuffer = new CMeshBuffer<T>(descriptor, srcIndexBuffer->getType());

				dstBuffer->getMaterial() = srcBuffer->getMaterial();

				copyIndices(srcIndexBuffer, dstBuffer->getIndexBuffer());
				copyVertices(srcBuffer->getVertexBuffer(0), 0, srcBuffer->getVertexDescriptor(), dstBuffer->getVertexBuffer(0), 0, dstBuffer->getVertexDescriptor(), copyCustomAttribute);

				dstMesh->addMeshBuffer(dstBuffer);
				dstBuffer->drop();
			}

			dstMesh->BoundingBox = mesh->getBoundingBox();

			return dstMesh;
		}

		//! Get amount of polygons in mesh.
		/** \param mesh Input mesh
		\return Number of polygons in mesh. */
		virtual s32 getPolyCount(IMesh* mesh) const = 0;		

		//! Vertex cache optimization according to the Forsyth paper
		/** More information can be found at
		http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html

		The function is thread-safe (read: you can optimize several
		meshes in different threads).

		\param mesh Source mesh for the operation.
		\return A new mesh optimized for the vertex cache. */
		virtual IMesh* createForsythOptimizedMesh(const IMesh *mesh) const = 0;
};

} // end namespace scene
} // end namespace irr


#endif
