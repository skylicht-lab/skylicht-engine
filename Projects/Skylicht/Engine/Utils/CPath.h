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

namespace Skylicht
{
	/**
	 * @brief Path manipulation helpers for engine resource and file paths.
	 * @ingroup Utilities
	 *
	 * Most methods return a reference to a shared temporary string. Copy the returned
	 * value before calling another utility that may reuse the same temporary buffer.
	 */
	class SKYLICHT_API CPath
	{
	public:
		/**
		 * @brief Get the file name, including extension, from a path.
		 * @param path Input path.
		 * @return File name stored in the shared temporary buffer.
		 */
		static const std::string& getFileName(const std::string& path);

		/**
		 * @brief Get only the extension from a file path.
		 * @param path Input path.
		 * @return Extension without the dot, or an empty string when no extension exists.
		 */
		static const std::string& getFileNameExt(const std::string& path);

		/**
		 * @brief Get the file name without its extension.
		 * @param path Input path.
		 * @return File name without extension.
		 */
		static const std::string& getFileNameNoExt(const std::string& path);

		/**
		 * @brief Replace the extension of a file path.
		 * @param path Input path.
		 * @param ext New extension.
		 * @return Path with the new extension.
		 */
		static const std::string& replaceFileExt(const std::string& path, const std::string& ext);

		/**
		 * @brief Get the folder portion of a path.
		 * @param path Input path.
		 * @return Folder path without the file name.
		 */
		static const std::string& getFolderPath(const std::string& path);

		/**
		 * @brief Get the parent folder of a path or folder.
		 * @param path Input path.
		 * @return Parent folder path.
		 */
		static const std::string& getParentFolderPath(const std::string& path);

		/**
		 * @brief Normalize slashes and collapse `..` path elements.
		 * @param path Input path.
		 * @return Normalized path.
		 */
		static const std::string& normalizePath(const std::string& path);

		/**
		 * @brief Convert a path to be relative to a folder when they share a root.
		 * @param path Absolute or base path to convert.
		 * @param folder Folder used as the relative origin.
		 * @return Relative path, or the original path when no shared root exists.
		 */
		static const std::string& getRelativePath(const std::string& path, const std::string& folder);

		/**
		 * @brief Match a file name against a simple wildcard pattern.
		 *
		 * Supports `?` for one character and `*` for a variable number of characters.
		 * Matching is case-insensitive.
		 *
		 * @param path Path whose file name will be tested.
		 * @param searchPattern Pattern to match.
		 * @return True if the file name matches the pattern.
		 */
		static bool searchMatch(const std::string& path, const std::string& searchPattern);
	};
}
