#include "pch.h"
#include "SkylichtEngine.h"
#include "CLocalize.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CLocalize);

		CLocalize::CLocalize() :
			m_language(0)
		{

		}

		CLocalize::~CLocalize()
		{

		}

		void CLocalize::init(const char* src)
		{
			std::string ext = CPath::getFileNameExt(src);
			if (ext == "csv")
				m_dataSheet.openCSV(src);
			else
				m_dataSheet.open(src);

			if (m_dataSheet.getSheetCount() == 0)
				return;

			CXMLSpreadsheet::SSheet* sheet = m_dataSheet.getSheet(0);
			m_langs.clear();
			m_strToRow.clear();
			m_language = 0;

			if (sheet->Rows.size() > 0)
			{
				CXMLSpreadsheet::SRow* header = sheet->Rows.front();
				for (CXMLSpreadsheet::SCell* c : header->Cells)
					m_langs.push_back(c->Value);

				for (CXMLSpreadsheet::SRow* r : sheet->Rows)
					m_strToRow[r->Cells[0]->Value] = r;
			}
		}

		const char* CLocalize::getString(const char* id)
		{
			auto it = m_strToRow.find(id);
			if (it == m_strToRow.end())
				return NULL;
			return it->second->Cells[m_language]->Value.c_str();
		}
	}
}