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
	/// @brief The object class supports material management
	/// @ingroup Materials
	/// 
	/// @code
	/// std::vector<std::string> textureFolders;
	/// ArrayMaterial material = CMaterialManager::getInstance()->loadMaterial("SampleAnimations/HeroArtwork/Hero.mat", true, textureFolders);
	/// @endcode
	class SKYLICHT_API CMaterialManager
	{
	public:
		DECLARE_SINGLETON(CMaterialManager)

	protected:
		std::map<std::string, ArrayMaterial> m_materials;

		ArrayMaterial m_listGenerateMaterials;

	public:
		CMaterialManager();

		virtual ~CMaterialManager();

		static std::vector<std::string> getMaterialExts();

		static bool isMaterialExt(const char* ext);

		bool isMaterialLoaded(const char* filename);

		void releaseAllMaterials();

		void releaseAllMaterials(const char* package);

		ArrayMaterial& loadMaterial(const char* filename, bool loadTexture, const std::vector<std::string>& textureFolders);

		void unloadMaterial(const char* filename);

		CMaterial* createMaterial(ArrayMaterial& materials);

		void replaceTexture(ITexture* oldTexture, ITexture* newTexture);

		void deleteMaterial(ArrayMaterial& materials, CMaterial* material);

		void saveMaterial(const ArrayMaterial& materials, const char* filename);

		void exportMaterial(CEntityPrefab* prefab, const char* filename);

		ArrayMaterial initDefaultMaterial(CEntityPrefab* prefab);
	};
}