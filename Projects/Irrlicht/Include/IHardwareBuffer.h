// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_HARDWARE_BUFFER_H_INCLUDED__
#define __I_HARDWARE_BUFFER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "EDriverTypes.h"
#include "EHardwareBufferFlags.h"

namespace irr
{
namespace video
{

enum E_HARDWARE_BUFFER_TYPE
{
	//! Does not change anything
	EHBT_NONE=0,
	//! Change the vertex mapping
	EHBT_VERTEX,
	//! Change the index mapping
	EHBT_INDEX,
	//! Change the stream output/feedback mapping (stream output also include vertex)
	EHBT_STREAM_OUTPUT,
	//! Change the shader resource mapping
	EHBT_SHADER_RESOURCE,
	//! Change the compute mapping
	EHBT_COMPUTE,
	//! Change the constants mapping
	EHBT_CONSTANTS,
	//! Change the system mapping
	EHBT_SYSTEM
};

enum E_HARDWARE_BUFFER_ACCESS
{
	EHBA_DYNAMIC = 0,
	EHBA_DEFAULT,
	EHBA_IMMUTABLE,
	EHBA_SYSTEM_MEMORY
};

enum E_HARDWARE_BUFFER_FLAGS
{
	EHBF_INDEX_16_BITS = 1,
	EHBF_INDEX_32_BITS = 1 << 1,
	EHBF_VERTEX_ADDITIONAL_BIND = 1 << 2,
	EHBF_INDEX_ADDITIONAL_BIND = 1 << 3,
	EHBF_SHADER_ADDITIONAL_BIND = 1 << 4,
	EHBF_DRAW_INDIRECT_ARGS = 1 << 5,
	EHBF_COMPUTE_RAW = 1 << 6,
	EHBF_COMPUTE_STRUCTURED = 1 << 7,
	EHBF_COMPUTE_APPEND = 1 << 8,
	EHBF_COMPUTE_CONSUME = 1 << 9
};

class IHardwareBuffer : public virtual IReferenceCounted
{
public:
	IHardwareBuffer(const scene::E_HARDWARE_MAPPING mapping, const u32 flags, const u32 size, const E_HARDWARE_BUFFER_TYPE type, const E_DRIVER_TYPE driverType) :
		Mapping(mapping), Flags(flags), Size(size), Type(type), DriverType(driverType), RequiredUpdate(true)
	{
	}

	virtual ~IHardwareBuffer()
	{
	}

	// Update hardware buffer.
	virtual bool update(const scene::E_HARDWARE_MAPPING mapping, const u32 size, const void* data) = 0;

	// Inform if update is required.
	inline bool isRequiredUpdate() const
	{
		return RequiredUpdate;
	}

	// Request update.
	inline void requestUpdate()
	{
		RequiredUpdate = true;
	}

	//! Get mapping for buffer.
	inline scene::E_HARDWARE_MAPPING getMapping() const
	{
		return Mapping;
	}

	//! Get flags for buffer.
	inline u32 getFlags() const
	{
		return Flags;
	}

	//! Get size of buffer in bytes.
	inline u32 size() const
	{
		return Size;
	}

	//! Get type of buffer.
	inline E_HARDWARE_BUFFER_TYPE getType() const
	{
		return Type;
	}

	//! Get driver type of buffer.
	inline E_DRIVER_TYPE getDriverType() const
	{
		return DriverType;
	}

protected:
	scene::E_HARDWARE_MAPPING Mapping;
	u32 Flags;
	u32 Size;
	E_HARDWARE_BUFFER_TYPE Type;
	E_DRIVER_TYPE DriverType;

	bool RequiredUpdate;
};

}
}

#endif
