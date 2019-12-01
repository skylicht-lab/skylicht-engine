#ifndef _PATH_H_
#define _PATH_H_

namespace Skylicht
{
	class CPath
	{
	public:
		static std::string getFileName(const std::string& path);

		static std::string getFileNameExt(const std::string& path);

		static std::string getFileNameNoExt(const std::string& path);

		static std::string replaceFileExt(const std::string& path, const std::string& ext);

		static std::string getFolderPath(const std::string& path);

		static std::string normalizePath(const std::string& path);
	};
}

#endif
