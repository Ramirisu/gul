//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <ds/config.hpp>

#include <ds/byte.hpp>
#include <ds/type_traits.hpp>
#include <ds/utility.hpp>

namespace ds {

DS_CXX17_INLINE constexpr std::size_t dynamic_extent = std::size_t(-1);

namespace detail {

  template <typename T, std::size_t Extent>
  struct span_storage {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(T* pointer, std::size_t) noexcept
        : data_(pointer)
    {
    }

    T* data_ = nullptr;
    static constexpr std::size_t size_ = Extent;
  };

  template <typename T>
  struct span_storage<T, dynamic_extent> {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(T* pointer, std::size_t count) noexcept
        : data_(pointer)
        , size_(count)
    {
    }

    T* data_ = nullptr;
    std::size_t size_ = 0;
  };

  template <std::size_t Extent,
            bool CanDefaultConstruct
            = (Extent == 0 || Extent == dynamic_extent)>
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
class span : private detail::span_storage<T, Extent>,
             private detail::span_default_constructible<Extent> {
  using base_type = detail::span_storage<T, Extent>;
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

    DS_CXX14_CONSTEXPR reference operator*() noexcept
    {
      DS_ASSERT(curr_ != nullptr);
      return *curr_;
    }

    DS_CXX14_CONSTEXPR pointer operator->() noexcept
    {
      DS_ASSERT(curr_ != nullptr);
      return curr_;
    }

    DS_CXX14_CONSTEXPR reference operator[](difference_type n)
    {
      DS_ASSERT(curr_ != nullptr);
      return *(curr_ + n);
    }

    DS_CXX14_CONSTEXPR iterator_impl& operator++()
    {
      DS_ASSERT(curr_ != nullptr);
      ++curr_;
      return *this;
    }

    DS_CXX14_CONSTEXPR iterator_impl operator++(int)
    {
      DS_ASSERT(curr_ != nullptr);
      auto it = *this;
      ++*this;
      return it;
    }

    DS_CXX14_CONSTEXPR iterator_impl& operator--()
    {
      DS_ASSERT(curr_ != nullptr);
      --curr_;
      return *this;
    }

    DS_CXX14_CONSTEXPR iterator_impl operator--(int)
    {
      DS_ASSERT(curr_ != nullptr);
      auto it = *this;
      --*this;
      return it;
    }

    friend DS_CXX14_CONSTEXPR iterator_impl& operator+=(iterator_impl& it,
                                                        difference_type n)
    {
      DS_ASSERT(it.curr_ != nullptr);
      it.curr_ += n;
      return it;
    }

    friend DS_CXX14_CONSTEXPR iterator_impl& operator-=(iterator_impl& it,
                                                        difference_type n)
    {
      DS_ASSERT(it.curr_ != nullptr);
      it.curr_ -= n;
      return it;
    }

    friend DS_CXX14_CONSTEXPR iterator_impl operator+(const iterator_impl& it,
                                                      difference_type n)
    {
      auto temp = it;
      return temp += n;
    }

    friend DS_CXX14_CONSTEXPR iterator_impl operator+(difference_type n,
                                                      const iterator_impl& it)
    {
      auto temp = it;
      return temp += n;
    }

    friend DS_CXX14_CONSTEXPR iterator_impl operator-(const iterator_impl& it,
                                                      difference_type n)
    {
      auto temp = it;
      return temp -= n;
    }

    friend DS_CXX14_CONSTEXPR difference_type
    operator-(const iterator_impl& lhs, const iterator_impl& rhs)
    {
      return lhs.curr_ - rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator==(const iterator_impl& lhs,
                                              const iterator_impl& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator!=(const iterator_impl& lhs,
                                              const iterator_impl& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator<(const iterator_impl& lhs,
                                             const iterator_impl& rhs)
    {
      return lhs.curr_ < rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator<=(const iterator_impl& lhs,
                                              const iterator_impl& rhs)
    {
      return lhs.curr_ <= rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator>(const iterator_impl& lhs,
                                             const iterator_impl& rhs)
    {
      return lhs.curr_ > rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator>=(const iterator_impl& lhs,
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
      : base_type(first, last - first)
      , dc_base_type(in_place)
  {
  }

  template <std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
      : base_type(arr, N)
      , dc_base_type(in_place)
  {
  }

  template <class U,
            std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
      , dc_base_type(in_place)
  {
  }

  template <class U,
            std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(const std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
      , dc_base_type(in_place)
  {
  }

  DS_CXX14_CONSTEXPR span& operator=(const span&) noexcept = default;

  DS_CXX14_CONSTEXPR reference front() const
  {
    DS_ASSERT(size() > 0);
    return *data();
  }

  DS_CXX14_CONSTEXPR reference back() const
  {
    DS_ASSERT(size() > 0);
    return *(data() + size() - 1);
  }

  DS_CXX14_CONSTEXPR reference operator[](size_type index) const
  {
    DS_ASSERT(index < size());
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
  DS_CXX14_CONSTEXPR span<T, Count> first() const
  {
    static_assert(Count <= Extent, "[span::first] Count out of range");
    DS_ASSERT(Count <= size());
    return span<T, Count>(data(), Count);
  }

  DS_CXX14_CONSTEXPR span<T, dynamic_extent> first(size_type count) const
  {
    DS_ASSERT(count <= size());
    return span<T, dynamic_extent>(data(), count);
  }

  template <std::size_t Count>
  DS_CXX14_CONSTEXPR span<T, Count> last() const
  {
    static_assert(Count <= Extent, "[span::first] Count out of range");
    DS_ASSERT(Count <= size());
    return span<T, Count>(data() + (size() - Count), Count);
  }

  DS_CXX14_CONSTEXPR span<T, dynamic_extent> last(size_type count) const
  {
    DS_ASSERT(count <= size());
    return span<T, dynamic_extent>(data() + (size() - count), count);
  }

  template <std::size_t Offset, std::size_t Count = dynamic_extent>
  DS_CXX14_CONSTEXPR span<element_type,
                          (Count != dynamic_extent        ? Count
                               : Extent != dynamic_extent ? Extent - Offset
                                                          : dynamic_extent)>
  subspan() const
  {
    static_assert(Offset <= Extent, "[span::subspan] Offset out of range");
    static_assert(Count != dynamic_extent && Count + Offset <= Extent,
                  "[span::subspan] Count out of range");
    DS_ASSERT(Offset <= size());
    DS_ASSERT(Count != dynamic_extent && Count + Offset <= size());
    return span<element_type,
                (Count != dynamic_extent        ? Count
                     : Extent != dynamic_extent ? Extent - Offset
                                                : dynamic_extent)>(
        data() + Offset, Count);
  }

  DS_CXX14_CONSTEXPR span<element_type, dynamic_extent>
  subspan(size_type offset, size_type count = dynamic_extent) const
  {
    DS_ASSERT(offset <= size());
    DS_ASSERT(count != dynamic_extent && count + offset <= size());
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

#ifdef DS_HAS_CXX17
template <typename T>
span(T) -> span<T>;
#endif

template <class T, std::size_t N>
span<const byte, (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>
as_bytes(span<T, N> s) noexcept
{
  return span<const byte,
              (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>(
      reinterpret_cast<const byte*>(s.data()), s.size_bytes());
}

template <class T, std::size_t N>
span<byte, (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>
as_writable_bytes(span<T, N> s) noexcept
{
  return span<byte, (N == dynamic_extent ? dynamic_extent : N * sizeof(T))>(
      reinterpret_cast<byte*>(s.data()), s.size_bytes());
}
}
