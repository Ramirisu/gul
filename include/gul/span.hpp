//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/byte.hpp>
#include <gul/type_traits.hpp>
#include <gul/utility.hpp>

GUL_NAMESPACE_BEGIN

GUL_CXX17_INLINE constexpr std::size_t dynamic_extent = std::size_t(-1);

namespace detail {

template <typename T, std::size_t Extent>
struct span_storage_base {
  constexpr span_storage_base() noexcept = default;

  constexpr span_storage_base(T* pointer, std::size_t) noexcept
      : data_(pointer)
  {
  }

  T* data_ = nullptr;
  static constexpr std::size_t size_ = Extent;
};

template <typename T>
struct span_storage_base<T, dynamic_extent> {
  constexpr span_storage_base() noexcept = default;

  constexpr span_storage_base(T* pointer, std::size_t count) noexcept
      : data_(pointer)
      , size_(count)
  {
  }

  T* data_ = nullptr;
  std::size_t size_ = 0;
};

template <std::size_t Extent,
          bool CanDefaultConstruct = (Extent == 0 || Extent == dynamic_extent)>
struct span_default_constructible {
  constexpr span_default_constructible() noexcept = default;

  constexpr span_default_constructible(in_place_t) noexcept { }
};

template <std::size_t Extent>
struct span_default_constructible<Extent, false> {
  constexpr span_default_constructible() noexcept = delete;

  constexpr span_default_constructible(in_place_t) noexcept { }
};
}

template <typename T, std::size_t Extent = dynamic_extent>
class span : private detail::span_storage_base<T, Extent>,
             private detail::span_default_constructible<Extent> {
  using base_type = detail::span_storage_base<T, Extent>;
  using dc_base_type = detail::span_default_constructible<Extent>;

  class iterator_impl {
    friend class span;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    constexpr iterator_impl() noexcept = default;

    GUL_CXX14_CONSTEXPR reference operator*() noexcept
    {
      GUL_ASSERT(curr_ != nullptr);
      return *curr_;
    }

    GUL_CXX14_CONSTEXPR pointer operator->() noexcept
    {
      GUL_ASSERT(curr_ != nullptr);
      return curr_;
    }

    GUL_CXX14_CONSTEXPR reference operator[](difference_type n)
    {
      GUL_ASSERT(curr_ != nullptr);
      return *(curr_ + n);
    }

    GUL_CXX14_CONSTEXPR iterator_impl& operator++()
    {
      GUL_ASSERT(curr_ != nullptr);
      ++curr_;
      return *this;
    }

    GUL_CXX14_CONSTEXPR iterator_impl operator++(int)
    {
      GUL_ASSERT(curr_ != nullptr);
      auto it = *this;
      ++*this;
      return it;
    }

    GUL_CXX14_CONSTEXPR iterator_impl& operator--()
    {
      GUL_ASSERT(curr_ != nullptr);
      --curr_;
      return *this;
    }

    GUL_CXX14_CONSTEXPR iterator_impl operator--(int)
    {
      GUL_ASSERT(curr_ != nullptr);
      auto it = *this;
      --*this;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR iterator_impl& operator+=(iterator_impl& it,
                                                         difference_type n)
    {
      GUL_ASSERT(it.curr_ != nullptr);
      it.curr_ += n;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR iterator_impl& operator-=(iterator_impl& it,
                                                         difference_type n)
    {
      GUL_ASSERT(it.curr_ != nullptr);
      it.curr_ -= n;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR iterator_impl operator+(const iterator_impl& it,
                                                       difference_type n)
    {
      auto temp = it;
      return temp += n;
    }

    friend GUL_CXX14_CONSTEXPR iterator_impl operator+(difference_type n,
                                                       const iterator_impl& it)
    {
      auto temp = it;
      return temp += n;
    }

    friend GUL_CXX14_CONSTEXPR iterator_impl operator-(const iterator_impl& it,
                                                       difference_type n)
    {
      auto temp = it;
      return temp -= n;
    }

    friend GUL_CXX14_CONSTEXPR difference_type
    operator-(const iterator_impl& lhs, const iterator_impl& rhs)
    {
      return lhs.curr_ - rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator==(const iterator_impl& lhs,
                                               const iterator_impl& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator!=(const iterator_impl& lhs,
                                               const iterator_impl& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator<(const iterator_impl& lhs,
                                              const iterator_impl& rhs)
    {
      return lhs.curr_ < rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator<=(const iterator_impl& lhs,
                                               const iterator_impl& rhs)
    {
      return lhs.curr_ <= rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator>(const iterator_impl& lhs,
                                              const iterator_impl& rhs)
    {
      return lhs.curr_ > rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator>=(const iterator_impl& lhs,
                                               const iterator_impl& rhs)
    {
      return lhs.curr_ >= rhs.curr_;
    }

  private:
    constexpr iterator_impl(T* curr)
        : curr_(curr)
    {
    }

    T* curr_ = nullptr;
  };

public:
  using element_type = T;
  using value_type = remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = iterator_impl;
  using reverse_iterator = std::reverse_iterator<iterator>;

  static constexpr std::size_t extent = Extent;

  constexpr span() noexcept = default;

  constexpr span(pointer first, size_type size)
      : base_type(first, size)
      , dc_base_type(in_place)
  {
  }

  constexpr span(pointer first, pointer last)
      : base_type(first, size_type(last - first))
      , dc_base_type(in_place)
  {
  }

  template <typename It,
            GUL_REQUIRES(std::is_convertible<
                         typename std::iterator_traits<It>::reference,
                         element_type&>::value)>
  constexpr GUL_CXX20_EXPLICIT(Extent != dynamic_extent)
      span(It first, size_type size)
      : base_type(first.operator->(), size)
      , dc_base_type(in_place)
  {
  }

  template <typename It,
            GUL_REQUIRES(std::is_convertible<
                         typename std::iterator_traits<It>::reference,
                         element_type&>::value)>
  constexpr GUL_CXX20_EXPLICIT(Extent != dynamic_extent) span(It first, It last)
      : base_type(first.operator->(), size_type(last - first))
      , dc_base_type(in_place)
  {
  }

  template <std::size_t N,
            GUL_REQUIRES(extent == dynamic_extent || N == extent)>
  constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
      : base_type(arr, N)
      , dc_base_type(in_place)
  {
  }

  template <class U,
            std::size_t N,
            GUL_REQUIRES(extent == dynamic_extent || N == extent)>
  constexpr span(std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
      , dc_base_type(in_place)
  {
  }

  template <class U,
            std::size_t N,
            GUL_REQUIRES(extent == dynamic_extent || N == extent)>
  constexpr span(const std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
      , dc_base_type(in_place)
  {
  }

  GUL_CXX14_CONSTEXPR span(const span&) noexcept = default;

  GUL_CXX14_CONSTEXPR span& operator=(const span&) noexcept = default;

  GUL_CXX14_CONSTEXPR reference front() const
  {
    GUL_ASSERT(size() > 0);
    return *data();
  }

  GUL_CXX14_CONSTEXPR reference back() const
  {
    GUL_ASSERT(size() > 0);
    return *(data() + size() - 1);
  }

  GUL_CXX14_CONSTEXPR reference operator[](size_type index) const
  {
    GUL_ASSERT(index < size());
    return *(data() + index);
  }

  constexpr pointer data() const noexcept
  {
    return this->data_;
  }

  constexpr size_type size() const noexcept
  {
    return this->size_;
  }

  constexpr size_type size_bytes() const noexcept
  {
    return size() * sizeof(T);
  }

  constexpr bool empty() const noexcept
  {
    return size() == 0;
  }

  template <std::size_t Count>
  GUL_CXX14_CONSTEXPR span<T, Count> first() const
  {
    static_assert(Count <= Extent, "[span::first] Count out of range");
    GUL_ASSERT(Count <= size());
    return span<T, Count>(data(), Count);
  }

  GUL_CXX14_CONSTEXPR span<T, dynamic_extent> first(size_type count) const
  {
    GUL_ASSERT(count <= size());
    return span<T, dynamic_extent>(data(), count);
  }

  template <std::size_t Count>
  GUL_CXX14_CONSTEXPR span<T, Count> last() const
  {
    static_assert(Count <= Extent, "[span::first] Count out of range");
    GUL_ASSERT(Count <= size());
    return span<T, Count>(data() + (size() - Count), Count);
  }

  GUL_CXX14_CONSTEXPR span<T, dynamic_extent> last(size_type count) const
  {
    GUL_ASSERT(count <= size());
    return span<T, dynamic_extent>(data() + (size() - count), count);
  }

  template <std::size_t Offset, std::size_t Count = dynamic_extent>
  GUL_CXX14_CONSTEXPR span<element_type,
                           (Count != dynamic_extent        ? Count
                                : Extent != dynamic_extent ? Extent - Offset
                                                           : dynamic_extent)>
  subspan() const
  {
    static_assert(Offset <= Extent, "[span::subspan] Offset out of range");
    static_assert(Count != dynamic_extent && Count + Offset <= Extent,
                  "[span::subspan] Count out of range");
    GUL_ASSERT(Offset <= size());
    GUL_ASSERT(Count != dynamic_extent && Count + Offset <= size());
    return span<element_type,
                (Count != dynamic_extent        ? Count
                     : Extent != dynamic_extent ? Extent - Offset
                                                : dynamic_extent)>(
        data() + Offset, Count);
  }

  GUL_CXX14_CONSTEXPR span<element_type, dynamic_extent>
  subspan(size_type offset, size_type count = dynamic_extent) const
  {
    GUL_ASSERT(offset <= size());
    GUL_ASSERT(count != dynamic_extent && count + offset <= size());
    return span<element_type, dynamic_extent>(data() + offset, count);
  }

  constexpr iterator begin() const noexcept
  {
    return iterator(data());
  }

  constexpr iterator end() const noexcept
  {
    return iterator(data() + size());
  }

  constexpr reverse_iterator rbegin() const noexcept
  {
    return reverse_iterator(end());
  }

  constexpr reverse_iterator rend() const noexcept
  {
    return reverse_iterator(begin());
  }
};

template <typename T, std::size_t Extent>
constexpr std::size_t span<T, Extent>::extent;

#ifdef GUL_HAS_CXX17

template <typename It, typename EndOrSize>
span(It, EndOrSize)
    -> span<remove_reference_t<typename std::iterator_traits<It>::reference>>;

template <typename T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template <typename T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template <typename T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

#endif

namespace detail {
template <std::size_t N, std::size_t Mul>
struct extent_multiplier : integral_constant<std::size_t, N * Mul> { };
template <std::size_t Mul>
struct extent_multiplier<dynamic_extent, Mul>
    : integral_constant<std::size_t, dynamic_extent> { };
}

template <class T, std::size_t N>
auto as_bytes(span<T, N> s) noexcept
    -> span<const byte, detail::extent_multiplier<N, sizeof(N)>::value>
{
  return span<const byte, detail::extent_multiplier<N, sizeof(N)>::value>(
      reinterpret_cast<const byte*>(s.data()), s.size_bytes());
}

template <class T, std::size_t N>
auto as_writable_bytes(span<T, N> s) noexcept
    -> span<byte, detail::extent_multiplier<N, sizeof(N)>::value>
{
  return span<byte, detail::extent_multiplier<N, sizeof(N)>::value>(
      reinterpret_cast<byte*>(s.data()), s.size_bytes());
}

GUL_NAMESPACE_END
