#include <ds_test.h>

#include <ds/expected.hpp>

using namespace ds;

TEST_SUITE_BEGIN("expected");

namespace {
template <typename T>
class default_constructible : std::vector<T> {
  using __base_type = std::vector<T>;

public:
  default_constructible() noexcept = default;

  default_constructible(int value)
      : __base_type({ value })
  {
  }

  default_constructible(std::initializer_list<T> init)
      : __base_type(std::move(init))
  {
  }

  default_constructible(__base_type init)
      : __base_type(std::move(init))
  {
  }

  default_constructible(const default_constructible&) noexcept = default;

  default_constructible(default_constructible&&) noexcept = default;

  default_constructible& operator=(const default_constructible&) noexcept
      = default;

  default_constructible& operator=(default_constructible&&) noexcept = default;

  using __base_type::size;

  friend bool operator==(const default_constructible& lhs,
                         const default_constructible& rhs) noexcept
  {
    return static_cast<const __base_type&>(lhs)
        == static_cast<const __base_type&>(rhs);
  }
};

template <typename T>
using dc = default_constructible<T>;

template <typename T>
class non_default_constructible : std::vector<T> {
  using __base_type = std::vector<T>;

public:
  non_default_constructible(int value)
      : __base_type({ value })
  {
  }

  non_default_constructible(std::initializer_list<T> init)
      : __base_type(std::move(init))
  {
  }

  non_default_constructible(__base_type init)
      : __base_type(std::move(init))
  {
  }

  non_default_constructible(const non_default_constructible&) noexcept
      = default;

  non_default_constructible(non_default_constructible&&) noexcept = default;

  non_default_constructible&
  operator=(const non_default_constructible&) noexcept
      = default;

  non_default_constructible& operator=(non_default_constructible&&) noexcept
      = default;

  using __base_type::size;

  friend bool operator==(const non_default_constructible& lhs,
                         const non_default_constructible& rhs) noexcept
  {
    return static_cast<const __base_type&>(lhs)
        == static_cast<const __base_type&>(rhs);
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
  static_assert(is_same<Op<Test>, Expected>::value, "");
};
}

TEST_CASE("unexpected")
{
  {
    auto s = ds::unexpected<int>(1);
    CHECK_EQ(s.error(), 1);
  }
  {
    auto s = ds::unexpected<dc<int>>(in_place, 1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(in_place, { 0, 1, 2 });
    CHECK_EQ(s.error(), dc<int>({ 0, 1, 2 }));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    ds::unexpected<dc<int>> d(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    ds::unexpected<dc<int>> d(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    ds::unexpected<dc<int>> d(std::move(s));
    CHECK_EQ(s.error(), dc<int>());
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    ds::unexpected<dc<int>> d(std::move(s));
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    auto d = s;
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    auto d = s;
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    auto d = std::move(s);
    CHECK_EQ(s.error(), dc<int>());
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    auto d = std::move(s);
    CHECK_EQ(s.error(), dc<int>(1));
    CHECK_EQ(d.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    CHECK_EQ(s.error(), dc<int>(1));
  }
  {
    auto s = ds::unexpected<dc<int>>(1);
    CHECK_EQ(std::move(s).error(), dc<int>(1));
  }
  {
    const auto s = ds::unexpected<dc<int>>(1);
    CHECK_EQ(std::move(s).error(), dc<int>(1));
  }
  {
    auto lhs = ds::unexpected<dc<int>>(1);
    auto rhs = ds::unexpected<dc<int>>(2);
    swap(lhs, rhs);
    CHECK_EQ(lhs.error(), dc<int>(2));
    CHECK_EQ(rhs.error(), dc<int>(1));
  }
}

TEST_CASE("trivially")
{
  using type_void = expected<void, int>;
  static_assert(is_trivially_copy_constructible<type_void>::value, "");
  static_assert(is_trivially_move_constructible<type_void>::value, "");
  static_assert(is_trivially_copy_assignable<type_void>::value, "");
  static_assert(is_trivially_move_assignable<type_void>::value, "");
  static_assert(is_trivially_destructible<type_void>::value, "");

  using type_int = expected<int, int>;
  static_assert(is_trivially_copy_constructible<type_int>::value, "");
  static_assert(is_trivially_move_constructible<type_int>::value, "");
  static_assert(is_trivially_copy_assignable<type_int>::value, "");
  static_assert(is_trivially_move_assignable<type_int>::value, "");
  static_assert(is_trivially_destructible<type_int>::value, "");

  using type_int_dc = expected<int, dc<int>>;
  static_assert(!is_trivially_copy_constructible<type_int_dc>::value, "");
  static_assert(!is_trivially_move_constructible<type_int_dc>::value, "");
  static_assert(!is_trivially_copy_assignable<type_int_dc>::value, "");
  static_assert(!is_trivially_move_assignable<type_int_dc>::value, "");
  static_assert(!is_trivially_destructible<type_int_dc>::value, "");

  using type_dc_int = expected<dc<int>, int>;
  static_assert(!is_trivially_copy_constructible<type_dc_int>::value, "");
  static_assert(!is_trivially_move_constructible<type_dc_int>::value, "");
  static_assert(!is_trivially_copy_assignable<type_dc_int>::value, "");
  static_assert(!is_trivially_move_assignable<type_dc_int>::value, "");
  static_assert(!is_trivially_destructible<type_dc_int>::value, "");

  using type_dc = expected<dc<int>, dc<int>>;
  static_assert(!is_trivially_copy_constructible<type_dc>::value, "");
  static_assert(!is_trivially_move_constructible<type_dc>::value, "");
  static_assert(!is_trivially_copy_assignable<type_dc>::value, "");
  static_assert(!is_trivially_move_assignable<type_dc>::value, "");
  static_assert(!is_trivially_destructible<type_dc>::value, "");
}

TEST_CASE("default constructor")
{
  static_assert(is_default_constructible<expected<void, int>>::value, "");
  static_assert(is_default_constructible<expected<int, int>>::value, "");
  static_assert(is_default_constructible<expected<dc<int>, int>>::value, "");
  static_assert(!is_default_constructible<expected<ndc<int>, int>>::value, "");
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
    auto d = ds::unexpected<int>(1);
    auto b = expected<ndc<int>, int>(d);
    CHECK(!b);
  }
  {
    auto d = ds::unexpected<int>(1);
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
    auto une = ds::unexpected<int>(1);
    exp = une;
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<void, int>();
    auto une = ds::unexpected<int>(1);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<int, int>();
    auto une = ds::unexpected<int>(1);
    exp = une;
    CHECK_EQ(exp.error(), 1);
  }
  {
    auto exp = expected<int, int>();
    auto une = ds::unexpected<int>(1);
    exp = std::move(une);
    CHECK_EQ(exp.error(), 1);
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
  assert_is_same<deref_op, const expected<int, int>, const int&&>();
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
  assert_is_same<fn_value, const expected<int, int>, const int&&>();
  {
    auto exp = expected<void, int>();
    static_assert_same<decltype(exp.value()), void>();
  }
  {
    const auto exp = expected<void, int>();
    static_assert_same<decltype(exp.value()), void>();
  }
  {
    auto exp = expected<void, int>();
    static_assert_same<decltype(std::move(exp).value()), void>();
  }
  {
    const auto exp = expected<void, int>();
    static_assert_same<decltype(std::move(exp).value()), void>();
  }
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
  auto v_to_s = []() -> expected<std::string, int> { return "1"; };
  {
    auto u = expected<void, int>(unexpect, 0);
    CHECK_EQ(u.and_then(v_to_s).error(), 0);
    auto e = expected<void, int>();
    CHECK_EQ(e.and_then(v_to_s).value(), "1");
  }
  {
    const auto u = expected<void, int>(unexpect, 0);
    CHECK_EQ(u.and_then(v_to_s).error(), 0);
    const auto e = expected<void, int>();
    CHECK_EQ(e.and_then(v_to_s).value(), "1");
  }
  {
    auto u = expected<void, int>(unexpect, 0);
    CHECK_EQ(std::move(u).and_then(v_to_s).error(), 0);
    auto e = expected<void, int>();
    CHECK_EQ(std::move(e).and_then(v_to_s).value(), "1");
  }
  {
    const auto u = expected<void, int>(unexpect, 0);
    CHECK_EQ(std::move(u).and_then(v_to_s).error(), 0);
    const auto e = expected<void, int>();
    CHECK_EQ(std::move(e).and_then(v_to_s).value(), "1");
  }
  auto i_to_s
      = [](int i) -> expected<std::string, int> { return std::to_string(i); };
  {
    auto u = expected<int, int>(unexpect, 0);
    CHECK_EQ(u.and_then(i_to_s).error(), 0);
    auto e = expected<int, int>(1);
    CHECK_EQ(e.and_then(i_to_s).value(), "1");
  }
  {
    const auto u = expected<int, int>(unexpect, 0);
    CHECK_EQ(u.and_then(i_to_s).error(), 0);
    const auto e = expected<int, int>(1);
    CHECK_EQ(e.and_then(i_to_s).value(), "1");
  }
  {
    auto u = expected<int, int>(unexpect, 0);
    CHECK_EQ(std::move(u).and_then(i_to_s).error(), 0);
    auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).and_then(i_to_s).value(), "1");
  }
  {
    const auto u = expected<int, int>(unexpect, 0);
    CHECK_EQ(std::move(u).and_then(i_to_s).error(), 0);
    const auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).and_then(i_to_s).value(), "1");
  }
}

TEST_CASE("or_else")
{
  auto i_to_s = [](int i) -> expected<int, std::string> {
    return ds::unexpected<std::string>(std::to_string(i));
  };
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.or_else(i_to_s).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(e.or_else(i_to_s).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.or_else(i_to_s).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(e.or_else(i_to_s).value(), 1);
  }
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).or_else(i_to_s).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).or_else(i_to_s).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).or_else(i_to_s).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).or_else(i_to_s).value(), 1);
  }
}

TEST_CASE("transform")
{
  auto v_to_s = []() -> std::string { return "1"; };
  {
    auto u = expected<void, int>(unexpect);
    CHECK_EQ(u.transform(v_to_s).error(), 0);
    auto e = expected<void, int>();
    CHECK_EQ(e.transform(v_to_s).value(), "1");
  }
  {
    const auto u = expected<void, int>(unexpect);
    CHECK_EQ(u.transform(v_to_s).error(), 0);
    const auto e = expected<void, int>();
    CHECK_EQ(e.transform(v_to_s).value(), "1");
  }
  {
    auto u = expected<void, int>(unexpect);
    CHECK_EQ(std::move(u).transform(v_to_s).error(), 0);
    auto e = expected<void, int>();
    CHECK_EQ(std::move(e).transform(v_to_s).value(), "1");
  }
  {
    const auto u = expected<void, int>(unexpect);
    CHECK_EQ(std::move(u).transform(v_to_s).error(), 0);
    const auto e = expected<void, int>();
    CHECK_EQ(std::move(e).transform(v_to_s).value(), "1");
  }
  auto i_to_s = [](int i) -> std::string { return std::to_string(i); };
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform(i_to_s).error(), 0);
    auto e = expected<int, int>(1);
    CHECK_EQ(e.transform(i_to_s).value(), "1");
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform(i_to_s).error(), 0);
    const auto e = expected<int, int>(1);
    CHECK_EQ(e.transform(i_to_s).value(), "1");
  }
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform(i_to_s).error(), 0);
    auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform(i_to_s).value(), "1");
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform(i_to_s).error(), 0);
    const auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform(i_to_s).value(), "1");
  }
}

TEST_CASE("transform_error")
{
  auto i_to_s = [](int i) -> std::string { return std::to_string(i); };
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform_error(i_to_s).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(e.transform_error(i_to_s).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(u.transform_error(i_to_s).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(e.transform_error(i_to_s).value(), 1);
  }
  {
    auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(i_to_s).error(), "0");
    auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform_error(i_to_s).value(), 1);
  }
  {
    const auto u = expected<int, int>(unexpect);
    CHECK_EQ(std::move(u).transform_error(i_to_s).error(), "0");
    const auto e = expected<int, int>(1);
    CHECK_EQ(std::move(e).transform_error(i_to_s).value(), 1);
  }
}

TEST_CASE("swap")
{
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
}

TEST_CASE("compare")
{
  CHECK(expected<void, int>() == expected<void, int>());
  CHECK(expected<void, int>(unexpect) == expected<void, int>(unexpect));
  CHECK(expected<int, int>() == expected<int, int>());
  CHECK(expected<int, int>(unexpect) == expected<int, int>(unexpect));
  CHECK(expected<int, int>() == 0);
  CHECK(expected<int, int>(unexpect) == ds::unexpected<int>(0));
}

TEST_SUITE_END();
