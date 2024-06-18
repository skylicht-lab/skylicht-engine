/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

// IRRLICHT 3D ENGINE
#include "irrlicht.h"
#include "IRandomizer.h"
#include "ILogger.h"
#include "irrOS.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

// SKYLICHT ENGINE
#include "SkylichtSystemAPI.h"

#include "SkylichtConfig.h"
#include "Skylicht.h"

#ifdef BUILD_SKYLICHT_COMPONENTS
#include "CollisionConfig.h"
#endif

#ifdef BUILD_SKYLICHT_COLLISION
#include "CollisionConfig.h"
#endif

#ifdef BUILD_SKYLICHT_COMPONENTS
#include "ComponentsConfig.h"
#endif

#ifdef BUILD_SKYLICHT_AUDIO
#include "SkylichtAudioAPI.h"
#include "SkylichtAudio.h"
#endif

using namespace Skylicht;

// STL C++
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <fstream>
#include <string>