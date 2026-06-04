// Copyright 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// PackedGLEnums_autogen.h:
//   Declares ANGLE-specific enums classes for GLEnum and functions operating
//   on them.

#ifndef COMMON_PACKEDGLENUMS_H_
#define COMMON_PACKEDGLENUMS_H_

#include <array>
#include <bitset>
#include <cstddef>

#include <EGL/egl.h>

namespace angle
{

// Return the number of elements of a packed enum, including the InvalidEnum element.
template <typename E>
constexpr size_t EnumSize()
{
    using UnderlyingType = typename std::underlying_type<E>::type;
    return static_cast<UnderlyingType>(E::EnumCount);
}

// Implementation of AllEnums which allows iterating over all the possible values for a packed enums
// like so:
//     for (auto value : AllEnums<MyPackedEnum>()) {
//         // Do something with the enum.
//     }

template <typename E>
class EnumIterator final
{
  private:
    using UnderlyingType = typename std::underlying_type<E>::type;

  public:
    EnumIterator(E value) : mValue(static_cast<UnderlyingType>(value)) {}
    EnumIterator &operator++()
    {
        mValue++;
        return *this;
    }
    bool operator==(const EnumIterator &other) const { return mValue == other.mValue; }
    bool operator!=(const EnumIterator &other) const { return mValue != other.mValue; }
    E operator*() const { return static_cast<E>(mValue); }

  private:
    UnderlyingType mValue;
};

template <typename E, size_t MaxSize = EnumSize<E>()>
struct AllEnums
{
    EnumIterator<E> begin() const { return {static_cast<E>(0)}; }
    EnumIterator<E> end() const { return {static_cast<E>(MaxSize)}; }
};

// PackedEnumMap<E, T> is like an std::array<T, E::EnumCount> but is indexed with enum values. It
// implements all of the std::array interface except with enum values instead of indices.
template <typename E, typename T, size_t MaxSize = EnumSize<E>()>
class PackedEnumMap
{
    using UnderlyingType = typename std::underlying_type<E>::type;
    using Storage        = std::array<T, MaxSize>;

  public:
    using InitPair = std::pair<E, T>;

    constexpr PackedEnumMap() = default;

    constexpr PackedEnumMap(std::initializer_list<InitPair> init) : mPrivateData{}
    {
        // We use a for loop instead of range-for to work around a limitation in MSVC.
        for (const InitPair *it = init.begin(); it != init.end(); ++it)
        {
            mPrivateData[static_cast<UnderlyingType>(it->first)] = it->second;
        }
    }

    // types:
    using value_type      = T;
    using pointer         = T *;
    using const_pointer   = const T *;
    using reference       = T &;
    using const_reference = const T &;

    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using iterator               = typename Storage::iterator;
    using const_iterator         = typename Storage::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // No explicit construct/copy/destroy for aggregate type
    void fill(const T &u) { mPrivateData.fill(u); }
    void swap(PackedEnumMap<E, T, MaxSize> &a) noexcept { mPrivateData.swap(a.mPrivateData); }

    // iterators:
    iterator begin() noexcept { return mPrivateData.begin(); }
    const_iterator begin() const noexcept { return mPrivateData.begin(); }
    iterator end() noexcept { return mPrivateData.end(); }
    const_iterator end() const noexcept { return mPrivateData.end(); }

    reverse_iterator rbegin() noexcept { return mPrivateData.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return mPrivateData.rbegin(); }
    reverse_iterator rend() noexcept { return mPrivateData.rend(); }
    const_reverse_iterator rend() const noexcept { return mPrivateData.rend(); }

    // capacity:
    constexpr size_type size() const noexcept { return mPrivateData.size(); }
    constexpr size_type max_size() const noexcept { return mPrivateData.max_size(); }
    constexpr bool empty() const noexcept { return mPrivateData.empty(); }

    // element access:
    reference operator[](E n)
    {
        return mPrivateData[static_cast<UnderlyingType>(n)];
    }

    constexpr const_reference operator[](E n) const
    {
        return mPrivateData[static_cast<UnderlyingType>(n)];
    }

    const_reference at(E n) const { return mPrivateData.at(static_cast<UnderlyingType>(n)); }
    reference at(E n) { return mPrivateData.at(static_cast<UnderlyingType>(n)); }

    reference front() { return mPrivateData.front(); }
    const_reference front() const { return mPrivateData.front(); }
    reference back() { return mPrivateData.back(); }
    const_reference back() const { return mPrivateData.back(); }

    T *data() noexcept { return mPrivateData.data(); }
    const T *data() const noexcept { return mPrivateData.data(); }

    bool operator==(const PackedEnumMap &rhs) const { return mPrivateData == rhs.mPrivateData; }
    bool operator!=(const PackedEnumMap &rhs) const { return mPrivateData != rhs.mPrivateData; }

    template <typename SubT = T>
    typename std::enable_if<std::is_integral<SubT>::value>::type operator+=(
        const PackedEnumMap<E, SubT, MaxSize> &rhs)
    {
        for (E e : AllEnums<E, MaxSize>())
        {
            at(e) += rhs[e];
        }
    }

  private:
    Storage mPrivateData;
};

}  // namespace angle

#endif
