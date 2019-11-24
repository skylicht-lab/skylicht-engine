// Copyright (C) 2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_INDEX_BUFFER_H_INCLUDED__
#define __I_INDEX_BUFFER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrArray.h"
#include "IHardwareBuffer.h"

namespace irr
{
namespace video
{
	enum E_INDEX_TYPE
	{
		EIT_16BIT = 0,
		EIT_32BIT
	};
}
namespace scene
{
	class IIndexBuffer : public virtual IReferenceCounted
	{
	public:
		IIndexBuffer() : HardwareBuffer(0)
		{
		}

		virtual ~IIndexBuffer()
		{
			if (HardwareBuffer)
				HardwareBuffer->drop();
		}

		virtual void clear() = 0;

		virtual u32 getLast() = 0;

		virtual void set_used(u32 used) = 0;

		virtual void reallocate(u32 size) = 0;

		virtual u32 allocated_size() const = 0;

		virtual s32 linear_reverse_search(const u32& element) const = 0;

		virtual video::E_INDEX_TYPE getType() const = 0;

		virtual void setType(video::E_INDEX_TYPE type) = 0;

		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const = 0;

		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING hardwareMappingHint) = 0;

		virtual void addIndex(const u32& index) = 0;

		virtual u32 getIndex(u32 id) const = 0;

		virtual void* getIndices() = 0;

		virtual u32 getIndexCount() const = 0;

		virtual u32 getIndexSize() const = 0;

		virtual void setIndex(u32 id, u32 index) = 0;

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
