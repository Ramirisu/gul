//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/lru_map.hpp>

#include <functional>
#include <random>
#include <type_traits>

using namespace gul;

TEST_SUITE_BEGIN("lru_map");

TEST_CASE("basic")
{
  lru_map<int, int> m(2);
  CHECK_EQ(m.size(), 0);
  CHECK_EQ(m.capacity(), 2);
  CHECK(m.try_insert(1, 10));
  CHECK(!m.empty());
  CHECK_EQ(m.size(), 1);
  CHECK_EQ(m.peek(1), optional<int>(10));
  CHECK(!m.try_insert(1, 10));
  CHECK(m.try_insert(2, 20));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(2), optional<int>(20));
  CHECK(!m.try_insert(2, 20));
  CHECK(m.try_insert(3, 30));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(3), optional<int>(30));
  CHECK(!m.try_insert(3, 30));
  CHECK(!m.contains(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  CHECK(!m.erase(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  CHECK(m.erase(2));
  CHECK(!m.contains(2));
  CHECK(m.contains(3));
  m.clear();
  CHECK_EQ(m.size(), 0);
  CHECK(m.empty());
  CHECK(!m.contains(1));
  CHECK(!m.contains(2));
  CHECK(!m.contains(3));
  CHECK_EQ(
      m.max_size(),
      std::min(
          std::list<std::pair<int, int>>().max_size(),
          std::map<int, typename std::list<std::pair<int, int>>::iterator>()
              .max_size())
          / 2);
}

TEST_CASE("Compare")
{
  struct compare {
    bool operator()(int lhs, int rhs) const noexcept
    {
      return lhs > rhs;
    }
  };

  compare comp;
  lru_map<int, int, compare> m {
    10, { { 3, 10 }, { 2, 50 }, { 4, 30 }, { 1, 20 }, { 0, 0 } }, comp
  };
  int temp = 5;
  for (const auto& kv : m) {
    CHECK(kv.first < temp);
    temp = kv.first;
  }
}

TEST_CASE("range constructor")
{
  const std::vector<std::pair<int, int>> v(
      { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
  lru_map<int, int> m(2, v.begin(), v.end());
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(3), optional<int>(30));
  CHECK_EQ(m.peek(4), optional<int>(40));
}

TEST_CASE("copy constructor")
{
  lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  lru_map<int, int> c(m);
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), optional<int>(10));
  CHECK_EQ(c.peek(2), optional<int>(20));
}

TEST_CASE("move constructor")
{
  lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  lru_map<int, int> c(std::move(m));
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), optional<int>(10));
  CHECK_EQ(c.peek(2), optional<int>(20));
}

TEST_CASE("copy assignment operator")
{
  lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  lru_map<int, int> c(2, { { 3, 30 }, { 4, 40 } });
  c = m;
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), optional<int>(10));
  CHECK_EQ(c.peek(2), optional<int>(20));
}

TEST_CASE("move assignment operator")
{
  lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  lru_map<int, int> c(2, { { 3, 30 }, { 4, 40 } });
  c = std::move(m);
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), optional<int>(10));
  CHECK_EQ(c.peek(2), optional<int>(20));
}

TEST_CASE("peek|cpeek")
{
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    STATIC_ASSERT_SAME(decltype(m.peek(1)), optional<int&>);
    CHECK_EQ(m.peek(0), optional<int&>());
    int v = 10;
    CHECK_EQ(m.peek(1), optional<int&>(v));
    m.peek(1).value() = 100;
    v = 100;
    CHECK_EQ(m.peek(1), optional<int&>(v));
  }
  {
    const lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    STATIC_ASSERT_SAME(decltype(m.peek(1)), optional<const int&>);
    CHECK_EQ(m.peek(0), optional<int&>());
    int v = 10;
    CHECK_EQ(m.peek(1), optional<const int&>(v));
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    STATIC_ASSERT_SAME(decltype(m.cpeek(1)), optional<const int&>);
    CHECK_EQ(m.cpeek(0), optional<int&>());
    int v = 10;
    CHECK_EQ(m.cpeek(1), optional<const int&>(v));
  }
}

TEST_CASE("get|cget")
{
  lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
  STATIC_ASSERT_SAME(decltype(m.get(1)), optional<int&>);
  STATIC_ASSERT_SAME(decltype(m.cget(1)), optional<const int&>);
  {
    int val = 10;
    CHECK_EQ(m.get(1), optional<int&>(val));
    CHECK_NE(m.peek_lru(),
             optional<std::pair<int, int>>(std::pair<int, int> { 1, 10 }));
  }
  {
    int val = 20;
    CHECK_EQ(m.get(2), optional<int&>(val));
    CHECK_NE(m.peek_lru(),
             optional<std::pair<int, int>>(std::pair<int, int> { 2, 20 }));
  }
  {
    int val = 10;
    CHECK_EQ(m.cget(1), optional<int&>(val));
    CHECK_NE(m.peek_lru(),
             optional<std::pair<int, int>>(std::pair<int, int> { 1, 10 }));
  }
  {
    int val = 20;
    CHECK_EQ(m.cget(2), optional<int&>(val));
    CHECK_NE(m.peek_lru(),
             optional<std::pair<int, int>>(std::pair<int, int> { 2, 20 }));
  }
  {
    CHECK_EQ(m.get(0), optional<int&>());
    CHECK_EQ(m.cget(0), optional<int&>());
    m.clear();
    CHECK_EQ(m.peek_lru(), optional<std::pair<int, int>>());
  }
}

TEST_CASE("try_assign")
{
  lru_map<int, int> m(2, { { 1, 10 } });
  CHECK(m.try_assign(1, 100));
  CHECK_EQ(m.peek(1), optional<int>(100));
  CHECK(!m.try_assign(2, 20));
}

TEST_CASE("insert_or_assign")
{
  lru_map<int, int> m(2, { { 1, 10 } });
  CHECK(!m.insert_or_assign(1, 100));
  CHECK_EQ(m.peek(1), optional<int>(100));
  CHECK(m.insert_or_assign(2, 20));
  CHECK_EQ(m.peek(2), optional<int>(20));
}

TEST_CASE("erase iterator")
{
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    CHECK_EQ(m.erase(m.end()), m.end());
    CHECK_EQ(m.erase(m.cend()), m.cend());
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.begin(), 1);
    auto it_next = std::next(it, 1);
    CHECK_EQ(m.erase(it), it_next);
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.cbegin(), 1);
    auto it_next = std::next(it, 1);
    CHECK_EQ(m.erase(it), it_next);
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    CHECK_EQ(m.erase(m.begin(), m.begin()), m.begin());
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.begin(), 1);
    auto it_next = std::next(it, 2);
    CHECK_EQ(m.erase(it, it_next), it_next);
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    CHECK_EQ(m.erase(m.cbegin(), m.cbegin()), m.cbegin());
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.cbegin(), 1);
    auto it_next = std::next(it, 2);
    CHECK_EQ(m.erase(it, it_next), it_next);
  }
}

TEST_CASE("iterator")
{
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    m.get(3);
    m.get(1);
    m.get(4);
    m.get(2);
    auto it = m.begin();
    CHECK_EQ(*it++, std::pair<int, int> { 1, 10 });
    CHECK_EQ(*it++, std::pair<int, int> { 2, 20 });
    CHECK_EQ(*it++, std::pair<int, int> { 3, 30 });
    CHECK_EQ(*it++, std::pair<int, int> { 4, 40 });
    CHECK_EQ(it, m.end());
    it--;
    it--;
    it--;
    it--;
    CHECK_EQ(it, m.begin());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const lru_map<int, int> m(4,
                              { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), const std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.cbegin(); it != m.cend(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), const std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const lru_map<int, int> m(4,
                              { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), const std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.crbegin(); it != m.crend(); ++it) {
      STATIC_ASSERT_SAME(decltype(*it), const std::pair<int, int>&);
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
}

TEST_CASE("lower_bound|upper_bound|equal_range")
{
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(decltype(m.lower_bound(0)), decltype(m)::iterator);
    CHECK_EQ(m.lower_bound(0), std::next(m.begin(), 0));
    CHECK_EQ(m.lower_bound(1), std::next(m.begin(), 0));
    CHECK_EQ(m.lower_bound(2), std::next(m.begin(), 1));
    CHECK_EQ(m.lower_bound(3), std::next(m.begin(), 1));
    CHECK_EQ(m.lower_bound(4), std::next(m.begin(), 2));
    CHECK_EQ(m.lower_bound(5), std::next(m.begin(), 2));
    CHECK_EQ(m.lower_bound(6), m.end());
  }
  {
    const lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(decltype(m.lower_bound(0)), decltype(m)::const_iterator);
    CHECK_EQ(m.lower_bound(0), std::next(m.begin(), 0));
    CHECK_EQ(m.lower_bound(1), std::next(m.begin(), 0));
    CHECK_EQ(m.lower_bound(2), std::next(m.begin(), 1));
    CHECK_EQ(m.lower_bound(3), std::next(m.begin(), 1));
    CHECK_EQ(m.lower_bound(4), std::next(m.begin(), 2));
    CHECK_EQ(m.lower_bound(5), std::next(m.begin(), 2));
    CHECK_EQ(m.lower_bound(6), m.end());
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(decltype(m.upper_bound(0)), decltype(m)::iterator);
    CHECK_EQ(m.upper_bound(0), std::next(m.begin(), 0));
    CHECK_EQ(m.upper_bound(1), std::next(m.begin(), 1));
    CHECK_EQ(m.upper_bound(2), std::next(m.begin(), 1));
    CHECK_EQ(m.upper_bound(3), std::next(m.begin(), 2));
    CHECK_EQ(m.upper_bound(4), std::next(m.begin(), 2));
    CHECK_EQ(m.upper_bound(5), m.end());
    CHECK_EQ(m.upper_bound(6), m.end());
  }
  {
    const lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(decltype(m.upper_bound(0)), decltype(m)::const_iterator);
    CHECK_EQ(m.upper_bound(0), std::next(m.begin(), 0));
    CHECK_EQ(m.upper_bound(1), std::next(m.begin(), 1));
    CHECK_EQ(m.upper_bound(2), std::next(m.begin(), 1));
    CHECK_EQ(m.upper_bound(3), std::next(m.begin(), 2));
    CHECK_EQ(m.upper_bound(4), std::next(m.begin(), 2));
    CHECK_EQ(m.upper_bound(5), m.end());
    CHECK_EQ(m.upper_bound(6), m.end());
  }
  {
    lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(decltype(m.equal_range(0)),
                       std::pair<decltype(m)::iterator, decltype(m)::iterator>);
    CHECK_EQ(m.equal_range(0).first, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(0).second, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(1).first, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(1).second, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(2).first, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(2).second, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(3).first, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(3).second, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(4).first, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(4).second, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(5).first, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(5).second, m.end());
    CHECK_EQ(m.equal_range(6).first, m.end());
    CHECK_EQ(m.equal_range(6).second, m.end());
  }
  {
    const lru_map<int, int> m(4, { { 1, 10 }, { 3, 30 }, { 5, 50 } });
    STATIC_ASSERT_SAME(
        decltype(m.equal_range(0)),
        std::pair<decltype(m)::const_iterator, decltype(m)::const_iterator>);
    CHECK_EQ(m.equal_range(0).first, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(0).second, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(1).first, std::next(m.begin(), 0));
    CHECK_EQ(m.equal_range(1).second, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(2).first, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(2).second, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(3).first, std::next(m.begin(), 1));
    CHECK_EQ(m.equal_range(3).second, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(4).first, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(4).second, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(5).first, std::next(m.begin(), 2));
    CHECK_EQ(m.equal_range(5).second, m.end());
    CHECK_EQ(m.equal_range(6).first, m.end());
    CHECK_EQ(m.equal_range(6).second, m.end());
  }
}

TEST_CASE("random test")
{
  auto m = lru_map<int, int>(64);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<> dist(0, 96);
  for (int i = 0; i < 65536; ++i) {
    auto key = dist(gen);
    m.try_insert(key, i);
  }
}

TEST_CASE("key_comp|value_comp")
{
  lru_map<int, int> m(1);
  CHECK(m.key_comp()(0, 1));
  CHECK(m.value_comp()(typename lru_map<int, int>::value_type { 2, 4 },
                       typename lru_map<int, int>::value_type { 3, 1 }));
}

TEST_SUITE_END();
