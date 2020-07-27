// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine"
// Upgrade GPU Compute Shader feature

#ifndef __IRR_IRW_BUFFER_H_INCLUDED__
#define __IRR_IRW_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "IReferenceCounted.h"
#include "EDriverTypes.h"

namespace irr
{
	namespace video
	{
		class IRWBuffer : public virtual IReferenceCounted
		{
		public:
			IRWBuffer(ECOLOR_FORMAT format, u32 numElements) :
				DriverType(EDT_NULL),
				Format(format),
				NumElements(numElements)
			{
			}

			E_DRIVER_TYPE getDriverType() const { return DriverType; };

			virtual void* lock(bool readOnly) = 0;

			virtual void unlock() = 0;

		protected:

			E_DRIVER_TYPE DriverType;
			ECOLOR_FORMAT Format;
			u32 NumElements;
		};
	}
}

#endif