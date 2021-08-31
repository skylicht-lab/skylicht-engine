#include "Base.hh"
#include "AppInclude.h"
#include "Utils/XMLSpreadsheet/CXMLSpreadsheet.h"
#include "Utils/XMLSpreadsheet/CXMLTableData.h"

using namespace irr;
using namespace Skylicht;

class RowData : public CObjectSerializable
{
public:
	CIntProperty ID;
	CStringProperty Name;

public:
	RowData() :
		CObjectSerializable("RowData"),
		ID(this, "ID"),
		Name(this, "Name")
	{

	}

	~RowData()
	{

	}
};

void testSpreadsheet()
{
	CXMLSpreadsheet excel;

	TEST_CASE("Open XMLSpreadsheet");
	if (excel.open("BuiltIn/Spreadsheet/Test.xml") == true)
	{
		// test parse sheet count
		TEST_ASSERT_THROW(excel.getSheetCount() == 2);

		// test get cell
		CXMLSpreadsheet::SCell* a1 = excel.getCell(excel[0], "A1");
		TEST_ASSERT_THROW(a1->Value == "A1");

		CXMLSpreadsheet::SCell* b2 = excel.getCell(excel[0], "B2");
		TEST_ASSERT_THROW(b2->Value == "B2");

		CXMLSpreadsheet::SCell* c3 = excel.getCell(excel[0], "C3");
		TEST_ASSERT_THROW(c3->NumberInt == 3);

		CXMLSpreadsheet::SCell* a5 = excel.getCell(excel[0], "A5");
		TEST_ASSERT_THROW(a5->TimeDay == 4);
		TEST_ASSERT_THROW(a5->TimeMonth == 12);
		TEST_ASSERT_THROW(a5->TimeYear == 2020);

		CXMLSpreadsheet::SCell* b5 = excel.getCell(excel[0], "B5");
		TEST_ASSERT_THROW(b5->TimeDay == 4);
		TEST_ASSERT_THROW(b5->TimeMonth == 12);
		TEST_ASSERT_THROW(b5->TimeYear == 2020);
		TEST_ASSERT_THROW(b5->TimeHour == 14);
		TEST_ASSERT_THROW(b5->TimeMin == 5);

		b2 = excel.getCell(excel[1], "B2");
		TEST_ASSERT_THROW(b2->Value == "A");

		// test get range
		std::list<CXMLSpreadsheet::SCell*> list = excel.getRange(excel[0], "A1", "E1");
		TEST_ASSERT_THROW(list.size() == 4);

		list = excel.getRange(excel[0], "A1", "E8");
		TEST_ASSERT_THROW(list.size() > 0);

		list = excel.getRange(excel[0], "H1", "J1");
		TEST_ASSERT_THROW(list.size() == 0);


		// test fetch table data
		CXMLTableData table(excel.getSheet(1));
		table.addColumn("ID");
		table.addColumn("Name");

		std::vector<RowData*> data;

		// fetch data from row 1
		table.fetchData(data, 1, -1);

		// test data fetch
		TEST_ASSERT_THROW(data.size() == 4);
		for (int i = 0; i < 4; i++)
		{
			TEST_ASSERT_THROW(data[i]->ID.get() == i);
		}

		table.freeData(data);
	}
}