/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
	template <class T>
	class CArrayUtils
	{
	protected:
		core::array<T> m_array;
		T* m_ptr;
		int m_alloc;
		int m_count;
	public:
		CArrayUtils() :
			m_alloc(0),
			m_count(0),
			m_ptr(NULL)
		{

		}

		inline void reset()
		{
			m_count = 0;
		}

		inline T* pointer()
		{
			return m_ptr;
		}

		inline core::array<T>& getArray()
		{
			return m_array;
		}

		inline int count()
		{
			return m_count;
		}

		void push(T element)
		{
			if (m_count + 1 >= m_alloc)
			{
				m_alloc = (m_count + 1) * 2;
				if (m_alloc < 32)
					m_alloc = 32;
				m_array.set_used(m_alloc);
				m_ptr = m_array.pointer();
			}

			m_ptr[m_count++] = element;
		}
	};
}