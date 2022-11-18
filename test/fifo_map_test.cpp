//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/fifo_map.hpp>

using namespace gul;

TEST_SUITE_BEGIN("fifo_map");

TEST_CASE("basic")
{
  fifo_map<int, int> m;
  CHECK_EQ(m.size(), 0);
  CHECK(m.try_insert(1, 10));
  CHECK_EQ(m.size(), 1);
  CHECK_EQ(m.get(1), 10);
  CHECK(!m.try_insert(1, 10));
  CHECK(m.try_insert(2, 20));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.get(2), 20);
  CHECK(!m.try_insert(2, 20));
  CHECK(m.try_insert(3, 30));
  CHECK_EQ(m.size(), 3);
  CHECK_EQ(m.get(3), 30);
  CHECK(!m.try_insert(3, 30));
  CHECK(m.contains(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  m.clear();
  CHECK_EQ(m.size(), 0);
  CHECK(!m.contains(1));
  CHECK(!m.contains(2));
  CHECK(!m.contains(3));
}

TEST_CASE("copy ctor")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  fifo_map<int, int> m(init);
  fifo_map<int, int> c(m);
  m.clear();
  CHECK_EQ(c.size(), 3);
  auto it = c.begin();
  for (auto init_it = init.begin(); it != c.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == c.end());
}

TEST_CASE("copy assigment operator")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  fifo_map<int, int> m(init);
  fifo_map<int, int> c;
  c = m;
  m.clear();
  CHECK_EQ(c.size(), 3);
  auto it = c.begin();
  for (auto init_it = init.begin(); it != c.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == c.end());
}

TEST_CASE("get|cget")
{
  {
    fifo_map<int, int> m({ { 1, 10 }, { 2, 20 } });
    static_assert_same<decltype(m.get(1)), optional<int&>>();
    static_assert_same<decltype(m.cget(1)), optional<const int&>>();
    CHECK_EQ(m.get(1), 10);
    CHECK_EQ(m.cget(1), 10);
    m.get(1).value() = 100;
    CHECK_EQ(m.get(1), 100);
  }
  {
    const fifo_map<int, int> m({ { 1, 10 }, { 2, 20 } });
    static_assert_same<decltype(m.get(1)), optional<const int&>>();
    static_assert_same<decltype(m.cget(1)), optional<const int&>>();
    CHECK_EQ(m.get(1), 10);
    CHECK_EQ(m.cget(1), 10);
  }
}

TEST_CASE("try_assign")
{
  fifo_map<int, int> m({ { 1, 10 }, { 2, 20 } });
  CHECK(!m.try_assign(3, 30));
  CHECK(m.try_assign(1, 100));
  CHECK_EQ(m.get(1), 100);
}

TEST_CASE("insert_or_assign")
{
  fifo_map<int, int> m({ { 1, 10 }, { 2, 20 } });
  CHECK(m.insert_or_assign(3, 30));
  CHECK_EQ(m.get(3), 30);
  CHECK(!m.insert_or_assign(1, 100));
  CHECK_EQ(m.get(1), 100);
}

TEST_CASE("insertion order")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  fifo_map<int, int> m(init);
  CHECK_EQ(m.size(), 3);
  auto it = m.begin();
  for (auto init_it = init.begin(); it != m.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == m.end());
}

TEST_CASE("iterator dereferece type")
{
  fifo_map<char, int> m;
  static_assert_same<decltype(*m.begin()), std::pair<const char, int>&>();
  static_assert_same<decltype(*m.cbegin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*m.rbegin()), std::pair<const char, int>&>();
  static_assert_same<decltype(*m.crbegin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*m.end()), std::pair<const char, int>&>();
  static_assert_same<decltype(*m.cend()), const std::pair<const char, int>&>();
  static_assert_same<decltype(*m.rend()), std::pair<const char, int>&>();
  static_assert_same<decltype(*m.crend()), const std::pair<const char, int>&>();

  const fifo_map<char, int> cm;
  static_assert_same<decltype(*cm.begin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.cbegin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.rbegin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.crbegin()),
                     const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.end()), const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.cend()), const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.rend()), const std::pair<const char, int>&>();
  static_assert_same<decltype(*cm.crend()),
                     const std::pair<const char, int>&>();
}

TEST_SUITE_END();
