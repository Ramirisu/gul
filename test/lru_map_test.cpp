#include <ds_test.h>

#include <ds/lru_map.hpp>

#include <random>

TEST_SUITE_BEGIN("lru_map");

TEST_CASE("basic")
{
  ds::lru_map<int, int> m(2);
  CHECK_EQ(m.size(), 0);
  CHECK_EQ(m.capacity(), 2);
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
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.at(3), 30);
  CHECK_EQ(m[3], 30);
  CHECK(!m.insert({ 3, 30 }));
  CHECK(!m.contains(1));
  m.clear();
  CHECK_EQ(m.size(), 0);
  CHECK(!m.contains(2));
  CHECK(!m.contains(3));
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
  std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
  const ds::lru_map<int, int> m(4,
                                { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
  for (auto it = m.begin(); it != m.end(); ++it) {
    CHECK_EQ(exp.at(it->first), it->second);
    exp.erase(it->first);
  }
  CHECK(exp.empty());
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
