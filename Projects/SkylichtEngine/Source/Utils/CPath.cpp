#include "pch.h"
#include "CPath.h"
#include "CStringImp.h"

namespace Skylicht
{
	char tempPath[512];
	char resultPath[512];

	std::string CPath::getFileName(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFileName<char, char>(tempPath, resultPath);
		return resultPath;
	}

	std::string CPath::getFileNameExt(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFileNameExt<char, char>(tempPath, resultPath);
		return resultPath;
	}

	std::string CPath::getFileNameNoExt(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFileNameNoExt<char, char>(tempPath, resultPath);
		return resultPath;
	}

	std::string CPath::replaceFileExt(const std::string& path, const std::string& ext)
	{
		CStringImp::copy<char, const char>(resultPath, path.c_str());
		CStringImp::replacePathExt(resultPath, ext.c_str());
		return resultPath;
	}

	std::string CPath::getFolderPath(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFolderPath<char, char>(tempPath, resultPath);
		return resultPath;
	}

	std::string CPath::normalizePath(const std::string& path)
	{
		std::vector<std::string> listFolder;
		std::vector<std::string> finalFolder;

		// copy and replace
		CStringImp::replaceText<char>(path.c_str(), "\\", "/", tempPath);

		int nPos = 0;

		while (CStringImp::split<char>(resultPath, tempPath, "/", &nPos) == true)
		{
			listFolder.push_back(resultPath);
		}

		for (int i = 0, n = (int)listFolder.size(); i < n; i++)
		{
			if (listFolder[i] != ".." || finalFolder.size() == 0)
				finalFolder.push_back(listFolder[i]);
			else
			{
				int id = (int)finalFolder.size() - 1;
				finalFolder.erase(finalFolder.begin() + id);
			}
		}

		std::string finalPath;

		for (int i = 0, n = (int)finalFolder.size(); i < n; i++)
		{
			finalPath += finalFolder[i];
			if (i + 1 < n)
				finalPath = finalPath + "/";
		}

		return finalPath;
	}

}