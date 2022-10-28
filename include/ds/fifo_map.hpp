#pragma once

#include <cstddef>
#include <ds/config.hpp>

#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

namespace ds {
template <typename Key, typename T>
class fifo_map {
  using map_type = std::map<Key, T>;
  using queue_type = std::vector<typename map_type::iterator>;

public:
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename map_type::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;

    const_iterator(typename queue_type::const_iterator curr)
        : curr_(std::move(curr))
    {
    }

    const_iterator operator++(int)
    {
      auto it = *this;
      ++*this;
      return it;
    }

    const_iterator& operator++()
    {
      ++curr_;
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

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.curr_ == rhs.curr_;
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.curr_ != rhs.curr_;
    }

  private:
    typename queue_type::const_iterator curr_;
  };

  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  fifo_map() = default;

  fifo_map(std::initializer_list<value_type> init)
  {
    for (auto& value : init) {
      insert(std::move(value));
    }
  }

  fifo_map(const fifo_map& other)
  {
    for (const auto& iter : other.queue_) {
      insert(*iter);
    }
  }

  fifo_map& operator=(const fifo_map& other)
  {
    if (this != std::addressof(other)) {
      clear();
      for (const auto& iter : other.queue_) {
        insert(*iter);
      }
    }

    return *this;
  }

  fifo_map(fifo_map&&) = default;

  fifo_map& operator=(fifo_map&&) = default;

  bool insert(const value_type& value)
  {
    if (map_.find(value.first) != std::end(map_)) {
      return false;
    }

    auto res = map_.insert(value);
    queue_.push_back(res.first);
    return true;
  }

  void clear()
  {
    map_.clear();
    queue_.clear();
  }

  mapped_type& operator[](const key_type& key)
  {
    DS_ASSERT(contains(key));
    return map_[key];
  }

  mapped_type& at(const key_type& key)
  {
    DS_ASSERT(contains(key));
    return map_.at(key);
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

  const_iterator begin() const
  {
    return const_iterator(queue_.begin());
  }

  const_iterator end() const
  {
    return const_iterator(queue_.end());
  }

private:
  map_type map_;
  queue_type queue_;
};
}
