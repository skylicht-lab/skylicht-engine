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
		CStringImp::getFileName<char, char>(resultPath, tempPath);
		return resultPath;
	}

	std::string CPath::getFileNameExt(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFileNameExt<char, char>(resultPath, tempPath);
		return resultPath;
	}

	std::string CPath::getFileNameNoExt(const std::string& path)
	{
		CStringImp::copy<char, const char>(tempPath, path.c_str());
		CStringImp::getFileNameNoExt<char, char>(resultPath, tempPath);
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
		CStringImp::getFolderPath<char, char>(resultPath, tempPath);
		return resultPath;
	}

	std::string CPath::normalizePath(const std::string& path)
	{
		std::vector<std::string> listFolder;
		std::vector<std::string> finalFolder;

		// copy and replace
		CStringImp::replaceText<char>(tempPath, path.c_str(), "\\", "/");

		int nPos = 0;

		while (CStringImp::split<char>(resultPath, tempPath, "/", &nPos) == true)
		{
			listFolder.push_back(resultPath);
		}

		for (u32 i = 0, n = (u32)listFolder.size(); i < n; i++)
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

		for (u32 i = 0, n = (u32)finalFolder.size(); i < n; i++)
		{
			finalPath += finalFolder[i];
			if (i + 1 < n)
				finalPath = finalPath + "/";
		}

		return finalPath;
	}

	std::string CPath::getRelativePath(const std::string& path, const std::string& folder)
	{
		std::string result;

		std::vector<std::string> listFolder;
		std::vector<std::string> listFolderOfPath;

		std::string baseFolder = getFolderPath(path);
		std::string filename = getFileName(path);

		int nPos = 0;
		CStringImp::replaceText<char>(tempPath, folder.c_str(), "\\", "/");
		while (CStringImp::split<char>(resultPath, tempPath, "/", &nPos) == true)
		{
			listFolder.push_back(resultPath);
		}

		nPos = 0;
		CStringImp::replaceText<char>(tempPath, baseFolder.c_str(), "\\", "/");
		while (CStringImp::split<char>(resultPath, tempPath, "/", &nPos) == true)
		{
			listFolderOfPath.push_back(resultPath);
		}

		// they are not relative
		if (listFolderOfPath[0] != listFolder[0])
			return path;

		// find the same root path
		u32 i = 0;
		for (; i < listFolder.size() && i < listFolderOfPath.size() && (listFolder[i] == listFolderOfPath[i]); ++i)
		{

		}

		// add relative path
		u32 j = i;
		for (; i < listFolder.size(); ++i)
			result += "../";

		// add path to this file
		for (; j < listFolderOfPath.size(); ++j)
		{
			result += listFolderOfPath[j];
			result += "/";
		}

		result += filename;
		return result;
	}

	bool CPath::searchMatch(const std::string& path, const std::string& searchPattern)
	{
		std::string fn = CPath::getFileName(path);

		const char* filename = fn.c_str();
		const char* search = searchPattern.c_str();

		int len = CStringImp::length(filename);
		int searchLen = CStringImp::length(search);

		for (int i = 0; i < len - searchLen + 1; i++)
		{
			if (tolower(filename[i]) == tolower(search[0]) ||
				search[0] == '?' ||
				search[0] == '*')
			{
				bool found = true;
				bool passPattern = true;

				for (int j = 0, l = 0; j < searchLen && i + l < len; j++, l++)
				{
					if (search[j] == '?')
					{
						// alway true
					}
					else if (search[j] == '*')
					{
						passPattern = false;

						// next char == this char
						if (j < searchLen - 1 &&
							search[j + 1] == filename[i + l])
						{
							l--;
							passPattern = true;
						}
						else
						{
							// rollback at * character
							j--;
						}
					}
					else if (tolower(filename[i + l]) != tolower(search[j]))
					{
						// false
						found = false;
						break;
					}
				}

				if (found == true && passPattern)
					return true;
			}
		}

		return false;
	}
}