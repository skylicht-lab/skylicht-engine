#pragma once

#include <string>

namespace Skylicht
{
	namespace Editor
	{
		enum class EImageMapStatus
		{
			None = 0,
			Error,
			FileError,
			NotFound,
			Downloaded,
		};

		enum class EImageMapType
		{
			GSatellite = 0,
			OSMTerrain
		};

		struct SImageMapElement
		{
			ITexture* Texture;
			EImageMapStatus Status;
			EImageMapType Type;
			std::string Path;
			long X;
			long Y;
			int Z;

			SImageMapElement()
			{
				Texture = NULL;
				Status = EImageMapStatus::None;
				Type = EImageMapType::GSatellite;
				X = 0;
				Y = 0;
				Z = -1;
			}
		};

		struct SImageDownload
		{
			SImageMapElement Image;
			std::string Url;

			SImageDownload()
			{
			}
		};

		std::string getMapTileURL(EImageMapType type, long x, long y, long z);

		std::string getMapTileLocalCache(EImageMapType type, long x, long y, long z);
	}
}