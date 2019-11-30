// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#pragma once

namespace irr
{
	class IrrlichtDevice;
}

namespace Skylicht
{

// initSkylicht
void initSkylicht(IrrlichtDevice *device, bool server = false);

// releaseSkylicht
void releaseSkylicht();

// updateSkylicht
void updateSkylicht();

// getIrrlichtDevice
IrrlichtDevice* getIrrlichtDevice();

// getVideoDriver
IVideoDriver* getVideoDriver();

// getTimeStep
float getTimeStep();

// setTimeStep
void setTimeStep(float timestep);

}