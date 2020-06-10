#include "pch.h"
#include "Base.hh"
#include "TestMemoryStream.h"

#include "Utils/CMemoryStream.h"

using namespace Skylicht;

void testMemoryStream()
{
	// 1mb buffer
	CMemoryStream writeStream(1024 * 1024);

	int testWrite[] = { 1, 2, 3 };
	int testRead[3];

	TEST_CASE("CMemoryStream");
	writeStream.writeChar(1);
	writeStream.writeShort(2);
	writeStream.writeInt(3);
	writeStream.writeFloat(4.0f);
	writeStream.writeDouble(5.0);
	writeStream.writeLong(6);
	writeStream.writeString(std::string("CMemoryStream"));
	writeStream.writeData(testWrite, sizeof(testWrite));

	int size = writeStream.getSize();
	TEST_ASSERT_THROW(size == 57);

	CMemoryStream readStream(writeStream.getData(), writeStream.getSize());
	TEST_ASSERT_THROW(readStream.readChar() == 1);
	TEST_ASSERT_THROW(readStream.readShort() == 2);
	TEST_ASSERT_THROW(readStream.readInt() == 3);
	TEST_ASSERT_FLOAT_EQUAL(readStream.readFloat(), 4.0f);
	TEST_ASSERT_FLOAT_EQUAL(readStream.readDouble(), 5.0);
	TEST_ASSERT_THROW(readStream.readLong() == 6);
	TEST_ASSERT_STRING_EQUAL(readStream.readString().c_str(), "CMemoryStream");
	readStream.readData(testRead, sizeof(int) * 3);
	TEST_ASSERT_THROW(memcmp(testWrite, testRead, sizeof(int) * 3) == 0);
}