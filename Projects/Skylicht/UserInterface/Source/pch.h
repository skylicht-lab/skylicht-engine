//
// pch.h
// Header for standard system include files.
//

#pragma once

#if defined(_WIN32)

#ifdef HAVE_SKYLICHT_NETWORK
#include <Ws2tcpip.h>
#include <Winsock2.h>
#endif

#include <memory.h>
#include <stdlib.h>

// Windows Header Files:
#include <windows.h>
#else
#include <memory.h>
#include <unistd.h>

#ifdef HAVE_SKYLICHT_NETWORK
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#define SOCKET  int
#define INVALID_SOCKET  -1
#define SOCKET_ERROR -1
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