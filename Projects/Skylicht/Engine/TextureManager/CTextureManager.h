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
	/// @brief The object class supports texture management
	/// @ingroup Materials
	/// 
	/// @code
	/// ITexture *texture = CTextureManager::getInstance()->getTexture("BuiltIn/Textures/Skylicht.png");
	/// @endcode
	class SKYLICHT_API CTextureManager
	{
	public:
		DECLARE_SINGLETON(CTextureManager)

		static const char* getGlobalName();

	protected:
		struct STexturePackage
		{
			std::string Package;
			std::string Path;
			ITexture* Texture;

			STexturePackage()
			{
				Texture = NULL;
			}
		};

		std::string m_currentPackage;
		std::vector<STexturePackage*> m_textureList;

		std::vector<std::string> m_listCommonTexture;
		int m_loadCommonPos;

		ITexture* m_nullNormalMap;
		ITexture* m_nullTexture;

	public:
		CTextureManager();

		virtual ~CTextureManager();

		static std::vector<std::string> getTextureExts();

		static bool isTextureExt(const char* ext);

		inline void setCurrentPackage(const char* name)
		{
			m_currentPackage = name;
		}

		inline const char* getCurrentPackage()
		{
			return m_currentPackage.c_str();
		}

		void registerTexture(ITexture* tex, const char* path);

		void removeAllTexture();

		void removeTexture(const char* namePackage);

		void removeTexture(ITexture* tex);

		const char* getTexturePath(ITexture* tex);

		bool existTexture(const char* path);

		bool findRealTexturePath(const char* path, std::string& result);

		bool isTextureLoaded(const char* path);

		ITexture* getTexture(const char* path);

		ITexture* getTexture(const char* filename, const std::vector<std::string>& folders);

		ITexture* getTextureFromRealPath(const char* path);

		ITexture* getCubeTexture(
			const char* pathX1,
			const char* pathX2,
			const char* pathY1,
			const char* pathY2,
			const char* pathZ1,
			const char* pathZ2);

		ITexture* getTextureArray(std::vector<std::string>& listTexture);

		ITexture* getNullNormalMap()
		{
			return m_nullNormalMap;
		}

		ITexture* getNullTexture()
		{
			return m_nullTexture;
		}

		ITexture* createTransformTexture2D(const char* name, core::matrix4* transforms, int w, int h);

		ITexture* createVectorTexture2D(const char* name, core::vector3df* vectors, int w, int h);
	};

}