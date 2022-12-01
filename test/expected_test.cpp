//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/expected.hpp>

#include <cstring>

using namespace gul;

TEST_SUITE_BEGIN("expected");

namespace {
template <typename T>
class default_constructible : std::vector<T> {
  using base_type = std::vector<T>;

public:
  default_constructible() = default;

  default_constructible(int value)
      : base_type({ value })
  {
  }

  default_constructible(std::initializer_list<T> init)
      : base_type(std::move(init))
  {
  }

  default_constructible(base_type init)
      : base_type(std::move(init))
  {
  }

  default_constructible(const default_constructible&) = default;

  default_constructible(default_constructible&&) = default;

  default_constructible& operator=(const default_constructible&) = default;

  default_constructible& operator=(default_constructible&&) = default;

  using base_type::size;

  friend bool operator==(const default_constructible& lhs,
                         const default_constructible& rhs)
  {
    return static_cast<const base_type&>(lhs)
        == static_cast<const base_type&>(rhs);
  }
};

template <typename T>
using dc = default_constructible<T>;

template <typename T>
class non_default_constructible : std::vector<T> {
  using base_type = std::vector<T>;

public:
  non_default_constructible(int value)
      : base_type({ value })
  {
  }

  non_default_constructible(std::initializer_list<T> init)
      : base_type(std::move(init))
  {
  }

  non_default_constructible(base_type init)
      : base_type(std::move(init))
  {
  }

  non_default_constructible(const non_default_constructible&) = default;

  non_default_constructible(non_default_constructible&&) = default;

  non_default_constructible& operator=(const non_default_constructible&)
      = default;

  non_default_constructible& operator=(non_default_constructible&&) = default;

  using base_type::size;

  friend bool operator==(const non_default_constructible& lhs,
                         const non_default_constructible& rhs)
  {
    return static_cast<const base_type&>(lhs)
        == static_cast<const base_type&>(rhs);
  }
};

template <typename T>
using ndc = non_default_constructible<T>;

template <typename Exp>
using deref_op = decltype(*std::declval<Exp>());

template <typename Exp>
using fn_value = decltype(std::declval<Exp>().value());

template <template <typename...> class Op, typename Test, typename Expected>
struct assert_is_same {
  STATIC_ASSERT(std::is_same<Op<Test>, Expected>::value);
};
}

TEST_CASE("unexpected")
{
  {
    auto s = gul::unexpected<int>(1);
    CHECK_EQ(s.error(), 1);
  }
  {
    auto s = gul::unexpected<dc<int>>(in_place, 1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(in_place, { 0, 1, 2 });
    CHECK_EQ(s.error(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    gul::unexpected<dc<int>> d(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    gul::unexpected<dc<int>> d(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    gul::unexpected<dc<int>> d(std::move(s));
    CHECK_EQ(s.error(), dc<int>());
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    gul::unexpected<dc<int>> d(std::move(s));
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    auto d = s;
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    auto d = s;
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    auto d = std::move(s);
    CHECK_EQ(s.error(), dc<int>());
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    auto d = std::move(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    auto s = gul::unexpected<dc<int>>(1);
    CHECK_EQ(std::move(s).error(), dc<int>(1));
  }
  {
    const auto s = gul::unexpected<dc<int>>(1);
    CHECK_EQ(std::move(s).error(), dc<int>(1));
  }
  {
    auto lhs = gul::unexpected<dc<int>>(1);
    auto rhs = gul::unexpected<dc<int>>(2);
    swap(lhs, rhs);
    CHECK_EQ(lhs.error(), dc<int>(2));
    CHECK_EQ(rhs.error(), dc<int>(1));
  }
  {
    CHECK(gul::unexpected<int>(1) == gul::unexpected<int>(1));
    CHECK(!(gul::unexpected<int>(1) == gul::unexpected<int>(2)));
    CHECK(!(gul::unexpected<int>(1) != gul::unexpected<int>(1)));
    CHECK(gul::unexpected<int>(1) != gul::unexpected<int>(2));
  }
}

TEST_CASE("bad_expected_access")
{
#if !GUL_NO_EXCEPTIONS
  {
    auto ex = bad_expected_access<int>(1);
    CHECK_EQ(ex.error(), 1);
  }
  {
    const auto ex = bad_expected_access<int>(1);
    CHECK_EQ(ex.error(), 1);
  }
  {
    auto ex = bad_expected_access<int>(1);
    CHECK_EQ(std::move(ex).error(), 1);
  }
  {
    const auto ex = bad_expected_access<int>(1);
    CHECK_EQ(std::move(ex).error(), 1);
  }
  {
    auto ex = bad_expected_access<int>(1);
    CHECK(strcmp(ex.what(), "bad expected access") == 0);
  }
#endif
}

TEST_CASE("trivially")
{
  {
    using type = expected<void, int>;
    STATIC_ASSERT(detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(detail::is_trivially_copy_assignable<type>::value);
#ifndef GUL_CXX_COMPILER_GCC48
    STATIC_ASSERT(detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(std::is_trivially_destructible<type>::value);
  }
  {
    using type = expected<int, int>;
    STATIC_ASSERT(detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(detail::is_trivially_copy_assignable<type>::value);
#ifndef GUL_CXX_COMPILER_GCC48
    STATIC_ASSERT(detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(std::is_trivially_destructible<type>::value);
  }
  {
    using type = expected<int, dc<int>>;
    STATIC_ASSERT(!detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_copy_assignable<type>::value);
#ifndef GUL_CXX_COMPILER_GCC48
    STATIC_ASSERT(!detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(!std::is_trivially_destructible<type>::value);
  }
  {
    using type = expected<dc<int>, int>;
    STATIC_ASSERT(!detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_copy_assignable<type>::value);
#ifndef GUL_CXX_COMPILER_GCC48
    STATIC_ASSERT(!detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(!std::is_trivially_destructible<type>::value);
  }
  {
    using type = expected<dc<int>, dc<int>>;
    STATIC_ASSERT(!detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_copy_assignable<type>::value);
#ifndef GUL_CXX_COMPILER_GCC48
    STATIC_ASSERT(!detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(!std::is_trivially_destructible<type>::value);
  }
}

TEST_CASE("default constructor")
{
  STATIC_ASSERT(std::is_default_constructible<expected<void, int>>::value);
  STATIC_ASSERT(std::is_default_constructible<expected<int, int>>::value);
  STATIC_ASSERT(std::is_default_constructible<expected<dc<int>, int>>::value);
  STATIC_ASSERT(!std::is_default_constructible<expected<ndc<int>, int>>::value);
  {
    auto exp = expected<void, int>();
    CHECK(exp);
  }
  {
    auto exp = expected<int, int>();
    CHECK(exp);
    CHECK_EQ(exp.value(), 0);
  }
  {
    auto exp = expected<dc<int>, int>();
    CHECK(exp);
    CHECK_EQ(exp.value(), dc<int>());
  }
  {
    auto exp = expected<ndc<int>, int>(1);
    CHECK(exp);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
}

TEST_CASE("copy/move constructor")
{
  {
    auto s = expected<dc<int>, dc<int>>({ 0, 1, 2 });
    expected<dc<int>, dc<int>> d(s);
    CHECK_EQ(s.value(), dc<int>({ 0, 1, 2 }));
    CHECK_EQ(d.value(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto s = expected<dc<int>, dc<int>>(unexpect, { 0, 1, 2 });
    expected<dc<int>, dc<int>> d(s);
    CHECK_EQ(s.error(), dc<int>({ 0, 1, 2 }));
    CHECK_EQ(d.error(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto s = expected<dc<int>, dc<int>>({ 0, 1, 2 });
    expected<dc<int>, dc<int>> d(std::move(s));
    CHECK_EQ(s.value(), dc<int>());
    CHECK_EQ(d.value(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto s = expected<dc<int>, dc<int>>(unexpect, { 0, 1, 2 });
    expected<dc<int>, dc<int>> d(std::move(s));
    CHECK_EQ(s.error(), dc<int>());
    CHECK_EQ(d.error(), dc<int>({ 0, 1, 2 }));
  }
}

TEST_CASE("perfect forwarding constructor")
{
  {
    auto exp = expected<int, int>(1);
    CHECK(exp);
    CHECK_EQ(exp.value(), 1);
  }
  {
    auto exp = expected<ndc<int>, int>({ 0, 1, 2 });
    CHECK(exp);
    CHECK_EQ(exp.value(), ndc<int>({ 0, 1, 2 }));
  }
}

TEST_CASE("converting constructor")
{
  {
    auto d = expected<std::vector<int>, int>(unexpect, 1);
    auto b = expected<ndc<int>, int>(d);
    CHECK(!d);
    CHECK(!b);
  }
  {
    auto d = expected<std::vector<int>, int>(in_place, { 0, 1, 2 });
    auto b = expected<ndc<int>, int>(d);
    CHECK(d);
    CHECK_EQ(d.value(), std::vector<int> { 0, 1, 2 });
    CHECK(b);
    CHECK_EQ(b.value(), ndc<int>(std::vector<int> { 0, 1, 2 }));
  }
  {
    auto d = expected<std::vector<int>, int>(unexpect, 1);
    auto b = expected<ndc<int>, int>(std::move(d));
    CHECK(!d);
    CHECK(!b);
  }
  {
    auto d = expected<std::vector<int>, int>(in_place, { 0, 1, 2 });
    auto b = expected<ndc<int>, int>(std::move(d));
    CHECK(d);
    CHECK_EQ(d.value(), std::vector<int> {});
    CHECK(b);
    CHECK_EQ(b.value(), ndc<int>(std::vector<int> { 0, 1, 2 }));
  }
  {
    auto d = gul::unexpected<int>(1);
    auto b = expected<ndc<int>, int>(d);
    CHECK(!b);
  }
  {
    auto d = gul::unexpected<int>(1);
    auto b = expected<ndc<int>, int>(std::move(d));
    CHECK(!b);
  }
}

TEST_CASE("in-place constructor")
{
  {
    auto exp = expected<void, int>(in_place);
    CHECK(exp);
  }
  {
    auto exp = expected<void, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<ndc<int>, int>(in_place, 1);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, int>(in_place, { 0, 1, 2 });
    CHECK_EQ(exp.value(), ndc<int>({ 0, 1, 2 }));
  }
  {
    auto exp = expected<int, ndc<int>>(unexpect, 1);
    CHECK_EQ(exp.error(), ndc<int>(1));
  }
  {
    auto exp = expected<int, ndc<int>>(unexpect, { 0, 1, 2 });
    CHECK_EQ(exp.error(), ndc<int>({ 0, 1, 2 }));
  }
}

TEST_CASE("perfect forwarding assignment operator")
{
  {
    auto exp = expected<dc<int>, int>();
    auto val = dc<int>({ 0, 1, 2 });
    exp = val;
    CHECK_EQ(exp.value(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto exp = expected<dc<int>, int>(unexpect, 0);
    auto val = dc<int>({ 0, 1, 2 });
    exp = val;
    CHECK_EQ(exp.value(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto exp = expected<dc<int>, int>();
    auto val = dc<int>({ 0, 1, 2 });
    exp = std::move(val);
    CHECK_EQ(exp.value(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto exp = expected<dc<int>, int>(unexpect, 0);
    auto val = dc<int>({ 0, 1, 2 });
    exp = std::move(val);
    CHECK_EQ(exp.value(), dc<int>({ 0, 1, 2 }));
  }
}

TEST_CASE("converting assignment operator")
{
  {
    auto exp = expected<void, int>();
    auto une = gul::unexpected<int>(2);
    exp = une;
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<void, int>(unexpect);
    auto une = gul::unexpected<int>(2);
    exp = une;
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<void, int>();
    auto une = gul::unexpected<int>(2);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<void, int>(unexpect);
    auto une = gul::unexpected<int>(2);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<int, int>();
    auto une = gul::unexpected<int>(2);
    exp = une;
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<int, int>(unexpect, 1);
    auto une = gul::unexpected<int>(2);
    exp = une;
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<int, int>();
    auto une = gul::unexpected<int>(2);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 2);
  }
  {
    auto exp = expected<int, int>(unexpect, 1);
    auto une = gul::unexpected<int>(2);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 2);
  }
}

TEST_CASE("operator->")
{
  {
    auto exp = expected<ndc<int>, int>({ 0, 1, 2 });
    CHECK_EQ(exp->size(), 3);
  }
  {
    const auto exp = expected<ndc<int>, int>({ 0, 1, 2 });
    CHECK_EQ(exp->size(), 3);
  }
}

TEST_CASE("operator*")
{
  assert_is_same<deref_op, expected<void, int>&, void>();
  assert_is_same<deref_op, const expected<void, int>&, void>();
  assert_is_same<deref_op, expected<void, int>, void>();
  assert_is_same<deref_op, const expected<void, int>, void>();
  assert_is_same<deref_op, expected<int, int>&, int&>();
  assert_is_same<deref_op, const expected<int, int>&, const int&>();
  assert_is_same<deref_op, expected<int, int>, int&&>();
#ifndef GUL_CXX_COMPILER_GCC48
  assert_is_same<deref_op, const expected<int, int>, const int&&>();
#endif
  {
    auto exp = expected<int, int>(1);
    CHECK_EQ(*exp, 1);
  }
  {
    const auto exp = expected<int, int>(1);
    CHECK_EQ(*exp, 1);
  }
  {
    auto exp = expected<int, int>(1);
    CHECK_EQ(*std::move(exp), 1);
  }
  {
    const auto exp = expected<int, int>(1);
    CHECK_EQ(*std::move(exp), 1);
  }
  {
    auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(*exp, ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(*exp, ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(*std::move(exp), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(*std::move(exp), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(*exp, ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(*exp, ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(*std::move(exp), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(*std::move(exp), ndc<int>(1));
  }
}

TEST_CASE("has_value")
{
  CHECK(expected<int, int>().has_value());
  CHECK(!expected<int, int>(unexpect, 1).has_value());
}

TEST_CASE("value")
{
  assert_is_same<fn_value, expected<void, int>&, void>();
  assert_is_same<fn_value, const expected<void, int>&, void>();
  assert_is_same<fn_value, expected<void, int>, void>();
  assert_is_same<fn_value, const expected<void, int>, void>();
  assert_is_same<fn_value, expected<int, int>&, int&>();
  assert_is_same<fn_value, const expected<int, int>&, const int&>();
  assert_is_same<fn_value, expected<int, int>, int&&>();
#ifndef GUL_CXX_COMPILER_GCC48
  assert_is_same<fn_value, const expected<int, int>, const int&&>();
#endif

  {
    auto exp = expected<void, int>(in_place);
    exp.value();
  }
  {
    const auto exp = expected<void, int>(in_place);
    exp.value();
  }
  {
    auto exp = expected<void, int>(in_place);
    std::move(exp).value();
  }
  {
    const auto exp = expected<void, int>(in_place);
    std::move(exp).value();
  }
#if !GUL_NO_EXCEPTIONS
  {
    auto exp = expected<void, int>(unexpect);
    CHECK_THROWS_AS(exp.value(), bad_expected_access<int>);
  }
  {
    const auto exp = expected<void, int>(unexpect);
    CHECK_THROWS_AS(exp.value(), bad_expected_access<int>);
  }
  {
    auto exp = expected<void, int>(unexpect);
    CHECK_THROWS_AS(std::move(exp).value(), bad_expected_access<int>);
  }
  {
    const auto exp = expected<void, int>(unexpect);
    CHECK_THROWS_AS(std::move(exp).value(), bad_expected_access<int>);
  }
#endif
  {
    auto exp = expected<int, int>(1);
    CHECK_EQ(exp.value(), 1);
  }
  {
    const auto exp = expected<int, int>(1);
    CHECK_EQ(exp.value(), 1);
  }
  {
    auto exp = expected<int, int>(1);
    CHECK_EQ(std::move(exp).value(), 1);
  }
  {
    const auto exp = expected<int, int>(1);
    CHECK_EQ(std::move(exp).value(), 1);
  }
#if !GUL_NO_EXCEPTIONS
  {
    auto exp = expected<int, int>(unexpect);
    CHECK_THROWS_AS(exp.value(), bad_expected_access<int>);
  }
  {
    const auto exp = expected<int, int>(unexpect);
    CHECK_THROWS_AS(exp.value(), bad_expected_access<int>);
  }
  {
    auto exp = expected<int, int>(unexpect);
    CHECK_THROWS_AS(std::move(exp).value(), bad_expected_access<int>);
  }
  {
    const auto exp = expected<int, int>(unexpect);
    CHECK_THROWS_AS(std::move(exp).value(), bad_expected_access<int>);
  }
#endif
  {
    auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(std::move(exp).value(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, int>(1);
    CHECK_EQ(std::move(exp).value(), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(exp.value(), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(std::move(exp).value(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(1);
    CHECK_EQ(std::move(exp).value(), ndc<int>(1));
  }
}

TEST_CASE("error")
{
  {
    auto exp = expected<void, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    const auto exp = expected<void, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<void, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    const auto exp = expected<void, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    auto exp = expected<int, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    const auto exp = expected<int, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<int, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    const auto exp = expected<int, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    auto exp = expected<ndc<int>, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    const auto exp = expected<ndc<int>, int>(unexpect, 1);
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<ndc<int>, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    const auto exp = expected<ndc<int>, int>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), 1);
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(unexpect, 1);
    CHECK_EQ(exp.error(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(unexpect, 1);
    CHECK_EQ(exp.error(), ndc<int>(1));
  }
  {
    auto exp = expected<ndc<int>, ndc<int>>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), ndc<int>(1));
  }
  {
    const auto exp = expected<ndc<int>, ndc<int>>(unexpect, 1);
    CHECK_EQ(std::move(exp).error(), ndc<int>(1));
  }
}

TEST_CASE("value_or")
{
  {
    auto u = expected<int, int>(unexpect, 1);
    CHECK_EQ(u.value_or(2LL), 2);
    auto e = expected<int, int>(1);
    CHECK_EQ(e.value_or(2LL), 1);
  }
  {
    CHECK_EQ(expected<int, int>(unexpect, 1).value_or(2LL), 2);
    CHECK_EQ(expected<int, int>(1).value_or(2LL), 1);
  }
}

TEST_CASE("error_or")
{
  {
    auto u = expected<int, int>(unexpect, 1);
    CHECK_EQ(u.error_or(2LL), 1);
    auto e = expected<int, int>(1);
    CHECK_EQ(e.error_or(2LL), 2);
  }
  {
    CHECK_EQ(expected<int, int>(unexpect, 1).error_or(2LL), 1);
    CHECK_EQ(expected<int, int>(1).error_or(2LL), 2);
  }
}

TEST_CASE("emplace")
{
  static_assert_same<decltype(std::declval<expected<void, int>&>().emplace()),
                     void>();
  static_assert_same<decltype(std::declval<expected<int, int>&>().emplace(1)),
                     int&>();
  {
    auto e = expected<void, int>(unexpect, 0);
    e.emplace();
    CHECK(e);
  }
  {
    auto e = expected<void, int>();
    e.emplace();
    CHECK(e);
  }
  {
    auto e = expected<ndc<int>, int>(unexpect, 0);
    e.emplace(1);
    CHECK_EQ(e.value(), ndc<int>(1));
  }
  {
    auto e = expected<ndc<int>, int>(1);
    e.emplace(1);
    CHECK_EQ(e.value(), ndc<int>(1));
  }
  {
    auto e = expected<ndc<int>, int>(unexpect, 0);
    e.emplace({ 0, 1, 2 });
    CHECK_EQ(e.value(), ndc<int>({ 0, 1, 2 }));
  }
  {
    auto e = expected<ndc<int>, int>(1);
    e.emplace({ 0, 1, 2 });
    CHECK_EQ(e.value(), ndc<int>({ 0, 1, 2 }));
  }
}

TEST_CASE("and_then")
{
  {
    auto f = []() -> expected<std::string, int> { return "1"; };
    {
      auto u = expected<void, int>(unexpect, 0);
      CHECK_EQ(u.and_then(f).error(), 0);
      auto e = expected<void, int>();
      CHECK_EQ(e.and_then(f).value(), "1");
    }
    {
      const auto u = expected<void, int>(unexpect, 0);
      CHECK_EQ(u.and_then(f).error(), 0);
      const auto e = expected<void, int>();
      CHECK_EQ(e.and_then(f).value(), "1");
    }
    {
      auto u = expected<void, int>(unexpect, 0);
      CHECK_EQ(std::move(u).and_then(f).error(), 0);
      auto e = expected<void, int>();
      CHECK_EQ(std::move(e).and_then(f).value(), "1");
    }
    {
      const auto u = expected<void, int>(unexpect, 0);
      CHECK_EQ(std::move(u).and_then(f).error(), 0);
      const auto e = expected<void, int>();
      CHECK_EQ(std::move(e).and_then(f).value(), "1");
    }
  }
  {
    auto f
        = [](int i) -> expected<std::string, int> { return std::to_string(i); };
    {
      auto u = expected<int, int>(unexpect, 0);
      CHECK_EQ(u.and_then(f).error(), 0);
      auto e = expected<int, int>(1);
      CHECK_EQ(e.and_then(f).value(), "1");
    }
    {
      const auto u = expected<int, int>(unexpect, 0);
      CHECK_EQ(u.and_then(f).error(), 0);
      const auto e = expected<int, int>(1);
      CHECK_EQ(e.and_then(f).value(), "1");
    }
    {
      auto u = expected<int, int>(unexpect, 0);
      CHECK_EQ(std::move(u).and_then(f).error(), 0);
      auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).and_then(f).value(), "1");
    }
    {
      const auto u = expected<int, int>(unexpect, 0);
      CHECK_EQ(std::move(u).and_then(f).error(), 0);
      const auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).and_then(f).value(), "1");
    }
  }
}

TEST_CASE("or_else")
{
  {
    auto f = [](int i) -> expected<void, std::string> {
      return gul::unexpected<std::string>(std::to_string(i));
    };
    {
      auto u = expected<void, int>(unexpect);
      CHECK_EQ(u.or_else(f).error(), "0");
      auto e = expected<void, int>(in_place);
      CHECK(e.or_else(f));
    }
    {
      const auto u = expected<void, int>(unexpect);
      CHECK_EQ(u.or_else(f).error(), "0");
      const auto e = expected<void, int>(in_place);
      CHECK(e.or_else(f));
    }
    {
      auto u = expected<void, int>(unexpect);
      CHECK_EQ(std::move(u).or_else(f).error(), "0");
      auto e = expected<void, int>(in_place);
      CHECK(std::move(e).or_else(f));
    }
    {
      const auto u = expected<void, int>(unexpect);
      CHECK_EQ(std::move(u).or_else(f).error(), "0");
      const auto e = expected<void, int>(in_place);
      CHECK(std::move(e).or_else(f));
    }
  }
  {
    auto f = [](int i) -> expected<int, std::string> {
      return gul::unexpected<std::string>(std::to_string(i));
    };
    {
      auto u = expected<int, int>(unexpect);
      CHECK_EQ(u.or_else(f).error(), "0");
      auto e = expected<int, int>(1);
      CHECK_EQ(e.or_else(f).value(), 1);
    }
    {
      const auto u = expected<int, int>(unexpect);
      CHECK_EQ(u.or_else(f).error(), "0");
      const auto e = expected<int, int>(1);
      CHECK_EQ(e.or_else(f).value(), 1);
    }
    {
      auto u = expected<int, int>(unexpect);
      CHECK_EQ(std::move(u).or_else(f).error(), "0");
      auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).or_else(f).value(), 1);
    }
    {
      const auto u = expected<int, int>(unexpect);
      CHECK_EQ(std::move(u).or_else(f).error(), "0");
      const auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).or_else(f).value(), 1);
    }
  }
}

TEST_CASE("transform")
{
  {
    auto f = []() -> std::string { return "1"; };
    {
      auto u = expected<void, int>(unexpect);
      CHECK_EQ(u.transform(f).error(), 0);
      auto e = expected<void, int>();
      CHECK_EQ(e.transform(f).value(), "1");
    }
    {
      const auto u = expected<void, int>(unexpect);
      CHECK_EQ(u.transform(f).error(), 0);
      const auto e = expected<void, int>();
      CHECK_EQ(e.transform(f).value(), "1");
    }
    {
      auto u = expected<void, int>(unexpect);
      CHECK_EQ(std::move(u).transform(f).error(), 0);
      auto e = expected<void, int>();
      CHECK_EQ(std::move(e).transform(f).value(), "1");
    }
    {
      const auto u = expected<void, int>(unexpect);
      CHECK_EQ(std::move(u).transform(f).error(), 0);
      const auto e = expected<void, int>();
      CHECK_EQ(std::move(e).transform(f).value(), "1");
    }
  }
  {
    auto f = [](int i) -> std::string { return std::to_string(i); };
    {
      auto u = expected<int, int>(unexpect);
      CHECK_EQ(u.transform(f).error(), 0);
      auto e = expected<int, int>(1);
      CHECK_EQ(e.transform(f).value(), "1");
    }
    {
      const auto u = expected<int, int>(unexpect);
      CHECK_EQ(u.transform(f).error(), 0);
      const auto e = expected<int, int>(1);
      CHECK_EQ(e.transform(f).value(), "1");
    }
    {
      auto u = expected<int, int>(unexpect);
      CHECK_EQ(std::move(u).transform(f).error(), 0);
      auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).transform(f).value(), "1");
    }
    {
      const auto u = expected<int, int>(unexpect);
      CHECK_EQ(std::move(u).transform(f).error(), 0);
      const auto e = expected<int, int>(1);
      CHECK_EQ(std::move(e).transform(f).value(), "1");
    }
  }
}

TEST_CASE("transform_error")
{
  auto f = [](int i) -> std::string { return std::to_string(i); };
  {
    auto u = expected<void, int>(unexpect);
    CHECK_EQ(u.transform_error(f).error(), "0");
    auto e = expected<void, int>(in_place);
    CHECK(e.transform_error(f));
  }
  {
    const auto u = expected<void, int>(unexpect);
    CHECK_EQ(u.transform_error(f).error(), "0");
    const auto e = expected<void, int>(in_place);
    CHECK(e.transform_error(f));
  }
  {
    auto u = expected<void, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(f).error(), "0");
    auto e = expected<void, int>(in_place);
    CHECK(std::move(e).transform_error(f));
  }
  {
    const auto u = expected<void, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(f).error(), "0");
    const auto e = expected<void, int>(in_place);
    CHECK(std::move(e).transform_error(f));
  }
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform_error(f).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(e.transform_error(f).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform_error(f).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(e.transform_error(f).value(), 1);
  }
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(f).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform_error(f).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(f).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform_error(f).value(), 1);
  }
}

TEST_CASE("swap")
{
  {
    auto a = expected<void, int>(unexpect);
    auto b = expected<void, int>(unexpect);
    swap(a, b);
    CHECK_EQ(a.error(), 0);
    CHECK_EQ(b.error(), 0);
  }
  {
    auto a = expected<void, int>(unexpect);
    auto b = expected<void, int>();
    swap(a, b);
    CHECK(a);
    CHECK_EQ(b.error(), 0);
  }
  {
    auto a = expected<void, int>();
    auto b = expected<void, int>(unexpect);
    swap(a, b);
    CHECK_EQ(a.error(), 0);
    CHECK(b);
  }
  {
    auto a = expected<void, int>();
    auto b = expected<void, int>();
    swap(a, b);
    CHECK(a);
    CHECK(b);
  }
  {
    auto a = expected<ndc<int>, ndc<int>>(unexpect, { 7, 8, 9 });
    auto b = expected<ndc<int>, ndc<int>>(unexpect, 0);
    swap(a, b);
    CHECK_EQ(a.error(), ndc<int>(0));
    CHECK_EQ(b.error(), ndc<int>({ 7, 8, 9 }));
  }
  {
    auto a = expected<ndc<int>, ndc<int>>(unexpect, { 7, 8, 9 });
    auto b = expected<ndc<int>, ndc<int>>(1);
    swap(a, b);
    CHECK_EQ(a.value(), ndc<int>(1));
    CHECK_EQ(b.error(), ndc<int>({ 7, 8, 9 }));
  }
  {
    auto a = expected<ndc<int>, ndc<int>>(in_place, { 0, 1, 2 });
    auto b = expected<ndc<int>, ndc<int>>(unexpect, 0);
    swap(a, b);
    CHECK_EQ(a.error(), ndc<int>(0));
    CHECK_EQ(b.value(), ndc<int>({ 0, 1, 2 }));
  }
  {
    auto a = expected<ndc<int>, ndc<int>>(in_place, { 0, 1, 2 });
    auto b = expected<ndc<int>, ndc<int>>(1);
    swap(a, b);
    CHECK_EQ(a.value(), ndc<int>(1));
    CHECK_EQ(b.value(), ndc<int>({ 0, 1, 2 }));
  }

  class throw_move_constructible {
  public:
    throw_move_constructible(int val)
        : val_(val)
    {
    }

    throw_move_constructible(const throw_move_constructible&) = default;
    throw_move_constructible(throw_move_constructible&&) = default;
    throw_move_constructible& operator=(const throw_move_constructible&)
        = default;
    throw_move_constructible& operator=(throw_move_constructible&&) = default;

    int v() const noexcept
    {
      return val_;
    }

  private:
    int val_ = 0;
  };
  {
    auto a = expected<throw_move_constructible, int>(1);
    auto b = expected<throw_move_constructible, int>(unexpect, 2);
    swap(a, b);
    CHECK_EQ(a.error(), 2);
    CHECK_EQ(b.value().v(), 1);
  }
  {
    auto a = expected<int, throw_move_constructible>(1);
    auto b = expected<int, throw_move_constructible>(unexpect, 2);
    swap(a, b);
    CHECK_EQ(a.error().v(), 2);
    CHECK_EQ(b.value(), 1);
  }
}

TEST_CASE("compare")
{
  CHECK(expected<void, int>() == expected<void, int>());
  CHECK(!(expected<void, int>() != expected<void, int>()));
  CHECK(expected<void, int>(unexpect) == expected<void, int>(unexpect));
  CHECK(!(expected<void, int>(unexpect) != expected<void, int>(unexpect)));
  CHECK(!(expected<void, int>() == expected<void, int>(unexpect)));
  CHECK(expected<void, int>() != expected<void, int>(unexpect));
  CHECK(!(expected<void, int>(unexpect) == expected<void, int>()));
  CHECK(expected<void, int>(unexpect) != expected<void, int>());

  CHECK(expected<int, int>() == expected<int, int>());
  CHECK(!(expected<int, int>() != expected<int, int>()));
  CHECK(expected<int, int>(unexpect) == expected<int, int>(unexpect));
  CHECK(!(expected<int, int>(unexpect) != expected<int, int>(unexpect)));
  CHECK(!(expected<int, int>() == expected<int, int>(unexpect)));
  CHECK(expected<int, int>() != expected<int, int>(unexpect));
  CHECK(!(expected<int, int>(unexpect) == expected<int, int>()));
  CHECK(expected<int, int>(unexpect) != expected<int, int>());

  CHECK(expected<int, int>() == 0);
  CHECK(!(expected<int, int>(unexpect) == 0));
  CHECK(!(expected<int, int>() != 0));
  CHECK(expected<int, int>(unexpect) != 0);

  CHECK(!(expected<int, int>() == gul::unexpected<int>(0)));
  CHECK(expected<int, int>(unexpect) == gul::unexpected<int>(0));
  CHECK(expected<int, int>() != gul::unexpected<int>(0));
  CHECK(!(expected<int, int>(unexpect) != gul::unexpected<int>(0)));
}

TEST_SUITE_END();
