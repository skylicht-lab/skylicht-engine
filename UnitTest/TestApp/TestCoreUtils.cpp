#include "Base.hh"
#include "TestCoreUtils.h"

#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

using namespace Skylicht;

void testStringImp()
{
	char stringTest[512] = { 0 };
	wchar_t wstringTest[512] = { 0 };
	std::string s;

	// UNIT TEST: CPath
	TEST_CASE("CPath::getFileName");
	s = CPath::getFileName("C:\\Data\\Readme.md");
	TEST_ASSERT_THROW(s == "Readme.md");
	s = CPath::getFileName("Data/Readme.md");
	TEST_ASSERT_THROW(s == "Readme.md");
	s = CPath::getFileName(".\\Readme.md");
	TEST_ASSERT_THROW(s == "Readme.md");


	TEST_CASE("CPath::getFileNameExt");
	s = CPath::getFileNameExt("C:\\Data\\Readme.md");
	TEST_ASSERT_THROW(s == "md");
	s = CPath::getFileNameExt("Data/Readme.md");
	TEST_ASSERT_THROW(s == "md");
	s = CPath::getFileNameExt(".\\Readme.md");
	TEST_ASSERT_THROW(s == "md");


	TEST_CASE("CPath::getFileNameNoExt");
	s = CPath::getFileNameNoExt("C:\\Data\\Readme.md");
	TEST_ASSERT_THROW(s == "Readme");
	s = CPath::getFileNameNoExt("Data/Readme.md");
	TEST_ASSERT_THROW(s == "Readme");
	s = CPath::getFileNameNoExt(".\\Readme.md");
	TEST_ASSERT_THROW(s == "Readme");
	s = CPath::getFileNameNoExt("Readme.md");
	TEST_ASSERT_THROW(s == "Readme");

	TEST_CASE("CPath::replaceFileExt");
	s = CPath::replaceFileExt("C:\\Data\\Readme.md", ".txt");
	TEST_ASSERT_THROW(s == "C:\\Data/Readme.txt");
	s = CPath::replaceFileExt("Data/Readme.md", ".txt");
	TEST_ASSERT_THROW(s == "Data/Readme.txt");
	s = CPath::replaceFileExt(".\\Readme.md", ".txt");
	TEST_ASSERT_THROW(s == "./Readme.txt");


	TEST_CASE("CPath::getFolderPath");
	s = CPath::getFolderPath("C:\\Data\\Readme.md");
	TEST_ASSERT_THROW(s == "C:\\Data");
	s = CPath::getFolderPath("Data/Readme.md");
	TEST_ASSERT_THROW(s == "Data");
	s = CPath::getFolderPath(".\\Readme.md");
	TEST_ASSERT_THROW(s == ".");

	TEST_CASE("CPath::normalizePath");
	s = CPath::normalizePath("C:\\Data\\..\\Readme.md");
	TEST_ASSERT_THROW(s == "C:/Readme.md");
	s = CPath::normalizePath("C:\\Data\\..\\Bin\\Readme.md");
	TEST_ASSERT_THROW(s == "C:/Bin/Readme.md");
	s = CPath::normalizePath("C:\\Data\\..\\Bin\\..\\Readme.md");
	TEST_ASSERT_THROW(s == "C:/Readme.md");
	s = CPath::normalizePath("C:\\Data\\Bin\\..\\..\\Readme.md");
	TEST_ASSERT_THROW(s == "C:/Readme.md");

	// UNIT TEST: CStringImp
	TEST_CASE("CStringImp::convertUTF8ToUnicode");
	CStringImp::convertUTF8ToUnicode("Skylicht Technology", wstringTest);
	TEST_ASSERT_THROW(std::wstring(wstringTest) == L"Skylicht Technology");


	TEST_CASE("CStringImp::convertUnicodeToUTF8");
	CStringImp::convertUnicodeToUTF8(L"Skylicht Technology", stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, "Skylicht Technology");


	TEST_CASE("CStringImp::length");
	TEST_ASSERT_EQUAL(CStringImp::length("Hello world"), 11);


	TEST_CASE("CStringImp::find string");
	TEST_ASSERT_EQUAL(CStringImp::find("Hello Skylicht", "Skylicht", 0), 6);
	TEST_ASSERT_EQUAL(CStringImp::find("Hello Skylicht", "Skylicht", 7), -1);
	TEST_ASSERT_EQUAL(CStringImp::find("Hello Skylicht Hello", "Hello", 7), 15);


	TEST_CASE("CStringImp::find character");
	TEST_ASSERT_EQUAL(CStringImp::find<const char>("Hello Skylicht", 'S', 0), 6);
	TEST_ASSERT_EQUAL(CStringImp::find<const char>("Hello Skylicht", 'Z', 0), -1);


	TEST_CASE("CStringImp::trimleft");
	strcpy(stringTest, " \tHello Skylicht");
	CStringImp::trimleft(stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");


	TEST_CASE("CStringImp::trimRight");
	strcpy(stringTest, " \tHello Skylicht   ");
	CStringImp::trimright(stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, " \tHello Skylicht");

	TEST_CASE("CStringImp::trim");
	strcpy(stringTest, " \tHello Skylicht   ");
	CStringImp::trim(stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");


	TEST_CASE("CStringImp::trim character");
	strcpy(stringTest, "## @Hello Skylicht@ ##");
	CStringImp::trim(stringTest, '#');
	TEST_ASSERT_STRING_EQUAL(stringTest, " @Hello Skylicht@ ");


	TEST_CASE("CStringImp::countEntry");
	strcpy(stringTest, "## @Hello Skylicht@ ##");
	TEST_ASSERT_EQUAL(CStringImp::countEntry(stringTest, '#'), 4);

	TEST_CASE("CStringImp::copy");
	CStringImp::copy(stringTest, L"Hello Skylicht");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");


	TEST_CASE("CStringImp::copyAt");
	CStringImp::copyAt(stringTest, L"Hello Skylicht", 100);
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");

	strcpy(stringTest, "@@ @@");
	CStringImp::copyAt(stringTest, L"Hello Skylicht", 2);
	TEST_ASSERT_STRING_EQUAL(stringTest, "@@Hello Skylicht");


	TEST_CASE("CStringImp::cat");
	stringTest[0] = 0;
	CStringImp::cat(stringTest, L"Hello ");
	CStringImp::cat(stringTest, L"Skylicht");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");


	TEST_CASE("CStringImp::comp");
	TEST_ASSERT_EQUAL(CStringImp::comp("TestA", "TestB"), -1);
	TEST_ASSERT_EQUAL(CStringImp::comp("TestB", "TestA"), 1);
	TEST_ASSERT_EQUAL(CStringImp::comp("TestA", "TestA"), 0);
	TEST_ASSERT_EQUAL(CStringImp::comp("TestA", "TestB", 4), 0);


	TEST_CASE("CStringImp::mid");
	CStringImp::mid(stringTest, "123456789", 2, 4);
	TEST_ASSERT_STRING_EQUAL(stringTest, "34");


	TEST_CASE("CStringImp::getBlock");
	CStringImp::getBlock(stringTest, "Hello @Skylicht@, How are you", "@");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Skylicht");

	CStringImp::findBlock(stringTest, "Hello {Skylicht}, How are you", "{}");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Skylicht");
	TEST_ASSERT_EQUAL(CStringImp::findBlock(stringTest, "Hello {Skylicht}, How are you", "{}@", 0), -1);
	TEST_ASSERT_EQUAL(CStringImp::findBlock(stringTest, "Hello {Skylicht}, How are you", "", 0), -1);
	TEST_ASSERT_EQUAL(CStringImp::findBlock(stringTest, "Hello {Skylicht}, How are you", "{$", 0), -1);


	TEST_CASE("CStringImp::findBlockString");
	CStringImp::findBlockString(stringTest, "Hello <b>Skylicht</b>, How are you", "<b>", "</b>");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Skylicht");
	TEST_ASSERT_EQUAL(CStringImp::findBlockString(stringTest, "Hello <b>Skylicht</b>, How are you", "<b>", "@"), -1);
	TEST_ASSERT_EQUAL(CStringImp::findBlockString(stringTest, "Hello <b>Skylicht</b>, How are you", "<b>", "</b>", 12), -1);


	TEST_CASE("CStringImp::inorgeLoopChar");
	strcpy(stringTest, "This  is   my text   ");
	CStringImp::inorgeLoopChar(stringTest, ' ');
	TEST_ASSERT_STRING_EQUAL(stringTest, "This is my text ");

	TEST_CASE("CStringImp::split");
	char splitResult[512];
	strcpy(stringTest, "Hello,Skylicht#Engine|");
	int pos = 0;
	int count = 0;
	while (CStringImp::split(splitResult, stringTest, ",#|", &pos))
	{
		if (count == 1)
			TEST_ASSERT_STRING_EQUAL(splitResult, "Skylicht");

		count++;
	}
	TEST_ASSERT_EQUAL(count, 3);


	TEST_CASE("CStringImp::splitString");
	std::vector<std::string> splitArray;
	CStringImp::splitString(stringTest, ",#|", splitArray);
	TEST_ASSERT_STRING_EQUAL(splitArray[1].c_str(), "Skylicht");
	TEST_ASSERT_EQUAL(splitArray.size(), 3);


	TEST_CASE("CStringImp::findStringInList");
	TEST_ASSERT_EQUAL(CStringImp::findStringInList(splitArray, "Skylicht"), 1);


	TEST_CASE("CStringImp::format");
	CStringImp::format(stringTest, "%d", 1);
	TEST_ASSERT_STRING_EQUAL(stringTest, "1");

	CStringImp::format(stringTest, "Hello %s", "Skylicht");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Hello Skylicht");


	TEST_CASE("CStringImp::parseToInt");
	int number = 0;
	CStringImp::parseToInt("100", &number);
	TEST_ASSERT_EQUAL(number, 100);
	CStringImp::parseToUInt("100", &number);
	TEST_ASSERT_EQUAL(number, 100);


	TEST_CASE("CStringImp::parseToFloat");
	float floatNumber = 0;
	CStringImp::parseToFloat("0.5", &floatNumber);
	TEST_ASSERT_EQUAL(floatNumber, 0.5f);


	TEST_CASE("CStringImp::parseFromHex");
	CStringImp::parseFromHex("FF", &number);
	TEST_ASSERT_EQUAL(number, 255);


	TEST_CASE("CStringImp::toLower");
	strcpy(stringTest, "HeLlo sKYliCHt");
	CStringImp::toLower(stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, "hello skylicht");


	TEST_CASE("CStringImp::toUpper");
	strcpy(stringTest, "HeLlo sKYliCHt");
	CStringImp::toUpper(stringTest);
	TEST_ASSERT_STRING_EQUAL(stringTest, "HELLO SKYLICHT");


	TEST_CASE("CStringImp::getFolderPath");
	CStringImp::getFolderPath(stringTest, "C:\\Data\\Skylicht");
	TEST_ASSERT_STRING_EQUAL(stringTest, "C:\\Data");
	CStringImp::getFolderPath(stringTest, "Data/Skylicht");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Data");
	CStringImp::getFolderPath(stringTest, "");
	TEST_ASSERT_STRING_EQUAL(stringTest, "");


	TEST_CASE("CStringImp::getFileName");
	CStringImp::getFileName(stringTest, "C:\\Data\\Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme.md");
	CStringImp::getFileName(stringTest, "Data/Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme.md");
	CStringImp::getFileName(stringTest, ".\\Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme.md");

	TEST_CASE("CStringImp::getFileNameExt");
	CStringImp::getFileNameExt(stringTest, "C:\\Data\\Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "md");
	CStringImp::getFileNameExt(stringTest, "C/Data/Readme");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme");


	TEST_CASE("CStringImp::getFileNameNoExt");
	CStringImp::getFileNameNoExt(stringTest, "C:\\Data\\Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme");
	CStringImp::getFileNameNoExt(stringTest, "C:\\Data\\Readme");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme");
	CStringImp::getFileNameNoExt(stringTest, "Data/Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme");
	CStringImp::getFileNameNoExt(stringTest, ".\\Readme.md");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme");


	TEST_CASE("CStringImp::replaceExt");
	strcpy(stringTest, "C:\\Data\\Readme.md");
	CStringImp::replaceExt(stringTest, ".txt");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Readme.txt");


	TEST_CASE("CStringImp::replacePathExt");
	strcpy(stringTest, "C:\\Data\\Readme.md");
	CStringImp::replacePathExt(stringTest, ".txt");
	TEST_ASSERT_STRING_EQUAL(stringTest, "C:\\Data/Readme.txt");


	TEST_CASE("CStringImp::shortName");
	CStringImp::shortName(stringTest, "Skylicht Technology", 10);
	TEST_ASSERT_EQUAL(strlen(stringTest), 10);


	TEST_CASE("CStringImp::replaceText");
	CStringImp::replaceText(stringTest, "Skylicht Technology", " ", "__");
	TEST_ASSERT_STRING_EQUAL(stringTest, "Skylicht__Technology");
}

void testCoreUtils()
{
	testStringImp();
}