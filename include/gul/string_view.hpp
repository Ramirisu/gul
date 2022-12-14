//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/type_traits.hpp>

#include <climits>
#include <cstddef>
#include <string>

#ifdef GUL_HAS_CXX17
#include <string_view>
#endif

GUL_NAMESPACE_BEGIN

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view {

  class const_iterator_impl {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = const CharT;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    constexpr const_iterator_impl(const CharT* curr)
        : curr_(curr)
    {
    }

    GUL_CXX14_CONSTEXPR reference operator*() noexcept
    {
      return *curr_;
    }

    GUL_CXX14_CONSTEXPR pointer operator->() noexcept
    {
      return curr_;
    }

    GUL_CXX14_CONSTEXPR reference operator[](difference_type n)
    {
      return *(curr_ + n);
    }

    GUL_CXX14_CONSTEXPR const_iterator_impl& operator++()
    {
      ++curr_;
      return *this;
    }

    GUL_CXX14_CONSTEXPR const_iterator_impl operator++(int)
    {
      auto it = *this;
      ++*this;
      return it;
    }

    GUL_CXX14_CONSTEXPR const_iterator_impl& operator--()
    {
      --curr_;
      return *this;
    }

    GUL_CXX14_CONSTEXPR const_iterator_impl operator--(int)
    {
      auto it = *this;
      --*this;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR const_iterator_impl&
    operator+=(const_iterator_impl& it, difference_type n)
    {
      it.curr_ += n;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR const_iterator_impl&
    operator-=(const_iterator_impl& it, difference_type n)
    {
      it.curr_ -= n;
      return it;
    }

    friend GUL_CXX14_CONSTEXPR const_iterator_impl
    operator+(const const_iterator_impl& it, difference_type n)
    {
      auto temp = it;
      return temp += n;
    }

    friend GUL_CXX14_CONSTEXPR const_iterator_impl
    operator+(difference_type n, const const_iterator_impl& it)
    {
      auto temp = it;
      return temp += n;
    }

    friend GUL_CXX14_CONSTEXPR const_iterator_impl
    operator-(const const_iterator_impl& it, difference_type n)
    {
      auto temp = it;
      return temp -= n;
    }

    friend GUL_CXX14_CONSTEXPR difference_type
    operator-(const const_iterator_impl& lhs, const const_iterator_impl& rhs)
    {
      return lhs.curr_ - rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator==(const const_iterator_impl& lhs,
                                               const const_iterator_impl& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator!=(const const_iterator_impl& lhs,
                                               const const_iterator_impl& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator<(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ < rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator<=(const const_iterator_impl& lhs,
                                               const const_iterator_impl& rhs)
    {
      return lhs.curr_ <= rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator>(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ > rhs.curr_;
    }

    friend GUL_CXX14_CONSTEXPR bool operator>=(const const_iterator_impl& lhs,
                                               const const_iterator_impl& rhs)
    {
      return lhs.curr_ >= rhs.curr_;
    }

  private:
    const CharT* curr_;
  };

  void throw_out_of_range(const char* msg) const
  {
#if GUL_NO_EXCEPTIONS
    GUL_UNUSED(msg);
    std::abort();
#else
    throw std::out_of_range(msg);
#endif
  }

public:
  using traits_type = Traits;
  using value_type = CharT;
  using pointer = CharT*;
  using const_pointer = const CharT*;
  using reference = CharT&;
  using const_reference = const CharT&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = const_iterator_impl;
  using const_iterator = const_iterator_impl;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr size_type npos = size_type(-1);

  constexpr basic_string_view() noexcept
      : start_(nullptr)
      , size_(0)
  {
  }

  constexpr basic_string_view(const CharT* s) noexcept
      : start_(s)
      , size_(Traits::length(s))
  {
  }

  constexpr basic_string_view(const CharT* s, size_type size) noexcept
      : start_(s)
      , size_(size)
  {
  }

  constexpr basic_string_view(const std::basic_string<CharT>& str) noexcept
      : start_(str.data())
      , size_(str.size())
  {
  }

  constexpr basic_string_view(std::nullptr_t) = delete;

  constexpr basic_string_view(const basic_string_view&) noexcept = default;

  GUL_CXX14_CONSTEXPR basic_string_view&
  operator=(const basic_string_view&) noexcept
      = default;

  GUL_CXX14_CONSTEXPR const_reference operator[](size_type index) const
  {
    GUL_ASSERT(index < size());
    return *(start_ + index);
  }

  GUL_CXX14_CONSTEXPR const_reference at(size_type index) const
  {
    if (index >= size()) {
      throw_out_of_range("[basic_string_view::at] index out of range");
    }

    return *(start_ + index);
  }

  GUL_CXX14_CONSTEXPR const_reference front() const
  {
    GUL_ASSERT(size() > 0);
    return *start_;
  }

  GUL_CXX14_CONSTEXPR const_reference back() const
  {
    GUL_ASSERT(size() > 0);
    return *(start_ + (size() - 1));
  }

  constexpr const_pointer data() const noexcept
  {
    return start_;
  }

  constexpr size_type size() const noexcept
  {
    return size_;
  }

  constexpr size_type length() const noexcept
  {
    return size();
  }

  constexpr size_type max_size() const noexcept
  {
    return std::numeric_limits<size_type>::max() / sizeof(CharT);
  }

  constexpr bool empty() const noexcept
  {
    return size() == 0;
  }

  GUL_CXX14_CONSTEXPR void remove_prefix(size_type count)
  {
    GUL_ASSERT(count <= size_);
    start_ += count;
    size_ -= count;
  }

  GUL_CXX14_CONSTEXPR void remove_suffix(size_type count)
  {
    GUL_ASSERT(count <= size_);
    size_ -= count;
  }

  GUL_CXX14_CONSTEXPR void swap(basic_string_view& other) noexcept
  {
    std::swap(start_, other.start_);
    std::swap(size_, other.size_);
  }

  GUL_CXX14_CONSTEXPR size_type copy(CharT* dest,
                                     size_type count,
                                     size_type index) const
  {
    if (index > size_) {
      throw_out_of_range("[basic_string_view::copy] index out of range");
    }

    const auto len = std::min(count, size_ - index);
    Traits::copy(dest, data() + index, len);
    return len;
  }

  GUL_CXX14_CONSTEXPR basic_string_view substr(size_type index = 0,
                                               size_type count = npos) const
  {
    if (index > size()) {
      throw_out_of_range("[basic_string_view::substr] index out of range");
    }

    return basic_string_view(start_ + index, std::min(count, size_ - index));
  }

  GUL_CXX14_CONSTEXPR basic_string_view first(size_type count) const
  {
    GUL_ASSERT(count <= size());
    return substr(0, count);
  }

  GUL_CXX14_CONSTEXPR basic_string_view last(size_type count) const
  {
    GUL_ASSERT(count <= size());
    return substr(size() - count);
  }

  GUL_CXX14_CONSTEXPR int compare(basic_string_view sv) const noexcept
  {
    const auto len = std::min(size(), sv.size());

    const auto result = Traits::compare(data(), sv.data(), len);
    if (result < 0) {
      return -1;
    }
    if (result > 0) {
      return 1;
    }
    if (size() < sv.size()) {
      return -1;
    }
    if (size() > sv.size()) {
      return 1;
    }

    return 0;
  }

  GUL_CXX14_CONSTEXPR int
  compare(size_type index1, size_type count1, basic_string_view sv) const

  {
    return substr(index1, count1).compare(sv);
  }

  GUL_CXX14_CONSTEXPR int compare(size_type index1,
                                  size_type count1,
                                  basic_string_view sv,
                                  size_type index2,
                                  size_type count2) const

  {
    return substr(index1, count1).compare(sv.substr(index2, count2));
  }

  GUL_CXX14_CONSTEXPR int compare(const CharT* s) const
  {
    return compare(basic_string_view(s));
  }

  GUL_CXX14_CONSTEXPR int
  compare(size_type index1, size_type count1, const CharT* s) const

  {
    return substr(index1, count1).compare(basic_string_view(s));
  }

  GUL_CXX14_CONSTEXPR int compare(size_type index1,
                                  size_type count1,
                                  const CharT* s,
                                  size_type count2) const

  {
    return substr(index1, count1).compare(basic_string_view(s, count2));
  }

  GUL_CXX14_CONSTEXPR bool starts_with(basic_string_view sv) const noexcept
  {
    return substr(0, sv.size()) == sv;
  }

  GUL_CXX14_CONSTEXPR bool starts_with(CharT c) const noexcept
  {
    return !empty() && Traits::eq(front(), c);
  }

  GUL_CXX14_CONSTEXPR bool starts_with(const CharT* s) const
  {
    return starts_with(basic_string_view(s));
  }

  GUL_CXX14_CONSTEXPR bool ends_with(basic_string_view sv) const noexcept
  {
    return size() >= sv.size() && compare(size() - sv.size(), npos, sv) == 0;
  }

  GUL_CXX14_CONSTEXPR bool ends_with(CharT c) const noexcept
  {
    return !empty() && Traits::eq(back(), c);
  }

  GUL_CXX14_CONSTEXPR bool ends_with(const CharT* s) const
  {
    return ends_with(basic_string_view(s));
  }

  GUL_CXX14_CONSTEXPR bool contains(basic_string_view sv) const noexcept
  {
    return find(sv) != npos;
  }

  GUL_CXX14_CONSTEXPR bool contains(CharT c) const noexcept
  {
    return contains(basic_string_view(std::addressof(c), 1));
  }

  GUL_CXX14_CONSTEXPR bool contains(const CharT* s) const
  {
    return contains(basic_string_view(s));
  }

  GUL_CXX14_CONSTEXPR size_type find(basic_string_view sv,
                                     size_type index = 0) const noexcept
  {
    if (sv.size() > size() || index > (size() - sv.size())) {
      return npos;
    }

    if (sv.empty()) {
      return index;
    }

    auto end = data() + (size() - sv.size());
    for (auto current = data() + index;; ++current) {
      current = Traits::find(current, end - current + 1, sv.front());
      if (current == nullptr) {
        break;
      }

      if (Traits::compare(current, sv.data(), sv.size()) == 0) {
        return current - data();
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type find(CharT ch,
                                     size_type index = 0) const noexcept
  {
    return find(basic_string_view(std::addressof(ch), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type find(const CharT* s,
                                     size_type index,
                                     size_type count) const noexcept
  {
    return find(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type find(const CharT* s,
                                     size_type index = 0) const noexcept
  {
    return find(basic_string_view(s), index);
  }

  GUL_CXX14_CONSTEXPR size_type rfind(basic_string_view sv,
                                      size_type index = npos) const noexcept
  {
    if (sv.empty()) {
      return std::min(index, size());
    }

    if (sv.size() <= size()) {
      for (auto current = data() + std::min(index, size() - sv.size());
           current >= data(); --current) {
        if (Traits::eq(*current, sv.front())
            && Traits::compare(current, sv.data(), sv.size()) == 0) {
          return current - data();
        }
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type rfind(CharT c,
                                      size_type index = npos) const noexcept
  {
    return rfind(basic_string_view(std::addressof(c), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type rfind(const CharT* s,
                                      size_type index,
                                      size_type count) const noexcept
  {
    return rfind(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type rfind(const CharT* s,
                                      size_type index = npos) const noexcept
  {
    return rfind(basic_string_view(s), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_of(basic_string_view sv,
                                              size_type index
                                              = 0) const noexcept
  {
    if (sv.size() > 0 && index < size()) {
      const auto end = data() + size();
      for (auto current = start_ + index; current < end; ++current) {
        if (Traits::find(sv.data(), sv.size(), *current) != nullptr) {
          return current - start_;
        }
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type find_first_of(CharT c,
                                              size_type index
                                              = 0) const noexcept
  {
    return find_first_of(basic_string_view(std::addressof(c), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_of(const CharT* s,
                                              size_type index,
                                              size_type count) const noexcept
  {
    return find_first_of(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_of(const CharT* s,
                                              size_type index
                                              = 0) const noexcept
  {
    return find_first_of(basic_string_view(s), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_of(basic_string_view sv,
                                             size_type index
                                             = npos) const noexcept
  {
    if (sv.size() > 0 && size() > 0) {
      const auto end = data();
      for (auto current = start_ + std::min(index, size() - 1); current >= end;
           --current) {
        if (Traits::find(sv.data(), sv.size(), *current) != nullptr) {
          return current - start_;
        }
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type find_last_of(CharT c,
                                             size_type index
                                             = npos) const noexcept
  {
    return find_last_of(basic_string_view(std::addressof(c), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_of(const CharT* s,
                                             size_type index,
                                             size_type count) const noexcept
  {
    return find_last_of(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_of(const CharT* s,
                                             size_type index
                                             = npos) const noexcept
  {
    return find_last_of(basic_string_view(s), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_not_of(basic_string_view sv,
                                                  size_type index
                                                  = 0) const noexcept
  {
    if (sv.size() > 0 && index < size()) {
      const auto end = data() + size();
      for (auto current = start_ + index; current < end; ++current) {
        if (Traits::find(sv.data(), sv.size(), *current) == nullptr) {
          return current - start_;
        }
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type find_first_not_of(CharT c,
                                                  size_type index
                                                  = 0) const noexcept
  {
    return find_first_not_of(basic_string_view(std::addressof(c), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_not_of(
      const CharT* s, size_type index, size_type count) const noexcept
  {
    return find_first_not_of(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_first_not_of(const CharT* s,
                                                  size_type index
                                                  = 0) const noexcept
  {
    return find_first_not_of(basic_string_view(s), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_not_of(basic_string_view sv,
                                                 size_type index
                                                 = npos) const noexcept
  {

    if (size() > 0) {
      const auto end = data();
      for (auto current = start_ + std::min(index, size() - 1); current >= end;
           --current) {
        if (Traits::find(sv.data(), sv.size(), *current) == nullptr) {
          return current - start_;
        }
      }
    }

    return npos;
  }

  GUL_CXX14_CONSTEXPR size_type find_last_not_of(CharT c,
                                                 size_type index
                                                 = npos) const noexcept
  {
    return find_last_not_of(basic_string_view(std::addressof(c), 1), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_not_of(const CharT* s,
                                                 size_type index,
                                                 size_type count) const noexcept
  {
    return find_last_not_of(basic_string_view(s, count), index);
  }

  GUL_CXX14_CONSTEXPR size_type find_last_not_of(const CharT* s,
                                                 size_type index
                                                 = npos) const noexcept
  {
    return find_last_not_of(basic_string_view(s), index);
  }

  constexpr const_iterator begin() const
  {
    return const_iterator(data());
  }

  constexpr const_iterator cbegin() const
  {
    return const_iterator(data());
  }

  constexpr const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(end());
  }

  constexpr const_reverse_iterator crbegin() const
  {
    return const_reverse_iterator(cend());
  }

  constexpr const_iterator end() const
  {
    return const_iterator(data() + size());
  }

  constexpr const_iterator cend() const
  {
    return const_iterator(data() + size());
  }

  constexpr const_reverse_iterator rend() const
  {
    return const_reverse_iterator(begin());
  }

  constexpr const_reverse_iterator crend() const
  {
    return const_reverse_iterator(cbegin());
  }

#ifdef GUL_HAS_CXX17
  constexpr operator std::basic_string_view<CharT, Traits>() const noexcept
  {
    return std::basic_string_view<CharT, Traits>(start_, size_);
  }
#endif

  constexpr explicit operator std::basic_string<CharT, Traits>() const
  {
    return std::basic_string<CharT, Traits>(start_, size_);
  }

private:
  const CharT* start_;
  size_type size_;
};

template <class CharT, class Traits>
constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::npos;

template <class CharT, class Traits>
GUL_CXX14_CONSTEXPR void
swap(basic_string_view<CharT, Traits>& lhs,
     basic_string_view<CharT, Traits>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

template <class CharT, class Traits>
constexpr bool operator==(
    const basic_string_view<CharT, Traits>& lhs,
    const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <class CharT, class Traits>
constexpr bool operator!=(
    const basic_string_view<CharT, Traits>& lhs,
    const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <class CharT, class Traits>
constexpr bool
operator<(const basic_string_view<CharT, Traits>& lhs,
          const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <class CharT, class Traits>
constexpr bool operator<=(
    const basic_string_view<CharT, Traits>& lhs,
    const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <class CharT, class Traits>
constexpr bool
operator>(const basic_string_view<CharT, Traits>& lhs,
          const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <class CharT, class Traits>
constexpr bool operator>=(
    const basic_string_view<CharT, Traits>& lhs,
    const type_identity_t<basic_string_view<CharT, Traits>>& rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
#if defined(GUL_HAS_CXX20) || defined(__cpp_char8_t)
using u8string_view = basic_string_view<char8_t>;
#endif

inline namespace literals {
inline namespace string_view_literals {
  constexpr string_view operator"" _sv(const char* str,
                                       std::size_t len) noexcept
  {
    return string_view(str, len);
  }

  constexpr wstring_view operator"" _sv(const wchar_t* str,
                                        std::size_t len) noexcept
  {
    return wstring_view(str, len);
  }

  constexpr u16string_view operator"" _sv(const char16_t* str,
                                          std::size_t len) noexcept
  {
    return u16string_view(str, len);
  }

  constexpr u32string_view operator"" _sv(const char32_t* str,
                                          std::size_t len) noexcept
  {
    return u32string_view(str, len);
  }

#if defined(GUL_HAS_CXX20) || defined(__cpp_char8_t)
  constexpr u8string_view operator"" _sv(const char8_t* str,
                                         std::size_t len) noexcept
  {
    return u8string_view(str, len);
  }
#endif
}
}

GUL_NAMESPACE_END

namespace std {
template <>
struct hash<gul::string_view> {
  std::size_t operator()(gul::string_view sv) const
  {
#ifdef GUL_HAS_CXX17
    return hash<string_view>()(string_view(sv.data(), sv.size()));
#else
    return hash<string>()(static_cast<string>(sv));
#endif
  }
};
template <>
struct hash<gul::wstring_view> {
  std::size_t operator()(gul::wstring_view sv) const
  {
#ifdef GUL_HAS_CXX17
    return hash<wstring_view>()(wstring_view(sv.data(), sv.size()));
#else
    return hash<wstring>()(static_cast<wstring>(sv));
#endif
  }
};
template <>
struct hash<gul::u16string_view> {
  std::size_t operator()(gul::u16string_view sv) const
  {
#ifdef GUL_HAS_CXX17
    return hash<u16string_view>()(u16string_view(sv.data(), sv.size()));
#else
    return hash<u16string>()(static_cast<u16string>(sv));
#endif
  }
};
template <>
struct hash<gul::u32string_view> {
  std::size_t operator()(gul::u32string_view sv) const
  {
#ifdef GUL_HAS_CXX17
    return hash<u32string_view>()(u32string_view(sv.data(), sv.size()));
#else
    return hash<u32string>()(static_cast<u32string>(sv));
#endif
  }
};
#if defined(GUL_HAS_CXX20) || defined(__cpp_char8_t)
template <>
struct hash<gul::u8string_view> {
  std::size_t operator()(gul::u8string_view sv) const
  {
    return hash<u8string_view>()(u8string_view(sv.data(), sv.size()));
  }
};
#endif
}
