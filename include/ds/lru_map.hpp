#pragma once

#include <ds/config.hpp>

#include <initializer_list>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <utility>

namespace ds {
template <typename Key, typename T>
class lru_map {
  using list_type = std::list<std::pair<Key, T>>;
  using map_type = std::map<Key, typename list_type::iterator>;

public:
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename list_type::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    const_iterator(typename map_type::const_iterator curr)
        : curr_(std::move(curr))
    {
    }

    const_iterator operator++(int) noexcept
    {
      auto it = *this;
      ++*this;
      return it;
    }

    const_iterator& operator++() noexcept
    {
      ++curr_;
      return *this;
    }

    reference operator*() const noexcept
    {
      return *(curr_->second);
    }

    pointer operator->() const noexcept
    {
      return &*curr_->second;
    }

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

  private:
    typename map_type::const_iterator curr_;
  };

  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_Type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  lru_map(size_type capacity)
      : capacity_(capacity)
  {
  }

  lru_map(size_type capacity, std::initializer_list<value_type> init)
      : lru_map(capacity)
  {
    for (auto& value : init) {
      insert(std::move(value));
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

  bool insert(const value_type& value)
  {
    if (map_.find(value.first) != std::end(map_)) {
      return false;
    }

    if (recently_used_.size() == capacity_) {
      remove_least_recently_used();
    }

    recently_used_.push_front(value);
    auto it = recently_used_.begin();
    map_[it->first] = it;
    return true;
  }

  void clear()
  {
    recently_used_.clear();
    map_.clear();
  }

  mapped_type& operator[](const key_type& key)
  {
    DS_ASSERT(contains(key));
    update_recently_used(key);
    return map_[key]->second;
  }

  mapped_type& at(const key_type& key)
  {
    DS_ASSERT(contains(key));
    update_recently_used(key);
    return map_.at(key)->second;
  }

  bool contains(const key_type& key) const noexcept
  {
    return map_.find(key) != std::end(map_);
  }

  size_type capacity() const noexcept
  {
    return capacity_;
  }

  size_type size() const noexcept
  {
    return map_.size();
  }

  size_type max_size() const noexcept
  {
    return std::numeric_limits<size_type>::max();
  }

  const_iterator begin() const
  {
    return const_iterator(std::begin(map_));
  }

  const_iterator end() const
  {
    return const_iterator(std::end(map_));
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
    for (auto it = std::begin(recently_used_); it != std::end(recently_used_);
         ++it) {
      map_[it->first] = it;
    }
  }

  std::size_t capacity_;

  // most recently used <==> least recently used
  list_type recently_used_;

  map_type map_;
};
}
