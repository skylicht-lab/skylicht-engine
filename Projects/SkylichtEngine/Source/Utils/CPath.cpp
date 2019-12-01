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