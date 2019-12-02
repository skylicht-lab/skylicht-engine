#pragma once

#include "Base.hh"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

void testStringImp()
{
	char stringTest[512] = { 0 };
	wchar_t wstringTest[512] = { 0 };

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
}

void testCoreUtils()
{
	testStringImp();
}