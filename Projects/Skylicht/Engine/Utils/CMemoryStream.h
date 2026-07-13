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

namespace Skylicht
{
	/**
	 * @brief In-memory byte stream for binary serialization and deserialization.
	 * @ingroup Utilities
	 *
	 * A stream can own its backing buffer or wrap an external memory block. Streams
	 * created from external memory cannot grow automatically.
	 */
	class SKYLICHT_API CMemoryStream
	{
	protected:
		unsigned char* m_memory;
		unsigned int m_size;
		unsigned int m_totalSize;
		unsigned int m_pos;
		bool m_fromMemory;
	public:
		/**
		 * @brief Create an owning memory stream.
		 * @param initMem Initial buffer capacity in bytes.
		 */
		CMemoryStream(unsigned int initMem = 512);

		/**
		 * @brief Wrap an existing memory block for reading or fixed-size writing.
		 * @param fromMem External memory block. The stream does not delete this pointer.
		 * @param size Size of the external memory block in bytes.
		 */
		CMemoryStream(unsigned char* fromMem, unsigned int size);

		/**
		 * @brief Copy a stream into a new backing buffer.
		 *
		 * The copy starts with read position 0 and contains the used bytes from the source.
		 * Ownership behavior follows the implementation in the source file.
		 *
		 * @param stream Source stream.
		 */
		CMemoryStream(const CMemoryStream& stream);

		/**
		 * @brief Destroy the stream and release owned memory.
		 */
		virtual ~CMemoryStream();

		/**
		 * @brief Ensure the stream has enough capacity for an upcoming write.
		 * @param writeSize Number of bytes that will be appended.
		 * @return True if the stream has capacity, false for fixed external memory.
		 */
		bool autoGrow(unsigned int writeSize);

		/**
		 * @brief Append raw bytes to the stream.
		 * @param data Source data.
		 * @param size Number of bytes to write.
		 */
		void writeData(const void* data, unsigned int size);
		/**
		 * @brief Append the full contents of another memory stream.
		 * @param stream Source stream.
		 */
		void writeStream(CMemoryStream* stream);

		/** @brief Append a signed 8-bit character. */
		void writeChar(char data);
		/** @brief Append a signed 16-bit integer. */
		void writeShort(short data);
		/** @brief Append an unsigned 16-bit integer. */
		void writeUShort(unsigned short data);
		/** @brief Append a signed 32-bit integer. */
		void writeInt(int data);
		/** @brief Append an unsigned 32-bit integer. */
		void writeUInt(unsigned int data);
		/** @brief Append a 32-bit floating-point value. */
		void writeFloat(float data);
		/** @brief Append a 64-bit floating-point value. */
		void writeDouble(double data);
		/** @brief Append a UTF-8/narrow string with its character count prefix. */
		void writeString(const std::string& s);
		/** @brief Append a wide string with its character count prefix. */
		void writeString(const std::wstring& s);
		/**
		 * @brief Append an array of floats.
		 * @param f Source float array.
		 * @param count Number of float values.
		 */
		void writeFloatArray(const float* f, int count);

		/**
		 * @brief Read raw bytes from the current read position.
		 * @param data Destination buffer.
		 * @param size Requested number of bytes.
		 * @return Number of bytes actually read.
		 */
		unsigned int readData(void* data, unsigned int size);

		/** @brief Read a signed 8-bit character and advance the read position. */
		char readChar();
		/** @brief Read a signed 16-bit integer and advance the read position. */
		short readShort();
		/** @brief Read an unsigned 16-bit integer and advance the read position. */
		unsigned short readUShort();
		/** @brief Read a signed 32-bit integer and advance the read position. */
		int readInt();
		/** @brief Read an unsigned 32-bit integer and advance the read position. */
		unsigned int readUInt();
		/** @brief Read a 32-bit floating-point value and advance the read position. */
		float readFloat();
		/** @brief Read a 64-bit floating-point value and advance the read position. */
		double readDouble();
		/** @brief Read a narrow string written by `writeString(const std::string&)`. */
		std::string readString();
		/** @brief Read a wide string written by `writeString(const std::wstring&)`. */
		std::wstring readWString();
		/**
		 * @brief Read an array of floats and advance the read position.
		 * @param f Destination float array.
		 * @param count Number of float values to read.
		 */
		void readFloatArray(float* f, int count);

		/**
		 * @brief Get the backing memory pointer.
		 * @return Pointer to the stream buffer.
		 */
		unsigned char* getData()
		{
			return m_memory;
		}

		/**
		 * @brief Get the number of bytes written or available for reading.
		 * @return Used stream size in bytes.
		 */
		unsigned int getSize()
		{
			return m_size;
		}

		/**
		 * @brief Get the current read position.
		 * @return Current position in bytes.
		 */
		unsigned int getPos()
		{
			return m_pos;
		}

		/**
		 * @brief Get the total allocated capacity.
		 * @return Buffer capacity in bytes.
		 */
		unsigned int getTotalSize()
		{
			return m_totalSize;
		}

		/**
		 * @brief Set the current read position.
		 * @param pos New byte position.
		 */
		void setPos(unsigned int pos)
		{
			m_pos = pos;
		}

		/**
		 * @brief Clear written data and reset the read position.
		 */
		void resetWrite()
		{
			m_size = 0;
			m_pos = 0;
		}

		/**
		 * @brief Encrypt the stream contents.
		 *
		 * Currently implemented as a no-op.
		 */
		void encrypt();
		/**
		 * @brief Decrypt the stream contents.
		 *
		 * Currently implemented as a no-op.
		 */
		void decrypt();
	};

}
