//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <ds/config.hpp>

#include <ds/functional.hpp>
#include <ds/type_traits.hpp>
#include <ds/utility.hpp>

DS_NAMESPACE_BEGIN

struct nullopt_t {
  struct tag_t {
    explicit constexpr tag_t() = default;
  };
  explicit constexpr nullopt_t(tag_t, tag_t) { }
};

DS_CXX17_INLINE constexpr nullopt_t nullopt { nullopt_t::tag_t {},
                                              nullopt_t::tag_t {} };

#if !DS_NO_EXCEPTIONS
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
#if DS_NO_EXCEPTIONS
    std::abort();
#else
    throw bad_optional_access();
#endif
  }
};

template <typename T, bool = is_trivially_destructible<T>::value>
struct optional_destruct_base : optional_throw_base {
  union {
    char nul_;
    T val_;
  };
  bool has_;

  DS_CXX14_CONSTEXPR optional_destruct_base() noexcept
      : nul_()
      , has_(false)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit optional_destruct_base(in_place_t, Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  DS_CXX14_CONSTEXPR void reset() noexcept
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

  DS_CXX14_CONSTEXPR optional_destruct_base() noexcept
      : nul_()
      , has_(false)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit optional_destruct_base(in_place_t, Args&&... args)
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

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  DS_CXX14_CONSTEXPR void reset() noexcept
  {
    if (has_) {
      val_.~T();
      has_ = false;
    }
  }
};

template <typename T, bool = is_reference<T>::value>
struct optional_storage_base : optional_destruct_base<T> {
  using optional_destruct_base<T>::optional_destruct_base;

  DS_CXX14_CONSTEXPR T& operator*() & noexcept
  {
    DS_ASSERT(this->has_);
    return this->val_;
  }

  DS_CXX14_CONSTEXPR const T& operator*() const& noexcept
  {
    DS_ASSERT(this->has_);
    return this->val_;
  }

  DS_CXX14_CONSTEXPR T&& operator*() && noexcept
  {
    DS_ASSERT(this->has_);
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR const T&& operator*() const&& noexcept
  {
    DS_ASSERT(this->has_);
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR T& value() &
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return this->val_;
  }

  DS_CXX14_CONSTEXPR const T& value() const&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return this->val_;
  }

  DS_CXX14_CONSTEXPR T&& value() &&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR const T&& value() const&&
  {
    if (!this->has_) {
      this->throw_bad_optional_access();
    }
    return std::move(this->val_);
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR T& construct(Args&&... args)
  {
    DS_ASSERT(!this->has_);
    auto& val
        = *(::new (std::addressof(this->val_)) T(std::forward<Args>(args)...));
    this->has_ = true;
    return val;
  }

  DS_CXX14_CONSTEXPR void destroy()
  {
    DS_ASSERT(this->has_);
    this->val_.~T();
    this->has_ = false;
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct(std::forward<Other>(other).value());
    }
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void assign_from(Other&& other)
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
};

template <typename T>
struct optional_storage_base<T, true> : optional_throw_base {
  remove_reference_t<T>* valptr_;

  DS_CXX14_CONSTEXPR optional_storage_base() noexcept
      : valptr_(nullptr)
  {
  }

  template <typename U>
  DS_CXX14_CONSTEXPR optional_storage_base(in_place_t, U&& val) noexcept
      : valptr_(std::addressof(val))
  {
  }

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return valptr_ != nullptr;
  }

  DS_CXX14_CONSTEXPR void reset() noexcept
  {
    valptr_ = nullptr;
  }

  DS_CXX14_CONSTEXPR T& operator*() const& noexcept
  {
    DS_ASSERT(has_value());
    return *valptr_;
  }

  DS_CXX14_CONSTEXPR T&& operator*() const&& noexcept
  {
    DS_ASSERT(has_value());
    return std::forward<T>(*valptr_);
  }

  DS_CXX14_CONSTEXPR T& value() const&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
    return *valptr_;
  }

  DS_CXX14_CONSTEXPR T&& value() const&&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
    return std::forward<T>(*valptr_);
  }

  template <typename U>
  DS_CXX14_CONSTEXPR T& construct(U&& u)
  {
    DS_ASSERT(!has_value());
    valptr_ = std::addressof(u);
    return *valptr_;
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct(std::forward<Other>(other).value());
    }
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void assign_from(Other&& other)
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
};

template <bool B>
struct optional_storage_base<void, B> : optional_throw_base {
  bool has_;

  DS_CXX14_CONSTEXPR optional_storage_base() noexcept
      : has_(false)
  {
  }

  DS_CXX14_CONSTEXPR optional_storage_base(in_place_t) noexcept
      : has_(true)
  {
  }

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return has_;
  }

  DS_CXX14_CONSTEXPR void reset() noexcept
  {
    has_ = false;
  }

  DS_CXX14_CONSTEXPR void operator*() & noexcept
  {
    DS_ASSERT(has_);
  }

  DS_CXX14_CONSTEXPR void operator*() const& noexcept
  {
    DS_ASSERT(has_);
  }

  DS_CXX14_CONSTEXPR void operator*() && noexcept
  {
    DS_ASSERT(has_);
  }

  DS_CXX14_CONSTEXPR void operator*() const&& noexcept
  {
    DS_ASSERT(has_);
  }

  DS_CXX14_CONSTEXPR void value() &
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  DS_CXX14_CONSTEXPR void value() const&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  DS_CXX14_CONSTEXPR void value() &&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  DS_CXX14_CONSTEXPR void value() const&&
  {
    if (!has_value()) {
      this->throw_bad_optional_access();
    }
  }

  DS_CXX14_CONSTEXPR void construct() noexcept
  {
    has_ = true;
  }

  DS_CXX14_CONSTEXPR void destroy() noexcept
  {
    has_ = false;
  }
};

template <typename T,
          bool
          = disjunction<is_void<T>, is_trivially_copy_constructible<T>>::value>
struct optional_copy_construct_base : optional_storage_base<T> {
  using optional_storage_base<T>::optional_storage_base;
};

template <typename T>
struct optional_copy_construct_base<T, false> : optional_storage_base<T> {
  using optional_storage_base<T>::optional_storage_base;

  DS_CXX14_CONSTEXPR optional_copy_construct_base() = default;

  DS_CXX14_CONSTEXPR
  optional_copy_construct_base(const optional_copy_construct_base& other)
  {
    this->construct_from(other);
  }

  DS_CXX14_CONSTEXPR
  optional_copy_construct_base(optional_copy_construct_base&&) = default;

  DS_CXX14_CONSTEXPR optional_copy_construct_base&
  operator=(const optional_copy_construct_base&)
      = default;

  DS_CXX14_CONSTEXPR optional_copy_construct_base&
  operator=(optional_copy_construct_base&&)
      = default;
};

template <typename T,
          bool
          = disjunction<is_void<T>, is_trivially_move_constructible<T>>::value>
struct optional_move_construct_base : optional_copy_construct_base<T> {
  using optional_copy_construct_base<T>::optional_copy_construct_base;
};

template <typename T>
struct optional_move_construct_base<T, false>
    : optional_copy_construct_base<T> {
  using optional_copy_construct_base<T>::optional_copy_construct_base;

  DS_CXX14_CONSTEXPR optional_move_construct_base() = default;

  DS_CXX14_CONSTEXPR
  optional_move_construct_base(const optional_move_construct_base&) = default;

  DS_CXX14_CONSTEXPR
  optional_move_construct_base(optional_move_construct_base&& other)
  {
    this->construct_from(std::move(other));
  }

  DS_CXX14_CONSTEXPR optional_move_construct_base&
  operator=(const optional_move_construct_base&)
      = default;

  DS_CXX14_CONSTEXPR optional_move_construct_base&
  operator=(optional_move_construct_base&&)
      = default;
};

template <typename T,
          bool
          = disjunction<is_void<T>,
                        conjunction<is_trivially_destructible<T>,
                                    is_trivially_copy_constructible<T>,
                                    is_trivially_copy_assignable<T>>>::value>
struct optional_copy_assign_base : optional_move_construct_base<T> {
  using optional_move_construct_base<T>::optional_move_construct_base;
};

template <typename T>
struct optional_copy_assign_base<T, false> : optional_move_construct_base<T> {
  using optional_move_construct_base<T>::optional_move_construct_base;

  DS_CXX14_CONSTEXPR optional_copy_assign_base() = default;

  DS_CXX14_CONSTEXPR
  optional_copy_assign_base(const optional_copy_assign_base&) = default;

  DS_CXX14_CONSTEXPR
  optional_copy_assign_base(optional_copy_assign_base&&) = default;

  DS_CXX14_CONSTEXPR optional_copy_assign_base&
  operator=(const optional_copy_assign_base& other)
  {
    this->assign_from(other);
    return *this;
  }

  DS_CXX14_CONSTEXPR optional_copy_assign_base&
  operator=(optional_copy_assign_base&&)
      = default;
};

template <typename T,
          bool
          = disjunction<is_void<T>,
                        conjunction<is_trivially_destructible<T>,
                                    is_trivially_move_constructible<T>,
                                    is_trivially_move_assignable<T>>>::value>
struct optional_move_assign_base : optional_copy_assign_base<T> {
  using optional_copy_assign_base<T>::optional_copy_assign_base;
};

template <typename T>
struct optional_move_assign_base<T, false> : optional_copy_assign_base<T> {
  using optional_copy_assign_base<T>::optional_copy_assign_base;

  DS_CXX14_CONSTEXPR optional_move_assign_base() = default;

  DS_CXX14_CONSTEXPR
  optional_move_assign_base(const optional_move_assign_base&) = default;

  DS_CXX14_CONSTEXPR
  optional_move_assign_base(optional_move_assign_base&&) = default;

  DS_CXX14_CONSTEXPR optional_move_assign_base&
  operator=(const optional_move_assign_base&)
      = default;

  DS_CXX14_CONSTEXPR optional_move_assign_base&
  operator=(optional_move_assign_base&& other)
  {
    this->assign_from(std::move(other));
    return *this;
  }
};
}

template <typename T>
class optional : private detail::optional_move_assign_base<T> {
  using base_type = detail::optional_move_assign_base<T>;

  template <typename U>
  struct is_optional_constructible
      : negation<conjunction<is_constructible<T, optional<U>&>,
                             is_constructible<T, const optional<U>&>,
                             is_constructible<T, optional<U>&&>,
                             is_constructible<T, const optional<U>&&>,
                             is_convertible<optional<U>&, T>,
                             is_convertible<const optional<U>&, T>,
                             is_convertible<optional<U>&&, T>,
                             is_convertible<const optional<U>&&, T>>> { };
  template <typename U>
  struct is_optional_assignable
      : negation<conjunction<
            is_constructible<T, optional<U>&>,
            is_constructible<T, const optional<U>&>,
            is_constructible<T, optional<U>&&>,
            is_constructible<T, const optional<U>&&>,
            is_convertible<optional<U>&, T>,
            is_convertible<const optional<U>&, T>,
            is_convertible<optional<U>&&, T>,
            is_convertible<const optional<U>&&, T>,
            is_assignable<add_lvalue_reference_t<T>, optional<U>&>,
            is_assignable<add_lvalue_reference_t<T>, const optional<U>&>,
            is_assignable<add_lvalue_reference_t<T>, optional<U>&&>,
            is_assignable<add_lvalue_reference_t<T>, const optional<U>&&>>> { };

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_and_then_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
  {
    using Opt = remove_cvref_t<invoke_result_t<F>>;
    if (self.has_value()) {
      return Opt(invoke(std::forward<F>(f)));
    } else {
      return Opt(nullopt);
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_and_then_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f),
                                        std::forward<Self>(self).value()))>
  {
    using Opt = remove_cvref_t<
        invoke_result_t<F, decltype(std::declval<Self>().value())>>;
    if (self.has_value()) {
      return Opt(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
    } else {
      return Opt(nullopt);
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_transform_impl(std::true_type, Self&& self, F&& f)
      -> optional<remove_cvref_t<decltype(invoke(std::forward<F>(f)))>>
  {
    using Opt = optional<remove_cvref_t<invoke_result_t<F>>>;
    if (self.has_value()) {
      return Opt(invoke(std::forward<F>(f)));
    } else {
      return Opt(nullopt);
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_transform_impl(std::false_type, Self&& self, F&& f)
      -> optional<remove_cvref_t<decltype(invoke(
          std::forward<F>(f), std::forward<Self>(self).value()))>>
  {
    using Opt = optional<remove_cvref_t<
        invoke_result_t<F, decltype(std::declval<Self>().value())>>>;
    if (self.has_value()) {
      return Opt(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
    } else {
      return Opt(nullopt);
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_or_else_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
  {
    using Opt = remove_cvref_t<Self>;
    if (self.has_value()) {
      return Opt(in_place);
    } else {
      return Opt(invoke(std::forward<F>(f)));
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  optional_or_else_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
  {
    using Opt = remove_cvref_t<Self>;
    if (self.has_value()) {
      return Opt(std::forward<Self>(self).value());
    } else {
      return Opt(invoke(std::forward<F>(f)));
    }
  }

public:
  static_assert(!is_same<remove_cvref_t<T>, in_place_t>::value,
                "[optional] T = `in_place_t` is ill-formed.");
  static_assert(!is_same<remove_cvref_t<T>, nullopt_t>::value,
                "[optional] T = `nullopt_t` is ill-formed.");
  static_assert(
      disjunction<is_void<T>, is_destructible<T>, is_lvalue_reference<T>>::
          value,
      "[optional] T can be `void`, destructible or lvalue reference type.");
  static_assert(!is_array<T>::value, "[optional] T = U[] is ill-formed.");

  using value_type = T;

  DS_CXX14_CONSTEXPR optional() noexcept = default;

  DS_CXX14_CONSTEXPR optional(nullopt_t) noexcept { }

  template <
      typename... Args,
      enable_if_t<disjunction<is_void<T>, is_constructible<T, Args...>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR explicit optional(in_place_t, Args&&... args)
      : base_type(in_place, std::forward<Args>(args)...)
  {
  }

  template <
      typename U,
      typename... Args,
      enable_if_t<
          conjunction<
              negation<is_void<T>>,
              is_constructible<T, std::initializer_list<U>&, Args...>>::value,
          int>
      = 0>
  DS_CXX14_CONSTEXPR
  optional(in_place_t, std::initializer_list<U> init, Args&&... args)
      : base_type(in_place, init, std::forward<Args>(args)...)
  {
  }

  template <
      typename U,
      enable_if_t<conjunction<negation<is_void<T>>,
                              negation<is_same<remove_cvref_t<U>, in_place_t>>,
                              negation<is_same<remove_cvref_t<U>, optional>>,
                              is_constructible<T, U>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR optional(U&& val)
      : base_type(in_place, std::forward<U>(val))
  {
  }

  DS_CXX14_CONSTEXPR optional(const optional&) = default;

  DS_CXX14_CONSTEXPR
  optional(optional&&) noexcept(is_nothrow_move_constructible<T>::value)
      = default;

  template <typename U,
            enable_if_t<conjunction<is_constructible<T, const U&>,
                                    is_optional_constructible<U>>::value,
                        int>
            = 0>
  DS_CXX14_CONSTEXPR optional(const optional<U>& other)
  {
    this->construct_from(other);
  }

  template <typename U,
            enable_if_t<conjunction<is_constructible<T, U>,
                                    is_optional_constructible<U>>::value,
                        int>
            = 0>
  DS_CXX14_CONSTEXPR optional(optional<U>&& other)
  {
    this->construct_from(std::move(other));
  }

  ~optional() noexcept = default;

  DS_CXX14_CONSTEXPR optional& operator=(nullopt_t) noexcept
  {
    this->reset();
    return *this;
  }

  template <
      typename U,
      enable_if_t<
          conjunction<negation<is_void<T>>,
                      negation<is_same<remove_cvref_t<U>, optional>>,
                      is_constructible<T, U>,
                      is_assignable<add_lvalue_reference_t<T>, U>,
                      disjunction<negation<is_scalar<T>>,
                                  negation<is_same<decay_t<U>, T>>>>::value,
          int>
      = 0>
  DS_CXX14_CONSTEXPR optional& operator=(U&& u)
  {
    if (this->has_value()) {
      this->value() = std::forward<U>(u);
    } else {
      this->construct(std::forward<U>(u));
    }
    return *this;
  }

  DS_CXX14_CONSTEXPR optional& operator=(const optional&) = default;

  DS_CXX14_CONSTEXPR optional& operator=(optional&&) noexcept(
      conjunction<is_nothrow_move_assignable<T>,
                  std::is_nothrow_move_constructible<T>>::value)
      = default;

  template <typename U,
            enable_if_t<
                conjunction<is_constructible<T, const U&>,
                            is_assignable<add_lvalue_reference_t<T>, const U&>,
                            is_optional_assignable<U>>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR optional& operator=(const optional<U>& other)
  {
    this->assign_from(other);
    return *this;
  }

  template <
      typename U,
      enable_if_t<conjunction<is_constructible<T, U&&>,
                              is_assignable<add_lvalue_reference_t<T>, U&&>,
                              is_optional_assignable<U>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR optional& operator=(optional<U>&& other)
  {
    this->assign_from(std::move(other));
    return *this;
  }

  DS_CXX14_CONSTEXPR explicit operator bool() const noexcept
  {
    return this->has_value();
  }

  template <bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR add_pointer_t<T> operator->() noexcept
  {
    return std::addressof(value());
  }

  template <bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR add_pointer_t<const T> operator->() const noexcept
  {
    return std::addressof(value());
  }

  using base_type::operator*;

  using base_type::has_value;

  using base_type::value;

  template <typename U>
  DS_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) const&
  {
    if (this->has_value()) {
      return value();
    } else {
      return static_cast<remove_reference_t<T>>(std::forward<U>(default_value));
    }
  }

  template <typename U>
  DS_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) &&
  {
    if (this->has_value()) {
      return std::move(value());
    } else {
      return static_cast<remove_reference_t<T>>(std::forward<U>(default_value));
    }
  }

  template <
      typename... Args,
      enable_if_t<disjunction<is_void<T>, is_constructible<T, Args...>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR auto emplace(Args&&... args) -> add_lvalue_reference_t<T>
  {
    if (this->has_value()) {
      this->reset();
    }

    return this->construct(std::forward<Args>(args)...);
  }

  template <
      typename U,
      typename... Args,
      enable_if_t<
          is_constructible<T, std::initializer_list<U>&, Args&&...>::value,
          int>
      = 0>
  DS_CXX14_CONSTEXPR auto emplace(std::initializer_list<U> init, Args&&... args)
      -> add_lvalue_reference_t<T>
  {
    if (this->has_value()) {
      this->destroy();
    }

    return this->construct(init, std::forward<Args>(args)...);
  }

  using base_type::reset;

  template <typename F>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) & -> decltype(optional_and_then_impl(
      is_void<T> {}, *this, std::forward<F>(f)))

  {
    return optional_and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  and_then(F&& f) const& -> decltype(optional_and_then_impl(is_void<T> {},
                                                            *this,
                                                            std::forward<F>(f)))
  {
    return optional_and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) && -> decltype(optional_and_then_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return optional_and_then_impl(is_void<T> {}, std::move(*this),
                                  std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  and_then(F&& f) const&& -> decltype(optional_and_then_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return optional_and_then_impl(is_void<T> {}, std::move(*this),
                                  std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  transform(F&& f) & -> decltype(optional_transform_impl(is_void<T> {},
                                                         *this,
                                                         std::forward<F>(f)))
  {
    return optional_transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  transform(F&& f) const& -> decltype(optional_transform_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return optional_transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  transform(F&& f) && -> decltype(optional_transform_impl(is_void<T> {},
                                                          std::move(*this),
                                                          std::forward<F>(f)))
  {
    return optional_transform_impl(is_void<T> {}, std::move(*this),
                                   std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  transform(F&& f) const&& -> decltype(optional_transform_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return optional_transform_impl(is_void<T> {}, std::move(*this),
                                   std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) & -> decltype(optional_or_else_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return optional_or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  or_else(F&& f) const& -> decltype(optional_or_else_impl(is_void<T> {},
                                                          *this,
                                                          std::forward<F>(f)))
  {
    return optional_or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) && -> decltype(optional_or_else_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return optional_or_else_impl(is_void<T> {}, std::move(*this),
                                 std::forward<F>(f));
  }

  template <typename F>
  DS_CXX14_CONSTEXPR auto
  or_else(F&& f) const&& -> decltype(optional_or_else_impl(is_void<T> {},
                                                           std::move(*this),
                                                           std::forward<F>(f)))
  {
    return optional_or_else_impl(is_void<T> {}, std::move(*this),
                                 std::forward<F>(f));
  }

  DS_CXX14_CONSTEXPR void
  swap(optional& other) noexcept(conjunction<is_nothrow_move_constructible<T>,
                                             is_nothrow_swappable<T>>::value)
  {
    if (this->has_value()) {
      if (other) {
        using std::swap;
        swap(value(), other.value());
      } else {
        other.construct(std::move(value()));
        this->destroy();
        this->has_ = false;
        other.has_ = true;
      }
    } else {
      if (other) {
        this->construct(std::move(other.value()));
        other.destroy();
        this->has_ = true;
        other.has_ = false;
      }
    }
  }

  friend DS_CXX14_CONSTEXPR void
  swap(optional& lhs, optional& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }
};

#ifdef DS_HAS_CXX17

template <typename T>
optional(T) -> optional<T>;

#endif

inline DS_CXX14_CONSTEXPR bool operator==(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return lhs.has_value() == rhs.has_value();
}

inline DS_CXX14_CONSTEXPR bool operator!=(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return lhs.has_value() != rhs.has_value();
}

inline DS_CXX14_CONSTEXPR bool operator<(const optional<void>& lhs,
                                         const optional<void>& rhs)
{
  return !lhs && rhs;
}

inline DS_CXX14_CONSTEXPR bool operator<=(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return !lhs || rhs;
}

inline DS_CXX14_CONSTEXPR bool operator>(const optional<void>& lhs,
                                         const optional<void>& rhs)
{
  return lhs && !rhs;
}

inline DS_CXX14_CONSTEXPR bool operator>=(const optional<void>& lhs,
                                          const optional<void>& rhs)
{
  return lhs || !rhs;
}

template <typename T1,
          typename T2,
          enable_if_t<detail::is_eq_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator==(const optional<T1>& lhs,
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
          enable_if_t<detail::is_ne_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator!=(const optional<T1>& lhs,
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
          enable_if_t<detail::is_lt_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<(const optional<T1>& lhs,
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
          enable_if_t<detail::is_le_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<=(const optional<T1>& lhs,
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
          enable_if_t<detail::is_gt_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>(const optional<T1>& lhs,
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
          enable_if_t<detail::is_ge_comparable_with<T1, T2>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>=(const optional<T1>& lhs,
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
DS_CXX14_CONSTEXPR bool operator==(const optional<T>& opt, nullopt_t) noexcept
{
  return !opt;
}

#ifndef DS_HAS_CXX20

template <typename T>
DS_CXX14_CONSTEXPR bool operator==(nullopt_t, const optional<T>& opt) noexcept
{
  return !opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator!=(const optional<T>& opt, nullopt_t) noexcept
{
  return !!opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator!=(nullopt_t, const optional<T>& opt) noexcept
{
  return !!opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator<(const optional<T>& opt, nullopt_t) noexcept
{
  DS_UNUSED(opt);
  return false;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator<(nullopt_t, const optional<T>& opt) noexcept
{
  return !!opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator<=(const optional<T>& opt, nullopt_t) noexcept
{
  return !opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator<=(nullopt_t, const optional<T>& opt) noexcept
{
  DS_UNUSED(opt);
  return true;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator>(const optional<T>& opt, nullopt_t) noexcept
{
  return !!opt;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator>(nullopt_t, const optional<T>& opt) noexcept
{
  DS_UNUSED(opt);
  return false;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator>=(const optional<T>& opt, nullopt_t) noexcept
{
  DS_UNUSED(opt);
  return true;
}

template <typename T>
DS_CXX14_CONSTEXPR bool operator>=(nullopt_t, const optional<T>& opt) noexcept
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
          enable_if_t<detail::is_eq_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator==(const optional<T>& opt, const U& val)
{
  return opt && opt.value() == val;
}

template <typename T,
          typename U,
          enable_if_t<detail::is_eq_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator==(const T& val, const optional<U>& opt)
{
  return opt && val == opt.value();
}

template <typename T,
          typename U,
          enable_if_t<detail::is_ne_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator!=(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() != val;
}

template <typename T,
          typename U,
          enable_if_t<detail::is_ne_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator!=(const T& val, const optional<U>& opt)
{
  return !opt || val != opt.value();
}

template <typename T,
          typename U,
          enable_if_t<detail::is_lt_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() < val;
}

template <typename T,
          typename U,
          enable_if_t<detail::is_lt_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<(const T& val, const optional<U>& opt)
{
  return opt && val < opt.value();
}

template <typename T,
          typename U,
          enable_if_t<detail::is_le_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<=(const optional<T>& opt, const U& val)
{
  return !opt || opt.value() <= val;
}

template <typename T,
          typename U,
          enable_if_t<detail::is_le_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator<=(const T& val, const optional<U>& opt)
{
  return opt && val <= opt.value();
}

template <typename T,
          typename U,
          enable_if_t<detail::is_gt_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>(const optional<T>& opt, const U& val)
{
  return !(opt <= val);
}

template <typename T,
          typename U,
          enable_if_t<detail::is_gt_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>(const T& val, const optional<U>& opt)
{
  return !(val <= opt);
}

template <typename T,
          typename U,
          enable_if_t<detail::is_ge_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>=(const optional<T>& opt, const U& val)
{
  return !(opt < val);
}

template <typename T,
          typename U,
          enable_if_t<detail::is_ge_comparable_with<T, U>::value, int> = 0>
DS_CXX14_CONSTEXPR bool operator>=(const T& val, const optional<U>& opt)
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

DS_NAMESPACE_END
