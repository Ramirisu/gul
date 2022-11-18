//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

GUL_NAMESPACE_BEGIN

struct in_place_t {
  explicit in_place_t() = default;
};

GUL_CXX17_INLINE constexpr in_place_t in_place {};

GUL_NAMESPACE_END