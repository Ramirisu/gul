//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/type_traits.hpp>

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
  friend void swap(non_swappable&, non_swappable&) = delete;
};

struct nothrow_non_swappable {
  friend void swap(nothrow_non_swappable&, nothrow_non_swappable&) noexcept
      = delete;
};

TEST_CASE("is_swappable_with")
{
  static_assert(!is_swappable_with<int, int>::value, "");
  static_assert(is_swappable_with<int&, int&>::value, "");
  static_assert(
      is_swappable_with<std::std_swappable&, std::std_swappable&>::value, "");
  static_assert(
      is_swappable_with<non_std_swappable&, non_std_swappable&>::value, "");
  static_assert(!is_swappable_with<non_swappable&, non_swappable&>::value, "");

  static_assert(!is_swappable_with<int&, double&>::value, "");
}

TEST_CASE("is_swappable")
{
  static_assert(is_swappable<int>::value, "");
  static_assert(is_swappable<std::std_swappable>::value, "");
  static_assert(is_swappable<non_std_swappable>::value, "");
  static_assert(!is_swappable<non_swappable>::value, "");
}

TEST_CASE("is_nothrow_swappable_with")
{
  static_assert(!is_nothrow_swappable_with<int, int>::value, "");
  static_assert(is_nothrow_swappable_with<int&, int&>::value, "");
  static_assert(!is_nothrow_swappable_with<std::std_swappable&,
                                           std::std_swappable&>::value,
                "");
  static_assert(
      !is_nothrow_swappable_with<non_std_swappable&, non_std_swappable&>::value,
      "");
  static_assert(
      !is_nothrow_swappable_with<non_swappable&, non_swappable&>::value, "");
  static_assert(is_nothrow_swappable_with<std::std_nothrow_swappable&,
                                          std::std_nothrow_swappable&>::value,
                "");
  static_assert(is_nothrow_swappable_with<non_std_nothrow_swappable&,
                                          non_std_nothrow_swappable&>::value,
                "");
  static_assert(!is_nothrow_swappable_with<nothrow_non_swappable&,
                                           nothrow_non_swappable&>::value,
                "");
  static_assert(!is_nothrow_swappable_with<non_std_swappable&, int&>::value,
                "");
}

TEST_CASE("is_nothrow_swappable")
{
  static_assert(is_nothrow_swappable<int>::value, "");
  static_assert(!is_nothrow_swappable<std::std_swappable>::value, "");
  static_assert(!is_nothrow_swappable<non_std_swappable>::value, "");
  static_assert(!is_nothrow_swappable<non_swappable>::value, "");
  static_assert(is_nothrow_swappable<std::std_nothrow_swappable>::value, "");
  static_assert(is_nothrow_swappable<non_std_nothrow_swappable>::value, "");
  static_assert(!is_nothrow_swappable<nothrow_non_swappable>::value, "");
}

TEST_CASE("is_nothrow_convertible")
{
  static_assert(is_nothrow_convertible<void, void>::value, "");
  static_assert(!is_nothrow_convertible<int, void>::value, "");
  static_assert(!is_nothrow_convertible<void, int>::value, "");
  static_assert(is_nothrow_convertible<int, int>::value, "");
  struct to { };
  struct to_nothrow { };
  struct from {
    operator to();
    operator to_nothrow() noexcept;
  };
  static_assert(!is_nothrow_convertible<from, to>::value, "");
  static_assert(is_nothrow_convertible<from, to_nothrow>::value, "");
}

TEST_CASE("is_invocable")
{
  {
    auto f = []() {};
    auto fi = [](int) {};
    auto fn = []() noexcept {};
    auto fin = [](int) noexcept {};
    static_assert(is_invocable<decltype(f)>::value, "");
    static_assert(!is_invocable<decltype(f), int>::value, "");
    static_assert(!is_invocable<decltype(fi)>::value, "");
    static_assert(is_invocable<decltype(fi), int>::value, "");
    static_assert(is_invocable<decltype(fn)>::value, "");
    static_assert(!is_invocable<decltype(fn), int>::value, "");
    static_assert(!is_invocable<decltype(fin)>::value, "");
    static_assert(is_invocable<decltype(fin), int>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable<decltype(f)>::value);
    static_assert(!is_nothrow_invocable<decltype(f), int>::value);
    static_assert(!is_nothrow_invocable<decltype(fi)>::value);
    static_assert(!is_nothrow_invocable<decltype(fi), int>::value);
    static_assert(is_nothrow_invocable<decltype(fn)>::value);
    static_assert(!is_nothrow_invocable<decltype(fn), int>::value);
    static_assert(!is_nothrow_invocable<decltype(fin)>::value);
    static_assert(is_nothrow_invocable<decltype(fin), int>::value);
#endif
    auto f_ri = []() { return 1; };
    auto fn_ri = []() noexcept { return 1; };
    static_assert(is_invocable_r<void, decltype(f)>::value, "");
    static_assert(!is_invocable_r<int, decltype(f)>::value, "");
    static_assert(is_invocable_r<void, decltype(fn)>::value, "");
    static_assert(!is_invocable_r<int, decltype(fn)>::value, "");
    static_assert(is_invocable_r<void, decltype(f_ri)>::value, "");
    static_assert(is_invocable_r<int, decltype(f_ri)>::value, "");
    static_assert(is_invocable_r<void, decltype(fn_ri)>::value, "");
    static_assert(is_invocable_r<int, decltype(fn_ri)>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable_r<void, decltype(f)>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(f)>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(fn)>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(fn)>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(f_ri)>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(f_ri)>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(fn_ri)>::value);
    static_assert(is_nothrow_invocable_r<int, decltype(fn_ri)>::value);
#endif
  }
  {
    struct f {
      void operator()();
    };
    struct fi {
      void operator()(int);
    };
    struct fn {
      void operator()() noexcept;
    };
    struct fin {
      void operator()(int) noexcept;
    };
    static_assert(is_invocable<f>::value, "");
    static_assert(!is_invocable<f, int>::value, "");
    static_assert(!is_invocable<fi>::value, "");
    static_assert(is_invocable<fi, int>::value, "");
    static_assert(is_invocable<fn>::value, "");
    static_assert(!is_invocable<fn, int>::value, "");
    static_assert(!is_invocable<fin>::value, "");
    static_assert(is_invocable<fin, int>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable<f>::value);
    static_assert(!is_nothrow_invocable<f, int>::value);
    static_assert(!is_nothrow_invocable<fi>::value);
    static_assert(!is_nothrow_invocable<fi, int>::value);
    static_assert(is_nothrow_invocable<fn>::value);
    static_assert(!is_nothrow_invocable<fn, int>::value);
    static_assert(!is_nothrow_invocable<fin>::value);
    static_assert(is_nothrow_invocable<fin, int>::value);
#endif
    struct f_ri {
      int operator()();
    };
    struct fn_ri {
      int operator()() noexcept;
    };
    static_assert(is_invocable_r<void, f>::value, "");
    static_assert(!is_invocable_r<int, f>::value, "");
    static_assert(is_invocable_r<void, fn>::value, "");
    static_assert(!is_invocable_r<int, fn>::value, "");
    static_assert(is_invocable_r<void, f_ri>::value, "");
    static_assert(is_invocable_r<int, f_ri>::value, "");
    static_assert(is_invocable_r<void, fn_ri>::value, "");
    static_assert(is_invocable_r<int, fn_ri>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable_r<void, f>::value);
    static_assert(!is_nothrow_invocable_r<int, f>::value);
    static_assert(is_nothrow_invocable_r<void, fn>::value);
    static_assert(!is_nothrow_invocable_r<int, fn>::value);
    static_assert(!is_nothrow_invocable_r<void, f_ri>::value);
    static_assert(!is_nothrow_invocable_r<int, f_ri>::value);
    static_assert(is_nothrow_invocable_r<void, fn_ri>::value);
    static_assert(is_nothrow_invocable_r<int, fn_ri>::value);
#endif
  }
  {
    struct s {
      void f();
      void fc() const;
      void fi(int);
      void fic(int) const;
      void fn() noexcept;
      void fcn() const noexcept;
      void fin(int) noexcept;
      void ficn(int) const noexcept;
      int f_ri();
      int fc_ri() const;
      int fn_ri() noexcept;
      int fcn_ri() const noexcept;
      int o = 0;
      const int oc = 0;
    };
    static_assert(is_invocable<decltype(&s::f), s>::value, "");
    static_assert(!is_invocable<decltype(&s::f), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::fc), s>::value, "");
    static_assert(!is_invocable<decltype(&s::fc), s, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fi), s>::value, "");
    static_assert(is_invocable<decltype(&s::fi), s, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fic), s>::value, "");
    static_assert(is_invocable<decltype(&s::fic), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::f), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::f), s*, int>::value, "");
    static_assert(is_invocable<decltype(&s::fc), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::fc), s*, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fi), s*>::value, "");
    static_assert(is_invocable<decltype(&s::fi), s*, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fic), s*>::value, "");
    static_assert(is_invocable<decltype(&s::fic), s*, int>::value, "");
    static_assert(is_invocable<decltype(&s::fn), s>::value, "");
    static_assert(!is_invocable<decltype(&s::fn), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::fcn), s>::value, "");
    static_assert(!is_invocable<decltype(&s::fcn), s, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fin), s>::value, "");
    static_assert(is_invocable<decltype(&s::fin), s, int>::value, "");
    static_assert(!is_invocable<decltype(&s::ficn), s>::value, "");
    static_assert(is_invocable<decltype(&s::ficn), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::fn), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::fn), s*, int>::value, "");
    static_assert(is_invocable<decltype(&s::fcn), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::fcn), s*, int>::value, "");
    static_assert(!is_invocable<decltype(&s::fin), s*>::value, "");
    static_assert(is_invocable<decltype(&s::fin), s*, int>::value, "");
    static_assert(!is_invocable<decltype(&s::ficn), s*>::value, "");
    static_assert(is_invocable<decltype(&s::ficn), s*, int>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable<decltype(&s::f), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::f), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fc), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fc), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fi), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fi), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fic), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fic), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::f), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::f), s*, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fc), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fc), s*, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fi), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fi), s*, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fic), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fic), s*, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fn), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fn), s, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fcn), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fcn), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fin), s>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fin), s, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::ficn), s>::value);
    static_assert(is_nothrow_invocable<decltype(&s::ficn), s, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fn), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fn), s*, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fcn), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fcn), s*, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::fin), s*>::value);
    static_assert(is_nothrow_invocable<decltype(&s::fin), s*, int>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::ficn), s*>::value);
    static_assert(is_nothrow_invocable<decltype(&s::ficn), s*, int>::value);
#endif
    static_assert(is_invocable_r<void, decltype(&s::f), s>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::f), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fc), s>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fc), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::f), s*>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::f), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fc), s*>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fc), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fn), s>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fn), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fcn), s>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fcn), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fn), s*>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fn), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fcn), s*>::value, "");
    static_assert(!is_invocable_r<int, decltype(&s::fcn), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::f_ri), s>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::f_ri), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fc_ri), s>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fc_ri), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::f_ri), s*>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::f_ri), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fc_ri), s*>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fc_ri), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fn_ri), s>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fn_ri), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fcn_ri), s>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fcn_ri), s>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fn_ri), s*>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fn_ri), s*>::value, "");
    static_assert(is_invocable_r<void, decltype(&s::fcn_ri), s*>::value, "");
    static_assert(is_invocable_r<int, decltype(&s::fcn_ri), s*>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::f), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::f), s>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::fc), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fc), s>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::f), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::f), s*>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::fc), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fc), s*>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fn), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fn), s>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fcn), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fcn), s>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fn), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fn), s*>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fcn), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fcn), s*>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::f_ri), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::f_ri), s>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::fc_ri), s>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fc_ri), s>::value);
    static_assert(!is_nothrow_invocable_r<void, decltype(&s::f_ri), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::f_ri), s*>::value);
    static_assert(
        !is_nothrow_invocable_r<void, decltype(&s::fc_ri), s*>::value);
    static_assert(!is_nothrow_invocable_r<int, decltype(&s::fc_ri), s*>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fn_ri), s>::value);
    static_assert(is_nothrow_invocable_r<int, decltype(&s::fn_ri), s>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fcn_ri), s>::value);
    static_assert(is_nothrow_invocable_r<int, decltype(&s::fcn_ri), s>::value);
    static_assert(is_nothrow_invocable_r<void, decltype(&s::fn_ri), s*>::value);
    static_assert(is_nothrow_invocable_r<int, decltype(&s::fn_ri), s*>::value);
    static_assert(
        is_nothrow_invocable_r<void, decltype(&s::fcn_ri), s*>::value);
    static_assert(is_nothrow_invocable_r<int, decltype(&s::fcn_ri), s*>::value);
#endif

    static_assert(is_invocable<decltype(&s::o), s>::value, "");
    static_assert(!is_invocable<decltype(&s::o), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::oc), s>::value, "");
    static_assert(!is_invocable<decltype(&s::oc), s, int>::value, "");
    static_assert(is_invocable<decltype(&s::o), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::o), s*, int>::value, "");
    static_assert(is_invocable<decltype(&s::oc), s*>::value, "");
    static_assert(!is_invocable<decltype(&s::oc), s*, int>::value, "");
#ifdef GUL_HAS_CXX17
    static_assert(is_nothrow_invocable<decltype(&s::o), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::o), s, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::oc), s>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::oc), s, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::o), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::o), s*, int>::value);
    static_assert(is_nothrow_invocable<decltype(&s::oc), s*>::value);
    static_assert(!is_nothrow_invocable<decltype(&s::oc), s*, int>::value);
#endif
  }
}

TEST_SUITE_END();
