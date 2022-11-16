//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <ds/config.hpp>

#include <ds/optional.hpp>
#include <ds/type_traits.hpp>

#include <deque>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>

DS_NAMESPACE_BEGIN

template <typename Key, typename T>
class fifo_map {
  using value_type_impl = std::pair<Key, T>;
  using map_type = std::map<Key, T>;
  using queue_type = std::deque<typename map_type::iterator>;

  template <bool Const>
  class iterator_impl {
    using parent_iterator = conditional_t<Const,
                                          typename queue_type::const_iterator,
                                          typename queue_type::iterator>;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = conditional_t<Const,
                                     const typename map_type::value_type,
                                     typename map_type::value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    iterator_impl(parent_iterator curr)
        : curr_(std::move(curr))
    {
    }

    iterator_impl operator++(int)
    {
      auto it = *this;
      ++*this;
      return it;
    }

    iterator_impl& operator++()
    {
      ++curr_;
      return *this;
    }

    iterator_impl operator--(int)
    {
      auto it = *this;
      --*this;
      return it;
    }

    iterator_impl& operator--()
    {
      --curr_;
      return *this;
    }

    reference operator*() noexcept
    {
      return **curr_;
    }

    pointer operator->() noexcept
    {
      return &*curr_;
    }

    friend bool operator==(const iterator_impl& lhs, const iterator_impl& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend bool operator!=(const iterator_impl& lhs, const iterator_impl& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

  private:
    parent_iterator curr_;
  };

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = iterator_impl<false>;
  using const_iterator = iterator_impl<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  fifo_map() = default;

  fifo_map(std::initializer_list<value_type> init)
  {
    for (auto& value : init) {
      insert_or_assign(std::move(value).first, std::move(value).second);
    }
  }

  fifo_map(const fifo_map& other)
  {
    for (const auto& it : other.queue_) {
      insert_or_assign(it->first, it->second);
    }
  }

  fifo_map& operator=(const fifo_map& other)
  {
    if (this != std::addressof(other)) {
      clear();
      for (const auto& it : other.queue_) {
        insert_or_assign(it->first, it->second);
      }
    }

    return *this;
  }

  fifo_map(fifo_map&&) = default;

  fifo_map& operator=(fifo_map&&) = default;

  optional<T&> get(const key_type& key) noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    }

    return nullopt;
  }

  optional<const T&> get(const key_type& key) const noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    }

    return nullopt;
  }

  optional<const T&> cget(const key_type& key) const noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    }

    return nullopt;
  }

  bool try_insert(const key_type& key, const mapped_type& value)
  {
    if (map_.find(key) != map_.end()) {
      return false;
    }

    queue_.push_back(map_.insert({ key, value }).first);
    return true;
  }

  bool try_assign(const key_type& key, const mapped_type& value)
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second = value;
      return true;
    }

    return false;
  }

  // return true if insertion took place
  bool insert_or_assign(const key_type& key, const mapped_type& value)
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second = value;
      return false;
    }

    queue_.push_back(map_.insert({ key, value }).first);
    return true;
  }

  void clear() noexcept
  {
    map_.clear();
    queue_.clear();
  }

  bool contains(const key_type& key) const noexcept
  {
    return map_.find(key) != std::end(map_);
  }

  size_type size() const noexcept
  {
    return map_.size();
  }

  size_type max_size() const noexcept
  {
    return std::numeric_limits<size_type>::max();
  }

  iterator begin()
  {
    return iterator(queue_.begin());
  }

  const_iterator begin() const
  {
    return const_iterator(queue_.begin());
  }

  const_iterator cbegin() const
  {
    return const_iterator(queue_.cbegin());
  }

  reverse_iterator rbegin()
  {
    return reverse_iterator(queue_.rbegin());
  }

  const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(queue_.rbegin());
  }

  const_reverse_iterator crbegin() const
  {
    return const_reverse_iterator(queue_.crbegin());
  }

  iterator end()
  {
    return iterator(queue_.end());
  }

  const_iterator end() const
  {
    return const_iterator(queue_.end());
  }

  const_iterator cend() const
  {
    return const_iterator(queue_.cend());
  }

  reverse_iterator rend()
  {
    return reverse_iterator(queue_.rend());
  }

  const_reverse_iterator rend() const
  {
    return const_reverse_iterator(queue_.rend());
  }

  const_reverse_iterator crend() const
  {
    return const_reverse_iterator(queue_.crend());
  }

private:
  map_type map_;
  queue_type queue_;
};

DS_NAMESPACE_END
