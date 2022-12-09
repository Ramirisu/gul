//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/invoke.hpp>

TEST_SUITE_BEGIN("invoke");

TEST_CASE("invoke")
{
  {
    auto f = []() { return 1; };
    CHECK_EQ(gul::invoke(f), 1);
  }
  {
    auto f = [](int i) { return i; };
    CHECK_EQ(gul::invoke(f, 1), 1);
  }
  {
    struct s {
      int f(int i)
      {
        return i;
      }
    };
    s obj;
    CHECK_EQ(gul::invoke(&s::f, obj, 1), 1);
  }
  {
    struct s {
      int f(int i)
      {
        return i;
      }
    };
    s obj;
    std::reference_wrapper<s> ref(obj);
    CHECK_EQ(gul::invoke(&s::f, ref, 1), 1);
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
    CHECK_EQ(gul::invoke(&s::f, ptr, 1), 1);
  }
  {
    struct s {
      int f = 1;
    };
    s obj;
    CHECK_EQ(gul::invoke(&s::f, obj), 1);
  }
  {
    struct s {
      int f = 1;
    };
    s obj;
    std::reference_wrapper<s> ref(obj);
    CHECK_EQ(gul::invoke(&s::f, ref), 1);
  }
  {
    struct s {
      int f = 1;
    };
    s obj;
    s* ptr = &obj;
    CHECK_EQ(gul::invoke(&s::f, ptr), 1);
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
  CHECK_EQ(gul::invoke_r<to>(f).i, 1);
}

TEST_SUITE_END();
