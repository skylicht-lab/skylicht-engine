#include "pch.h"
#include "CMD5.h"
#include "Crypto/md5.h"

namespace Skylicht
{
	namespace Network
	{
		std::string CMD5::calc(const char* file)
		{
			std::string result;
			
			io::IReadFile* fileObject = getIrrlichtDevice()->getFileSystem()->createAndOpenFile(file);
			if (fileObject == NULL)
				return result;
			
			// todo read file stream
			long filesize = fileObject->getSize();
			if (filesize == 0)
				return result;
			
			// cache 2MB
			int cacheSize = 2 * 1024 * 1024;
			int sizeRemain = filesize;
			
			unsigned char* fileData = new unsigned char[cacheSize];
			
			// calc md5 checksum
			MD5_CTX	md5Contex;
			md5_init(&md5Contex);
			
			while (sizeRemain > 0)
			{
				int sizeRead = cacheSize;
				if (sizeRead > sizeRemain)
					sizeRead = sizeRemain;
				
				fileObject->read(fileData, sizeRead);
				
				md5_update(&md5Contex, fileData, (unsigned long)sizeRead);
				
				sizeRemain -= sizeRead;
			}
			
			unsigned char ret[16];
			memset(ret, 0, 16);
			md5_final(&md5Contex, ret);
			
			char md5string[33];
			for (int i = 0; i < 16; ++i)
				sprintf(&md5string[i * 2], "%02x", (unsigned int)ret[i]);
			
			result = md5string;
			
			delete fileData;
			fileObject->drop();
			
			return result;
		}
		
		std::string CMD5::calc(unsigned char* buffer, int size)
		{
			unsigned char ret[16];
			memset(ret, 0, 16);
			
			// calc md5 checksum
			MD5_CTX	md5Contex;
			md5_init(&md5Contex);
			md5_update(&md5Contex, buffer, (unsigned long)size);
			md5_final(&md5Contex, ret);
			
			char md5string[33];
			for (int i = 0; i < 16; ++i)
				sprintf(&md5string[i * 2], "%02x", (unsigned int)ret[i]);
			
			return std::string(md5string);
		}
	}
}
