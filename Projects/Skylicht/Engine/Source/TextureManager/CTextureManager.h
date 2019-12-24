#ifndef _CTEXTURE_MANAGER_H_
#define _CTEXTURE_MANAGER_H_

#include "pch.h"

#include "Utils/CGameSingleton.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	class CTextureManager : public CGameSingleton<CTextureManager>
	{
	public:
		static const char *GlobalPackage;

	protected:
		struct STexturePackage
		{
			std::string	package;
			ITexture	*texture;
		};

		std::string						m_currentPackage;
		std::vector<STexturePackage*>	m_textureList;

		std::vector<std::string>		m_listCommonTexture;
		int		m_loadCommonPos;

	public:
		CTextureManager();
		virtual ~CTextureManager();

		void setCurrentPackage(const char *name)
		{
			m_currentPackage = name;
		}

		const char* getCurrentPackage()
		{
			return m_currentPackage.c_str();
		}

		void registerTexture(ITexture* tex);

		void removeAllTexture();

		void removeTexture(const char *namePackage);

		void removeTexture(ITexture *tex);
		
		ITexture* getTexture(const char *path);

		ITexture* getTextureFromRealPath(const char *path);

		ITexture* getCubeTexture(
			const char *pathX1,
			const char *pathX2,
			const char *pathY1,
			const char *pathY2,
			const char *pathZ1,
			const char *pathZ2);

		ITexture* getTextureArray(std::vector<std::string>& listTexture);
	};

}

#endif
