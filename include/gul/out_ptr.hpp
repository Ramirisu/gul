//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/out_ptr.hpp>

#include <gul/type_traits.hpp>
#include <gul/utility.hpp>

#include <memory>

GUL_NAMESPACE_BEGIN

namespace out_ptr_detail {

template <typename Pointer>
struct out_ptr_base {
  operator void**() const noexcept
  {
    return reinterpret_cast<void**>(static_cast<Pointer*>(*this));
  }
};

template <>
struct out_ptr_base<void> { };

}

template <typename Smart, typename Pointer, typename... Args>
class out_ptr_t : public out_ptr_detail::out_ptr_base<Pointer> {
  using pointer_of_or_t = detail::out_ptr_detail::pointer_of_or<Smart, Pointer>;

  template <typename Tuple, std::size_t... Is>
  void apply_impl(std::true_type, Tuple&& t, index_sequence<Is...>)
  {
#if defined(GUL_CXX_COMPILER_MSVC2015)
    GUL_UNUSED(t);
#endif
    sptr_.reset(static_cast<pointer_of_or_t>(ptr_),
                std::get<Is>(std::forward<Tuple>(t))...);
  }

  template <typename Tuple, std::size_t... Is>
  void apply_impl(std::false_type, Tuple&& t, index_sequence<Is...>)
  {
#if defined(GUL_CXX_COMPILER_MSVC2015)
    GUL_UNUSED(t);
#endif
    sptr_ = Smart(static_cast<pointer_of_or_t>(ptr_),
                  std::get<Is>(std::forward<Tuple>(t))...);
  }

public:
  static_assert(
      !is_specialization_of<Smart, std::shared_ptr>::value
          || sizeof...(Args) > 0,
      "[out_ptr_t] std::shared_ptr should be reset with a custom deleter");

  explicit out_ptr_t(Smart& sptr, Args... args)
      : sptr_(sptr)
      , args_(std::forward<Args>(args)...)
      , ptr_()
  {
  }

  out_ptr_t(const out_ptr_t&) = delete;

  out_ptr_t(out_ptr_t&& other)
      : sptr_(other.sptr_)
      , args_(std::move(other).args_)
      , ptr_(exchange(other.ptr_, Pointer()))
  {
  }

  ~out_ptr_t()
  {
    if (ptr_) {
      apply_impl(detail::out_ptr_detail::is_resettable<void, Smart, Pointer,
                                                       Args...> {},
                 std::move(args_), index_sequence_for<Args...> {});
    }
  }

  operator Pointer*() const noexcept
  {
    return std::addressof(const_cast<Pointer&>(ptr_));
  }

private:
  Smart& sptr_;
  std::tuple<Args...> args_;
  Pointer ptr_;
};

template <typename Pointer = void, typename Smart, typename... Args>
auto out_ptr(Smart& sptr, Args&&... args)
    -> out_ptr_t<Smart,
                 conditional_t<std::is_void<Pointer>::value,
                               detail::out_ptr_detail::pointer_of<Smart>,
                               Pointer>,
                 Args...>
{
  return out_ptr_t<
      Smart,
      conditional_t<std::is_void<Pointer>::value,
                    detail::out_ptr_detail::pointer_of<Smart>, Pointer>,
      Args...>(sptr, std::forward<Args>(args)...);
}

template <typename Smart, typename Pointer, typename... Args>
class inout_ptr_t : public out_ptr_detail::out_ptr_base<Pointer> {
  using pointer_of_or_t = detail::out_ptr_detail::pointer_of_or<Smart, Pointer>;

  template <typename Tuple, std::size_t... Is>
  void
  apply_impl(std::true_type, std::false_type, Tuple&& t, index_sequence<Is...>)
  {
#if defined(GUL_CXX_COMPILER_MSVC2015)
    GUL_UNUSED(t);
#endif
    sptr_ = Smart(static_cast<pointer_of_or_t>(ptr_),
                  std::get<Is>(std::forward<Tuple>(t))...);
  }

  template <typename Tuple, std::size_t... Is>
  void
  apply_impl(std::false_type, std::true_type, Tuple&& t, index_sequence<Is...>)
  {
#if defined(GUL_CXX_COMPILER_MSVC2015)
    GUL_UNUSED(t);
#endif
    sptr_.release();
    sptr_.reset(static_cast<pointer_of_or_t>(ptr_),
                std::get<Is>(std::forward<Tuple>(t))...);
  }

  template <typename Tuple, std::size_t... Is>
  void
  apply_impl(std::false_type, std::false_type, Tuple&& t, index_sequence<Is...>)
  {
#if defined(GUL_CXX_COMPILER_MSVC2015)
    GUL_UNUSED(t);
#endif
    sptr_.release();
    sptr_ = Smart(static_cast<pointer_of_or_t>(ptr_),
                  std::get<Is>(std::forward<Tuple>(t))...);
  }

public:
  static_assert(
      !is_specialization_of<Smart, std::shared_ptr>::value,
      "[inout_ptr_t] Smart must not be a specialization of std::shared_ptr");

  explicit inout_ptr_t(Smart& sptr, Args... args)
      : sptr_(sptr)
      , args_(std::forward<Args>(args)...)
      , ptr_(detail::out_ptr_detail::get_pointer<Smart>(sptr))
  {
  }

  inout_ptr_t(const inout_ptr_t&) = delete;

  inout_ptr_t(inout_ptr_t&& other)
      : sptr_(other.sptr_)
      , args_(std::move(other).args_)
      , ptr_(exchange(other.ptr_, Pointer()))
  {
  }

  ~inout_ptr_t()
  {
    if (ptr_) {
      apply_impl(std::is_pointer<Smart> {},
                 detail::out_ptr_detail::is_resettable<void, Smart, Pointer,
                                                       Args...> {},
                 std::move(args_), make_index_sequence<sizeof...(Args)> {});
    }
  }

  operator Pointer*() const noexcept
  {
    return std::addressof(const_cast<Pointer&>(ptr_));
  }

private:
  Smart& sptr_;
  std::tuple<Args...> args_;
  Pointer ptr_;
};

template <typename Pointer = void, typename Smart, typename... Args>
auto inout_ptr(Smart& sptr, Args&&... args)
    -> inout_ptr_t<Smart,
                   conditional_t<std::is_void<Pointer>::value,
                                 detail::out_ptr_detail::pointer_of<Smart>,
                                 Pointer>,
                   Args...>
{
  return inout_ptr_t<
      Smart,
      conditional_t<std::is_void<Pointer>::value,
                    detail::out_ptr_detail::pointer_of<Smart>, Pointer>,
      Args...>(sptr, std::forward<Args>(args)...);
}

GUL_NAMESPACE_END
