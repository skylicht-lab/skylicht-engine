/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#include <stddef.h>
#include "Entity/IEntityData.h"

namespace Skylicht
{
	class CLightCullingData;

	class SKYLICHT_API CRenderLightData : public IEntityData
	{
	protected:
		u32 LightLayers;

		bool EnableSortLight;

	public:
		struct SCacheLight
		{
			CLightCullingData* Lights[4];
			float Distances[4];
			int Count;
			size_t Signature;

			SCacheLight()
			{
				invalidate();
			}

			void invalidate()
			{
				Count = 0;
				Signature = 0;

				for (int i = 0; i < 4; i++)
				{
					Lights[i] = NULL;
					Distances[i] = 0.0f;
				}
			}
		};

		bool LightCacheValid;
		size_t CachedLightVersion;
		core::vector3df CachedLightPosition;
		u32 CachedLightLayers;

		SCacheLight CachedDirectionalLights;
		SCacheLight CachedPointLights;
		SCacheLight CachedSpotLights;
		SCacheLight CachedAreaLights;

		CRenderLightData() :
			LightLayers(1),
			EnableSortLight(false),
			LightCacheValid(false),
			CachedLightVersion(0),
			CachedLightLayers(1)
		{
		}

		virtual ~CRenderLightData()
		{

		}

		inline bool isSortingLights()
		{
			return LightLayers != 1 || EnableSortLight;
		}

		inline void setLightLayers(u32 layers)
		{
			if (LightLayers != layers)
			{
				LightLayers = layers;
				invalidateLightCache();
			}
		}

		inline u32 getLightLayers()
		{
			return LightLayers;
		}

		inline void enableSortLight(bool b)
		{
			if (EnableSortLight != b)
			{
				EnableSortLight = b;
				invalidateLightCache();
			}
		}

		inline bool isEnableSortLight()
		{
			return EnableSortLight;
		}

		inline void invalidateLightCache()
		{
			LightCacheValid = false;
			CachedLightVersion = 0;
			CachedDirectionalLights.invalidate();
			CachedPointLights.invalidate();
			CachedSpotLights.invalidate();
			CachedAreaLights.invalidate();
		}
	};
}
