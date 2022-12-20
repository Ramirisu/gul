//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/constructor_base.hpp>

#include <gul/invoke.hpp>
#include <gul/type_traits.hpp>
#include <gul/utility.hpp>

GUL_NAMESPACE_BEGIN

struct nullopt_t {
  struct tag_t {
    explicit constexpr tag_t() = default;
  };
  explicit constexpr nullopt_t(tag_t, tag_t) { }
};

GUL_CXX17_INLINE constexpr nullopt_t nullopt { nullopt_t::tag_t {},
                                               nullopt_t::tag_t {} };

#if !GUL_NO_EXCEPTIONS
class bad_optional_access : public std::exception {
public:
  bad_optional_access() noexcept = default;

  ~bad_optional_access() noexcept override = default;

  bad_optional_access(const bad_optional_access&) noexcept = default;

  bad_optional_access& operator=(const bad_optional_access&) noexcept = default;

  const char* what() const noexcept override
  {
    return "bad optional access";
  }
};
#endif

namespace detail {
struct optional_throw_base {
  void throw_bad_optional_access() const
  {
#if GUL_NO_EXCEPTIONS
    std::abort();
#else
    throw bad_optional_access();
#endif
  }
};

template <typename T, bool = std::is_trivially_destructible<T>::value>
struct optional_destruct_base : optional_throw_base {
  union {
    char nul_;
    T val_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR optional_destruct_base() noexcept
      : nul_()
      , has_(false)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit optional_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  GUL_CXX14_CONSTEXPR void reset() noexcept
  {
    has_ = false;
  }
};

template <typename T>
struct optional_destruct_base<T, false> : optional_throw_base {
  union {
    char nul_;
    T val_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR optional_destruct_base() noexcept
      : nul_()
      , has_(false)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit optional_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  ~optional_destruct_base()
  {
    if (has_) {
      val_.~T();
    }
  }

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  GUL_CXX14_CONSTEXPR void reset() noexcept
  {
    if (has_) {
      val_.~T();
      has_ = false;
    }
  }
};

template <typename T, bool = std::is_reference<T>::value>
struct optional_storage_base : optional_destruct_base<T> {
  using optional_destruct_base<T>::optional_destruct_base;

  GUL_CXX14_CONSTEXPR add_pointer_t<T> operator->() noexcept
  {
    GUL_ASSERT(this->has_);
    return std::addressof(this->val_);
  }

  GUL_CXX14_CONSTEXPR add_pointer_t<const T> operator->() const noexcept
  {
    GUL_ASSERT(this->has_);
    return std::addressof(this->val_);
  }

  GUL_CXX14_CONSTEXPR T& operator*() & noexcept
  {
    GUL_ASSERT(this->has_);
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR const T& operator*() const& noexcept
  {
    GUL_ASSERT(this->has_);
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR T&& operator*() && noexcept
  {
    GUL_ASSERT(this->has_);
    return std::move(this->val_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const T&& operator*() const&& noexcept
  {
    GUL_ASSERT(this->has_);
    return std::move(this->val_);
  }
#endif

  GUL_CXX14_CONSTEXPR T& value() &
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR const T& value() const&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR T&& value() &&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return std::move(this->val_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const T&& value() const&&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return std::move(this->val_);
  }
#endif

  template <typename... Args>
  GUL_CXX14_CONSTEXPR T& construct(Args&&... args)
  {
    GUL_ASSERT(!this->has_);
    auto& val
        = *(::new (std::addressof(this->val_)) T(std::forward<Args>(args)...));
    this->has_ = true;
    return val;
  }

  GUL_CXX14_CONSTEXPR void destroy()
  {
    GUL_ASSERT(this->has_);
    this->val_.~T();
    this->has_ = false;
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct(std::forward<Other>(other).value());
    }
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void assign_from(Other&& other)
  {
    if (this->has_) {
      if (other.has_value()) {
        this->val_ = std::forward<Other>(other).value();
      } else {
        this->reset();
      }
    } else {
      if (other.has_value()) {
        construct(std::forward<Other>(other).value());
        this->has_ = true;
      }
    }
  }

  GUL_CXX14_CONSTEXPR void swap(optional_storage_base& other) noexcept(
      conjunction<std::is_nothrow_move_constructible<T>,
                  detail::is_nothrow_swappable<T>>::value)
  {
    if (this->has_value()) {
      if (other.has_value()) {
        using std::swap;
        swap(value(), other.value());
      } else {
        other.construct(std::move(value()));
        (*this).destroy();
        (*this).has_ = false;
        other.has_ = true;
      }
    } else {
      if (other.has_value()) {
        (*this).construct(std::move(other.value()));
        other.destroy();
        (*this).has_ = true;
        other.has_ = false;
      }
    }
  }
};

template <typename T>
struct optional_storage_base<T, true> : optional_throw_base {
  remove_reference_t<T>* valptr_;

  GUL_CXX14_CONSTEXPR optional_storage_base() noexcept
      : valptr_(nullptr)
  {
  }

  template <typename U>
  GUL_CXX14_CONSTEXPR optional_storage_base(in_place_t, U&& val) noexcept
      : valptr_(std::addressof(val))
  {
  }

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return valptr_ != nullptr;
  }

  GUL_CXX14_CONSTEXPR void reset() noexcept
  {
    valptr_ = nullptr;
  }

  GUL_CXX14_CONSTEXPR add_pointer_t<T> operator->() noexcept
  {
    GUL_ASSERT(has_value());
    return valptr_;
  }

  GUL_CXX14_CONSTEXPR add_pointer_t<const T> operator->() const noexcept
  {
    GUL_ASSERT(has_value());
    return valptr_;
  }

  GUL_CXX14_CONSTEXPR T& operator*() const& noexcept
  {
    GUL_ASSERT(has_value());
    return *valptr_;
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR T&& operator*() const&& noexcept
  {
    GUL_ASSERT(has_value());
    return std::forward<T>(*valptr_);
  }
#endif

  GUL_CXX14_CONSTEXPR const T& value() const&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
    return *valptr_;
  }

  GUL_CXX14_CONSTEXPR T&& value() &&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
    return std::forward<T>(*valptr_);
  }

  template <typename U>
  GUL_CXX14_CONSTEXPR T& construct(U&& u)
  {
    GUL_ASSERT(!has_value());
    valptr_ = std::addressof(u);
    return *valptr_;
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct(std::forward<Other>(other).value());
    }
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void assign_from(Other&& other)
  {
    if (has_value()) {
      if (other.has_value()) {
        *valptr_ = std::forward<Other>(other).value();
      } else {
        reset();
      }
    } else {
      if (other.has_value()) {
        construct(std::forward<Other>(other).value());
      }
    }
  }

  GUL_CXX14_CONSTEXPR void swap(optional_storage_base&)
  {
    static_assert(!std::is_reference<T>::value,
                  "[optional::swap] T must not be a reference type");
  }
};

template <bool B>
struct optional_storage_base<void, B> : optional_throw_base {
  bool has_;

  GUL_CXX14_CONSTEXPR optional_storage_base() noexcept
      : has_(false)
  {
  }

  GUL_CXX14_CONSTEXPR optional_storage_base(in_place_t) noexcept
      : has_(true)
  {
  }

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  GUL_CXX14_CONSTEXPR void reset() noexcept
  {
    has_ = false;
  }

  GUL_CXX14_CONSTEXPR void operator->() noexcept
  {
    GUL_ASSERT(this->has_);
  }

  GUL_CXX14_CONSTEXPR void operator->() const noexcept
  {
    GUL_ASSERT(this->has_);
  }

  GUL_CXX14_CONSTEXPR void operator*() & noexcept
  {
    GUL_ASSERT(has_);
  }

  GUL_CXX14_CONSTEXPR void operator*() const& noexcept
  {
    GUL_ASSERT(has_);
  }

  GUL_CXX14_CONSTEXPR void operator*() && noexcept
  {
    GUL_ASSERT(has_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR void operator*() const&& noexcept
  {
    GUL_ASSERT(has_);
  }
#endif

  GUL_CXX14_CONSTEXPR void value() &
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  GUL_CXX14_CONSTEXPR void value() const&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  GUL_CXX14_CONSTEXPR void value() &&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR void value() const&&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }
#endif

  GUL_CXX14_CONSTEXPR void construct() noexcept
  {
    has_ = true;
  }

  GUL_CXX14_CONSTEXPR void swap(optional_storage_base& other) noexcept
  {
    std::swap(has_, other.has_);
  }
};

template <typename T,
          bool = disjunction<std::is_void<T>,
                             detail::is_trivially_copy_constructible<T>>::value>
struct optional_copy_construct_base : optional_storage_base<T> {
  using optional_storage_base<T>::optional_storage_base;
};

template <typename T>
struct optional_copy_construct_base<T, false> : optional_storage_base<T> {
  using optional_storage_base<T>::optional_storage_base;

  GUL_CXX14_CONSTEXPR optional_copy_construct_base() = default;

  GUL_CXX14_CONSTEXPR
  optional_copy_construct_base(const optional_copy_construct_base& other)
      : optional_storage_base<T>::optional_storage_base()
  {
    this->construct_from(other);
  }

  GUL_CXX14_CONSTEXPR
  optional_copy_construct_base(optional_copy_construct_base&&) = default;

  GUL_CXX14_CONSTEXPR optional_copy_construct_base&
  operator=(const optional_copy_construct_base&)
      = default;

  GUL_CXX14_CONSTEXPR optional_copy_construct_base&
  operator=(optional_copy_construct_base&&)
      = default;
};

template <typename T,
          bool = disjunction<std::is_void<T>,
                             detail::is_trivially_move_constructible<T>>::value>
struct optional_move_construct_base : optional_copy_construct_base<T> {
  using optional_copy_construct_base<T>::optional_copy_construct_base;
};

template <typename T>
struct optional_move_construct_base<T, false>
    : optional_copy_construct_base<T> {
  using optional_copy_construct_base<T>::optional_copy_construct_base;

  GUL_CXX14_CONSTEXPR optional_move_construct_base() = default;

  GUL_CXX14_CONSTEXPR
  optional_move_construct_base(const optional_move_construct_base&) = default;

  GUL_CXX14_CONSTEXPR
  optional_move_construct_base(optional_move_construct_base&& other)
  {
    this->construct_from(std::move(other));
  }

  GUL_CXX14_CONSTEXPR optional_move_construct_base&
  operator=(const optional_move_construct_base&)
      = default;

  GUL_CXX14_CONSTEXPR optional_move_construct_base&
  operator=(optional_move_construct_base&&)
      = default;
};

template <typename T,
          bool = disjunction<
              std::is_void<T>,
              conjunction<std::is_trivially_destructible<T>,
                          detail::is_trivially_copy_constructible<T>,
                          detail::is_trivially_copy_assignable<T>>>::value>
struct optional_copy_assign_base : optional_move_construct_base<T> {
  using optional_move_construct_base<T>::optional_move_construct_base;
};

template <typename T>
struct optional_copy_assign_base<T, false> : optional_move_construct_base<T> {
  using optional_move_construct_base<T>::optional_move_construct_base;

  GUL_CXX14_CONSTEXPR optional_copy_assign_base() = default;

  GUL_CXX14_CONSTEXPR
  optional_copy_assign_base(const optional_copy_assign_base&) = default;

  GUL_CXX14_CONSTEXPR
  optional_copy_assign_base(optional_copy_assign_base&&) = default;

  GUL_CXX14_CONSTEXPR optional_copy_assign_base&
  operator=(const optional_copy_assign_base& other)
  {
    this->assign_from(other);
    return *this;
  }

  GUL_CXX14_CONSTEXPR optional_copy_assign_base&
  operator=(optional_copy_assign_base&&)
      = default;
};

template <typename T,
          bool = disjunction<
              std::is_void<T>,
              conjunction<std::is_trivially_destructible<T>,
                          detail::is_trivially_move_constructible<T>,
                          detail::is_trivially_move_assignable<T>>>::value>
struct optional_move_assign_base : optional_copy_assign_base<T> {
  using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename T>
struct optional_move_assign_base<T, false> : optional_copy_assign_base<T> {
  using optional_copy_assign_base<T>::optional_copy_assign_base;

  GUL_CXX14_CONSTEXPR optional_move_assign_base() = default;

  GUL_CXX14_CONSTEXPR
  optional_move_assign_base(const optional_move_assign_base&) = default;

  GUL_CXX14_CONSTEXPR
  optional_move_assign_base(optional_move_assign_base&&) = default;

  GUL_CXX14_CONSTEXPR optional_move_assign_base&
  operator=(const optional_move_assign_base&)
      = default;

  GUL_CXX14_CONSTEXPR optional_move_assign_base&
  operator=(optional_move_assign_base&& other)
  {
    this->assign_from(std::move(other));
    return *this;
  }
};

template <typename T>
using optional_enable_copy_construct_base = detail::copy_construct_base<
    disjunction<std::is_void<T>, std::is_copy_constructible<T>>::value>;

template <typename T>
using optional_enable_move_construct_base = detail::move_construct_base<
    disjunction<std::is_void<T>, std::is_move_constructible<T>>::value>;

template <typename T>
using optional_enable_copy_assign_base = detail::copy_assign_base<
    disjunction<std::is_void<T>,
                conjunction<std::is_copy_constructible<T>,
                            std::is_copy_assignable<T>>>::value>;

template <typename T>
using optional_enable_move_assign_base = detail::move_assign_base<
    disjunction<std::is_void<T>,
                conjunction<std::is_move_constructible<T>,
                            std::is_move_assignable<T>>>::value>;
}

template <typename T>
class optional : private detail::optional_move_assign_base<T>,
                 private detail::optional_enable_copy_construct_base<T>,
                 private detail::optional_enable_move_construct_base<T>,
                 private detail::optional_enable_copy_assign_base<T>,
                 private detail::optional_enable_move_assign_base<T> {
  using base_type = detail::optional_move_assign_base<T>;

  template <typename U>
  struct is_optional_constructible
      : negation<conjunction<std::is_constructible<T, optional<U>&>,
                             std::is_constructible<T, const optional<U>&>,
                             std::is_constructible<T, optional<U>&&>,
                             std::is_constructible<T, const optional<U>&&>,
                             std::is_convertible<optional<U>&, T>,
                             std::is_convertible<const optional<U>&, T>,
                             std::is_convertible<optional<U>&&, T>,
                             std::is_convertible<const optional<U>&&, T>>> { };
  template <typename U>
  struct is_optional_assignable
      : negation<conjunction<
            std::is_constructible<T, optional<U>&>,
            std::is_constructible<T, const optional<U>&>,
            std::is_constructible<T, optional<U>&&>,
            std::is_constructible<T, const optional<U>&&>,
            std::is_convertible<optional<U>&, T>,
            std::is_convertible<const optional<U>&, T>,
            std::is_convertible<optional<U>&&, T>,
            std::is_convertible<const optional<U>&&, T>,
            std::is_assignable<add_lvalue_reference_t<T>, optional<U>&>,
            std::is_assignable<add_lvalue_reference_t<T>, const optional<U>&>,
            std::is_assignable<add_lvalue_reference_t<T>, optional<U>&&>,
            std::is_assignable<add_lvalue_reference_t<T>,
                               const optional<U>&&>>> { };

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  and_then_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<invoke_result_t<F>>
  {
    using Opt = remove_cvref_t<invoke_result_t<F>>;
    static_assert(is_specialization_of<Opt, gul::optional>::value, "");
    if (self.has_value()) {
      return gul::invoke(std::forward<F>(f));
    } else {
      return nullopt;
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  and_then_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(gul::invoke(std::forward<F>(f),
                                             std::forward<Self>(self).value()))>
  {
    using SelfT = decltype(std::forward<Self>(self).value());
    using Opt = remove_cvref_t<invoke_result_t<F, SelfT>>;
    static_assert(is_specialization_of<Opt, gul::optional>::value, "");
    if (self.has_value()) {
      return gul::invoke(std::forward<F>(f), std::forward<Self>(self).value());
    } else {
      return nullopt;
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_impl(std::true_type, Self&& self, F&& f)
      -> optional<remove_cv_t<invoke_result_t<F>>>
  {
    static_assert(std::is_void<remove_cv_t<invoke_result_t<F>>>::value, "");
    if (self.has_value()) {
      gul::invoke(std::forward<F>(f));
      return optional(in_place);
    } else {
      return nullopt;
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_impl(std::false_type, Self&& self, F&& f)
      -> optional<remove_cv_t<decltype(gul::invoke(
          std::forward<F>(f), std::forward<Self>(self).value()))>>
  {
    using SelfT = decltype(std::forward<Self>(self).value());
    using T2 = remove_cv_t<invoke_result_t<F, SelfT>>;
    static_assert(
        std::is_constructible<optional<T2>, invoke_result_t<F, SelfT>>::value,
        "");
    if (self.has_value()) {
      return gul::invoke(std::forward<F>(f), std::forward<Self>(self).value());
    } else {
      return nullopt;
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  or_else_impl(std::true_type, Self&& self, F&& f) -> optional
  {
    static_assert(
        std::is_same<remove_cvref_t<invoke_result_t<F>>, optional>::value, "");
    if (self.has_value()) {
      return optional(in_place);
    } else {
      return gul::invoke(std::forward<F>(f));
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  or_else_impl(std::false_type, Self&& self, F&& f) -> optional
  {
    static_assert(
        std::is_same<remove_cvref_t<invoke_result_t<F>>, optional>::value, "");
    if (self.has_value()) {
      return std::forward<Self>(self).value();
    } else {
      return gul::invoke(std::forward<F>(f));
    }
  }

public:
  static_assert(!std::is_same<remove_cvref_t<T>, in_place_t>::value,
                "[optional] T must not be in_place_t");
  static_assert(!std::is_same<remove_cvref_t<T>, nullopt_t>::value,
                "[optional] T must not be nullopt_t");
  static_assert(
      disjunction<std::is_void<T>,
                  std::is_destructible<T>,
                  std::is_lvalue_reference<T>>::value,
      "[optional] T can be a void type, a destructible type or a lvalue "
      "reference type");
  static_assert(!std::is_array<T>::value,
                "[optional] T must not be an array type");

  using value_type = T;

  GUL_CXX14_CONSTEXPR optional() noexcept = default;

  GUL_CXX14_CONSTEXPR optional(nullopt_t) noexcept { }

  template <typename... Args,
            GUL_REQUIRES(disjunction<std::is_void<T>,
                                     std::is_constructible<T, Args...>>::value)>
  GUL_CXX14_CONSTEXPR explicit optional(in_place_t, Args&&... args)
      : base_type(in_place, std::forward<Args>(args)...)
  {
  }

  template <
      typename U,
      typename... Args,
      GUL_REQUIRES(conjunction<negation<std::is_void<T>>,
                               std::is_constructible<T,
                                                     std::initializer_list<U>&,
                                                     Args...>>::value)>
  GUL_CXX14_CONSTEXPR
  optional(in_place_t, std::initializer_list<U> init, Args&&... args)
      : base_type(in_place, init, std::forward<Args>(args)...)
  {
  }

  template <typename U,
            GUL_REQUIRES(conjunction<
                         negation<std::is_void<T>>,
                         negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
                         negation<std::is_same<remove_cvref_t<U>, optional>>,
                         std::is_constructible<T, U>>::value)>
  GUL_CXX14_CONSTEXPR optional(U&& val)
      : base_type(in_place, std::forward<U>(val))
  {
  }

  GUL_CXX14_CONSTEXPR optional(const optional&) = default;

  GUL_CXX14_CONSTEXPR optional(optional&&) = default;

  template <typename U,
            GUL_REQUIRES(conjunction<std::is_constructible<T, const U&>,
                                     is_optional_constructible<U>>::value)>
  GUL_CXX14_CONSTEXPR optional(const optional<U>& other)
  {
    (*this).construct_from(other);
  }

  template <typename U,
            GUL_REQUIRES(conjunction<std::is_constructible<T, U>,
                                     is_optional_constructible<U>>::value)>
  GUL_CXX14_CONSTEXPR optional(optional<U>&& other)
  {
    (*this).construct_from(std::move(other));
  }

  ~optional() noexcept = default;

  GUL_CXX14_CONSTEXPR optional& operator=(nullopt_t) noexcept
  {
    reset();
    return *this;
  }

  template <
      typename U,
      GUL_REQUIRES(conjunction<
                   negation<std::is_void<T>>,
                   negation<std::is_same<remove_cvref_t<U>, optional>>,
                   std::is_constructible<T, U>,
                   std::is_assignable<add_lvalue_reference_t<T>, U>,
                   disjunction<negation<std::is_scalar<T>>,
                               negation<std::is_same<decay_t<U>, T>>>>::value)>
  GUL_CXX14_CONSTEXPR optional& operator=(U&& u)
  {
    if (has_value()) {
      (*this).value() = std::forward<U>(u);
    } else {
      (*this).construct(std::forward<U>(u));
    }
    return *this;
  }

  GUL_CXX14_CONSTEXPR optional& operator=(const optional&) = default;

  GUL_CXX14_CONSTEXPR optional& operator=(optional&&) = default;

  template <
      typename U,
      GUL_REQUIRES(
          conjunction<std::is_constructible<T, const U&>,
                      std::is_assignable<add_lvalue_reference_t<T>, const U&>,
                      is_optional_assignable<U>>::value)>
  GUL_CXX14_CONSTEXPR optional& operator=(const optional<U>& other)
  {
    (*this).assign_from(other);
    return *this;
  }

  template <typename U,
            GUL_REQUIRES(
                conjunction<std::is_constructible<T, U&&>,
                            std::is_assignable<add_lvalue_reference_t<T>, U&&>,
                            is_optional_assignable<U>>::value)>
  GUL_CXX14_CONSTEXPR optional& operator=(optional<U>&& other)
  {
    (*this).assign_from(std::move(other));
    return *this;
  }

  GUL_CXX14_CONSTEXPR explicit operator bool() const noexcept
  {
    return has_value();
  }

  using base_type::operator->;

  using base_type::operator*;

  using base_type::has_value;

  using base_type::value;

  template <typename U>
  GUL_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) const&
  {
    if (has_value()) {
      return value();
    } else {
      return static_cast<remove_reference_t<T>>(std::forward<U>(default_value));
    }
  }

  template <typename U>
  GUL_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) &&
  {
    if (has_value()) {
      return std::move(value());
    } else {
      return static_cast<remove_reference_t<T>>(std::forward<U>(default_value));
    }
  }

  template <typename... Args,
            GUL_REQUIRES(disjunction<std::is_void<T>,
                                     std::is_constructible<T, Args...>>::value)>
  GUL_CXX14_CONSTEXPR auto emplace(Args&&... args) -> add_lvalue_reference_t<T>
  {
    if (has_value()) {
      reset();
    }

    return (*this).construct(std::forward<Args>(args)...);
  }

  template <typename U,
            typename... Args,
            GUL_REQUIRES(std::is_constructible<T,
                                               std::initializer_list<U>&,
                                               Args&&...>::value)>
  GUL_CXX14_CONSTEXPR auto emplace(std::initializer_list<U> init,
                                   Args&&... args) -> add_lvalue_reference_t<T>
  {
    if (has_value()) {
      reset();
    }

    return (*this).construct(init, std::forward<Args>(args)...);
  }

  using base_type::reset;

  /// optional<T>::and_then(F) -> optional<T>
  /// F() -> optional<T>
  /// where T is void
  ///
  /// optional<T>::and_then(F) -> optional<U>
  /// F(T) -> optional<U>
  /// where T is not void
  template <typename F>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) & -> decltype(and_then_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))

  {
    return and_then_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) const& -> decltype(and_then_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) && -> decltype(and_then_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, std::move(*this),
                         std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) const&& -> decltype(and_then_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, std::move(*this),
                         std::forward<F>(f));
  }
#endif

  /// optional<T>::transform(F) -> optional<T>
  /// F() -> T
  /// where T is void
  ///
  /// optional<T>::transform(F) -> optional<U>
  /// F(T) -> U
  /// where T is not void
  template <typename F>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) & -> decltype(transform_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) const& -> decltype(transform_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) && -> decltype(transform_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, std::move(*this),
                          std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) const&& -> decltype(transform_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, std::move(*this),
                          std::forward<F>(f));
  }
#endif

  /// optional<T>::or_else(F) -> optional<T>
  /// F() -> optional<T>
  template <typename F,
            GUL_REQUIRES(conjunction<disjunction<std::is_void<T>,
                                                 std::is_copy_constructible<T>>,
                                     is_invocable<F>>::value)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) const& -> decltype(or_else_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            GUL_REQUIRES(conjunction<disjunction<std::is_void<T>,
                                                 std::is_move_constructible<T>>,
                                     is_invocable<F>>::value)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) && -> decltype(or_else_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, std::move(*this),
                        std::forward<F>(f));
  }

  using base_type::swap;

  friend GUL_CXX14_CONSTEXPR void
  swap(optional& lhs, optional& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }
};

#ifdef GUL_HAS_CXX17

template <typename T>
optional(T) -> optional<T>;

#endif

inline GUL_CXX14_CONSTEXPR bool operator==(const optional<void>& lhs,
                                           const optional<void>& rhs)
{
  return lhs.has_value() == rhs.has_value();
}

inline GUL_CXX14_CONSTEXPR bool operator!=(const optional<void>& lhs,
                                           const optional<void>& rhs)
{
  return lhs.has_value() != rhs.has_value();
}

inline GUL_CXX14_CONSTEXPR bool operator<(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return !lhs && rhs;
}

inline GUL_CXX14_CONSTEXPR bool operator<=(const optional<void>& lhs,
                                           const optional<void>& rhs)
{
  return !lhs || rhs;
}

inline GUL_CXX14_CONSTEXPR bool operator>(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return lhs && !rhs;
}

inline GUL_CXX14_CONSTEXPR bool operator>=(const optional<void>& lhs,
                                           const optional<void>& rhs)
{
  return lhs || !rhs;
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_eq_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const optional<T1>& lhs,
                                    const optional<T2>& rhs)
{
  if (lhs.has_value() != rhs.has_value()) {
    return false;
  }

  if (!lhs) {
    return true;
  }

  return lhs.value() == rhs.value();
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_ne_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const optional<T1>& lhs,
                                    const optional<T2>& rhs)
{
  if (lhs.has_value() != rhs.has_value()) {
    return true;
  }

  if (!lhs) {
    return false;
  }

  return lhs.value() != rhs.value();
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_lt_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator<(const optional<T1>& lhs,
                                   const optional<T2>& rhs)
{
  if (!rhs) {
    return false;
  }
  if (!lhs) {
    return true;
  }

  return lhs.value() < rhs.value();
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_le_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator<=(const optional<T1>& lhs,
                                    const optional<T2>& rhs)
{
  if (!lhs) {
    return true;
  }
  if (!rhs) {
    return false;
  }

  return lhs.value() <= rhs.value();
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_gt_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator>(const optional<T1>& lhs,
                                   const optional<T2>& rhs)
{
  if (!lhs) {
    return false;
  }
  if (!rhs) {
    return true;
  }

  return lhs.value() > rhs.value();
}

template <typename T1,
          typename T2,
          GUL_REQUIRES(detail::is_ge_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator>=(const optional<T1>& lhs,
                                    const optional<T2>& rhs)
{
  if (!rhs) {
    return true;
  }
  if (!lhs) {
    return false;
  }

  return lhs.value() >= rhs.value();
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator==(const optional<T>& opt, nullopt_t) noexcept
{
  return !opt;
}

#ifndef GUL_HAS_CXX20

template <typename T>
GUL_CXX14_CONSTEXPR bool operator==(nullopt_t, const optional<T>& opt) noexcept
{
  return !opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator!=(const optional<T>& opt, nullopt_t) noexcept
{
  return !!opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator!=(nullopt_t, const optional<T>& opt) noexcept
{
  return !!opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator<(const optional<T>& opt, nullopt_t) noexcept
{
  GUL_UNUSED(opt);
  return false;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator<(nullopt_t, const optional<T>& opt) noexcept
{
  return !!opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator<=(const optional<T>& opt, nullopt_t) noexcept
{
  return !opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator<=(nullopt_t, const optional<T>& opt) noexcept
{
  GUL_UNUSED(opt);
  return true;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator>(const optional<T>& opt, nullopt_t) noexcept
{
  return !!opt;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator>(nullopt_t, const optional<T>& opt) noexcept
{
  GUL_UNUSED(opt);
  return false;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator>=(const optional<T>& opt, nullopt_t) noexcept
{
  GUL_UNUSED(opt);
  return true;
}

template <typename T>
GUL_CXX14_CONSTEXPR bool operator>=(nullopt_t, const optional<T>& opt) noexcept
{
  return !opt;
}

#else

template <typename T>
constexpr std::strong_ordering operator<=>(const optional<T>& opt,
                                           nullopt_t) noexcept
{
  return !!opt <=> false;
}

#endif

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_eq_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const optional<T>& opt, const U& val)
{
  return opt && opt.value() == val;
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_eq_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const T& val, const optional<U>& opt)
{
  return opt && val == opt.value();
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_ne_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() != val;
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_ne_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const T& val, const optional<U>& opt)
{
  return !opt || val != opt.value();
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_lt_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator<(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() < val;
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_lt_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator<(const T& val, const optional<U>& opt)
{
  return opt && val < opt.value();
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_le_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator<=(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() <= val;
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_le_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator<=(const T& val, const optional<U>& opt)
{
  return opt && val <= opt.value();
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_gt_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator>(const optional<T>& opt, const U& val)
{
  return !(opt <= val);
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_gt_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator>(const T& val, const optional<U>& opt)
{
  return !(val <= opt);
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_ge_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator>=(const optional<T>& opt, const U& val)
{
  return !(opt < val);
}

template <typename T,
          typename U,
          GUL_REQUIRES(detail::is_ge_comparable_with<T, U>::value)>
GUL_CXX14_CONSTEXPR bool operator>=(const T& val, const optional<U>& opt)
{
  return !(val < opt);
}

template <typename T>
constexpr optional<decay_t<T>> make_optional(T&& value)
{
  return optional<decay_t<T>>(std::forward<T>(value));
}

template <typename T, typename... Args>
constexpr optional<T> make_optional(Args&&... args)
{
  return optional<T>(in_place, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
constexpr optional<T> make_optional(std::initializer_list<U> init,
                                    Args&&... args)
{
  return optional<T>(in_place, std::move(init), std::forward<Args>(args)...);
}

GUL_NAMESPACE_END
