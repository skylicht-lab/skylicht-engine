/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	/**
	 * @brief Reader for XML spreadsheet and CSV table data.
	 * @ingroup Utilities
	 *
	 * The class owns all sheet, row, and cell objects it creates. Pointers returned by
	 * lookup functions remain valid until `clear` or the spreadsheet destructor.
	 */
	class SKYLICHT_API CXMLSpreadsheet
	{
	public:
		/**
		 * @brief One spreadsheet cell.
		 */
		struct SCell
		{
			/** @brief One-based row index from the source document. */
			u32 Row;
			/** @brief One-based column index from the source document. */
			u32 Col;
			/** @brief Serialized property type of this cell. */
			EPropertyDataType Type;
			/** @brief Narrow string value. */
			std::string Value;
			/** @brief Unicode string value. */
			std::wstring UnicodeValue;
			/** @brief Integer numeric value. */
			int NumberInt;
			/** @brief Floating-point numeric value. */
			float NumberFloat;
			/** @brief Time value represented as seconds. */
			long Time;
			/** @brief Time year component. */
			int TimeYear;
			/** @brief Time month component. */
			int TimeMonth;
			/** @brief Time day component. */
			int TimeDay;
			/** @brief Time hour component. */
			int TimeHour;
			/** @brief Time minute component. */
			int TimeMin;
			/** @brief Time second component. */
			int TimeSec;

			/**
			 * @brief Construct an empty string cell.
			 */
			SCell()
			{
				Row = 0;
				Col = 0;
				Type = String;
				NumberInt = 0;
				NumberFloat = 0.0f;
				Time = 0;
				TimeYear = 0;
				TimeMonth = 0;
				TimeDay = 0;
				TimeHour = 0;
				TimeMin = 0;
				TimeSec = 0;
			}
		};

		/**
		 * @brief One spreadsheet row that owns its cells.
		 */
		struct SRow
		{
			/** @brief Row index from the source document. */
			u32 Index;
			/** @brief Cells contained in this row. */
			std::vector<SCell*> Cells;

			/**
			 * @brief Destroy all cells in the row.
			 */
			~SRow()
			{
				for (SCell* c : Cells)
				{
					delete c;
				}
				Cells.clear();
			}
		};

		/**
		 * @brief One spreadsheet sheet that owns its rows.
		 */
		struct SSheet
		{
			/** @brief Declared number of rows. */
			int NumRow;
			/** @brief Declared number of columns. */
			int NumCol;
			/** @brief Sheet name as narrow text. */
			std::string Name;
			/** @brief Sheet name as Unicode text. */
			std::wstring NameUnicode;
			/** @brief Rows contained in the sheet. */
			std::vector<SRow*> Rows;

			/**
			 * @brief Destroy all rows in the sheet.
			 */
			~SSheet()
			{
				clear();
			}

			/**
			 * @brief Remove and delete all rows.
			 */
			void clear()
			{
				for (SRow* r : Rows)
				{
					delete r;
				}
				Rows.clear();
			}
		};

	protected:
		std::vector<SSheet*> m_sheets;

	public:
		/**
		 * @brief Construct an empty spreadsheet.
		 */
		CXMLSpreadsheet();

		/**
		 * @brief Destroy the spreadsheet and all loaded data.
		 */
		virtual ~CXMLSpreadsheet();

		/**
		 * @brief Load spreadsheet data from an XML file.
		 * @param file Path to the XML spreadsheet file.
		 * @return True if loading succeeds.
		 */
		bool open(const char* file);

		/**
		 * @brief Load spreadsheet data from an existing XML reader.
		 * @param xmlReader XML reader positioned at the spreadsheet source.
		 * @return True if loading succeeds.
		 */
		bool open(io::IXMLReader* xmlReader);

		/**
		 * @brief Load spreadsheet data from a CSV file.
		 * @param file Path to the CSV file.
		 * @return True if loading succeeds.
		 */
		bool openCSV(const char* file);

		/**
		 * @brief Get the number of loaded sheets.
		 * @return Sheet count.
		 */
		inline u32 getSheetCount()
		{
			return (u32)m_sheets.size();
		}

		/**
		 * @brief Get a sheet by index.
		 * @param i Zero-based sheet index.
		 * @return Sheet pointer owned by the spreadsheet.
		 */
		SSheet* getSheet(int i)
		{
			return m_sheets[i];
		}

		/**
		 * @brief Get a sheet by index.
		 * @param i Zero-based sheet index.
		 * @return Sheet pointer owned by the spreadsheet.
		 */
		SSheet* operator[](int i)
		{
			return m_sheets[i];
		}

		/**
		 * @brief Find a cell by row and column.
		 * @param sheet Sheet to search.
		 * @param row Row index.
		 * @param col Column index.
		 * @return Cell pointer, or null when not found.
		 */
		SCell* getCell(SSheet* sheet, u32 row, u32 col);

		/**
		 * @brief Find a cell by spreadsheet cell name.
		 * @param sheet Sheet to search.
		 * @param cellName Cell name such as `A1`.
		 * @return Cell pointer, or null when not found.
		 */
		SCell* getCell(SSheet* sheet, const char* cellName);

		/**
		 * @brief Get all cells inside a rectangular range.
		 * @param sheet Sheet to search.
		 * @param fromRow First row index.
		 * @param fromCol First column index.
		 * @param toRow Last row index.
		 * @param toCol Last column index.
		 * @return List of matching cell pointers.
		 */
		std::list<SCell*> getRange(SSheet* sheet, u32 fromRow, u32 fromCol, u32 toRow, u32 toCol);

		/**
		 * @brief Get all cells inside a named rectangular range.
		 * @param sheet Sheet to search.
		 * @param from First cell name, such as `A1`.
		 * @param to Last cell name, such as `C10`.
		 * @return List of matching cell pointers.
		 */
		std::list<SCell*> getRange(SSheet* sheet, const char* from, const char* to);

		/**
		 * @brief Convert a spreadsheet cell name into row and column indexes.
		 * @param cellName Cell name such as `A1`.
		 * @param row Receives row index.
		 * @param col Receives column index.
		 * @return True if the name was converted.
		 */
		bool convertCellName(const char* cellName, u32& row, u32& col);

		/**
		 * @brief Clear all loaded sheets and data.
		 */
		void clear();

	protected:

		/**
		 * @brief Split one CSV line into cell strings.
		 * @param line Source CSV line.
		 * @param delimiter CSV delimiter character.
		 * @param cells Receives parsed cells.
		 * @return True if the line was split successfully.
		 */
		bool splitCsvLine(const std::string& line, char delimiter, std::vector<std::string>& cells);

		/**
		 * @brief Combine parsed columns with continuation cells for multi-line CSV fields.
		 * @param cols Current accumulated columns.
		 * @param cells Newly parsed cells.
		 * @return True when a complete row is available.
		 */
		bool combineLine(std::vector<std::string>& cols, std::vector<std::string>& cells);
	};
}
