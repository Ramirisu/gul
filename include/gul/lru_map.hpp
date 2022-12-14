//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/optional.hpp>

#include <initializer_list>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>

GUL_NAMESPACE_BEGIN

template <typename Key, typename T, typename Compare = std::less<Key>>
class lru_map {
  using value_type_impl = std::pair<Key, T>;
  using list_type = std::list<std::pair<Key, T>>;
  using map_type = std::map<Key, typename list_type::iterator, Compare>;

  template <bool Const>
  class iterator_impl {
    friend class lru_map;

    using parent_iterator = conditional_t<Const,
                                          typename map_type::const_iterator,
                                          typename map_type::iterator>;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = conditional_t<Const,
                                     const typename list_type::value_type,
                                     typename list_type::value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    iterator_impl(parent_iterator curr)
        : curr_(std::move(curr))
    {
    }

    iterator_impl operator++(int) noexcept
    {
      auto it = *this;
      ++*this;
      return it;
    }

    iterator_impl& operator++() noexcept
    {
      ++curr_;
      return *this;
    }

    iterator_impl operator--(int) noexcept
    {
      auto it = *this;
      --*this;
      return it;
    }

    iterator_impl& operator--() noexcept
    {
      --curr_;
      return *this;
    }

    reference operator*() const noexcept
    {
      return *curr_->second;
    }

    pointer operator->() const noexcept
    {
      return &*curr_->second;
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

  class value_compare_impl {
  public:
    value_compare_impl(Compare compare)
        : compare_(std::move(compare))
    {
    }

    bool operator()(const value_type_impl& lhs,
                    const value_type_impl& rhs) const
    {
      return compare_(lhs.first, rhs.first);
    }

  private:
    Compare compare_;
  };

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = value_type_impl;
  using size_type = std::size_t;
  using difference_Type = std::ptrdiff_t;
  using key_compare = Compare;
  using value_compare = value_compare_impl;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = iterator_impl<false>;
  using const_iterator = iterator_impl<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  lru_map(size_type capacity)
      : capacity_(capacity)
  {
    GUL_ASSERT(capacity > 0);
  }

  lru_map(size_type capacity, std::initializer_list<value_type> init)
      : capacity_(capacity)
  {
    GUL_ASSERT(capacity > 0);
    for (auto& value : init) {
      insert_or_assign(std::move(value.first), std::move(value.second));
    }
  }

  lru_map(size_type capacity,
          std::initializer_list<value_type> init,
          Compare comp)
      : capacity_(capacity)
      , map_(std::move(comp))
  {
    GUL_ASSERT(capacity > 0);
    for (auto& value : init) {
      insert_or_assign(std::move(value.first), std::move(value.second));
    }
  }

  template <typename InputIt>
  lru_map(size_type capacity, InputIt first, InputIt last)
      : capacity_(capacity)
  {
    GUL_ASSERT(capacity > 0);
    for (auto it = first; it != last; ++it) {
      using std::get;
      insert_or_assign(get<0>(*it), get<1>(*it));
    }
  }

  lru_map(const lru_map& other)
      : capacity_(other.capacity_)
      , recently_used_(other.recently_used_)
  {
    rebuild_map();
  }

  lru_map& operator=(const lru_map& other)
  {
    if (this != std::addressof(other)) {
      capacity_ = other.capacity_;
      recently_used_ = other.recently_used_;
      rebuild_map();
    }

    return *this;
  }

  lru_map(lru_map&&) = default;

  lru_map& operator=(lru_map&&) = default;

  optional<value_type> peek_lru() const noexcept
  {
    if (!recently_used_.empty()) {
      return recently_used_.back();
    }

    return nullopt;
  }

  optional<mapped_type&> peek(const key_type& key) noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second->second;
    }

    return nullopt;
  }

  optional<const mapped_type&> peek(const key_type& key) const noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second->second;
    }

    return nullopt;
  }

  optional<const mapped_type&> cpeek(const key_type& key) const noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second->second;
    }

    return nullopt;
  }

  optional<mapped_type&> get(const key_type& key) noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      update_recently_used(key);
      return it->second->second;
    }

    return nullopt;
  }

  optional<const mapped_type&> cget(const key_type& key) noexcept
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      update_recently_used(key);
      return it->second->second;
    }

    return nullopt;
  }

  bool try_insert(const key_type& key, const mapped_type& value)
  {
    if (map_.find(key) != map_.end()) {
      return false;
    }

    if (recently_used_.size() == capacity_) {
      remove_least_recently_used();
    }

    recently_used_.push_front({ key, value });
    map_[key] = recently_used_.begin();
    return true;
  }

  bool try_assign(const key_type& key, const mapped_type& value)
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second->second = value;
      update_recently_used(key);
      return true;
    }

    return false;
  }

  // return true if insertion took place
  bool insert_or_assign(const key_type& key, const mapped_type& value)
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second->second = value;
      update_recently_used(key);
      return false;
    }

    if (recently_used_.size() == capacity_) {
      remove_least_recently_used();
    }

    recently_used_.push_front({ key, value });
    map_[key] = recently_used_.begin();
    return true;
  }

  iterator lower_bound(const key_type& key)
  {
    return iterator(map_.lower_bound(key));
  }

  const_iterator lower_bound(const key_type& key) const
  {
    return const_iterator(map_.lower_bound(key));
  }

  iterator upper_bound(const key_type& key)
  {
    return iterator(map_.upper_bound(key));
  }

  const_iterator upper_bound(const key_type& key) const
  {
    return const_iterator(map_.upper_bound(key));
  }

  std::pair<iterator, iterator> equal_range(const key_type& key)
  {
    auto pair = map_.equal_range(key);
    return std::pair<iterator, iterator> { iterator(std::move(pair.first)),
                                           iterator(std::move(pair.second)) };
  }

  std::pair<const_iterator, const_iterator>
  equal_range(const key_type& key) const
  {
    auto pair = map_.equal_range(key);
    return std::pair<const_iterator, const_iterator> {
      const_iterator(std::move(pair.first)),
      const_iterator(std::move(pair.second))
    };
  }

  bool erase(const key_type& key)
  {
    auto it = map_.find(key);
    if (it != map_.end()) {
      recently_used_.erase(it->second);
      map_.erase(it);
      return true;
    }

    return false;
  }

  iterator erase(iterator pos)
  {
    if (pos == end()) {
      return pos;
    }

    recently_used_.erase(pos.curr_->second);
    return iterator(map_.erase(pos.curr_));
  }

  const_iterator erase(const_iterator pos)
  {
    if (pos == cend()) {
      return pos;
    }

    recently_used_.erase(pos.curr_->second);
    return const_iterator(map_.erase(pos.curr_));
  }

  iterator erase(iterator first, iterator last)
  {
    if (first == last) {
      return first;
    }

    for (auto pos = first.curr_; pos != last.curr_;) {
      recently_used_.erase(pos->second);
      pos = map_.erase(pos);
    }

    return last;
  }

  const_iterator erase(const_iterator first, const_iterator last)
  {
    if (first == last) {
      return first;
    }

    for (auto pos = first.curr_; pos != last.curr_;) {
      recently_used_.erase(pos->second);
      pos = map_.erase(pos);
    }

    return last;
  }

  void clear() noexcept
  {
    recently_used_.clear();
    map_.clear();
  }

  bool contains(const key_type& key) const noexcept
  {
    return map_.find(key) != map_.end();
  }

  size_type capacity() const noexcept
  {
    return capacity_;
  }

  bool empty() const noexcept
  {
    return map_.empty();
  }

  size_type size() const noexcept
  {
    return map_.size();
  }

  size_type max_size() const noexcept
  {
    return std::min(recently_used_.max_size(), map_.max_size()) / 2;
  }

  iterator begin()
  {
    return iterator(map_.begin());
  }

  const_iterator begin() const
  {
    return const_iterator(map_.begin());
  }

  const_iterator cbegin() const
  {
    return const_iterator(map_.cbegin());
  }

  reverse_iterator rbegin()
  {
    return reverse_iterator(map_.rbegin());
  }

  const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(map_.rbegin());
  }

  const_reverse_iterator crbegin() const
  {
    return const_reverse_iterator(map_.crbegin());
  }

  iterator end()
  {
    return iterator(map_.end());
  }

  const_iterator end() const
  {
    return const_iterator(map_.end());
  }

  const_iterator cend() const
  {
    return const_iterator(map_.cend());
  }

  reverse_iterator rend()
  {
    return reverse_iterator(map_.rend());
  }

  const_reverse_iterator rend() const
  {
    return const_reverse_iterator(map_.rend());
  }

  const_reverse_iterator crend() const
  {
    return const_reverse_iterator(map_.crend());
  }

  key_compare key_comp() const
  {
    return map_.key_comp();
  }

  value_compare value_comp() const
  {
    return value_compare(key_comp());
  }

private:
  void remove_least_recently_used()
  {
    const auto& key = recently_used_.back().first;
    map_.erase(key);
    recently_used_.pop_back();
  }

  void update_recently_used(const key_type& key)
  {
    auto& it = map_[key];
    auto kv = std::move(*it);
    recently_used_.erase(it);
    recently_used_.push_front(std::move(kv));
    it = recently_used_.begin();
  }

  void rebuild_map()
  {
    map_.clear();
    for (auto it = recently_used_.begin(); it != recently_used_.end(); ++it) {
      map_[it->first] = it;
    }
  }

  std::size_t capacity_;

  // most recently used <==> least recently used
  list_type recently_used_;

  map_type map_;
};

GUL_NAMESPACE_END
