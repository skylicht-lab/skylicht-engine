// Copyright (C) 2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_VERTEX_BUFFER_H_INCLUDED__
#define __I_VERTEX_BUFFER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrArray.h"
#include "IHardwareBuffer.h"

namespace irr
{
namespace scene
{
	class IVertexBuffer : public virtual IReferenceCounted
	{
	public:
		IVertexBuffer() : HardwareBuffer(0)
		{
		}

		virtual ~IVertexBuffer()
		{
			if (HardwareBuffer)
				HardwareBuffer->drop();
		}

		virtual void clear() = 0;

		virtual void set_used(u32 used) = 0;

		virtual void reallocate(u32 size) = 0;

		virtual u32 allocated_size() const = 0;

		virtual s32 linear_reverse_search(const void* element) const = 0;

		virtual void fill(u32 used) = 0;

		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const = 0;

		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING hardwareMappingHint) = 0;

		virtual void addVertex(const void* vertex) = 0;

		virtual const void* getVertex(u32 id) const = 0;

		virtual void* getVertices() = 0;

		virtual u32 getVertexCount() const = 0;

		virtual u32 getVertexSize() const = 0;

		virtual void setVertex(u32 id, const void* vertex) = 0;

		virtual void setDirty() = 0;

		virtual u32 getChangedID() const = 0;

		video::IHardwareBuffer* getHardwareBuffer() const
		{
			return HardwareBuffer;
		}

		// externalMemoryHandler parameter is used only by hardware buffers.
		void setHardwareBuffer(video::IHardwareBuffer* hardwareBuffer, bool externalMemoryHandler = false)
		{
			if (!externalMemoryHandler && HardwareBuffer)
				HardwareBuffer->drop();

			HardwareBuffer = hardwareBuffer;

			if (!externalMemoryHandler && HardwareBuffer)
				HardwareBuffer->grab();
		}

	protected:
		video::IHardwareBuffer* HardwareBuffer;
	};
}
}

#endif
