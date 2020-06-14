#include "pch.h"
#include "Base.hh"
#include "TestMemoryStream.h"

#include "Utils/CMemoryStream.h"

using namespace Skylicht;

void testMemoryStream()
{
	// test auto grow from 1 byte buffer
	CMemoryStream writeStream(1);

	int testWrite[] = { 1, 2, 3 };
	int testRead[3];

	core::vector3df writeVector(1.0f, 2.0f, 3.0f);
	core::vector3df readVector;

	TEST_CASE("CMemoryStream");
	writeStream.writeChar(1);
	writeStream.writeShort(2);
	writeStream.writeInt(3);
	writeStream.writeFloat(4.0f);
	writeStream.writeDouble(5.0);
	writeStream.writeString(std::string("CMemoryStream"));
	writeStream.writeData(testWrite, sizeof(testWrite));
	writeStream.writeFloatArray(&writeVector.X, 3);

	int size = writeStream.getSize();
	TEST_ASSERT_THROW(size == 61);

	CMemoryStream readStream(writeStream.getData(), writeStream.getSize());
	TEST_ASSERT_THROW(readStream.readChar() == 1);
	TEST_ASSERT_THROW(readStream.readShort() == 2);
	TEST_ASSERT_THROW(readStream.readInt() == 3);
	TEST_ASSERT_FLOAT_EQUAL(readStream.readFloat(), 4.0f);
	TEST_ASSERT_FLOAT_EQUAL(readStream.readDouble(), 5.0);
	TEST_ASSERT_STRING_EQUAL(readStream.readString().c_str(), "CMemoryStream");

	readStream.readData(testRead, sizeof(int) * 3);
	TEST_ASSERT_THROW(memcmp(testWrite, testRead, sizeof(int) * 3) == 0);

	readStream.readFloatArray(&readVector.X, 3);
	TEST_ASSERT_FLOAT_EQUAL(readVector.X, 1.0f);
	TEST_ASSERT_FLOAT_EQUAL(readVector.Y, 2.0f);
	TEST_ASSERT_FLOAT_EQUAL(readVector.Z, 3.0f);
}