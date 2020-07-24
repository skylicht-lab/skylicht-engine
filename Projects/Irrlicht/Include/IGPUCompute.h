// Copyright (C) 2020 Pham Hong Duc
// This file is part of the "Skylicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Add irrlicht compute shader feature

#ifndef __I_GPU_COMPUTE_H_INCLUDED__
#define __I_GPU_COMPUTE_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "IReferenceCounted.h"
#include "EDriverTypes.h"

namespace irr
{
	namespace video
	{
		class IRWBuffer;
		class ITexture;

		class IGPUCompute : public virtual IReferenceCounted
		{
		public:
			IGPUCompute() :
				DriverType(EDT_NULL)
			{

			}

			E_DRIVER_TYPE getDriverType() const { return DriverType; };

			virtual void setTexture(int slot, ITexture *texture) = 0;

			virtual void setBuffer(int slot, IRWBuffer *buffer) = 0;

			virtual void dispatch(int threadGroupX, int threadGroupY, int threadGroupZ) = 0;

		protected:

			E_DRIVER_TYPE DriverType;
		};
	}
}

#endif