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

#include "CMaterial.h"
#include "Utils/CSingleton.h"
#include "Entity/CEntityPrefab.h"

namespace Skylicht
{
	/// @brief Singleton class for loading, caching, exporting, and managing materials in Skylicht-Engine.
	/// @ingroup Materials
	/// 
	/// Provides functions to load materials from files, manage their lifetimes, export and save materials, replace textures, and initialize default materials from prefabs.
	/// 
	/// Example usage:
	/// @code
	/// std::vector<std::string> textureFolders;
	/// ArrayMaterial material = CMaterialManager::getInstance()->loadMaterial("SampleAnimations/HeroArtwork/Hero.mat", true, textureFolders);
	/// @endcode
	class SKYLICHT_API CMaterialManager
	{
	public:
		DECLARE_SINGLETON(CMaterialManager)

	protected:
		/// Map of loaded materials by filename
		std::map<std::string, ArrayMaterial> m_materials;

		/// List of generated materials not bound to a file
		ArrayMaterial m_listGenerateMaterials;

	public:
		/**
		 * @brief Constructor.
		 */
		CMaterialManager();

		/**
		 * @brief Destructor. Releases all loaded and generated materials.
		 */
		virtual ~CMaterialManager();

		/**
		 * @brief Get the list of supported material file extensions.
		 * @return Vector of supported extensions (e.g., "xml", "mat").
		 */
		static std::vector<std::string> getMaterialExts();

		/**
		 * @brief Check if the given extension is a valid material file extension.
		 * @param ext Extension string (without dot).
		 * @return True if supported, false otherwise.
		 */
		static bool isMaterialExt(const char* ext);

		/**
		 * @brief Check if a material file has already been loaded and cached.
		 * @param filename Path to material file.
		 * @return True if material is loaded, false otherwise.
		 */
		bool isMaterialLoaded(const char* filename);

		/**
		 * @brief Release all loaded and generated materials from memory.
		 */
		void releaseAllMaterials();

		/**
		 * @brief Release all materials that belong to a specific package.
		 * @param package The package name to match.
		 */
		void releaseAllMaterials(const char* package);

		/**
		 * @brief Load a material file (.mat or .xml), parse its contents, and return the loaded materials.
		 *        Materials are cached by filename and loaded only once.
		 * @param filename Path to material file.
		 * @param loadTexture If true, loads textures referenced in material.
		 * @param textureFolders Additional folders to search for textures.
		 * @return Reference to ArrayMaterial containing loaded materials.
		 */
		ArrayMaterial& loadMaterial(const char* filename, bool loadTexture, const std::vector<std::string>& textureFolders);

		/**
		 * @brief Unload and release all materials loaded from a specific file.
		 * @param filename Path to material file.
		 */
		void unloadMaterial(const char* filename);

		/**
		 * @brief Create a new material and add it to the given material array.
		 * @param materials ArrayMaterial to add the new material to.
		 * @return Pointer to the newly created material.
		 */
		CMaterial* createAndAddMaterial(ArrayMaterial& materials);

		/**
		 * @brief Replace all occurrences of an old texture with a new texture in all managed materials.
		 * @param oldTexture Pointer to the old texture.
		 * @param newTexture Pointer to the new texture.
		 */
		void replaceTexture(ITexture* oldTexture, ITexture* newTexture);

		/**
		 * @brief Delete a material from the given material array and also from the internal cache if present.
		 * @param materials ArrayMaterial containing the material.
		 * @param material Pointer to the material to delete.
		 */
		void removeAndDeleteMaterial(ArrayMaterial& materials, CMaterial* material);

		/**
		 * @brief Save a set of materials to a material file, serializing their parameters, textures, and properties.
		 * @param materials ArrayMaterial to save.
		 * @param filename Path to material file to write.
		 */
		void saveMaterial(const ArrayMaterial& materials, const char* filename);

		/**
		 * @brief Export materials from a prefab (e.g., from a model) to a material file.
		 *        Extracts shader, texture, and uniform information from the prefab's mesh data.
		 * @param prefab Pointer to the prefab containing mesh and material data.
		 * @param filename Path to material file to write.
		 */
		void exportMaterial(CEntityPrefab* prefab, const char* filename);

		/**
		 * @brief Initialize default materials for all meshes in a prefab, creating and assigning basic material data.
		 *        Useful for generating materials directly from model data.
		 * @param prefab Pointer to the prefab.
		 * @return ArrayMaterial containing created materials.
		 */
		ArrayMaterial initDefaultMaterial(CEntityPrefab* prefab);
	};
}