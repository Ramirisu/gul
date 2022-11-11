#pragma once

#include <ds/config.hpp>

#include <ds/type_traits.hpp>

#include <climits>
#include <string>

namespace ds {
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

    constexpr reference operator*() noexcept
    {
      return *curr_;
    }

    constexpr pointer operator->() noexcept
    {
      return curr_;
    }

    constexpr reference operator[](difference_type n)
    {
      return *(curr_ + n);
    }

    DS_CXX14_CONSTEXPR const_iterator_impl& operator++()
    {
      ++curr_;
      return *this;
    }

    DS_CXX14_CONSTEXPR const_iterator_impl operator++(int)
    {
      auto it = *this;
      ++*this;
      return it;
    }

    DS_CXX14_CONSTEXPR const_iterator_impl& operator--()
    {
      --curr_;
      return *this;
    }

    DS_CXX14_CONSTEXPR const_iterator_impl operator--(int)
    {
      auto it = *this;
      --*this;
      return it;
    }

    friend DS_CXX14_CONSTEXPR const_iterator_impl&
    operator+=(const_iterator_impl& it, difference_type n)
    {
      it.curr_ += n;
      return it;
    }

    friend DS_CXX14_CONSTEXPR const_iterator_impl&
    operator-=(const_iterator_impl& it, difference_type n)
    {
      it.curr_ -= n;
      return it;
    }

    friend DS_CXX14_CONSTEXPR const_iterator_impl
    operator+(const const_iterator_impl& it, difference_type n)
    {
      auto temp = it;
      return temp += n;
    }

    friend DS_CXX14_CONSTEXPR const_iterator_impl
    operator+(difference_type n, const const_iterator_impl& it)
    {
      auto temp = it;
      return temp += n;
    }

    friend DS_CXX14_CONSTEXPR const_iterator_impl
    operator-(const const_iterator_impl& it, difference_type n)
    {
      auto temp = it;
      return temp -= n;
    }

    friend DS_CXX14_CONSTEXPR difference_type
    operator-(const const_iterator_impl& lhs, const const_iterator_impl& rhs)
    {
      return lhs.curr_ - rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator==(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator!=(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator<(const const_iterator_impl& lhs,
                                             const const_iterator_impl& rhs)
    {
      return lhs.curr_ < rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator<=(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ <= rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator>(const const_iterator_impl& lhs,
                                             const const_iterator_impl& rhs)
    {
      return lhs.curr_ > rhs.curr_;
    }

    friend DS_CXX14_CONSTEXPR bool operator>=(const const_iterator_impl& lhs,
                                              const const_iterator_impl& rhs)
    {
      return lhs.curr_ >= rhs.curr_;
    }

  private:
    const CharT* curr_;
  };

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

  constexpr basic_string_view(std::nullptr_t) = delete;

  constexpr basic_string_view(const basic_string_view&) noexcept = default;

  DS_CXX14_CONSTEXPR basic_string_view&
  operator=(const basic_string_view&) noexcept
      = default;

  DS_CXX14_CONSTEXPR const_reference operator[](size_type index) const
  {
    DS_ASSERT(index < size());
    return *(start_ + index);
  }

  DS_CXX14_CONSTEXPR const_reference at(size_type index) const
  {
    if (index >= size()) {
      throw std::out_of_range("[basic_string_view::at] index out of range");
    }

    return *(start_ + index);
  }

  DS_CXX14_CONSTEXPR const_reference front() const
  {
    DS_ASSERT(size() > 0);
    return *start_;
  }

  DS_CXX14_CONSTEXPR const_reference back() const
  {
    DS_ASSERT(size() > 0);
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

  DS_CXX14_CONSTEXPR void remove_prefix(size_type count)
  {
    DS_ASSERT(count <= size_);
    start_ += count;
    size_ -= count;
  }

  DS_CXX14_CONSTEXPR void remove_suffix(size_type count)
  {
    DS_ASSERT(count <= size_);
    size_ -= count;
  }

  DS_CXX14_CONSTEXPR void swap(basic_string_view& other) noexcept
  {
    std::swap(start_, other.start_);
    std::swap(size_, other.size_);
  }

  DS_CXX14_CONSTEXPR size_type copy(CharT* dest,
                                    size_type count,
                                    size_type index) const
  {
    if (index > size_) {
      throw std::out_of_range("[basic_string_view::copy] index out of range");
    }

    const auto len = std::min(count, size_ - index);
    Traits::copy(dest, data() + index, len);
    return len;
  }

  constexpr basic_string_view substr(size_type index = 0,
                                     size_type count = npos) const
  {
    return basic_string_view(start_ + index, std::min(count, size_ - index));
  }

  DS_CXX14_CONSTEXPR int compare(basic_string_view sv) const noexcept
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

  DS_CXX14_CONSTEXPR int
  compare(size_type index1, size_type count1, basic_string_view sv) const

  {
    return substr(index1, count1).compare(sv);
  }

  DS_CXX14_CONSTEXPR int compare(size_type index1,
                                 size_type count1,
                                 basic_string_view sv,
                                 size_type index2,
                                 size_type count2) const

  {
    return substr(index1, count1).compare(sv.substr(index2, count2));
  }

  DS_CXX14_CONSTEXPR int compare(const CharT* s) const
  {
    return compare(basic_string_view(s));
  }

  DS_CXX14_CONSTEXPR int
  compare(size_type index1, size_type count1, const CharT* s) const

  {
    return substr(index1, count1).compare(basic_string_view(s));
  }

  DS_CXX14_CONSTEXPR int compare(size_type index1,
                                 size_type count1,
                                 const CharT* s,
                                 size_type count2) const

  {
    return substr(index1, count1).compare(basic_string_view(s, count2));
  }

  DS_CXX14_CONSTEXPR bool starts_with(basic_string_view sv) const noexcept
  {
    return substr(0, sv.size()) == sv;
  }

  DS_CXX14_CONSTEXPR bool starts_with(CharT c) const noexcept
  {
    return !empty() && Traits::eq(front(), c);
  }

  DS_CXX14_CONSTEXPR bool starts_with(const CharT* s) const
  {
    return starts_with(basic_string_view(s));
  }

  DS_CXX14_CONSTEXPR bool ends_with(basic_string_view sv) const noexcept
  {
    return size() >= sv.size() && compare(size() - sv.size(), npos, sv) == 0;
  }

  DS_CXX14_CONSTEXPR bool ends_with(CharT c) const noexcept
  {
    return !empty() && Traits::eq(back(), c);
  }

  DS_CXX14_CONSTEXPR bool ends_with(const CharT* s) const
  {
    return ends_with(basic_string_view(s));
  }

  DS_CXX14_CONSTEXPR bool contains(basic_string_view sv) const noexcept
  {
    return find(sv) != npos;
  }

  DS_CXX14_CONSTEXPR bool contains(CharT c) const noexcept
  {
    return find(c) != npos;
  }

  DS_CXX14_CONSTEXPR bool contains(const CharT* s) const
  {
    return find(s) != npos;
  }

  DS_CXX14_CONSTEXPR size_type find(basic_string_view sv,
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
        return npos;
      }

      if (Traits::compare(current, sv.data(), sv.size()) == 0) {
        return current - data();
      }
    }

    return npos;
  }

  DS_CXX14_CONSTEXPR size_type find(CharT ch,
                                    size_type index = 0) const noexcept
  {
    return find(basic_string_view(std::addressof(ch), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type find(const CharT* s,
                                    size_type index,
                                    size_type count) const
  {
    return find(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type find(const CharT* s, size_type index = 0) const
  {
    return find(basic_string_view(s), index);
  }

  DS_CXX14_CONSTEXPR size_type rfind(basic_string_view sv,
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

  DS_CXX14_CONSTEXPR size_type rfind(CharT c,
                                     size_type index = npos) const noexcept
  {
    return rfind(basic_string_view(std::addressof(c), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type rfind(const CharT* s,
                                     size_type index,
                                     size_type count) const
  {
    return rfind(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type rfind(const CharT* s,
                                     size_type index = npos) const
  {
    return rfind(basic_string_view(s), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_of(basic_string_view sv,
                                             size_type index = 0) const noexcept
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

  DS_CXX14_CONSTEXPR size_type find_first_of(CharT c,
                                             size_type index = 0) const noexcept
  {
    return find_first_of(basic_string_view(std::addressof(c), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_of(const CharT* s,
                                             size_type index,
                                             size_type count) const
  {
    return find_first_of(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_of(const CharT* s,
                                             size_type index = 0) const
  {
    return find_first_of(basic_string_view(s), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_of(basic_string_view sv,
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

  DS_CXX14_CONSTEXPR size_type find_last_of(CharT c,
                                            size_type index
                                            = npos) const noexcept
  {
    return find_last_of(basic_string_view(std::addressof(c), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_of(const CharT* s,
                                            size_type index,
                                            size_type count) const
  {
    return find_last_of(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_of(const CharT* s,
                                            size_type index = npos) const
  {
    return find_last_of(basic_string_view(s), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_not_of(basic_string_view sv,
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

  DS_CXX14_CONSTEXPR size_type find_first_not_of(CharT c,
                                                 size_type index
                                                 = 0) const noexcept
  {
    return find_first_not_of(basic_string_view(std::addressof(c), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_not_of(const CharT* s,
                                                 size_type index,
                                                 size_type count) const
  {
    return find_first_not_of(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type find_first_not_of(const CharT* s,
                                                 size_type index = 0) const
  {
    return find_first_not_of(basic_string_view(s), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_not_of(basic_string_view sv,
                                                size_type index
                                                = npos) const noexcept
  {

    if (sv.size() > 0 && size() > 0) {
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

  DS_CXX14_CONSTEXPR size_type find_last_not_of(CharT c,
                                                size_type index
                                                = npos) const noexcept
  {
    return find_last_not_of(basic_string_view(std::addressof(c), 1), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_not_of(const CharT* s,
                                                size_type index,
                                                size_type count) const
  {
    return find_last_not_of(basic_string_view(s, count), index);
  }

  DS_CXX14_CONSTEXPR size_type find_last_not_of(const CharT* s,
                                                size_type index = npos) const
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

  constexpr explicit operator std::basic_string<CharT, Traits>()
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
constexpr bool operator==(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <class CharT, class Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <class CharT, class Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <class CharT, class Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <class CharT, class Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <class CharT, class Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

inline namespace literals {
  inline namespace string_view_literals {
#if !defined(__clang__) || defined(DS_HAS_CXX14)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuser-defined-literals"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4455)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif

    constexpr string_view operator"" sv(const char* str,
                                        std::size_t len) noexcept
    {
      return string_view(str, len);
    }

    constexpr wstring_view operator"" sv(const wchar_t* str,
                                         std::size_t len) noexcept
    {
      return wstring_view(str, len);
    }

    constexpr u16string_view operator"" sv(const char16_t* str,
                                           std::size_t len) noexcept
    {
      return u16string_view(str, len);
    }

    constexpr u32string_view operator"" sv(const char32_t* str,
                                           std::size_t len) noexcept
    {
      return u32string_view(str, len);
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
  }
}
}

namespace std {
template <>
struct hash<ds::string_view> {
  std::size_t operator()(ds::string_view sv) const
  {
    return hash<string>()(static_cast<string>(sv));
  }
};
template <>
struct hash<ds::wstring_view> {
  std::size_t operator()(ds::wstring_view sv) const
  {
    return hash<wstring>()(static_cast<wstring>(sv));
  }
};
template <>
struct hash<ds::u16string_view> {
  std::size_t operator()(ds::u16string_view sv) const
  {
    return hash<u16string>()(static_cast<u16string>(sv));
  }
};
template <>
struct hash<ds::u32string_view> {
  std::size_t operator()(ds::u32string_view sv) const
  {
    return hash<u32string>()(static_cast<u32string>(sv));
  }
};
}
