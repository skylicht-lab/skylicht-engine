// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_MEMORY_READ_FILE_H_INCLUDED__
#define __C_MEMORY_READ_FILE_H_INCLUDED__

#include "IReadFile.h"
#include "IWriteFile.h"
#include "irrString.h"

namespace irr
{

namespace io
{

	/*!
		Class for reading from memory.
	*/
	class CMemoryReadFile : public IReadFile
	{
	public:

		//! Constructor
		CMemoryReadFile(const void* memory, long len, const io::path& fileName, bool deleteMemoryWhenDropped);

		//! Destructor
		virtual ~CMemoryReadFile();

		//! returns how much was read
		virtual s32 read(void* buffer, u32 sizeToRead) _IRR_OVERRIDE_;

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false) _IRR_OVERRIDE_;

		//! returns size of file
		virtual long getSize() const _IRR_OVERRIDE_;

		//! returns where in the file we are.
		virtual long getPos() const _IRR_OVERRIDE_;

		//! returns name of file
		virtual const io::path& getFileName() const _IRR_OVERRIDE_;

	private:

		const void *Buffer;
		long Len;
		long Pos;
		io::path Filename;
		bool deleteMemoryWhenDropped;
	};

	/*!
		Class for writing to memory.
	*/
	class CMemoryWriteFile : public IWriteFile
	{
	public:

		//! Constructor
		CMemoryWriteFile(void* memory, long len, const io::path& fileName, bool deleteMemoryWhenDropped);

		//! Destructor
		virtual ~CMemoryWriteFile();

		//! returns how much was written
		virtual s32 write(const void* buffer, u32 sizeToWrite) _IRR_OVERRIDE_;

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false) _IRR_OVERRIDE_;

		//! returns where in the file we are.
		virtual long getPos() const _IRR_OVERRIDE_;

		//! returns name of file
		virtual const io::path& getFileName() const _IRR_OVERRIDE_;

	private:

		void *Buffer;
		long Len;
		long Pos;
		io::path Filename;
		bool deleteMemoryWhenDropped;
	};

} // end namespace io
} // end namespace irr

#endif

