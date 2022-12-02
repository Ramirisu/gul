//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/optional.hpp>

using namespace gul;

TEST_SUITE_BEGIN("optional");

namespace {
template <typename T>
class default_constructible : public std::vector<T> {
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
};

template <typename T>
using dc = default_constructible<T>;

template <typename Exp>
using deref_op = decltype(*std::declval<Exp>());

template <typename Exp>
using fn_value = decltype(std::declval<Exp>().value());

template <template <typename...> class Op, typename Test, typename Expected>
struct assert_is_same {
  STATIC_ASSERT(std::is_same<Op<Test>, Expected>::value);
};
}

TEST_CASE("type assertion")
{
  {
    using type = optional<void>;
    STATIC_ASSERT(detail::is_trivially_copy_constructible<type>::value);
#if !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_copy_assignable<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48)
    STATIC_ASSERT(detail::is_trivially_move_constructible<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48) && !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(std::is_trivially_destructible<type>::value);
  }
  {
    using type = optional<int>;
    STATIC_ASSERT(detail::is_trivially_copy_constructible<type>::value);
#if !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_copy_assignable<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48)
    STATIC_ASSERT(detail::is_trivially_move_constructible<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48) && !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(std::is_trivially_destructible<type>::value);
  }
  {
    using type = optional<dc<int>>;
    STATIC_ASSERT(!detail::is_trivially_copy_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_copy_assignable<type>::value);
#if !defined(GUL_CXX_COMPILER_GCC48)
    STATIC_ASSERT(!detail::is_trivially_move_constructible<type>::value);
    STATIC_ASSERT(!detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(!std::is_trivially_destructible<type>::value);
  }
  {
    using type = optional<int&>;
    STATIC_ASSERT(detail::is_trivially_copy_constructible<type>::value);
#if !defined(GUL_CXX_COMPILER_GCC48) && !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_copy_assignable<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48)
    STATIC_ASSERT(detail::is_trivially_move_constructible<type>::value);
#endif
#if !defined(GUL_CXX_COMPILER_GCC48) && !defined(GUL_CXX_COMPILER_MSVC2015)
    STATIC_ASSERT(detail::is_trivially_move_assignable<type>::value);
#endif
    STATIC_ASSERT(std::is_trivially_destructible<type>::value);
  }
}

TEST_CASE("has_value")
{
  CHECK(!optional<void>().has_value());
  CHECK(optional<void>(in_place).has_value());
  CHECK(!optional<int>().has_value());
  CHECK(optional<int>(1).has_value());
  int val = 1;
  CHECK(!optional<int&>().has_value());
  CHECK(optional<int&>(val).has_value());
}

TEST_CASE("nullopt")
{
  {
    optional<void> o(nullopt);
    CHECK(!o);
  }
  {
    auto o = optional<void>(in_place);
    CHECK(o);
    o = nullopt;
    CHECK(!o);
  }
  {
    optional<int> o(nullopt);
    CHECK(!o);
  }
  {
    auto o = optional<int>(1);
    CHECK(o);
    o = nullopt;
    CHECK(!o);
  }
  {
    int val = 1;
    auto o = optional<int&>(val);
    CHECK(o);
    o = nullopt;
    CHECK(!o);
  }
}

TEST_CASE("in-place constructor")
{
  {
    auto o = optional<void>(in_place);
    CHECK(o);
  }
  {
    auto o = optional<int>(in_place, 1);
    CHECK_EQ(o.value(), 1);
  }
  {
    auto o = optional<dc<int>>(in_place, { 0, 1, 2 });
    CHECK_EQ(o.value(), dc<int> { 0, 1, 2 });
  }
  {
    int val = 1;
    auto o = optional<int&>(in_place, val);
    CHECK_EQ(o.value(), 1);
  }
}

TEST_CASE("perfect forwarding constructor")
{
  {
    auto o = optional<dc<int>>(dc<int>(1));
    CHECK_EQ(o.value(), dc<int>(1));
  }
  {
    auto val = dc<int>(1);
    auto o = optional<dc<int>&>(val);
    CHECK_EQ(o.value(), dc<int>(1));
  }
}

TEST_CASE("copy constructor")
{
  {
    auto s = optional<void>();
    auto d = optional<void>(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<void>(in_place);
    auto d = optional<void>(s);
    CHECK(s);
    CHECK(d);
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>(s);
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>(s);
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
}

TEST_CASE("move constructor")
{
  {
    auto s = optional<void>();
    auto d = optional<void>(std::move(s));
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<void>(in_place);
    auto d = optional<void>(std::move(s));
    CHECK(s);
    CHECK(d);
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>(std::move(s));
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>(std::move(s));
    CHECK_EQ(s, dc<int>());
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>(std::move(s));
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>(std::move(s));
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
}

TEST_CASE("converting constructor")
{
  {
    auto s = optional<std::vector<int>>();
    auto d = optional<dc<int>>(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<std::vector<int>>(in_place, { 0, 1, 2 });
    auto d = optional<dc<int>>(s);
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<std::vector<int>>();
    auto d = optional<dc<int>>(std::move(s));
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<std::vector<int>>(in_place, { 0, 1, 2 });
    auto d = optional<dc<int>>(std::move(s));
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<std::vector<int>&>(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<std::vector<int>&>(s);
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<std::vector<int>&>(std::move(s));
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<std::vector<int>&>(std::move(s));
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
}

TEST_CASE("perfect forwarding assignment operator")
{
  {
    auto o = optional<dc<int>>();
    CHECK(!o);
    o = dc<int>(1);
    CHECK_EQ(o.value(), dc<int>(1));
  }
  {
    auto o = optional<dc<int>&>();
    CHECK(!o);
    auto val = dc<int>(1);
    o = val;
    CHECK_EQ(o.value(), dc<int>(1));
    auto val2 = dc<int>({ 0, 1, 2 });
    o = val2;
    CHECK_EQ(o.value(), dc<int>({ 0, 1, 2 }));
    CHECK_EQ(val, val2);
  }
}

TEST_CASE("copy assignment operator")
{
  {
    auto s = optional<void>();
    auto d = optional<void>();
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<void>(in_place);
    auto d = optional<void>();
    d = s;
    CHECK(s);
    CHECK(d);
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>();
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>();
    d = s;
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>(1);
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>(1);
    d = s;
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>();
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>();
    d = s;
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>(val);
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto val2 = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>(val2);
    d = s;
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
}

TEST_CASE("move assignment operator")
{
  {
    auto s = optional<void>();
    auto d = optional<void>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<void>(in_place);
    auto d = optional<void>();
    d = std::move(s);
    CHECK(s);
    CHECK(d);
  }
  {
    auto s = optional<void>();
    auto d = optional<void>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<void>(in_place);
    auto d = optional<void>();
    d = std::move(s);
    CHECK(s);
    CHECK(d);
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>();
    d = std::move(s);
    CHECK_EQ(s, dc<int>());
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>>();
    auto d = optional<dc<int>>(1);
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<dc<int>>(1);
    auto d = optional<dc<int>>(1);
    d = std::move(s);
    CHECK_EQ(s, dc<int>());
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>();
    d = std::move(s);
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
  {
    auto val = dc<int>(1);
    auto s = optional<dc<int>&>();
    auto d = optional<dc<int>&>(val);
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>(1);
    auto val2 = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<dc<int>&>(val2);
    d = std::move(s);
    CHECK_EQ(s, dc<int>(1));
    CHECK_EQ(d, dc<int>(1));
  }
}

TEST_CASE("converting assignment operator")
{
  {
    auto s = optional<std::vector<int>>();
    auto d = optional<dc<int>>();
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<std::vector<int>>(in_place, { 0, 1, 2 });
    auto d = optional<dc<int>>();
    d = s;
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<std::vector<int>>();
    auto d = optional<dc<int>>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto s = optional<std::vector<int>>(in_place, { 0, 1, 2 });
    auto d = optional<dc<int>>();
    d = std::move(s);
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<std::vector<int>&>();
    d = s;
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<std::vector<int>&>();
    d = s;
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
  {
    auto s = optional<dc<int>&>();
    auto d = optional<std::vector<int>&>();
    d = std::move(s);
    CHECK(!s);
    CHECK(!d);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    auto s = optional<dc<int>&>(val);
    auto d = optional<std::vector<int>&>();
    d = std::move(s);
    CHECK(s);
    CHECK_EQ(d.value(), dc<int> { 0, 1, 2 });
  }
}

TEST_CASE("operator->")
{
  {
    int val = 1;
    auto exp = optional<int&>(val);
    CHECK_EQ(exp.operator->(), &val);
  }
  {
    auto exp = optional<dc<int>>(in_place, { 0, 1, 2 });
    CHECK_EQ(exp->size(), 3);
  }
  {
    const auto exp = optional<dc<int>>(in_place, { 0, 1, 2 });
    CHECK_EQ(exp->size(), 3);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    auto exp = optional<dc<int>&>(val);
    CHECK_EQ(exp->size(), 3);
  }
  {
    auto val = dc<int>({ 0, 1, 2 });
    const auto exp = optional<dc<int>&>(val);
    CHECK_EQ(exp->size(), 3);
  }
}

TEST_CASE("operator*")
{
  assert_is_same<deref_op, optional<void>&, void>();
  assert_is_same<deref_op, const optional<void>&, void>();
  assert_is_same<deref_op, optional<void>, void>();
  assert_is_same<deref_op, const optional<void>, void>();
  assert_is_same<deref_op, optional<int>&, int&>();
  assert_is_same<deref_op, const optional<int>&, const int&>();
  assert_is_same<deref_op, optional<int>, int&&>();
#if !defined(GUL_CXX_COMPILER_GCC48)
  assert_is_same<deref_op, const optional<int>, const int&&>();
#endif
  assert_is_same<deref_op, optional<int&>&, int&>();
  assert_is_same<deref_op, const optional<int&>&, int&>();
  assert_is_same<deref_op, optional<int&>, int&>();
  assert_is_same<deref_op, const optional<int&>, int&>();
  {
    auto o = optional<void>(in_place);
    *o;
  }
  {
    const auto o = optional<void>(in_place);
    *o;
  }
  {
    auto o = optional<void>(in_place);
    *std::move(o);
  }
  {
    const auto o = optional<void>(in_place);
    *std::move(o);
  }
  {
    auto o = optional<int>(1);
    CHECK_EQ(*o, 1);
  }
  {
    const auto o = optional<int>(1);
    CHECK_EQ(*o, 1);
  }
  {
    auto o = optional<int>(1);
    CHECK_EQ(*std::move(o), 1);
  }
  {
    const auto o = optional<int>(1);
    CHECK_EQ(*std::move(o), 1);
  }
  {
    int val = 1;
    auto o = optional<int&>(val);
    CHECK_EQ(*o, 1);
  }
  {
    int val = 1;
    const auto o = optional<int&>(val);
    CHECK_EQ(*o, 1);
  }
  {
    int val = 1;
    auto o = optional<int&>(val);
    CHECK_EQ(*std::move(o), 1);
  }
  {
    int val = 1;
    const auto o = optional<int&>(val);
    CHECK_EQ(*std::move(o), 1);
  }
}

TEST_CASE("value")
{
  assert_is_same<fn_value, optional<void>&, void>();
  assert_is_same<fn_value, const optional<void>&, void>();
  assert_is_same<fn_value, optional<void>, void>();
  assert_is_same<fn_value, const optional<void>, void>();
  assert_is_same<fn_value, optional<int>&, int&>();
  assert_is_same<fn_value, const optional<int>&, const int&>();
  assert_is_same<fn_value, optional<int>, int&&>();
#if !defined(GUL_CXX_COMPILER_GCC48)
  assert_is_same<fn_value, const optional<int>, const int&&>();
#endif
  assert_is_same<fn_value, optional<int&>&, int&>();
  assert_is_same<fn_value, const optional<int&>&, int&>();
  assert_is_same<fn_value, optional<int&>, int&>();
  assert_is_same<fn_value, const optional<int&>, int&>();
  {
    auto o = optional<void>(in_place);
    o.value();
  }
  {
    const auto o = optional<void>(in_place);
    o.value();
  }
  {
    auto o = optional<void>(in_place);
    std::move(o).value();
  }
  {
    const auto o = optional<void>(in_place);
    std::move(o).value();
  }
#if !GUL_NO_EXCEPTIONS
  {
    auto o = optional<void>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    const auto o = optional<void>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    auto o = optional<void>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
  {
    const auto o = optional<void>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
#endif
  {
    auto o = optional<int>(1);
    CHECK_EQ(o.value(), 1);
  }
  {
    const auto o = optional<int>(1);
    CHECK_EQ(o.value(), 1);
  }
  {
    auto o = optional<int>(1);
    CHECK_EQ(std::move(o).value(), 1);
  }
  {
    const auto o = optional<int>(1);
    CHECK_EQ(std::move(o).value(), 1);
  }
#if !GUL_NO_EXCEPTIONS
  {
    auto o = optional<int>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    const auto o = optional<int>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    auto o = optional<int>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
  {
    const auto o = optional<int>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
#endif
  {
    int val = 1;
    auto o = optional<int&>(val);
    CHECK_EQ(o.value(), 1);
  }
  {
    int val = 1;
    const auto o = optional<int&>(val);
    CHECK_EQ(o.value(), 1);
  }
  {
    int val = 1;
    auto o = optional<int&>(val);
    CHECK_EQ(std::move(o).value(), 1);
  }
  {
    int val = 1;
    const auto o = optional<int&>(val);
    CHECK_EQ(std::move(o).value(), 1);
  }
#if !GUL_NO_EXCEPTIONS
  {
    auto o = optional<int&>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    const auto o = optional<int&>();
    CHECK_THROWS_AS(o.value(), bad_optional_access);
  }
  {
    auto o = optional<int&>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
  {
    const auto o = optional<int&>();
    CHECK_THROWS_AS(std::move(o).value(), bad_optional_access);
  }
#endif
}

TEST_CASE("value_or")
{
  {
    optional<int> o;
    CHECK_EQ(o.value_or(2LL), 2);
  }
  {
    optional<int> o(1);
    CHECK_EQ(o.value_or(2LL), 1);
  }
  {
    optional<int> o;
    CHECK_EQ(std::move(o).value_or(2LL), 2);
  }
  {
    optional<int> o(1);
    CHECK_EQ(std::move(o).value_or(2LL), 1);
  }
  {
    optional<int&> o;
    CHECK_EQ(o.value_or(2LL), 2);
  }
  {
    int val = 1;
    optional<int&> o(val);
    CHECK_EQ(o.value_or(2LL), 1);
  }
  {
    optional<int&> o;
    CHECK_EQ(std::move(o).value_or(2LL), 2);
  }
  {
    int val = 1;
    optional<int&> o(val);
    CHECK_EQ(std::move(o).value_or(2LL), 1);
  }
}

TEST_CASE("emplace")
{
  {
    optional<void> o;
    CHECK(!o);
    o.emplace();
    CHECK(o);
  }
  {
    optional<void> o(in_place);
    CHECK(o);
    o.emplace();
    CHECK(o);
  }
  {
    optional<int> o;
    CHECK(!o);
    o.emplace(1);
    CHECK_EQ(o.value(), 1);
  }
  {
    optional<int> o(1);
    CHECK(o);
    o.emplace(1);
    CHECK_EQ(o.value(), 1);
  }
  {
    optional<dc<int>> o;
    CHECK(!o);
    o.emplace({ 3, 4, 5 });
    CHECK_EQ(o.value(), dc<int> { 3, 4, 5 });
  }
  {
    optional<dc<int>> o(in_place, { 0, 1, 2 });
    CHECK(o);
    o.emplace({ 3, 4, 5 });
    CHECK_EQ(o.value(), dc<int> { 3, 4, 5 });
  }
  {
    optional<int&> o;
    CHECK(!o);
    int val = 1;
    o.emplace(val);
    CHECK_EQ(o.value(), 1);
  }
  {
    int val = 1;
    optional<int&> o(val);
    CHECK(o);
    int val2 = 2;
    o.emplace(val2);
    CHECK_EQ(o.value(), 2);
  }
}

TEST_CASE("reset")
{
  {
    optional<void> o(in_place);
    CHECK(o);
    o.reset();
    CHECK(!o);
  }
  {
    optional<int> o(1);
    CHECK(o);
    o.reset();
    CHECK(!o);
  }
  {
    int val = 1;
    optional<int&> o(val);
    CHECK(o);
    o.reset();
    CHECK(!o);
  }
}

TEST_CASE("and_then")
{
  auto v_to_s = []() -> optional<std::string> { return "1"; };
  {
    auto n = optional<void>();
    CHECK(!n.and_then(v_to_s));
    auto v = optional<void>(in_place);
    CHECK_EQ(v.and_then(v_to_s).value(), "1");
  }
  {
    const auto n = optional<void>();
    CHECK(!n.and_then(v_to_s));
    const auto v = optional<void>(in_place);
    CHECK_EQ(v.and_then(v_to_s).value(), "1");
  }
  {
    auto n = optional<void>();
    CHECK(!std::move(n).and_then(v_to_s));
    auto v = optional<void>(in_place);
    CHECK_EQ(std::move(v).and_then(v_to_s).value(), "1");
  }
  {
    const auto n = optional<void>();
    CHECK(!std::move(n).and_then(v_to_s));
    const auto v = optional<void>(in_place);
    CHECK_EQ(std::move(v).and_then(v_to_s).value(), "1");
  }
  auto i_to_s
      = [](int i) -> optional<std::string> { return std::to_string(i); };
  {
    auto n = optional<int>();
    CHECK(!n.and_then(i_to_s));
    auto v = optional<int>(1);
    CHECK_EQ(v.and_then(i_to_s).value(), "1");
  }
  {
    const auto n = optional<int>();
    CHECK(!n.and_then(i_to_s));
    const auto v = optional<int>(1);
    CHECK_EQ(v.and_then(i_to_s).value(), "1");
  }
  {
    auto n = optional<int>();
    CHECK(!std::move(n).and_then(i_to_s));
    auto v = optional<int>(1);
    CHECK_EQ(std::move(v).and_then(i_to_s).value(), "1");
  }
  {
    const auto n = optional<int>();
    CHECK(!std::move(n).and_then(i_to_s));
    const auto v = optional<int>(1);
    CHECK_EQ(std::move(v).and_then(i_to_s).value(), "1");
  }
}

TEST_CASE("transform")
{
  auto v_to_s = []() -> optional<std::string> { return "1"; };
  {
    auto n = optional<void>();
    CHECK(!n.transform(v_to_s));
    auto v = optional<void>(in_place);
    CHECK_EQ(v.transform(v_to_s).value(), "1");
  }
  {
    const auto n = optional<void>();
    CHECK(!n.transform(v_to_s));
    const auto v = optional<void>(in_place);
    CHECK_EQ(v.transform(v_to_s).value(), "1");
  }
  {
    auto n = optional<void>();
    CHECK(!std::move(n).transform(v_to_s));
    auto v = optional<void>(in_place);
    CHECK_EQ(std::move(v).transform(v_to_s).value(), "1");
  }
  {
    const auto n = optional<void>();
    CHECK(!std::move(n).transform(v_to_s));
    const auto v = optional<void>(in_place);
    CHECK_EQ(std::move(v).transform(v_to_s).value(), "1");
  }
  auto i_to_s
      = [](int i) -> optional<std::string> { return std::to_string(i); };
  {
    auto n = optional<int>();
    CHECK(!n.transform(i_to_s));
    auto v = optional<int>(1);
    CHECK_EQ(v.transform(i_to_s).value(), "1");
  }
  {
    const auto n = optional<int>();
    CHECK(!n.transform(i_to_s));
    const auto v = optional<int>(1);
    CHECK_EQ(v.transform(i_to_s).value(), "1");
  }
  {
    auto n = optional<int>();
    CHECK(!std::move(n).transform(i_to_s));
    auto v = optional<int>(1);
    CHECK_EQ(std::move(v).transform(i_to_s).value(), "1");
  }
  {
    const auto n = optional<int>();
    CHECK(!std::move(n).transform(i_to_s));
    const auto v = optional<int>(1);
    CHECK_EQ(std::move(v).transform(i_to_s).value(), "1");
  }
}

TEST_CASE("or_else")
{
  auto fv = []() -> optional<void> { return optional<void>(in_place); };
  {
    auto n = optional<void>();
    CHECK(n.or_else(fv));
    auto v = optional<void>(in_place);
    CHECK(v.or_else(fv));
  }
  {
    const auto n = optional<void>();
    CHECK(n.or_else(fv));
    const auto v = optional<void>(in_place);
    CHECK(v.or_else(fv));
  }
  {
    auto n = optional<void>();
    CHECK(std::move(n).or_else(fv));
    auto v = optional<void>(in_place);
    CHECK(std::move(v).or_else(fv));
  }
  {
    const auto n = optional<void>();
    CHECK(std::move(n).or_else(fv));
    const auto v = optional<void>(in_place);
    CHECK(std::move(v).or_else(fv));
  }
  auto fi = []() -> optional<int> { return 2; };
  {
    auto n = optional<int>();
    CHECK_EQ(n.or_else(fi).value(), 2);
    auto v = optional<int>(1);
    CHECK_EQ(v.or_else(fi).value(), 1);
  }
  {
    const auto n = optional<int>();
    CHECK_EQ(n.or_else(fi).value(), 2);
    const auto v = optional<int>(1);
    CHECK_EQ(v.or_else(fi).value(), 1);
  }
  {
    auto n = optional<int>();
    CHECK_EQ(std::move(n).or_else(fi).value(), 2);
    auto v = optional<int>(1);
    CHECK_EQ(std::move(v).or_else(fi).value(), 1);
  }
  {
    const auto n = optional<int>();
    CHECK_EQ(std::move(n).or_else(fi).value(), 2);
    const auto v = optional<int>(1);
    CHECK_EQ(std::move(v).or_else(fi).value(), 1);
  }
}

TEST_CASE("swap")
{
  {
    auto a = optional<int>();
    auto b = optional<int>();
    swap(a, b);
    CHECK(!a);
    CHECK(!b);
  }
  {
    auto a = optional<int>();
    auto b = optional<int>(1);
    swap(a, b);
    CHECK_EQ(a.value(), 1);
    CHECK(!b);
  }
  {
    auto a = optional<int>(1);
    auto b = optional<int>();
    swap(a, b);
    CHECK(!a);
    CHECK_EQ(b.value(), 1);
  }
  {
    auto a = optional<int>(1);
    auto b = optional<int>(2);
    swap(a, b);
    CHECK_EQ(a.value(), 2);
    CHECK_EQ(b.value(), 1);
  }
}

TEST_CASE("compare")
{
  {
    const auto n = optional<void>();
    const auto v = optional<void>(in_place);
    CHECK(n == n);
    CHECK_FALSE(n == v);
    CHECK_FALSE(v == n);
    CHECK(v == v);
    CHECK_FALSE(n != n);
    CHECK(n != v);
    CHECK(v != n);
    CHECK_FALSE(v != v);
    CHECK_FALSE(n < n);
    CHECK(n < v);
    CHECK_FALSE(v < n);
    CHECK_FALSE(v < v);
    CHECK(n <= n);
    CHECK(n <= v);
    CHECK_FALSE(v <= n);
    CHECK(v <= v);
    CHECK_FALSE(n > n);
    CHECK_FALSE(n > v);
    CHECK(v > n);
    CHECK_FALSE(v > v);
    CHECK(n >= n);
    CHECK_FALSE(n >= v);
    CHECK(v >= n);
    CHECK(v >= v);
  }
  {
    const auto n = optional<int>();
    const auto v = optional<int64_t>(1);
    CHECK(n == n);
    CHECK_FALSE(n == v);
    CHECK_FALSE(v == n);
    CHECK(v == v);
    CHECK_FALSE(n != n);
    CHECK(n != v);
    CHECK(v != n);
    CHECK_FALSE(v != v);
    CHECK_FALSE(n < n);
    CHECK(n < v);
    CHECK_FALSE(v < n);
    CHECK_FALSE(v < v);
    CHECK(n <= n);
    CHECK(n <= v);
    CHECK_FALSE(v <= n);
    CHECK(v <= v);
    CHECK_FALSE(n > n);
    CHECK_FALSE(n > v);
    CHECK(v > n);
    CHECK_FALSE(v > v);
    CHECK(n >= n);
    CHECK_FALSE(n >= v);
    CHECK(v >= n);
    CHECK(v >= v);
  }
  {
    const auto n = optional<void>();
    const auto v = optional<void>(in_place);
    CHECK(n == nullopt);
    CHECK(nullopt == n);
    CHECK_FALSE(v == nullopt);
    CHECK_FALSE(nullopt == v);
    CHECK_FALSE(n != nullopt);
    CHECK_FALSE(nullopt != n);
    CHECK(v != nullopt);
    CHECK(nullopt != v);
    CHECK_FALSE(n < nullopt);
    CHECK_FALSE(nullopt < n);
    CHECK_FALSE(v < nullopt);
    CHECK(nullopt < v);
    CHECK(n <= nullopt);
    CHECK(nullopt <= n);
    CHECK_FALSE(v <= nullopt);
    CHECK(nullopt <= v);
    CHECK_FALSE(n > nullopt);
    CHECK_FALSE(nullopt > n);
    CHECK(v > nullopt);
    CHECK_FALSE(nullopt > v);
    CHECK(n >= nullopt);
    CHECK(nullopt >= n);
    CHECK(v >= nullopt);
    CHECK_FALSE(nullopt >= v);
  }
  {
    const auto n = optional<int>();
    const auto v = optional<int>(1);
    CHECK(n == nullopt);
    CHECK(nullopt == n);
    CHECK_FALSE(v == nullopt);
    CHECK_FALSE(nullopt == v);
    CHECK_FALSE(n != nullopt);
    CHECK_FALSE(nullopt != n);
    CHECK(v != nullopt);
    CHECK(nullopt != v);
    CHECK_FALSE(n < nullopt);
    CHECK_FALSE(nullopt < n);
    CHECK_FALSE(v < nullopt);
    CHECK(nullopt < v);
    CHECK(n <= nullopt);
    CHECK(nullopt <= n);
    CHECK_FALSE(v <= nullopt);
    CHECK(nullopt <= v);
    CHECK_FALSE(n > nullopt);
    CHECK_FALSE(nullopt > n);
    CHECK(v > nullopt);
    CHECK_FALSE(nullopt > v);
    CHECK(n >= nullopt);
    CHECK(nullopt >= n);
    CHECK(v >= nullopt);
    CHECK_FALSE(nullopt >= v);
  }
  {
    const auto n = optional<int>();
    const auto v = optional<int>(1);
    CHECK_FALSE(n == 1);
    CHECK_FALSE(1 == n);
    CHECK(v == 1);
    CHECK(1 == v);
    CHECK(n != 1);
    CHECK(1 != n);
    CHECK_FALSE(v != 1);
    CHECK_FALSE(1 != v);
    CHECK(n < 1);
    CHECK_FALSE(1 < n);
    CHECK_FALSE(v < 1);
    CHECK_FALSE(1 < v);
    CHECK(n <= 1);
    CHECK_FALSE(1 <= n);
    CHECK(v <= 1);
    CHECK(1 <= v);
    CHECK_FALSE(n > 1);
    CHECK(1 > n);
    CHECK_FALSE(v > 1);
    CHECK_FALSE(1 > v);
    CHECK_FALSE(n >= 1);
    CHECK(1 >= n);
    CHECK(v >= 1);
    CHECK(1 >= v);
  }
}

#ifdef SHALLTEAR_HAS_CXX17

TEST_CASE("deduction guide")
{
  STATIC_ASSERT(is_same<decltype(optional(1)), optional<int>>::value);
  STATIC_ASSERT(
      is_same<decltype(optional(dc<int>(1))), optional<dc<int>>>::value);
}

#endif

TEST_CASE("make_optional")
{
  auto o = make_optional(1);
  CHECK_EQ(o.value(), 1);

  auto i = make_optional<dc<int>>(1);
  CHECK_EQ(i.value(), dc<int>(1));

  auto vec = make_optional<dc<int>>({ 0, 1, 2 });
  CHECK_EQ(vec.value(), dc<int> { 0, 1, 2 });
}

TEST_SUITE_END();
