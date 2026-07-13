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
#include <cwctype>
#include <cstdarg>
#include <string>
#include <vector>

namespace Skylicht
{
	/**
	 * @brief Low-level string helper functions used by engine utilities.
	 * @ingroup Utilities
	 *
	 * Most template functions operate on null-terminated mutable buffers and assume
	 * the destination buffer is large enough for the result. Functions returning
	 * `const std::string&` or `const std::wstring&` use shared temporary storage and
	 * should be copied before the next call that may reuse that storage.
	 */
	class SKYLICHT_API CStringImp
	{
	public:
		/**
		 * @brief Count characters before the null terminator.
		 * @tparam T Character type.
		 * @param pString Null-terminated string.
		 * @return Character count, excluding the terminator.
		 */
		template <class T>
		static int length(T* pString)
		{
			int result = 0;
			while (pString[result] != 0)
				result++;

			return result;
		}

		/**
		 * @brief Find a character in a null-terminated string.
		 * @tparam T Character type.
		 * @param pString String to search.
		 * @param pCharSearch Character to find.
		 * @param begin Start index.
		 * @return Character index, or -1 when not found.
		 */
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

		/**
		 * @brief Find a substring in a null-terminated string.
		 * @tparam T Character type.
		 * @param pString String to search.
		 * @param pKeySearch Null-terminated substring to find.
		 * @param begin Start index.
		 * @return First matching index, or -1 when not found.
		 */
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

		/**
		 * @brief Remove leading whitespace characters from a mutable string.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 */
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

		/**
		 * @brief Remove trailing whitespace characters from a mutable string.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 */
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

		/**
		 * @brief Remove leading occurrences of a character.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 * @param delChar Character to trim.
		 */
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

		/**
		 * @brief Remove trailing occurrences of a character.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 * @param delChar Character to trim.
		 */
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

		/**
		 * @brief Remove leading and trailing whitespace characters.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 */
		template <class T>
		static void trim(T* pString)
		{
			CStringImp::trimleft<T>(pString);
			CStringImp::trimright<T>(pString);
		}

		/**
		 * @brief Remove leading and trailing occurrences of a character.
		 * @tparam T Character type.
		 * @param pString String buffer to modify.
		 * @param delChar Character to trim.
		 */
		template <class T>
		static void trim(T* pString, T delChar)
		{
			CStringImp::trimleft<T>(pString, delChar);
			CStringImp::trimright<T>(pString, delChar);
		}

		/**
		 * @brief Count occurrences of a character.
		 * @tparam T Character type.
		 * @param pString String to scan.
		 * @param entry Character to count.
		 * @param begin Start index.
		 * @return Number of occurrences.
		 */
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

		/**
		 * @brief Copy a null-terminated string while converting character type.
		 * @tparam T1 Destination character type.
		 * @tparam T2 Source character type.
		 * @param pStringDst Destination buffer.
		 * @param pStringSrc Source string, or null.
		 * @return Number of characters copied, excluding the terminator.
		 */
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

		/**
		 * @brief Copy a source string into a destination string at an index.
		 * @return New end index, or 0 when the input is invalid.
		 */
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

		/**
		 * @brief Append a source string to a destination string.
		 * @return Number of appended characters.
		 */
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

		/**
		 * @brief Compare two null-terminated strings by raw character bytes.
		 * @return 0 if equal, a positive value if the first string is greater, otherwise negative.
		 */
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

		/**
		 * @brief Compare a fixed number of characters by raw character bytes.
		 * @return Result of `memcmp`.
		 */
		template <class T>
		static int comp(T* pString1, T* pString2, int len)
		{
			return memcmp(pString1, pString2, len * sizeof(T));
		}

		/**
		 * @brief Copy a substring range.
		 * @param pStringDst Destination buffer.
		 * @param pStringSrc Source string.
		 * @param begin First source index.
		 * @param end One-past-last source index.
		 * @return Number of copied characters.
		 */
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

		/**
		 * @brief Extract text between two occurrences of the same delimiter.
		 * @return Index after the closing delimiter, or -1 when not found.
		 */
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

		/**
		 * @brief Extract text from a balanced block such as `{}` or `[]`.
		 * @return Index after the closing delimiter, or -1 when no balanced block exists.
		 */
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

		/**
		 * @brief Extract text from a balanced block with string begin/end markers.
		 * @return Index after the closing marker, or -1 when no balanced block exists.
		 */
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

		/**
		 * @brief Collapse repeated occurrences of a character to a single occurrence.
		 * @param pString String buffer to modify.
		 * @param charLoop Repeated character to collapse.
		 */
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

		/**
		 * @brief Split a string incrementally using any character from a delimiter set.
		 * @param lpStrResult Destination buffer for the current token.
		 * @param lpStringSearch Source string.
		 * @param lpStrSplit Null-terminated delimiter character set.
		 * @param pos In/out current scan position.
		 * @return True when a token was produced, false when no more tokens exist.
		 */
		template<class T>
		static bool split(T* lpStrResult, const T* lpStringSearch, const T* lpStrSplit, int* pos)
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

		/**
		 * @brief Split a narrow string into a vector.
		 * @param stringSplit Delimiter characters.
		 * @param search Source string.
		 * @param result Receives split tokens.
		 * @return Number of tokens.
		 */
		static int splitString(const char* stringSplit, const char* search, std::vector<std::string>& result);

		/**
		 * @brief Replace every occurrence of a substring.
		 * @param string Source string to process.
		 * @param from Substring to replace.
		 * @param to Replacement text.
		 * @return Replaced string.
		 */
		static std::string replaceAll(std::string& string, const std::string& from, const std::string& to);

		/**
		 * @brief Split a wide string into a vector.
		 * @param stringSplit Delimiter characters.
		 * @param search Source string.
		 * @param result Receives split tokens.
		 * @return Number of tokens.
		 */
		static int splitString(const wchar_t* stringSplit, const wchar_t* search, std::vector<std::wstring>& result);

		/**
		 * @brief Format text into a caller-provided buffer.
		 * @param lpString Destination buffer.
		 * @param lpStringFormat Printf-style format string.
		 * @return True when formatting succeeds.
		 */
		static bool format(char* lpString, const char* lpStringFormat, ...);

		/**
		 * @brief Parse a signed integer from a string.
		 * @return True when parsing succeeds.
		 */
		template<class T>
		static bool parseToInt(T* lpString, int* result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%d", result) == -1)
				return false;

			return true;
		}

		/**
		 * @brief Parse a floating-point value from a string.
		 * @return True when parsing succeeds.
		 */
		template<class T>
		static bool parseToFloat(T* lpString, float* result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%f", result) == -1)
				return false;

			return true;
		}

		/**
		 * @brief Parse an unsigned integer from a string.
		 * @return True when parsing succeeds.
		 */
		template<class T>
		static bool parseToUInt(T* lpString, int* result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%u", result) == -1)
				return false;

			return true;
		}

		/**
		 * @brief Parse a hexadecimal integer from a string.
		 * @return True when parsing succeeds.
		 */
		template<class T>
		static bool parseFromHex(T* lpString, int* result)
		{
			wchar_t lpAString[400];
			CStringImp::copy<wchar_t, T>(lpAString, lpString);

			if (swscanf(lpAString, L"%X", result) == -1)
				return false;

			return true;
		}

		/**
		 * @brief Convert a mutable string to lowercase in place.
		 */
		template<class T>
		static void toLower(T* lpString)
		{
			int i = 0;
			while (lpString[i] != 0)
			{
				lpString[i] = (int)towlower(lpString[i]);
				i++;
			}
		}

		/**
		 * @brief Convert a mutable string to uppercase in place.
		 */
		template<class T>
		static void toUpper(T* lpString)
		{
			int i = 0;
			while (lpString[i] != 0)
			{
				lpString[i] = (int)towupper(lpString[i]);
				i++;
			}
		}

		/**
		 * @brief Extract the folder path from a file path.
		 * @param dstString Destination buffer.
		 * @param lpString Source path.
		 */
		template<class T1, class T2>
		static void getFolderPath(T1* dstString, T2* lpString)
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

		/**
		 * @brief Extract the file name from a path.
		 * @param dstString Destination buffer.
		 * @param lpString Source path.
		 */
		template<class T1, class T2>
		static void getFileName(T1* dstString, T2* lpString)
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

		/**
		 * @brief Extract the file extension from a path.
		 * @param dstString Destination buffer.
		 * @param lpString Source path.
		 */
		template<class T1, class T2>
		static void getFileNameExt(T1* dstString, T2* lpString)
		{
			int i = CStringImp::length<T2>(lpString) - 1;
			bool haveExt = false;

			while (i > 0)
			{
				if (lpString[i] == '.')
				{
					haveExt = true;
					break;
				}

				if (lpString[i] == '\\' || lpString[i] == '/')
				{
					break;
				}
				i--;
			}

			if (i == 0 || !haveExt)
				CStringImp::copy<T1, const char>(dstString, "");
			else
				CStringImp::copy<T1, T2>(dstString, lpString + i + 1);
		}

		/**
		 * @brief Extract the file name without extension from a path.
		 * @param dstString Destination buffer.
		 * @param lpString Source path.
		 */
		template<class T1, class T2>
		static void getFileNameNoExt(T1* dstString, T2* lpString)
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

		/**
		 * @brief Shorten a name to a maximum character count.
		 *
		 * When truncated, the last one or two characters are replaced with dots.
		 */
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

		/**
		 * @brief Replace all occurrences of text in a source buffer.
		 * @param result Destination buffer.
		 * @param string Source string.
		 * @param search Text to find.
		 * @param replace Replacement text.
		 */
		template<class T>
		static void replaceText(T* result, const T* string, const T* search, const T* replace)
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

		/**
		 * @brief Decode one UTF-8 character and advance the source pointer.
		 * @param str In/out UTF-8 source pointer.
		 * @return Decoded Unicode character.
		 */
		static wchar_t utf8Char2Unicode(const char*& str);

		/**
		 * @brief Convert a wide string to an unsigned 16-bit Unicode buffer.
		 * @param src Source wide string.
		 * @return Newly allocated or internal Unicode buffer as implemented by the source file.
		 */
		static unsigned short* getUnicodeString(const wchar_t* src);

		/**
		 * @brief Convert UTF-8 text to a wide-character buffer.
		 * @param src UTF-8 source string.
		 * @param dst Destination wide-character buffer.
		 */
		static void convertUTF8ToUnicode(const char* src, wchar_t* dst);

		/**
		 * @brief Calculate the wide-character length required for a UTF-8 string.
		 * @param src UTF-8 source string.
		 * @return Required wide-character count.
		 */
		static int getUnicodeStringSize(const char* src);

		/**
		 * @brief Convert wide-character text to UTF-8.
		 * @param src Wide-character source string.
		 * @param dst Destination UTF-8 buffer.
		 */
		static void convertUnicodeToUTF8(const wchar_t* src, char* dst);

		/**
		 * @brief Calculate the UTF-8 byte length required for a wide string.
		 * @param src Wide-character source string.
		 * @return Required byte count.
		 */
		static int getUTF8StringSize(const wchar_t* src);

		/**
		 * @brief Replace the extension portion of a file name in place.
		 * @param lpPath Mutable path buffer.
		 * @param lpExt New extension.
		 */
		static void replaceExt(char* lpPath, const char* lpExt);

		/**
		 * @brief Replace the extension portion of a full path in place.
		 * @param lpPath Mutable path buffer.
		 * @param lpExt New extension.
		 */
		static void replacePathExt(char* lpPath, const char* lpExt);

		/**
		 * @brief Find a string in a vector.
		 * @param listString Vector to search.
		 * @param find Text to find.
		 * @return Index of the match, or -1 when not found.
		 */
		static int findStringInList(std::vector<std::string>& listString, const char* find);

		/**
		 * @brief Replace all occurrences of a substring in a `std::string`.
		 * @param subject String to modify.
		 * @param search Text to find.
		 * @param replace Replacement text.
		 */
		static void replaceString(std::string& subject, const std::string& search, const std::string& replace);

		/**
		 * @brief Convert wide-character text to UTF-8 using shared temporary storage.
		 * @param src Source wide string.
		 * @return UTF-8 string in shared temporary storage.
		 */
		static const std::string& convertUnicodeToUTF8(const wchar_t* src);

		/**
		 * @brief Convert UTF-8 text to wide-character text using shared temporary storage.
		 * @param src Source UTF-8 string.
		 * @return Wide string in shared temporary storage.
		 */
		static const std::wstring& convertUTF8ToUnicode(const char* src);

		/**
		 * @brief Return a lowercase copy of a narrow string.
		 * @param s Source string.
		 * @return Lowercase text in shared temporary storage.
		 */
		static const std::string& toLower(const std::string& s);

		/**
		 * @brief Return a lowercase copy of a wide string.
		 * @param s Source string.
		 * @return Lowercase text in shared temporary storage.
		 */
		static const std::wstring& toLower(const std::wstring& s);

		/**
		 * @brief Return an uppercase copy of a narrow string.
		 * @param s Source string.
		 * @return Uppercase text in shared temporary storage.
		 */
		static const std::string& toUpper(const std::string& s);

		/**
		 * @brief Return an uppercase copy of a wide string.
		 * @param s Source string.
		 * @return Uppercase text in shared temporary storage.
		 */
		static const std::wstring& toUpper(const std::wstring& s);

		/**
		 * @brief Format a number with thousand separators or compact suffixes.
		 * @param n Number to format.
		 * @param useK Allow compact `K` suffix.
		 * @param useM Allow compact `M` suffix.
		 * @return Formatted number in shared temporary storage.
		 */
		static const std::string& formatThousand(int n, bool useK, bool useM);
	};

}
