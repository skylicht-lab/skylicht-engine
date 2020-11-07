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

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cwchar>
#include <cstdarg>
#include <vector>

namespace Skylicht
{
	class CStringImp
	{
	public:
		template <class T>
		static int length(T* pString)
		{
			int result = 0;
			while (pString[result] != 0)
				result++;

			return result;
		}

		template <class T>
		static int find(T* pString, T pCharSearch, int begin = 0)
		{
			int index = begin;

			while (pString[index] != 0)
			{
				if (pString[index] == pCharSearch)
					return index;

				index++;
			}

			return -1;
		}

		template <class T>
		static int find(T* pString, T* pKeySearch, int begin = 0)
		{
			int size = CStringImp::length<T>(pKeySearch) * sizeof(T);
			int index = begin;

			while (pString[index] != 0)
			{
				if (memcmp(&pString[index], pKeySearch, size) == 0)
					return index;

				index++;
			}

			return -1;
		}

		template <class T>
		static void trimleft(T* pString)
		{
			int len = CStringImp::length<T>(pString);
			if (len == 0)
				return;

			while (pString[0] != 0 && (pString[0] == ' ' || pString[0] == '\r' || pString[0] == '\n' || pString[0] == '\t'))
			{
				len--;
				if (len == 0)
					return;

				memmove((void*)&pString[0], (void*)&pString[1], len * sizeof(T));
				pString[len] = 0;
			}
		}

		template <class T>
		static void trimright(T* pString)
		{
			int len = CStringImp::length<T>(pString);
			len--;

			for (; len >= 0; len--)
			{
				if (pString[len] == ' ' || pString[len] == '\r' || pString[len] == '\n' || pString[len] == '\t')
					pString[len] = 0;
				else
					return;
			}
		}

		template <class T>
		static void trimleft(T* pString, T delChar)
		{
			int len = CStringImp::length<T>(pString);
			if (len == 0)
				return;

			while (pString[0] != 0 && pString[0] == delChar)
			{
				len--;
				if (len == 0)
					return;

				memmove((void*)&pString[0], (void*)&pString[1], len * sizeof(T));
				pString[len] = 0;
			}
		}

		template <class T>
		static void trimright(T* pString, T delChar)
		{
			int len = CStringImp::length<T>(pString);
			len--;

			for (; len >= 0; len--)
			{
				if (pString[len] == delChar)
					pString[len] = 0;
				else
					return;
			}
		}

		template <class T>
		static void trim(T* pString)
		{
			CStringImp::trimleft<T>(pString);
			CStringImp::trimright<T>(pString);
		}

		template <class T>
		static void trim(T* pString, T delChar)
		{
			CStringImp::trimleft<T>(pString, delChar);
			CStringImp::trimright<T>(pString, delChar);
		}

		template <class T>
		static int countEntry(T* pString, T entry, int begin = 0)
		{
			int index = begin;
			int result = 0;

			while (pString[index] != 0)
			{
				if (pString[index] == entry)
					result++;

				index++;
			}

			return result;
		}

		template <class T1, class T2>
		static int copy(T1* pStringDst, T2* pStringSrc)
		{
			if (pStringSrc == 0)
			{
				pStringDst[0] = 0;
				return 0;
			}

			int index = 0;

			while (pStringSrc[index] != 0)
			{
				pStringDst[index] = (T1)(pStringSrc[index]);
				index++;
			}
			pStringDst[index] = 0;
			return index;
		}

		template <class T1, class T2>
		static int copyAt(T1* pStringDst, T2* pStringSrc, int at)
		{
			int index = at;
			int i = 0;

			if (pStringSrc == 0)
				return 0;

			if (length(pStringDst) < at || at < 0)
				return 0;

			while (pStringSrc[i] != 0)
			{
				pStringDst[index] = (T1)(pStringSrc[i]);
				index++; i++;
			}
			pStringDst[index] = 0;
			return index;
		}

		template <class T1, class T2>
		static int cat(T1* pStringDst, T2* pStringSrc)
		{
			int index = CStringImp::length<T1>(pStringDst);
			int i = 0;

			if (pStringSrc == 0)
				return index;

			while (pStringSrc[i] != 0)
			{
				pStringDst[index] = (T1)(pStringSrc[i]);
				index++; i++;
			}
			pStringDst[index] = 0;
			return i;
		}

		template <class T>
		static int comp(T* pString1, T* pString2)
		{
			int len1 = (int)(CStringImp::length<T>(pString1) * sizeof(T));
			int len2 = (int)(CStringImp::length<T>(pString2) * sizeof(T));
			int len = len1 < len2 ? len1 : len2;

			int r = memcmp(pString1, pString2, len);

			if (r == 0)
			{
				if (len1 == len2)
					return 0;
				if (len1 > len2)
					return 1;
				if (len1 < len2)
					return -1;
			}

			return r;
		}

		template <class T>
		static int comp(T* pString1, T* pString2, int len)
		{
			return memcmp(pString1, pString2, len * sizeof(T));
		}

		template <class T1, class T2>
		static int mid(T1* pStringDst, T2* pStringSrc, int begin, int end)
		{
			int index, i = 0;
			for (index = begin; index < end; index++)
			{
				pStringDst[i] = (T1)pStringSrc[index];
				i++;
			}

			pStringDst[i] = 0;

			return i;
		}

		// getBlock
		// Get string inside pBlock "@ ...... @"
		template <class T1, class T2>
		static int getBlock(T1* pStringDst, T2* pStringSrc, T2* pBlock, int begin = 0)
		{
			int dwBegin = CStringImp::find<T2>(pStringSrc, *pBlock, begin);
			if (dwBegin == -1)
				return -1;

			dwBegin++;
			int dwEnd = CStringImp::find<T2>(pStringSrc, *pBlock, dwBegin);
			if (dwEnd == -1)
				return -1;

			CStringImp::mid<T1, T2>(pStringDst, pStringSrc, dwBegin, dwEnd);
			return dwEnd + 1;
		}

		// findBlock
		// Get string inside pBlock "{}" or "[]" "<>" ...
		template <class T1, class T2>
		static int findBlock(T1* pStringDst, T2* pStringSrc, T2* pBlock, int begin = 0)
		{
			int i = begin;
			int block = 0;
			int midBegin = -1;

			if (length(pBlock) != 2)
				return -1;

			while (pStringSrc[i] != 0)
			{
				if (pStringSrc[i] == pBlock[0])		// '{' or '[' or '('
				{
					block++;
					if (block == 1)
						midBegin = i + 1;
					i++;
					continue;
				}
				if (pStringSrc[i] == pBlock[1])		// '}' or ']' or ')'
				{
					block--;
					if (block == 0 && midBegin != -1)
					{
						CStringImp::mid<T1, T2>(pStringDst, pStringSrc, midBegin, i);
						return i + 1;
					}

					// Error found '{' but can't find '}')
					if (block <= 0)
						return -1;
				}
				i++;
			}
			return -1;
		}

		// findBlockString
		// Get string inside pBlock "<begin>....<end>"
		template <class T1, class T2>
		static int findBlockString(T1* pStringDst, T2* pStringSrc, T2* pBlock1, T2* pBlock2, int begin = 0)
		{
			int blockLen1 = CStringImp::length<T2>(pBlock1);
			int blockLen2 = CStringImp::length<T2>(pBlock2);
			int len = blockLen1 > blockLen2 ? blockLen1 : blockLen2;

			int i = begin;
			int block = 0;
			int midBegin = -1;

			while (pStringSrc[i + len - 1] != 0)
			{
				// '<begin>
				if (CStringImp::comp<T2>(&pStringSrc[i], pBlock1, blockLen1) == 0)
				{
					block++;
					if (block == 1)
						midBegin = i + CStringImp::length<T2>(pBlock1);

					i++;
					continue;
				}
				// '</begin>'
				if (CStringImp::comp<T2>(&pStringSrc[i], pBlock2, blockLen2) == 0)
				{
					block--;
					if (block == 0 && midBegin != -1)
					{
						CStringImp::mid<T1, T2>(pStringDst, pStringSrc, midBegin, i);
						return i + 1;
					}
					if (block <= 0)
						return -1;
				}
				i++;
			}

			return -1;
		}

		// inorgeLoopChar
		// "this  is     my text  " -> "this is my text "
		template<class T>
		static void inorgeLoopChar(T* pString, T charLoop)
		{
			int i = 0;
			while (pString[i] != 0)
			{
				if (pString[i] == charLoop)
				{
					while (pString[i + 1] == charLoop && pString[i + 1] != 0)
					{
						int len = CStringImp::length<T>(&pString[i]);
						memmove(&pString[i], &pString[i + 1], len);
					}
				}

				i++;
			}
		}

		template<class T>
		static bool split(T* lpStrResult, const T* lpStringSearch, const T* lpStrSplit, int *pos)
		{
			int nLenSearch = CStringImp::length<const T>(lpStringSearch);
			if (*pos >= nLenSearch)
				return false;

			int nLen = CStringImp::length<const T>(lpStrSplit);
			if (nLen == 0)
				return false;

			int i;
			int minPos = -1;

			for (i = 0; i < nLen; i++)
			{
				int tempPos = *pos;

				tempPos = CStringImp::find<const T>(lpStringSearch, lpStrSplit[i], tempPos);
				if (tempPos == -1)
					continue;

				minPos = tempPos;
				break;
			}

			if (minPos == -1)
			{
				CStringImp::mid<T, const T>(lpStrResult, lpStringSearch, *pos, nLenSearch);
				*pos = nLenSearch;
				return true;
			}

			for (; i < nLen; i++)
			{
				int tempPos = CStringImp::find<const T>(lpStringSearch, lpStrSplit[i], *pos);
				if (tempPos == -1)
					continue;

				if (minPos > tempPos)
					minPos = tempPos;
			}

			CStringImp::mid<T, const T>(lpStrResult, lpStringSearch, *pos, minPos);
			*pos = minPos + 1;

			return true;
		}

		static int splitString(const char *stringSplit, const char *search, std::vector<std::string>& result)
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

		static int splitString(const wchar_t *stringSplit, const wchar_t *search, std::vector<std::wstring>& result)
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

		static bool format(char* lpString, const char* lpStringFormat, ...)
		{
			va_list listArgs;
			va_start(listArgs, lpStringFormat);

			int i = vsprintf(lpString, lpStringFormat, listArgs);
			if (i == -1)
				return false;

			return true;
		}

		template<class T>
		static bool parseToInt(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%d", result) == -1)
				return false;

			return true;
		}

		template<class T>
		static bool parseToFloat(T* lpString, float *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%f", result) == -1)
				return false;

			return true;
		}

		template<class T>
		static bool parseToUInt(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%u", result) == -1)
				return false;

			return true;
		}

		template<class T>
		static bool parseFromHex(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%X", result) == -1)
				return false;

			return true;
		}

		template<class T>
		static void toLower(T *lpString)
		{
			int i = 0;
			while (lpString[i] != 0)
			{
				lpString[i] = (int)towlower(lpString[i]);
				i++;
			}
		}

		template<class T>
		static void toUpper(T *lpString)
		{
			int i = 0;
			while (lpString[i] != 0)
			{
				lpString[i] = (int)towupper(lpString[i]);
				i++;
			}
		}

		template<class T1, class T2>
		static void getFolderPath(T1 *dstString, T2 *lpString)
		{
			int i = CStringImp::length<T2>(lpString) - 1;
			while (i >= 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/')
					break;
				i--;
			}
			CStringImp::copy<T1, T2>(dstString, lpString);
			if (i < 0)
				i = 0;
			dstString[i] = 0;
		}

		template<class T1, class T2>
		static void getFileName(T1 *dstString, T2 *lpString)
		{
			int i = CStringImp::length<T2>(lpString) - 1;
			while (i > 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/')
					break;
				i--;
			}
			if (i == 0)
				CStringImp::copy<T1, T2>(dstString, lpString);
			else
				CStringImp::copy<T1, T2>(dstString, lpString + i + 1);
		}

		template<class T1, class T2>
		static void getFileNameExt(T1 *dstString, T2 *lpString)
		{
			int i = CStringImp::length<T2>(lpString) - 1;
			while (i > 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/' || lpString[i] == '.')
					break;
				i--;
			}
			if (i == 0)
				CStringImp::copy<T1, T2>(dstString, lpString);
			else
				CStringImp::copy<T1, T2>(dstString, lpString + i + 1);
		}

		template<class T1, class T2>
		static void getFileNameNoExt(T1 *dstString, T2 *lpString)
		{
			int i = CStringImp::length<T2>(lpString) - 1;

			int dotPos = -1;
			while (i > 0)
			{
				if (lpString[i] == '.' && dotPos == -1)
					dotPos = i;
				else if (lpString[i] == '\\' || lpString[i] == '/')
					break;
				i--;
			}

			if (i == 0)
			{
				CStringImp::copy<T1, T2>(dstString, lpString);
				if (dotPos != -1)
					dstString[dotPos] = 0;
			}
			else
			{
				CStringImp::copy<T1, T2>(dstString, lpString + i + 1);
				if (dotPos != -1)
					dstString[dotPos - i - 1] = 0;
			}
		}

		template<class T>
		static void shortName(T* dest, const T* name, int maxchar)
		{
			int l = length<const T>(name);
			if (l <= maxchar)
			{
				copy<T, const T>(dest, name);
				return;
			}

			// copy
			for (int i = 0; i < maxchar; i++)
				dest[i] = name[i];

			dest[maxchar] = 0;

			if (maxchar - 1 > 0)
				dest[maxchar - 1] = (T)'.';

			if (maxchar - 2 > 0)
				dest[maxchar - 2] = (T)'.';

			//if (maxchar - 3 > 0)
			//	dest[maxchar - 3] = (T)'.';
		}

		template<class T>
		static void replaceText(T *result, const T *string, const T *search, const T *replace)
		{
			int lenSearch = length<const T>(search);
			int lenReplace = length<const T>(replace);
			int lenString = length<const T>(string);

			int j = 0;
			result[j] = 0;

			for (int i = 0, n = lenString; i < n; i++)
			{
				if (i <= lenString - lenSearch && memcmp(string + i, search, lenSearch * sizeof(T)) == 0)
				{
					cat<T, const T>(result + j, replace);

					i += lenSearch;
					i--; // for ++ next loop

					j += lenReplace;
					result[j] = 0;
				}
				else
				{
					result[j] = string[i];

					j++;
					result[j] = 0;
				}
			}
		}
		static wchar_t utf8Char2Unicode(const char *&str)
		{
			char c = *str++;
			if ((c & 0x80) == 0)
				return c;

			if (((c & 0xc0) == 0x80) || ((c & 0xfe) == 0xfe))
			{
				// I hope it should never happen
				return static_cast<unsigned char>(c);
			}

#pragma warning( disable : 4309 )
			char mask = 0xe0, value = 0xc0;
#pragma warning( default : 4309 )

			int i;
			for (i = 1; i < 6; ++i)
			{
				if ((c & mask) == value)
					break;
				value = mask;
				mask >>= 1;
			}

			wchar_t result = c & ~mask;
			for (; i > 0; --i)
			{
				c = *str++;
				if ((c & 0xc0) != 0x80)
				{
					// I hope should never happen
				}
				result <<= 6;
				result |= c & 0x3f;
			}
			return result;
		}

		static unsigned short* getUnicodeString(const wchar_t *src)
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

		static void convertUTF8ToUnicode(const char* src, wchar_t* dst)
		{
			int t = 0;

			const char *pos = src;

			while (*pos != 0)
				dst[t++] = utf8Char2Unicode(pos);

			dst[t++] = 0;
		}

		static int getUnicodeStringSize(const char* src)
		{
			int t = 0;

			const char *pos = src;

			while (*pos != 0)
			{
				utf8Char2Unicode(pos);
				t++;
			}

			return ++t;
		}

		static void convertUnicodeToUTF8(const wchar_t* src, char* dst)
		{
			int k = 0;
			int l = 0;

			while (src[k])
			{
				wchar_t c = src[k];

				if (c <= 0x007F)
				{
					dst[l++] = (char)c;
				}
				else if (c <= 0x07FF)
				{
					dst[l++] = 0xC0 | ((c >> 6) & 0x1F);
					dst[l++] = 0x80 | (c & 0x3F);
				}
				else
				{
					dst[l++] = 0xE0 | ((c >> 12) & 0x0F);
					dst[l++] = 0x80 | ((c >> 6) & 0x3F);
					dst[l++] = 0x80 | (c & 0x3F);
				}

				k++;
			}

			dst[l] = 0;
		}

		static int getUTF8StringSize(const wchar_t* src)
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

		static void replaceExt(char *lpPath, const char *lpExt)
		{
			char fileName[512] = { 0 };

			CStringImp::getFileNameNoExt(fileName, lpPath);
			CStringImp::copy(lpPath, fileName);
			CStringImp::cat(lpPath, lpExt);
		}

		static void replacePathExt(char *lpPath, const char *lpExt)
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

		static int findStringInList(std::vector<std::string>& listString, const char *find)
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
	};

}