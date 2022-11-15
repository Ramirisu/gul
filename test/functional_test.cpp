#include <ds_test.h>

#include <ds/functional.hpp>

using namespace ds;

TEST_SUITE_BEGIN("functional");

TEST_CASE("invoke")
{
  {
    auto f = []() { return 1; };
    CHECK_EQ(invoke(f), 1);
  }
  {
    auto f = [](int i) { return i; };
    CHECK_EQ(invoke(f, 1), 1);
  }
  {
    struct s {
      int f(int i)
      {
        return i;
      }
    };
    s obj;
    CHECK_EQ(invoke(&s::f, obj, 1), 1);
  }
  {
    struct s {
      int f(int i)
      {
        return i;
      }
    };
    s obj;
    s* ptr = &obj;
    CHECK_EQ(invoke(&s::f, ptr, 1), 1);
  }
  {
    struct s {
      int f = 1;
    };
    s obj;
    CHECK_EQ(invoke(&s::f, obj), 1);
  }
  {
    struct s {
      int f = 1;
    };
    s obj;
    s* ptr = &obj;
    CHECK_EQ(invoke(&s::f, ptr), 1);
  }
}

TEST_CASE("invoke_r")
{
  struct to {
    int i;
  };
  struct from {
    operator to()
    {
      return to { 1 };
    }
  };
  auto f = []() { return from {}; };
  CHECK_EQ(invoke_r<to>(f).i, 1);
}

TEST_SUITE_END();