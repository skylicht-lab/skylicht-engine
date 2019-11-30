#ifndef _STRING_IMP_H_
#define _STRING_IMP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cwchar>
#include <cstdarg>

namespace Skylicht
{
	class CStringImp
	{
	public:
		// length
		template <class T>
		static int length(T* pString)
		{
			int result = 0;
			while (pString[result] != 0)
				result++;

			return result;
		}

		// find
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

		// find
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

		// trimleft
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

		// trimright
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


		// trimleft
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

		// trimright
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


		// trim
		template <class T>
		static void trim(T* pString)
		{
			CStringImp::trimleft<T>(pString);
			CStringImp::trimright<T>(pString);
		}

		// trim
		template <class T>
		static void trim(T* pString, T delChar)
		{
			CStringImp::trimleft<T>(pString, delChar);
			CStringImp::trimright<T>(pString, delChar);
		}


		// countEntry
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

		// copy
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

		// copyAt
		template <class T1, class T2>
		static int copyAt(T1* pStringDst, T2* pStringSrc, int at)
		{
			int index = at;
			int i = 0;

			if (pStringSrc == 0)
				return index;

			while (pStringSrc[i] != 0)
			{
				pStringDst[index] = (T1)(pStringSrc[i]);
				index++; i++;
			}
			pStringDst[index] = 0;
			return index;
		}

		// cat
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

		// comp
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

		// comp
		template <class T>
		static int comp(T* pString1, T* pString2, int len)
		{
			return memcmp(pString1, pString2, len * sizeof(T));
		}

		// mid
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
		// Get string inside pBlock "{}" hay "[]" "<>" ...
		template <class T1, class T2>
		static int findBlock(T1* pStringDst, T2* pStringSrc, T2* pBlock, int begin = 0)
		{
			int i = begin;
			int block = 0;
			int midBegin = -1;

			while (pStringSrc[i] != 0)
			{
				if (pStringSrc[i] == pBlock[0])		// '{' hay '[' hay '('
				{
					block++;
					if (block == 1)
						midBegin = i + 1;
					i++;
					continue;
				}
				if (pStringSrc[i] == pBlock[1])		// '}' hay ']' hay ')'
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
					// Xoa cac ky tu giong no
					while (pString[i + 1] == charLoop && pString[i + 1] != 0)
					{
						int len = CStringImp::length<T>(&pString[i]);
						memmove(&pString[i], &pString[i + 1], len);
					}
				}

				i++;
			}
		}

		// split
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

			// Thiet lap chuoi moi
			CStringImp::mid<T, const T>(lpStrResult, lpStringSearch, *pos, minPos);
			*pos = minPos + 1;

			return true;
		}

		// splitString
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

		// format
		template<class T>
		static bool format(T* lpString, const T* lpFormat, ...)
		{
			va_list listArgs;
			va_start(listArgs, lpFormat);

			wchar_t lpWFormat[1024];
			wchar_t lpWResult[1024];

			CStringImp::copy<wchar_t, const T>(lpWFormat, lpFormat);
#ifdef __MINGW32__
			int i = vswprintf(lpWResult, lpWFormat, listArgs);
#else
			int i = vswprintf(lpWResult, 1024, lpWFormat, listArgs);
#endif
			if (i == -1)
				return false;
			
			CStringImp::copy<T, wchar_t>(lpString, lpWResult);
			return true;
		}

		// parseToInt
		template<class T>
		static bool parseToInt(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%d", result) == -1)
				return false;

			return true;
		}

		// parseToInt
		template<class T>
		static bool parseToFloat(T* lpString, float *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%f", result) == -1)
				return false;

			return true;
		}

		// parseToint
		template<class T>
		static bool parseToint(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%u", result) == -1)
				return false;

			return true;
		}

		// parseFromHex
		template<class T>
		static bool parseFromHex(T* lpString, int *result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%X", result) == -1)
				return false;

			return true;
		}

		// toLower
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

		// toLower
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

		// getFolderPath
		template<class T1, class T2>
		static void getFolderPath(T1 *lpString, T2 *dstString)
		{
			int i = CStringImp::length<T1>(lpString) - 1;
			while (i >= 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/')
					break;
				i--;
			}
			CStringImp::copy<T2, T1>(dstString, lpString);
			if (i < 0)
				i = 0;
			dstString[i] = 0;
		}

		// getFileName
		template<class T1, class T2>
		static void getFileName(T1 *lpString, T2 *dstString)
		{
			int i = CStringImp::length<T1>(lpString) - 1;
			while (i > 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/')
					break;
				i--;
			}
			if (i == 0)
				CStringImp::copy<T2, T1>(dstString, lpString);
			else
				CStringImp::copy<T2, T1>(dstString, lpString + i + 1);
		}

		// getFileNameExt
		template<class T1, class T2>
		static void getFileNameExt(T1 *lpString, T2 *dstString)
		{
			int i = CStringImp::length<T1>(lpString) - 1;
			while (i > 0)
			{
				if (lpString[i] == '\\' || lpString[i] == '/' || lpString[i] == '.')
					break;
				i--;
			}
			if (i == 0)
				CStringImp::copy<T2, T1>(dstString, lpString);
			else
				CStringImp::copy<T2, T1>(dstString, lpString + i + 1);
		}

		// getFileNameNoExt
		template<class T1, class T2>
		static void getFileNameNoExt(T1 *lpString, T2 *dstString)
		{
			int i = CStringImp::length<T1>(lpString) - 1;

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
				CStringImp::copy<T2, T1>(dstString, lpString);
			else
				CStringImp::copy<T2, T1>(dstString, lpString + i + 1);

			if (dotPos != -1)
				dstString[dotPos - i - 1] = 0;
		}

		template<class T>
		static void shortName(T* name, T* dest, int maxchar)
		{
			int l = length<T>(name);
			if (l <= maxchar)
			{
				copy<T, T>(dest, name);
				return;
			}

			// copy
			for (int i = 0; i < maxchar; i++)
				dest[i] = name[i];

			dest[maxchar] = 0;

			if (maxchar - 1 > 0)
				dest[maxchar - 1] = (T)'.';

			//if (maxchar - 2 > 0)
			//	dest[maxchar - 2] = (T)'.';

			//if (maxchar - 3 > 0)
			//	dest[maxchar - 3] = (T)'.';
		}

		template<class T>
		static void replaceText(const T *string, const T *search, const T *replace, T *result)
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

		// convertUTF8ToUnicode
		// convert the utf8 to unicode string		
		static wchar_t utf8Char2Unicode(const char *&str)
		{
			char c = *str++;
			if ((c & 0x80) == 0)
				return c;

			if (((c & 0xc0) == 0x80) || ((c & 0xfe) == 0xfe)) // should never happen
				return static_cast<unsigned char>(c);

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
				if ((c & 0xc0) != 0x80) // should never happen
				{
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

		// convertUnicodeToUTF8
		// convert unicode to utf8 string
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

			CStringImp::getFileNameNoExt<const char, char>(lpPath, fileName);
			CStringImp::copy<char, char>(lpPath, fileName);
			CStringImp::cat<char, const char>(lpPath, lpExt);
		}

		static void replacePathExt(char *lpPath, const char *lpExt)
		{
			char fileName[512] = { 0 };
			char folder[512] = { 0 };

			CStringImp::getFolderPath<const char, char>(lpPath, folder);
			CStringImp::getFileNameNoExt<const char, char>(lpPath, fileName);
			CStringImp::copy<char, char>(lpPath, folder);

			if (CStringImp::length<char>(folder) > 0)
				CStringImp::cat<char, const char>(lpPath, "/");

			CStringImp::cat<char, char>(lpPath, fileName);
			CStringImp::cat<char, const char>(lpPath, lpExt);
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

#endif
