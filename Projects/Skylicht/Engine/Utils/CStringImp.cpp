/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CStringImp.h"

namespace Skylicht
{
	std::string g_tempString;
	std::wstring g_tempWString;

	int CStringImp::splitString(const char* stringSplit, const char* search, std::vector<std::string>& result)
	{
		result.clear();

		char stringResult[512];
		int i = 0;

		while (CStringImp::split<char>(stringResult, (char*)stringSplit, (char*)search, &i) == true)
		{
			trim<char>(stringResult);
			result.push_back(stringResult);
		}

		if (result.size() == 0)
			result.push_back(stringSplit);

		return (int)result.size();
	}

	std::string CStringImp::replaceAll(std::string& string, const std::string& from, const std::string& to)
	{
		size_t start_pos = 0;
		while ((start_pos = string.find(from, start_pos)) != std::string::npos)
		{
			string.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return string;
	}

	int CStringImp::splitString(const wchar_t* stringSplit, const wchar_t* search, std::vector<std::wstring>& result)
	{
		result.clear();

		wchar_t stringResult[512];
		int i = 0;

		while (CStringImp::split<wchar_t>(stringResult, (wchar_t*)stringSplit, (wchar_t*)search, &i) == true)
		{
			trim<wchar_t>(stringResult);
			result.push_back(stringResult);
		}

		if (result.size() == 0)
			result.push_back(stringSplit);

		return (int)result.size();
	}

	bool CStringImp::format(char* lpString, const char* lpStringFormat, ...)
	{
		va_list listArgs;
		va_start(listArgs, lpStringFormat);

		int i = vsprintf(lpString, lpStringFormat, listArgs);
		va_end(listArgs);

		if (i == -1)
			return false;

		return true;
	}

	wchar_t CStringImp::utf8Char2Unicode(const char*& str)
	{
		const unsigned char* p = (const unsigned char*)str;
		unsigned char c = *p++;

		wchar_t result;
		int extraBytes = 0;

		if (c <= 0x7F)
		{
			result = c;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			result = c & 0x1F;
			extraBytes = 1;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			result = c & 0x0F;
			extraBytes = 2;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			result = c & 0x07;
			extraBytes = 3;
		}
		else
		{
			str++;
			return L'?';
		}

		for (int i = 0; i < extraBytes; ++i)
		{
			unsigned char nextByte = *p;
			if ((nextByte & 0xC0) != 0x80)
				break;

			result = (result << 6) | (nextByte & 0x3F);
			p++;
		}

		str = (const char*)p;
		return result;
	}

	unsigned short* CStringImp::getUnicodeString(const wchar_t* src)
	{
		static unsigned short s_buffer[2048];

		const wchar_t* t = src;
		unsigned short* d = s_buffer;

		while (*t)
		{
			*d = (unsigned short)*t;
			d++;
			t++;
		}

		*d = 0;
		return s_buffer;
	}

	void CStringImp::convertUTF8ToUnicode(const char* src, wchar_t* dst)
	{
		int t = 0;

		const char* pos = src;

		while (*pos != 0)
			dst[t++] = utf8Char2Unicode(pos);

		dst[t++] = 0;
	}

	int CStringImp::getUnicodeStringSize(const char* src)
	{
		int t = 0;

		const char* pos = src;

		while (*pos != 0)
		{
			utf8Char2Unicode(pos);
			t++;
		}

		return ++t;
	}

	void CStringImp::convertUnicodeToUTF8(const wchar_t* src, char* dst)
	{
		int k = 0;
		int l = 0;

		while (src[k])
		{
			unsigned int c = (unsigned int)src[k];

			if (c <= 0x007F)
			{
				dst[l++] = (char)c;
			}
			else if (c <= 0x07FF)
			{
				dst[l++] = 0xC0 | ((c >> 6) & 0x1F);
				dst[l++] = 0x80 | (c & 0x3F);
			}
			else if (c <= 0xFFFF)
			{
				dst[l++] = 0xE0 | ((c >> 12) & 0x0F);
				dst[l++] = 0x80 | ((c >> 6) & 0x3F);
				dst[l++] = 0x80 | (c & 0x3F);
			}
			else if (c <= 0x10FFFF)
			{
				dst[l++] = 0xF0 | ((c >> 18) & 0x07);
				dst[l++] = 0x80 | ((c >> 12) & 0x3F);
				dst[l++] = 0x80 | ((c >> 6) & 0x3F);
				dst[l++] = 0x80 | (c & 0x3F);
			}

			k++;
		}
		dst[l] = 0;
	}

	int CStringImp::getUTF8StringSize(const wchar_t* src)
	{
		int k = 0;
		int l = 0;

		while (src[k])
		{
			wchar_t c = src[k];

			if (c <= 0x007F)
			{
				l++;
			}
			else if (c <= 0x07FF)
			{
				l += 2;
			}
			else
			{
				l += 3;
			}

			k++;
		}

		return l;
	}

	void CStringImp::replaceExt(char* lpPath, const char* lpExt)
	{
		char fileName[512] = { 0 };

		CStringImp::getFileNameNoExt(fileName, lpPath);
		CStringImp::copy(lpPath, fileName);
		CStringImp::cat(lpPath, lpExt);
	}

	void CStringImp::replacePathExt(char* lpPath, const char* lpExt)
	{
		char fileName[512] = { 0 };
		char folder[512] = { 0 };

		CStringImp::getFolderPath(folder, lpPath);
		CStringImp::getFileNameNoExt(fileName, lpPath);
		CStringImp::copy(lpPath, folder);

		if (CStringImp::length(folder) > 0)
			CStringImp::cat(lpPath, "/");

		CStringImp::cat(lpPath, fileName);
		CStringImp::cat(lpPath, lpExt);
	}

	int CStringImp::findStringInList(std::vector<std::string>& listString, const char* find)
	{
		for (int i = 0, n = (int)listString.size(); i < n; i++)
		{
			if (listString[i] == find)
			{
				return i;
			}
		}
		return -1;
	}

	void CStringImp::replaceString(std::string& subject, const std::string& search, const std::string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos)
		{
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}

	const std::string& CStringImp::convertUnicodeToUTF8(const wchar_t* src)
	{
		int size = getUTF8StringSize(src) + 1;
		char* data = new char[size];
		memset(data, 0, size);
		convertUnicodeToUTF8(src, data);
		g_tempString = data;
		delete[]data;
		return g_tempString;
	}

	const std::wstring& CStringImp::convertUTF8ToUnicode(const char* src)
	{
		int size = getUnicodeStringSize(src) + 1;
		wchar_t* data = new wchar_t[size];
		memset(data, 0, sizeof(wchar_t) * size);
		convertUTF8ToUnicode(src, data);
		g_tempWString = data;
		delete[]data;
		return g_tempWString;
	}

	const std::string& CStringImp::toLower(const std::string& s)
	{
		std::string ret;
		for (const char& c : s)
			ret += tolower(c);
		g_tempString = ret;
		return g_tempString;
	}

	const std::wstring& CStringImp::toLower(const std::wstring& s)
	{
		std::wstring ret;
		for (const wchar_t& c : s)
			ret += towlower(c);
		g_tempWString = ret;
		return g_tempWString;
	}

	const std::string& CStringImp::toUpper(const std::string& s)
	{
		std::string ret;
		for (const char& c : s)
			ret += toupper(c);
		g_tempString = ret;
		return g_tempString;
	}

	const std::wstring& CStringImp::toUpper(const std::wstring& s)
	{
		std::wstring ret;
		for (const wchar_t& c : s)
			ret += towupper(c);
		g_tempWString = ret;
		return g_tempWString;
	}

	const std::string& CStringImp::formatThousand(int n, bool useK, bool useM)
	{
		std::string ret;
		std::string dot;

		bool addK = false;
		bool addM = false;
		bool negative = n < 0;
		long long value = negative ? -(long long)n : (long long)n;

		if (useM && value > 1000000)
		{
			int number = (int)(value / 1000000.0);
			char text[32];
			sprintf(text, "%d", number);
			ret = text;

			double d = value / 1000000.0 - (double)number;
			if (d > 0.0f)
			{
				sprintf(text, ",%02d", (int)(d * 100.0));
				dot = text;
			}

			addM = true;
		}
		else if (useK && value > 100000)
		{
			int number = (int)(value / 1000.0);
			char text[32];
			sprintf(text, "%d", number);
			ret = text;

			double d = value / 1000.0 - (double)number;
			if (d > 0.0f)
			{
				sprintf(text, ",%02d", (int)(d * 10.0));
				dot = text;
			}

			addK = true;
		}
		else
		{
			ret = std::to_string(n);
		}

		if (negative && (addM || addK))
			ret.insert(0, "-");

		int n_len = (int)ret.length();
		int insert_count = 0;

		if (n_len > 3)
		{
			for (int i = n_len - 3; i > 0; i -= 3)
				ret.insert(i, ".");
		}

		ret += dot;

		if (addM)
			ret += "M";
		else if (addK)
			ret += "K";

		g_tempString = ret;
		return g_tempString;
	}
}
