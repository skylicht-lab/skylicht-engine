/*
!@
MIT License

Copyright (c) 2026 Skylicht Technology CO., LTD

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

#ifdef USE_SHARED_HEAP_MEMORY

#include <cstddef>
#include <cstdint>
#include <limits>
#include <windows.h>
#include <new>

namespace Skylicht
{
	namespace Heap
	{
		inline size_t normalizeSize(size_t size) noexcept
		{
			return size == 0 ? 1 : size;
		}

		inline size_t normalizeAlignment(size_t alignment) noexcept
		{
			const size_t minAlignment = alignof(void*);
			return alignment < minAlignment ? minAlignment : alignment;
		}

		inline void* allocRawNoThrow(size_t size) noexcept
		{
			return ::HeapAlloc(::GetProcessHeap(), 0, normalizeSize(size));
		}

		inline void* allocRaw(size_t size)
		{
			for (;;)
			{
				if (void* p = allocRawNoThrow(size))
					return p;

				std::new_handler handler = std::get_new_handler();
				if (handler == NULL)
					throw std::bad_alloc();

				handler();
			}
		}

		inline void freeRaw(void* p) noexcept
		{
			if (p != NULL)
				::HeapFree(::GetProcessHeap(), 0, p);
		}

		inline void* allocAlignedNoThrow(size_t size, size_t alignment) noexcept
		{
			size = normalizeSize(size);
			alignment = normalizeAlignment(alignment);

			const size_t extra = alignment - 1 + sizeof(void*);

			void* base = ::HeapAlloc(::GetProcessHeap(), 0, size + extra);
			if (base == NULL)
				return NULL;

			const uintptr_t raw = reinterpret_cast<uintptr_t>(base) + sizeof(void*);
			const uintptr_t aligned = (raw + alignment - 1) & ~(static_cast<uintptr_t>(alignment) - 1);

			void* p = reinterpret_cast<void*>(aligned);
			reinterpret_cast<void**>(p)[-1] = base;
			return p;
		}

		inline void* allocAligned(size_t size, size_t alignment)
		{
			for (;;)
			{
				if (void* p = allocAlignedNoThrow(size, alignment))
					return p;

				std::new_handler handler = std::get_new_handler();
				if (handler == NULL)
					throw std::bad_alloc();

				handler();
			}
		}

		inline void freeAligned(void* p) noexcept
		{
			if (p == NULL)
				return;

			void* base = reinterpret_cast<void**>(p)[-1];
			::HeapFree(::GetProcessHeap(), 0, base);
		}
	}
}

inline void* operator new(size_t size)
{
	return Skylicht::Heap::allocRaw(size);
}

inline void operator delete(void* p) noexcept
{
	Skylicht::Heap::freeRaw(p);
}

inline void* operator new[](size_t size)
{
	return Skylicht::Heap::allocRaw(size);
}

inline void operator delete[](void* p) noexcept
{
	Skylicht::Heap::freeRaw(p);
}

inline void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	return Skylicht::Heap::allocRawNoThrow(size);
}

inline void operator delete(void* p, const std::nothrow_t&) noexcept
{
	Skylicht::Heap::freeRaw(p);
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
	return Skylicht::Heap::allocRawNoThrow(size);
}

inline void operator delete[](void* p, const std::nothrow_t&) noexcept
{
	Skylicht::Heap::freeRaw(p);
}

#if defined(__cpp_sized_deallocation) || defined(_MSC_VER)
inline void operator delete(void* p, size_t) noexcept
{
	Skylicht::Heap::freeRaw(p);
}

inline void operator delete[](void* p, size_t) noexcept
{
	Skylicht::Heap::freeRaw(p);
}
#endif

#if defined(__cpp_aligned_new)
inline void* operator new(size_t size, std::align_val_t alignment)
{
	return Skylicht::Heap::allocAligned(size, static_cast<size_t>(alignment));
}

inline void operator delete(void* p, std::align_val_t) noexcept
{
	Skylicht::Heap::freeAligned(p);
}

inline void* operator new[](size_t size, std::align_val_t alignment)
{
	return Skylicht::Heap::allocAligned(size, static_cast<size_t>(alignment));
}

inline void operator delete[](void* p, std::align_val_t) noexcept
{
	Skylicht::Heap::freeAligned(p);
}

inline void* operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	return Skylicht::Heap::allocAlignedNoThrow(size, static_cast<size_t>(alignment));
}

inline void operator delete(void* p, std::align_val_t, const std::nothrow_t&) noexcept
{
	Skylicht::Heap::freeAligned(p);
}

inline void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	return Skylicht::Heap::allocAlignedNoThrow(size, static_cast<size_t>(alignment));
}

inline void operator delete[](void* p, std::align_val_t, const std::nothrow_t&) noexcept
{
	Skylicht::Heap::freeAligned(p);
}

#if defined(__cpp_sized_deallocation) || defined(_MSC_VER)
inline void operator delete(void* p, size_t, std::align_val_t) noexcept
{
	Skylicht::Heap::freeAligned(p);
}

inline void operator delete[](void* p, size_t, std::align_val_t) noexcept
{
	Skylicht::Heap::freeAligned(p);
}
#endif
#endif

#endif
