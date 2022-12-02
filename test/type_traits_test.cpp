//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/type_traits.hpp>

using namespace gul::detail;
using namespace gul;

TEST_SUITE_BEGIN("type_traits");

namespace std {
struct std_swappable {
  friend void swap(std_swappable&, std_swappable&) { }
};
struct std_nothrow_swappable {
  friend void swap(std_nothrow_swappable&, std_nothrow_swappable&) noexcept { }
};
}

struct non_std_swappable {
  friend void swap(non_std_swappable&, non_std_swappable&) { }
  friend void swap(non_std_swappable&, int&) { }
};

struct non_std_nothrow_swappable {
  friend void swap(non_std_nothrow_swappable&,
                   non_std_nothrow_swappable&) noexcept
  {
  }
};

struct non_swappable {
#if !defined(GUL_CXX_COMPILER_GCC48)
  friend void swap(non_swappable&, non_swappable&) = delete;
#endif
};

struct nothrow_non_swappable {
#if !defined(GUL_CXX_COMPILER_GCC48)
  friend void swap(nothrow_non_swappable&, nothrow_non_swappable&) noexcept
      = delete;
#endif
};

TEST_CASE("is_swappable_with")
{
  STATIC_ASSERT(!is_swappable_with<int, int>::value);
  STATIC_ASSERT(is_swappable_with<int&, int&>::value);
  STATIC_ASSERT(
      is_swappable_with<std::std_swappable&, std::std_swappable&>::value);
  STATIC_ASSERT(
      is_swappable_with<non_std_swappable&, non_std_swappable&>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
  STATIC_ASSERT(!is_swappable_with<non_swappable&, non_swappable&>::value);
#endif

  STATIC_ASSERT(!is_swappable_with<int&, double&>::value);
}

TEST_CASE("is_swappable")
{
  STATIC_ASSERT(is_swappable<int>::value);
  STATIC_ASSERT(is_swappable<std::std_swappable>::value);
  STATIC_ASSERT(is_swappable<non_std_swappable>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
  STATIC_ASSERT(!is_swappable<non_swappable>::value);
#endif
}

TEST_CASE("is_nothrow_swappable_with")
{
  STATIC_ASSERT(!is_nothrow_swappable_with<int, int>::value);
  STATIC_ASSERT(is_nothrow_swappable_with<int&, int&>::value);
  STATIC_ASSERT(!is_nothrow_swappable_with<std::std_swappable&,
                                           std::std_swappable&>::value);
  STATIC_ASSERT(!is_nothrow_swappable_with<non_std_swappable&,
                                           non_std_swappable&>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
  STATIC_ASSERT(
      !is_nothrow_swappable_with<non_swappable&, non_swappable&>::value);
#endif
  STATIC_ASSERT(is_nothrow_swappable_with<std::std_nothrow_swappable&,
                                          std::std_nothrow_swappable&>::value);
  STATIC_ASSERT(is_nothrow_swappable_with<non_std_nothrow_swappable&,
                                          non_std_nothrow_swappable&>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
  STATIC_ASSERT(!is_nothrow_swappable_with<nothrow_non_swappable&,
                                           nothrow_non_swappable&>::value);
#endif
  STATIC_ASSERT(!is_nothrow_swappable_with<non_std_swappable&, int&>::value);
}

TEST_CASE("is_nothrow_swappable")
{
  STATIC_ASSERT(is_nothrow_swappable<int>::value);
  STATIC_ASSERT(!is_nothrow_swappable<std::std_swappable>::value);
  STATIC_ASSERT(!is_nothrow_swappable<non_std_swappable>::value);
  STATIC_ASSERT(is_nothrow_swappable<std::std_nothrow_swappable>::value);
  STATIC_ASSERT(is_nothrow_swappable<non_std_nothrow_swappable>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
  STATIC_ASSERT(!is_nothrow_swappable<non_swappable>::value);
  STATIC_ASSERT(!is_nothrow_swappable<nothrow_non_swappable>::value);
#endif
}

TEST_CASE("is_nothrow_convertible")
{
  STATIC_ASSERT(is_nothrow_convertible<void, void>::value);
  STATIC_ASSERT(!is_nothrow_convertible<int, void>::value);
  STATIC_ASSERT(!is_nothrow_convertible<void, int>::value);
  STATIC_ASSERT(is_nothrow_convertible<int, int>::value);
  struct to { };
  struct to_nothrow { };
  struct from {
    operator to()
    {
      return to();
    }
    operator to_nothrow() noexcept
    {
      return to_nothrow();
    }
  };
  STATIC_ASSERT(!is_nothrow_convertible<from, to>::value);
  STATIC_ASSERT(is_nothrow_convertible<from, to_nothrow>::value);
}

TEST_CASE("is_invocable")
{
  {
    auto f = []() {};
    auto fi = [](int) {};
    auto fn = []() noexcept {};
    auto fin = [](int) noexcept {};
    STATIC_ASSERT(is_invocable<decltype(f)>::value);
    STATIC_ASSERT(!is_invocable<decltype(f), int>::value);
    STATIC_ASSERT(!is_invocable<decltype(fi)>::value);
    STATIC_ASSERT(is_invocable<decltype(fi), int>::value);
    STATIC_ASSERT(is_invocable<decltype(fn)>::value);
    STATIC_ASSERT(!is_invocable<decltype(fn), int>::value);
    STATIC_ASSERT(!is_invocable<decltype(fin)>::value);
    STATIC_ASSERT(is_invocable<decltype(fin), int>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable<decltype(f)>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(f), int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(fi)>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(fi), int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(fn)>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(fn), int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(fin)>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(fin), int>::value);
#endif
    auto f_ri = []() { return 1; };
    auto fn_ri = []() noexcept { return 1; };
    STATIC_ASSERT(is_invocable_r<void, decltype(f)>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(f)>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(fn)>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(fn)>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(f_ri)>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(f_ri)>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(fn_ri)>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(fn_ri)>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(f)>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(f)>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(fn)>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(fn)>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(f_ri)>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(f_ri)>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(fn_ri)>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, decltype(fn_ri)>::value);
#endif
  }
  {
    struct f {
      void operator()() { }
    };
    struct fi {
      void operator()(int) { }
    };
    struct fn {
      void operator()() noexcept { }
    };
    struct fin {
      void operator()(int) noexcept { }
    };
    STATIC_ASSERT(is_invocable<f>::value);
    STATIC_ASSERT(!is_invocable<f, int>::value);
    STATIC_ASSERT(!is_invocable<fi>::value);
    STATIC_ASSERT(is_invocable<fi, int>::value);
    STATIC_ASSERT(is_invocable<fn>::value);
    STATIC_ASSERT(!is_invocable<fn, int>::value);
    STATIC_ASSERT(!is_invocable<fin>::value);
    STATIC_ASSERT(is_invocable<fin, int>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable<f>::value);
    STATIC_ASSERT(!is_nothrow_invocable<f, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<fi>::value);
    STATIC_ASSERT(!is_nothrow_invocable<fi, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<fn>::value);
    STATIC_ASSERT(!is_nothrow_invocable<fn, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<fin>::value);
    STATIC_ASSERT(is_nothrow_invocable<fin, int>::value);
#endif
    struct f_ri {
      int operator()()
      {
        return 0;
      }
    };
    struct fn_ri {
      int operator()() noexcept
      {
        return 0;
      }
    };
    STATIC_ASSERT(is_invocable_r<void, f>::value);
    STATIC_ASSERT(!is_invocable_r<int, f>::value);
    STATIC_ASSERT(is_invocable_r<void, fn>::value);
    STATIC_ASSERT(!is_invocable_r<int, fn>::value);
    STATIC_ASSERT(is_invocable_r<void, f_ri>::value);
    STATIC_ASSERT(is_invocable_r<int, f_ri>::value);
    STATIC_ASSERT(is_invocable_r<void, fn_ri>::value);
    STATIC_ASSERT(is_invocable_r<int, fn_ri>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable_r<void, f>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, f>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, fn>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, fn>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, f_ri>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, f_ri>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, fn_ri>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, fn_ri>::value);
#endif
  }
  {
    struct s {
      void f() { }
      void fc() const { }
      void fi(int) { }
      void fic(int) const { }
      void fn() noexcept { }
      void fcn() const noexcept { }
      void fin(int) noexcept { }
      void ficn(int) const noexcept { }
      int f_ri()
      {
        return 0;
      }
      int fc_ri() const
      {
        return 0;
      }
      int fn_ri() noexcept
      {
        return 0;
      }
      int fcn_ri() const noexcept
      {
        return 0;
      }
      int o = 0;
      const int oc = 0;
    };
    STATIC_ASSERT(is_invocable<decltype(&s::f), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::f), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fc), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fc), s, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fi), s>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fi), s, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fic), s>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fic), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::f), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::f), s*, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fc), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fc), s*, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fi), s*>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fi), s*, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fic), s*>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fic), s*, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fn), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fn), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fcn), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fcn), s, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fin), s>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fin), s, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::ficn), s>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::ficn), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fn), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fn), s*, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fcn), s*, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::fin), s*>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::fin), s*, int>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::ficn), s*>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::ficn), s*, int>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::f), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::f), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fc), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fc), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fi), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fi), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fic), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fic), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::f), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::f), s*, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fc), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fc), s*, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fi), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fi), s*, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fic), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fic), s*, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fn), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fn), s, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fcn), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fcn), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fin), s>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fin), s, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::ficn), s>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::ficn), s, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fn), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fn), s*, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fcn), s*, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::fin), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::fin), s*, int>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::ficn), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::ficn), s*, int>::value);
#endif
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::f), s>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::f), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fc), s>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fc), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::f), s*>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::f), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fc), s*>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fc), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fn), s>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fn), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fcn), s>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fcn), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fn), s*>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fn), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(!is_invocable_r<int, decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::f_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::f_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fc_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fc_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::f_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::f_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fc_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fc_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fn_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fn_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fcn_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fcn_ri), s>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fn_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fn_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<void, decltype(&s::fcn_ri), s*>::value);
    STATIC_ASSERT(is_invocable_r<int, decltype(&s::fcn_ri), s*>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::f), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::f), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::fc), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fc), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::f), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::f), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::fc), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fc), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fn), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fn), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fcn), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fcn), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fn), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fn), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fcn), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::f_ri), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::f_ri), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::fc_ri), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fc_ri), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<void, decltype(&s::f_ri), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::f_ri), s*>::value);
    STATIC_ASSERT(
        !is_nothrow_invocable_r<void, decltype(&s::fc_ri), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable_r<int, decltype(&s::fc_ri), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fn_ri), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, decltype(&s::fn_ri), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fcn_ri), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, decltype(&s::fcn_ri), s>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<void, decltype(&s::fn_ri), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, decltype(&s::fn_ri), s*>::value);
    STATIC_ASSERT(
        is_nothrow_invocable_r<void, decltype(&s::fcn_ri), s*>::value);
    STATIC_ASSERT(is_nothrow_invocable_r<int, decltype(&s::fcn_ri), s*>::value);
#endif

    STATIC_ASSERT(is_invocable<decltype(&s::o), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::o), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::oc), s>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::oc), s, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::o), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::o), s*, int>::value);
    STATIC_ASSERT(is_invocable<decltype(&s::oc), s*>::value);
    STATIC_ASSERT(!is_invocable<decltype(&s::oc), s*, int>::value);
#ifdef GUL_HAS_CXX17
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::o), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::o), s, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::oc), s>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::oc), s, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::o), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::o), s*, int>::value);
    STATIC_ASSERT(is_nothrow_invocable<decltype(&s::oc), s*>::value);
    STATIC_ASSERT(!is_nothrow_invocable<decltype(&s::oc), s*, int>::value);
#endif
  }
}

TEST_CASE("is_null_pointer")
{
  STATIC_ASSERT(!is_null_pointer<int>::value);
  STATIC_ASSERT(is_null_pointer<std::nullptr_t>::value);
}

TEST_CASE("is_bounded_array")
{
  STATIC_ASSERT(!is_bounded_array<int>::value);
  STATIC_ASSERT(is_bounded_array<int[10]>::value);
  STATIC_ASSERT(!is_bounded_array<int[]>::value);
}

TEST_CASE("is_unbounded_array")
{
  STATIC_ASSERT(!is_unbounded_array<int>::value);
  STATIC_ASSERT(!is_unbounded_array<int[10]>::value);
  STATIC_ASSERT(is_unbounded_array<int[]>::value);
}

TEST_CASE("is_scoped_enum")
{
  class c { };
  enum e {};
  enum class ec {};
  STATIC_ASSERT(!is_scoped_enum<int>::value);
  STATIC_ASSERT(!is_scoped_enum<c>::value);
  STATIC_ASSERT(!is_scoped_enum<e>::value);
  STATIC_ASSERT(is_scoped_enum<ec>::value);
}

namespace {
void f() { }
}

TEST_CASE("function_traits")
{
  auto l = []() {};
  void (*fptr)();
  fptr = f;
  struct c {
    void f() { }
    void fc() const { }
    void fl() & { }
    void fcl() const& { }
    void fr() && { }
    void fcr() const&& { }
    void fn() noexcept { }
    void fln() & noexcept { }
    void fcln() const& noexcept { }
    void frn() && noexcept { }
    void fcrn() const&& noexcept { }
    void operator()() { }
    double unary(int)
    {
      return 0;
    }
    int binary(char, long)
    {
      return 0;
    }
    long ternary(short, int, char)
    {
      return 0;
    }
  };
  static_assert_same<function_traits<decltype(l)>::result_type, void>();
  static_assert_same<function_traits<decltype(f)>::result_type, void>();
  static_assert_same<function_traits<decltype(fptr)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::f)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fc)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fl)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fcl)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fr)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fcr)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fn)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fln)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fcln)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::frn)>::result_type, void>();
  static_assert_same<function_traits<decltype(&c::fcrn)>::result_type, void>();
  static_assert_same<function_traits<c>::result_type, void>();

  static_assert_same<function_traits<decltype(&c::unary)>::result_type,
                     double>();
  STATIC_ASSERT(function_traits<decltype(&c::unary)>::arity == 1);
  static_assert_same<
      function_traits<decltype(&c::unary)>::template arg<0>::type, int>();
  static_assert_same<function_traits<decltype(&c::binary)>::result_type, int>();
  STATIC_ASSERT(function_traits<decltype(&c::binary)>::arity == 2);
  static_assert_same<
      function_traits<decltype(&c::binary)>::template arg<0>::type, char>();
  static_assert_same<
      function_traits<decltype(&c::binary)>::template arg<1>::type, long>();
  static_assert_same<function_traits<decltype(&c::ternary)>::result_type,
                     long>();
  STATIC_ASSERT(function_traits<decltype(&c::ternary)>::arity == 3);
  static_assert_same<
      function_traits<decltype(&c::ternary)>::template arg<0>::type, short>();
  static_assert_same<
      function_traits<decltype(&c::ternary)>::template arg<1>::type, int>();
  static_assert_same<
      function_traits<decltype(&c::ternary)>::template arg<2>::type, char>();
}

TEST_SUITE_END();
