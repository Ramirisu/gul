#pragma once

#include <ds/config.hpp>

#include <ds/type_traits.hpp>

#include <deque>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>

namespace ds {
template <typename Key, typename T, typename Compare = std::less<Key>>
class fifo_map {
  using value_type_impl = std::pair<Key, T>;
  using map_type = std::map<Key, T, Compare>;
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
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using value_compare = value_compare_impl;
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
}
