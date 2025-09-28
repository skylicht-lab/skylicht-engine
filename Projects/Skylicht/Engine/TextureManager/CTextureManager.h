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

#include "pch.h"

#include "Utils/CSingleton.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	/// @brief Texture Manager class provides APIs to load, retrieve, check, and release textures within the engine
	/// @ingroup Materials
	/// 
	/// Example usage:
	/// @code
	/// ITexture *texture = CTextureManager::getInstance()->getTexture("BuiltIn/Textures/Skylicht.png");
	/// @endcode
	class SKYLICHT_API CTextureManager
	{
	public:
		DECLARE_SINGLETON(CTextureManager)

		/**
		 * @brief Get the global package name.
		 * @return Global package name as a string.
		 */
		static const char* getGlobalName();

	protected:

		/**
		 * @struct STexturePackage
		 * @brief Structure storing texture info: package, path, and pointer to the texture.
		 */
		struct STexturePackage
		{
			/// Name of the package containing the texture.
			std::string Package;
			/// Actual path of the texture file.
			std::string Path;
			/// Pointer to the texture resource.
			ITexture* Texture;

			STexturePackage()
			{
				Texture = NULL;
			}
		};

		/// Current package used for loading textures.
		std::string m_currentPackage;

		/// List of loaded texture packages.
		std::vector<STexturePackage*> m_textureList;

		/// List of common textures.
		std::vector<std::string> m_listCommonTexture;

		/// Default null normal map texture.
		ITexture* m_nullNormalMap;

		/// Default null texture.
		ITexture* m_nullTexture;

	public:
		/**
		 * @brief Constructor.
		 */
		CTextureManager();

		/**
		* @brief Destructor. Releases all resources.
		*/
		virtual ~CTextureManager();

		/**
		 * @brief Get supported texture file extensions.
		 * @return Vector of supported extensions (e.g. png, jpg, bmp...).
		 */
		static std::vector<std::string> getTextureExts();

		/**
		 * @brief Check if a file extension is a supported texture format.
		 * @param ext File extension to check.
		 * @return True if supported, false otherwise.
		 */
		static bool isTextureExt(const char* ext);

		/**
		 * @brief Set the current texture package name.
		 * @param name Package name string.
		 */
		inline void setCurrentPackage(const char* name)
		{
			m_currentPackage = name;
		}

		/**
		 * @brief Get the current texture package name.
		 * @return Current package name string.
		 */
		inline const char* getCurrentPackage()
		{
			return m_currentPackage.c_str();
		}

		/**
		* @brief Remove all loaded textures.
		*/
		void removeAllTexture();

		/**
		 * @brief Remove all textures in a specific package.
		 * @param namePackage Name of the texture package.
		 */
		void removeTexture(const char* namePackage);

		/**
		 * @brief Remove a specific texture resource.
		 * @param tex Pointer to the texture to remove.
		 */
		void removeTexture(ITexture* tex);

		/**
		 * @brief Get the file path of a texture.
		 * @param tex Texture pointer.
		 * @return File path string of the texture.
		 */
		const char* getTexturePath(ITexture* tex);

		/**
		* @brief Check if a texture exists by path.
		* @param path Texture file path.
		* @return True if exists, false otherwise.
		*/
		bool existTexture(const char* path);

		/**
		 * @brief Finds the actual, existing file path for a texture, resolving the correct file extension.
		 * @param path Logical path to search for.
		 * @param result Output string for the real path found.
		 * @return True if found, false otherwise.
		 */
		bool resolveTexturePath(const char* path, std::string& result);

		/**
		 * @brief Check if a texture is already loaded.
		 * @param path Texture path.
		 * @return True if loaded, false otherwise.
		 */
		bool isTextureLoaded(const char* path);

		/**
		 * @brief Load and get a texture by path.
		 * @param path Texture file path.
		 * @return Pointer to loaded texture.
		 */
		ITexture* getTexture(const char* path);

		/**
		 * @brief Load and get a texture from a set of folders.
		 * @param filename Texture filename.
		 * @param folders List of folders to search for the texture.
		 * @return Pointer to loaded texture.
		 */
		ITexture* getTexture(const char* filename, const std::vector<std::string>& folders);

		/**
		* @brief Get texture using the real file path.
		* @param path Real file path.
		* @return Pointer to loaded texture.
		*/
		ITexture* getTextureFromRealPath(const char* path);

		/**
		* @brief Load a cube map texture from 6 image paths.
		* @param pathX1 Path for +X face.
		* @param pathX2 Path for -X face.
		* @param pathY1 Path for +Y face.
		* @param pathY2 Path for -Y face.
		* @param pathZ1 Path for +Z face.
		* @param pathZ2 Path for -Z face.
		* @return Pointer to cube map texture.
		*/
		ITexture* getCubeTexture(
			const char* pathX1,
			const char* pathX2,
			const char* pathY1,
			const char* pathY2,
			const char* pathZ1,
			const char* pathZ2);

		/**
		 * @brief Load a texture array from a list of texture paths.
		 * @param listTexture List of texture file paths.
		 * @return Pointer to texture array resource.
		 */
		ITexture* getTextureArray(std::vector<std::string>& listTexture);

		/**
		* @brief Get the default null normal map texture.
		* @return Pointer to null normal map.
		*/
		ITexture* getNullNormalMap()
		{
			return m_nullNormalMap;
		}

		/**
		 * @brief Get the default null texture.
		 * @return Pointer to null texture.
		 */
		ITexture* getNullTexture()
		{
			return m_nullTexture;
		}

		/**
		 * @brief Create a 2D texture from a list of transformation matrices.
		 * @param name Texture name.
		 * @param transforms Array of transformation matrices.
		 * @param w Width of matrix array.
		 * @param h Height of matrix array.
		 * @return Pointer to created texture.
		 */
		ITexture* createTransformTexture2D(const char* name, core::matrix4* transforms, int w, int h);

		/**
		 * @brief Create a 2D texture from a list of vector3df values.
		 * @param name Texture name.
		 * @param vectors Array of vector3df values.
		 * @param w Width of vector array.
		 * @param h Height of vector array.
		 * @return Pointer to created texture.
		 */
		ITexture* createVectorTexture2D(const char* name, core::vector3df* vectors, int w, int h);

	protected:

		/**
		 * @brief Register a texture with the manager.
		 * @param tex Pointer to texture.
		 * @param path File path of the texture.
		 */
		void registerTexture(ITexture* tex, const char* path);
	};

}