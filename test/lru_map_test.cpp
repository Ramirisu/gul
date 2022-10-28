#include <ds_test.h>

#include <ds/lru_map.hpp>

#include <functional>
#include <random>
#include <type_traits>

TEST_SUITE_BEGIN("lru_map");

TEST_CASE("basic")
{
  ds::lru_map<int, int> m(2);
  CHECK_EQ(m.size(), 0);
  CHECK_EQ(m.capacity(), 2);
  CHECK(m.insert({ 1, 10 }));
  CHECK(!m.empty());
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
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.at(3), 30);
  CHECK_EQ(m[3], 30);
  CHECK(!m.insert({ 3, 30 }));
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
  ds::lru_map<int, int, compare> m {
    10, { { 3, 10 }, { 2, 50 }, { 4, 30 }, { 1, 20 }, { 0, 0 } }, comp
  };
  int temp = 5;
  for (const auto& kv : m) {
    CHECK(kv.first < temp);
    temp = kv.first;
  }
}

TEST_CASE("range ctor")
{
  const std::vector<std::pair<int, int>> v(
      { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
  ds::lru_map<int, int> m(2, v.begin(), v.end());
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.at(3), 30);
  CHECK_EQ(m.at(4), 40);
}

TEST_CASE("copy ctor")
{
  ds::lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  ds::lru_map<int, int> c(m);
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.at(1), 10);
  CHECK_EQ(c.at(2), 20);
}

TEST_CASE("copy =")
{
  ds::lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  ds::lru_map<int, int> c(2, { { 3, 30 }, { 4, 40 } });
  c = m;
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.at(1), 10);
  CHECK_EQ(c.at(2), 20);
}

TEST_CASE("iterator")
{
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      static_assert_same<decltype(*it), std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const ds::lru_map<int, int> m(
        4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.cbegin(); it != m.cend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      static_assert_same<decltype(*it), std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const ds::lru_map<int, int> m(
        4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.crbegin(); it != m.crend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&> {};
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
}

TEST_CASE("random test")
{
  auto m = ds::lru_map<int, int>(64);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<> dist(0, 96);
  for (int i = 0; i < 65536; ++i) {
    auto key = dist(gen);
    m.insert({ key, i });
  }
}

TEST_SUITE_END();
