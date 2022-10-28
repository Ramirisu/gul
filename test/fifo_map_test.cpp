#include <ds_test.h>

#include <ds/fifo_map.hpp>
#include <initializer_list>

TEST_SUITE_BEGIN("fifo_map");

TEST_CASE("basic")
{
  ds::fifo_map<int, int> m;
  CHECK_EQ(m.size(), 0);
  CHECK(m.insert({ 1, 10 }));
  CHECK_EQ(m.size(), 1);
  CHECK_EQ(m.at(1), 10);
  CHECK_EQ(m[1], 10);
  CHECK(!m.insert({ 1, 10 }));
  CHECK(m.insert({ 2, 20 }));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.at(2), 20);
  CHECK_EQ(m[2], 20);
  CHECK(!m.insert({ 2, 20 }));
  CHECK(m.insert({ 3, 30 }));
  CHECK_EQ(m.size(), 3);
  CHECK_EQ(m.at(3), 30);
  CHECK_EQ(m[3], 30);
  CHECK(!m.insert({ 3, 30 }));
  CHECK(m.contains(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  m.clear();
  CHECK_EQ(m.size(), 0);
  CHECK(!m.contains(1));
  CHECK(!m.contains(2));
  CHECK(!m.contains(3));
}

TEST_CASE("insertion order")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  ds::fifo_map<int, int> m(init);
  CHECK_EQ(m.size(), 3);
  auto it = m.begin();
  for (auto init_it = init.begin(); it != m.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == m.end());
}

TEST_CASE("copy ctor")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  ds::fifo_map<int, int> m(init);
  ds::fifo_map<int, int> c(m);
  m.clear();
  CHECK_EQ(c.size(), 3);
  auto it = c.begin();
  for (auto init_it = init.begin(); it != c.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == c.end());
}

TEST_CASE("copy =")
{
  const std::initializer_list<std::pair<const int, int>> init { { 3, 30 },
                                                                { 1, 10 },
                                                                { 2, 20 } };
  ds::fifo_map<int, int> m(init);
  ds::fifo_map<int, int> c;
  c = m;
  m.clear();
  CHECK_EQ(c.size(), 3);
  auto it = c.begin();
  for (auto init_it = init.begin(); it != c.end(); ++it, ++init_it) {
    CHECK_EQ(*it, *init_it);
  }
  CHECK(it == c.end());
}

TEST_SUITE_END();
