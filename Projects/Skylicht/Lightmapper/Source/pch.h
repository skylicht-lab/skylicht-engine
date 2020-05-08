//
// pch.h
// Header for standard system include files.
//

#pragma once

#if defined(_WIN32)
#include <memory.h>
#include <stdlib.h>
#else
#include <memory.h>
#include <unistd.h>
#endif

// IRRLICHT 3D ENGINE
#include "irrlicht.h"
#include "IRandomizer.h"
#include "ILogger.h"
#include "irrOS.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

// STL C++
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <fstream>
#include <string>

namespace Skylicht
{

	// getIrrlichtDevice
	extern IrrlichtDevice* getIrrlichtDevice();

	// getVideoDriver
	extern IVideoDriver* getVideoDriver();

	// getTimeStep
	extern float getTimeStep();

}