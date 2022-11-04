#pragma once

#include <ds/config.hpp>

namespace ds {
struct in_place_t {
  explicit in_place_t() = default;
};

DS_CXX17_INLINE constexpr in_place_t in_place {};
}
