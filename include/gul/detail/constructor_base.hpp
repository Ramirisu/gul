//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/utility.hpp>

GUL_NAMESPACE_BEGIN

namespace detail {

template <bool EnableDefaultConstruct>
struct default_construct_base {
  default_construct_base() noexcept = default;
  default_construct_base(in_place_t) noexcept { }
  default_construct_base(const default_construct_base&) noexcept = default;
  default_construct_base(default_construct_base&&) noexcept = default;
  default_construct_base&
  operator=(const default_construct_base&) noexcept = default;
  default_construct_base&
  operator=(default_construct_base&&) noexcept = default;
};

template <>
struct default_construct_base<false> {
  default_construct_base() noexcept = delete;
  default_construct_base(in_place_t) noexcept { }
  default_construct_base(const default_construct_base&) noexcept = default;
  default_construct_base(default_construct_base&&) noexcept = default;
  default_construct_base&
  operator=(const default_construct_base&) noexcept = default;
  default_construct_base&
  operator=(default_construct_base&&) noexcept = default;
};

template <bool EnableCopyConstruct>
struct copy_construct_base {
};

template <>
struct copy_construct_base<false> {
  copy_construct_base() noexcept = default;
  copy_construct_base(const copy_construct_base&) noexcept = delete;
  copy_construct_base(copy_construct_base&&) noexcept = default;
  copy_construct_base& operator=(const copy_construct_base&) noexcept = default;
  copy_construct_base& operator=(copy_construct_base&&) noexcept = default;
};

template <bool EnableMoveConstruct>
struct move_construct_base {
};

template <>
struct move_construct_base<false> {
  move_construct_base() noexcept = default;
  move_construct_base(const move_construct_base&) noexcept = default;
  move_construct_base(move_construct_base&&) noexcept = delete;
  move_construct_base& operator=(const move_construct_base&) noexcept = default;
  move_construct_base& operator=(move_construct_base&&) noexcept = default;
};

template <bool EnableCopyAssign>
struct copy_assign_base {
};

template <>
struct copy_assign_base<false> {
  copy_assign_base() noexcept = default;
  copy_assign_base(const copy_assign_base&) noexcept = default;
  copy_assign_base(copy_assign_base&&) noexcept = default;
  copy_assign_base& operator=(const copy_assign_base&) noexcept = delete;
  copy_assign_base& operator=(copy_assign_base&&) noexcept = default;
};

template <bool EnableMoveAssign>
struct move_assign_base {
};

template <>
struct move_assign_base<false> {
  move_assign_base() noexcept = default;
  move_assign_base(const move_assign_base&) noexcept = default;
  move_assign_base(move_assign_base&&) noexcept = default;
  move_assign_base& operator=(const move_assign_base&) noexcept = default;
  move_assign_base& operator=(move_assign_base&&) noexcept = delete;
};

}

GUL_NAMESPACE_END
