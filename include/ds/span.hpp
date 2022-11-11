#pragma once

#include <ds/config.hpp>

#include <ds/type_traits.hpp>

namespace ds {

DS_CXX17_INLINE constexpr std::size_t dynamic_extent = std::size_t(-1);

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

template <typename T, std::size_t Extent = dynamic_extent>
class span : private span_storage<T, Extent> {
  using base_type = span_storage<T, Extent>;

public:
  using element_type = T;
  using value_type = remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  static constexpr std::size_t extent = Extent;

  constexpr span() noexcept = default;

  constexpr span(pointer first, size_type size)
      : base_type(first, size)
  {
  }

  constexpr span(pointer first, pointer last)
      : base_type(first, last - first)
  {
  }

  template <std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
      : base_type(arr, N)
  {
  }

  template <class U,
            std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
  {
  }

  template <class U,
            std::size_t N,
            enable_if_t<extent == dynamic_extent || N == extent, int> = 0>
  constexpr span(const std::array<U, N>& arr) noexcept
      : base_type(arr.data(), N)
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
};

template <typename T, std::size_t Extent>
constexpr std::size_t span<T, Extent>::extent;

#ifdef DS_HAS_CXX17
template <typename T>
span(T) -> span<T>;
#endif
}
