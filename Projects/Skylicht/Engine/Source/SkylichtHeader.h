/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/
#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#ifdef BUILD_SKYLICHT_NETWORK
#include <Ws2tcpip.h>
#include <Winsock2.h>
#endif

#include <memory.h>
#include <stdlib.h>

#include <Windows.h>

#else

#include <memory.h>
#include <unistd.h>

#ifdef BUILD_SKYLICHT_NETWORK
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

#define SOCKET  int
#define INVALID_SOCKET  -1
#define SOCKET_ERROR -1
#endif

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
	extern IrrlichtDevice* getIrrlichtDevice();

	extern IVideoDriver* getVideoDriver();

	extern float getTimeStep();

	extern float getTotalTime();
}