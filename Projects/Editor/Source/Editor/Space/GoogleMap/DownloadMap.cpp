#include "pch.h"
#include "DownloadMap.h"

#include <filesystem>
#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

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
					"https://khms0.google.com/kh/v=908?x=%ld&y=%ld&z=%ld",
					x, y, z
				);
				break;
			case EImageMapType::OSMTerrain:
				sprintf(lpUrl,
					"https://api.maptiler.com/maps/outdoor/%ld/%ld/%ld.png",
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
			// create folder cache
			if (!fs::exists("../Maps"))
				fs::create_directory("../Maps");

			char fileName[512] = { 0 };
			switch (type)
			{
			case EImageMapType::GSatellite:
			{
				sprintf(fileName, "../Maps/gmap_%ld_%ld_%ld.jpg", z, x, y);
				break;
			}
			case EImageMapType::OSMTerrain:
				sprintf(fileName, "../Maps/osm_%ld_%ld_%ld.png", z, x, y);
				break;
			default:
				break;
			}

			return std::string(fileName);
		}
	}
}