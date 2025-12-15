#pragma once

#include "Utils/CSingleton.h"
#include "Utils/XMLSpreadsheet/CXMLSpreadsheet.h"

namespace Skylicht
{
	namespace Editor
	{
		class CLocalize
		{
		protected:
			u32 m_language;

			std::vector<std::string> m_langs;
			std::map<std::string, CXMLSpreadsheet::SRow*> m_strToRow;

			CXMLSpreadsheet m_dataSheet;

		public:
			DECLARE_SINGLETON(CLocalize)

			CLocalize();

			virtual ~CLocalize();

			void init(const char* src);

			const char* getString(const char* id);

			inline void setLanguage(u32 lang)
			{
				m_language = lang;
			}

			inline u32 getLanguage()
			{
				return m_language;
			}

			const std::vector<std::string>& getLanguages()
			{
				return m_langs;
			}
		};
	}
}