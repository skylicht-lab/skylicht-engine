#pragma once

#include "pch.h"
#include "DownloadMap.h"

namespace Skylicht
{
	namespace Editor
	{
		std::string getMapTileURL(EImageMapType type, long x, long y, long z)
		{
			char lpUrl[512] = { 0 };

			switch (type)
			{
			case EImageMapType::GSatellite:
				sprintf(lpUrl,
					"https://khms0.google.com/kh/v=908?x=%ld&y=%ld&z=%d",
					x, y, z
				);
				break;
			case EImageMapType::OSMTerrain:
				sprintf(lpUrl,
					"https://api.maptiler.com/maps/outdoor/%d/%ld/%ld.png",
					z, x, y
				);
				break;
			default:
				break;
			}

			return std::string(lpUrl);
		}

		std::string getMapTileLocalCache(EImageMapType type, long x, long y, long z)
		{
			char fileName[512] = { 0 };
			switch (type)
			{
			case EImageMapType::GSatellite:
			{
				sprintf(fileName, "gmap_%d_%ld_%ld.jpg", z, x, y);
				break;
			}
			case EImageMapType::OSMTerrain:
				sprintf(fileName, "osm_%d_%ld_%ld.png", z, x, y);
				break;
			default:
				break;
			}
			return std::string(fileName);
		}
	}
}