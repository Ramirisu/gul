#pragma once

#include <ds/config.hpp>

#include <ds/functional.hpp>
#include <ds/type_traits.hpp>
#include <ds/utility.hpp>

namespace ds {
struct unexpect_t {
  struct tag_t {
    constexpr explicit tag_t() = default;
  };
  constexpr explicit unexpect_t(tag_t, tag_t) {};
};

DS_CXX17_INLINE constexpr unexpect_t unexpect { unexpect_t::tag_t {},
                                                unexpect_t::tag_t {} };

template <typename E>
class unexpected {
  E err_;

public:
  static_assert(
      !is_void<E>::value,
      "instantiation of unexpected with <E = in_place_t> is ill-formed");

  using error_type = E;

  template <
      typename E2,
      enable_if_t<conjunction<negation<is_same<remove_cvref_t<E2>, in_place_t>>,
                              negation<is_same<remove_cvref_t<E2>, unexpected>>,
                              is_constructible<E, E2>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR explicit unexpected(E2&& err) noexcept
      : err_(std::forward<E2>(err))
  {
  }

  template <typename... Args,
            enable_if_t<is_constructible<E, Args...>::value, int> = 0>
  DS_CXX14_CONSTEXPR explicit unexpected(in_place_t, Args&&... args) noexcept
      : err_(std::forward<Args>(args)...)
  {
  }

  template <typename U,
            typename... Args,
            enable_if_t<
                is_constructible<E, std::initializer_list<U>&, Args...>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR explicit unexpected(in_place_t,
                                         std::initializer_list<U> init,
                                         Args&&... args) noexcept
      : err_(init, std::forward<Args>(args)...)
  {
  }

  DS_CXX14_CONSTEXPR unexpected(const unexpected&) noexcept = default;

  DS_CXX14_CONSTEXPR unexpected(unexpected&&) noexcept = default;

  DS_CXX14_CONSTEXPR unexpected& operator=(const unexpected&) noexcept
      = default;

  DS_CXX14_CONSTEXPR unexpected& operator=(unexpected&&) noexcept = default;

  DS_CXX14_CONSTEXPR E& error() & noexcept
  {
    return err_;
  }

  DS_CXX14_CONSTEXPR const E& error() const& noexcept
  {
    return err_;
  }

  DS_CXX14_CONSTEXPR E&& error() && noexcept
  {
    return std::move(err_);
  }

  DS_CXX14_CONSTEXPR const E&& error() const&& noexcept
  {
    return std::move(err_);
  }

  DS_CXX14_CONSTEXPR void swap(unexpected& other) noexcept
  {
    using std::swap;
    swap(err_, other.err_);
  }

  friend DS_CXX14_CONSTEXPR void swap(unexpected& lhs, unexpected& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  friend DS_CXX14_CONSTEXPR bool operator==(const unexpected& lhs,
                                            const unexpected& rhs) noexcept
  {
    return lhs.err_ == rhs.err_;
  }
};

template <typename T,
          typename E,
          bool = disjunction<is_void<T>, is_trivially_destructible<T>>::value,
          bool = is_trivially_destructible<E>::value>
struct __expected_destruct_base {
  union {
    char nul_;
    conditional_t<is_void<T>::value, char, T> val_;
    E err_;
  };
  bool has_;

  DS_CXX14_CONSTEXPR __expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(in_place_t,
                                                       Args&&... args) noexcept
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(unexpect_t,
                                                       Args&&... args) noexcept
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }
};

template <typename T, typename E>
struct __expected_destruct_base<T, E, true, false> {
  union {
    char nul_;
    conditional_t<is_void<T>::value, char, T> val_;
    E err_;
  };
  bool has_;

  DS_CXX14_CONSTEXPR __expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(in_place_t,
                                                       Args&&... args) noexcept
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(unexpect_t,
                                                       Args&&... args) noexcept
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~__expected_destruct_base() noexcept
  {
    if (!has_) {
      err_.~E();
    }
  }
};

template <typename T, typename E>
struct __expected_destruct_base<T, E, false, true> {
  union {
    char nul_;
    T val_;
    E err_;
  };
  bool has_;

  DS_CXX14_CONSTEXPR __expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(in_place_t,
                                                       Args&&... args) noexcept
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(unexpect_t,
                                                       Args&&... args) noexcept
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~__expected_destruct_base() noexcept
  {
    if (has_) {
      val_.~T();
    }
  }
};

template <typename T, typename E>
struct __expected_destruct_base<T, E, false, false> {
  union {
    char nul_;
    T val_;
    E err_;
  };
  bool has_;

  DS_CXX14_CONSTEXPR __expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(in_place_t,
                                                       Args&&... args) noexcept
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR explicit __expected_destruct_base(unexpect_t,
                                                       Args&&... args) noexcept
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~__expected_destruct_base() noexcept
  {
    if (has_) {
      val_.~T();
    } else {
      err_.~E();
    }
  }
};

template <typename T, typename E>
struct __expected_storage_base : __expected_destruct_base<T, E> {
  using __expected_destruct_base<T, E>::__expected_destruct_base;

  DS_CXX14_CONSTEXPR T& operator*() & noexcept
  {
    return this->val_;
  }

  DS_CXX14_CONSTEXPR const T& operator*() const& noexcept
  {
    return this->val_;
  }

  DS_CXX14_CONSTEXPR T&& operator*() && noexcept
  {
    return std::move(this->val_);
  }
  DS_CXX14_CONSTEXPR const T&& operator*() const&& noexcept
  {
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return this->has_;
  }

  DS_CXX14_CONSTEXPR T& value() & noexcept
  {
    DS_ASSERT(this->has_);
    return this->val_;
  }

  DS_CXX14_CONSTEXPR const T& value() const& noexcept
  {
    DS_ASSERT(this->has_);
    return this->val_;
  }

  DS_CXX14_CONSTEXPR T&& value() && noexcept
  {
    DS_ASSERT(this->has_);
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR const T&& value() const&& noexcept
  {
    DS_ASSERT(this->has_);
    return std::move(this->val_);
  }

  DS_CXX14_CONSTEXPR E& __get_err() & noexcept
  {
    return this->err_;
  }

  DS_CXX14_CONSTEXPR const E& __get_err() const& noexcept
  {
    return this->err_;
  }

  DS_CXX14_CONSTEXPR E&& __get_err() && noexcept
  {
    return std::move(this->err_);
  }

  DS_CXX14_CONSTEXPR const E&& __get_err() const&& noexcept
  {
    return std::move(this->err_);
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR T& __construct_val(Args&&... args) noexcept
  {
    auto& val
        = *(::new (std::addressof(this->val_)) T(std::forward<Args>(args)...));
    this->has_ = true;
    return val;
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR E& __construct_err(Args&&... args) noexcept
  {
    auto& err
        = *(::new (std::addressof(this->err_)) E(std::forward<Args>(args)...));
    this->has_ = false;
    return err;
  }

  DS_CXX14_CONSTEXPR void __destroy_val() noexcept
  {
    this->val_.~T();
  }

  DS_CXX14_CONSTEXPR void __destroy_err() noexcept
  {
    this->err_.~E();
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void __construct_from(Other&& other) noexcept
  {
    if (other.has_value()) {
      __construct_val(std::forward<Other>(other).value());
    } else {
      __construct_err(std::forward<Other>(other).__get_err());
    }
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void __assign_from(Other&& other) noexcept
  {
    if (this->has_) {
      if (other.has_value()) {
        this->val_ = std::forward<Other>(other).value();
      } else {
        __destroy_val();
        __construct_err(std::forward<Other>(other).__get_err());
      }
    } else {
      if (other.has_value()) {
        __destroy_err();
        __construct_val(std::forward<Other>(other).value());
      } else {
        this->err_ = std::forward<Other>(other).__get_err();
      }
    }
  }
};

template <typename E>
struct __expected_storage_base<void, E> : __expected_destruct_base<void, E> {
  using __expected_destruct_base<void, E>::__expected_destruct_base;

  DS_CXX14_CONSTEXPR void operator*() const& noexcept { }

  DS_CXX14_CONSTEXPR void operator*() && noexcept { }

  DS_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return this->has_;
  }

  DS_CXX14_CONSTEXPR void value() & noexcept
  {
    DS_ASSERT(this->has_);
  }

  DS_CXX14_CONSTEXPR void value() const& noexcept
  {
    DS_ASSERT(this->has_);
  }

  DS_CXX14_CONSTEXPR void value() && noexcept
  {
    DS_ASSERT(this->has_);
  }

  DS_CXX14_CONSTEXPR void value() const&& noexcept
  {
    DS_ASSERT(this->has_);
  }

  DS_CXX14_CONSTEXPR E& __get_err() & noexcept
  {
    return this->err_;
  }

  DS_CXX14_CONSTEXPR const E& __get_err() const& noexcept
  {
    return this->err_;
  }

  DS_CXX14_CONSTEXPR E&& __get_err() && noexcept
  {
    return std::move(this->err_);
  }

  DS_CXX14_CONSTEXPR const E&& __get_err() const&& noexcept
  {
    return std::move(this->err_);
  }

  DS_CXX14_CONSTEXPR void __construct_val() noexcept
  {
    this->has_ = true;
  }

  template <typename... Args>
  DS_CXX14_CONSTEXPR E& __construct_err(Args&&... args) noexcept
  {
    auto& err
        = *(::new (std::addressof(this->err_)) E(std::forward<Args>(args)...));
    this->has_ = false;
    return err;
  }

  DS_CXX14_CONSTEXPR void __destroy_val() noexcept { }

  DS_CXX14_CONSTEXPR void __destroy_err() noexcept
  {
    this->err_.~E();
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void __construct_from(Other&& other) noexcept
  {
    if (other.has_value()) {
      __construct_val();
    } else {
      __construct_err(std::forward<Other>(other).__get_err());
    }
  }

  template <typename Other>
  DS_CXX14_CONSTEXPR void __assign_from(Other&& other) noexcept
  {
    if (this->has_) {
      if (other.has_value()) {
        this->val_ = std::forward<Other>(other).value();
      } else {
        __destroy_val();
        __construct_err(std::forward<Other>(other).__get_err());
      }
    } else {
      if (other.has_value()) {
        __destroy_err();
        __construct_val(std::forward<Other>(other).value());
      } else {
        this->err_ = std::forward<Other>(other).__get_err();
      }
    }
  }
};

template <typename T,
          typename E,
          bool
          = disjunction<is_void<T>, is_trivially_copy_constructible<T>>::value,
          bool = is_trivially_copy_constructible<E>::value>
struct __expected_copy_construct_base : __expected_storage_base<T, E> {
  using __expected_storage_base<T, E>::__expected_storage_base;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base() noexcept = default;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base(
      const __expected_copy_construct_base& other) noexcept
  {
    this->__construct_from(other);
  }

  DS_CXX14_CONSTEXPR
  __expected_copy_construct_base(__expected_copy_construct_base&&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base&
  operator=(const __expected_copy_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base&
  operator=(__expected_copy_construct_base&&) noexcept
      = default;
};

template <typename T, typename E>
struct __expected_copy_construct_base<T, E, true, true>
    : __expected_storage_base<T, E> {
  using __expected_storage_base<T, E>::__expected_storage_base;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_construct_base(const __expected_copy_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_construct_base(__expected_copy_construct_base&&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base&
  operator=(const __expected_copy_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_copy_construct_base&
  operator=(__expected_copy_construct_base&&) noexcept
      = default;
};

template <typename T,
          typename E,
          bool
          = disjunction<is_void<T>, is_trivially_move_constructible<T>>::value,
          bool = is_trivially_move_constructible<E>::value>
struct __expected_move_construct_base : __expected_copy_construct_base<T, E> {
  using __expected_copy_construct_base<T, E>::__expected_copy_construct_base;

  DS_CXX14_CONSTEXPR __expected_move_construct_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_move_construct_base(const __expected_move_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_construct_base(
      __expected_move_construct_base&& other) noexcept
  {
    this->__construct_from(std::move(other));
  }

  DS_CXX14_CONSTEXPR __expected_move_construct_base&
  operator=(const __expected_move_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_construct_base&
  operator=(__expected_move_construct_base&&) noexcept
      = default;
};

template <typename T, typename E>
struct __expected_move_construct_base<T, E, true, true>
    : __expected_copy_construct_base<T, E> {
  using __expected_copy_construct_base<T, E>::__expected_copy_construct_base;

  DS_CXX14_CONSTEXPR __expected_move_construct_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_move_construct_base(const __expected_move_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_move_construct_base(__expected_move_construct_base&&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_construct_base&
  operator=(const __expected_move_construct_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_construct_base&
  operator=(__expected_move_construct_base&&) noexcept
      = default;
};

template <typename T,
          typename E,
          bool
          = disjunction<is_void<T>,
                        conjunction<is_trivially_destructible<T>,
                                    is_trivially_copy_constructible<T>,
                                    is_trivially_copy_assignable<T>>>::value,
          bool = conjunction<is_trivially_destructible<E>,
                             is_trivially_copy_constructible<E>,
                             is_trivially_copy_assignable<E>>::value>
struct __expected_copy_assign_base : __expected_move_construct_base<T, E> {
  using __expected_move_construct_base<T, E>::__expected_move_construct_base;

  DS_CXX14_CONSTEXPR __expected_copy_assign_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_assign_base(const __expected_copy_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_assign_base(__expected_copy_assign_base&&) noexcept = default;

  DS_CXX14_CONSTEXPR __expected_copy_assign_base&
  operator=(const __expected_copy_assign_base& other) noexcept
  {
    this->__assign_from(other);
  }

  DS_CXX14_CONSTEXPR __expected_copy_assign_base&
  operator=(__expected_copy_assign_base&&) noexcept
      = default;
};

template <typename T, typename E>
struct __expected_copy_assign_base<T, E, true, true>
    : __expected_move_construct_base<T, E> {
  using __expected_move_construct_base<T, E>::__expected_move_construct_base;

  DS_CXX14_CONSTEXPR __expected_copy_assign_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_assign_base(const __expected_copy_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_copy_assign_base(__expected_copy_assign_base&&) noexcept = default;

  DS_CXX14_CONSTEXPR __expected_copy_assign_base&
  operator=(const __expected_copy_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_copy_assign_base&
  operator=(__expected_copy_assign_base&&) noexcept
      = default;
};

template <typename T,
          typename E,
          bool
          = disjunction<is_void<T>,
                        conjunction<is_trivially_destructible<T>,
                                    is_trivially_move_constructible<T>,
                                    is_trivially_move_assignable<T>>>::value,
          bool = conjunction<is_trivially_destructible<E>,
                             is_trivially_move_constructible<E>,
                             is_trivially_move_assignable<E>>::value>
struct __expected_move_assign_base : __expected_copy_assign_base<T, E> {
  using __expected_copy_assign_base<T, E>::__expected_copy_assign_base;

  DS_CXX14_CONSTEXPR __expected_move_assign_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_move_assign_base(const __expected_move_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_move_assign_base(__expected_move_assign_base&&) noexcept = default;

  DS_CXX14_CONSTEXPR __expected_move_assign_base&
  operator=(const __expected_move_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_assign_base&
  operator=(__expected_move_assign_base&& other) noexcept
  {
    this->__assign_from(std::move(other));
  }
};

template <typename T, typename E>
struct __expected_move_assign_base<T, E, true, true>
    : __expected_copy_assign_base<T, E> {
  using __expected_copy_assign_base<T, E>::__expected_copy_assign_base;

  DS_CXX14_CONSTEXPR __expected_move_assign_base() noexcept = default;

  DS_CXX14_CONSTEXPR
  __expected_move_assign_base(const __expected_move_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR
  __expected_move_assign_base(__expected_move_assign_base&&) noexcept = default;

  DS_CXX14_CONSTEXPR __expected_move_assign_base&
  operator=(const __expected_move_assign_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_move_assign_base&
  operator=(__expected_move_assign_base&&) noexcept
      = default;
};

template <typename T,
          bool = disjunction<is_void<T>, is_default_constructible<T>>::value>
struct __expected_default_constructible_base {
  DS_CXX14_CONSTEXPR __expected_default_constructible_base(in_place_t) noexcept
  {
  }

  DS_CXX14_CONSTEXPR __expected_default_constructible_base() noexcept = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base(
      const __expected_default_constructible_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base(
      __expected_default_constructible_base&&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base&
  operator=(const __expected_default_constructible_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base&
  operator=(__expected_default_constructible_base&&) noexcept
      = default;
};

template <typename T>
struct __expected_default_constructible_base<T, false> {
  DS_CXX14_CONSTEXPR __expected_default_constructible_base(in_place_t) noexcept
  {
  }

  DS_CXX14_CONSTEXPR __expected_default_constructible_base() noexcept = delete;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base(
      const __expected_default_constructible_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base(
      __expected_default_constructible_base&&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base&
  operator=(const __expected_default_constructible_base&) noexcept
      = default;

  DS_CXX14_CONSTEXPR __expected_default_constructible_base&
  operator=(__expected_default_constructible_base&&) noexcept
      = default;
};

template <typename T, typename E>
class expected;

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_and_then_impl(std::true_type, Self&& self, F&& f) noexcept
    -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
{
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using Exp = remove_cvref_t<invoke_result_t<F>>;
  static_assert(is_specialization_of<Exp, expected>::value, "");
  static_assert(is_same<typename Exp::error_type, E>::value, "");
  if (self.has_value()) {
    return Exp(invoke(std::forward<F>(f)));
  } else {
    return Exp(unexpect, std::forward<Self>(self).error());
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_and_then_impl(std::false_type, Self&& self, F&& f) noexcept
    -> remove_cvref_t<decltype(invoke(std::forward<F>(f),
                                      std::forward<Self>(self).value()))>
{
  using T = decltype(std::declval<Self>().value());
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using Exp = remove_cvref_t<invoke_result_t<F, T>>;
  static_assert(is_specialization_of<Exp, expected>::value, "");
  static_assert(is_same<typename Exp::error_type, E>::value, "");
  if (self.has_value()) {
    return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
  } else {
    return Exp(unexpect, std::forward<Self>(self).error());
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_or_else_impl(std::true_type, Self&& self, F&& f) noexcept
    -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
{
  using Exp = remove_cvref_t<invoke_result_t<F>>;
  using T = typename Exp::value_type;
  static_assert(is_specialization_of<Exp, expected>::value, "");
  static_assert(is_same<typename Exp::value_type, T>::value, "");
  if (self.has_value()) {
    return Exp();
  } else {
    return Exp(invoke(std::forward<F>(f)));
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_or_else_impl(std::false_type, Self&& self, F&& f) noexcept
    -> remove_cvref_t<decltype(invoke(std::forward<F>(f),
                                      std::forward<Self>(self).error()))>
{
  using E = decltype(std::declval<Self>().error());
  using Exp = remove_cvref_t<invoke_result_t<F, E>>;
  using T = typename Exp::value_type;
  static_assert(is_specialization_of<Exp, expected>::value, "");
  static_assert(is_same<typename Exp::value_type, T>::value, "");
  if (self.has_value()) {
    return Exp(in_place, std::forward<Self>(self).value());
  } else {
    return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).error()));
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_transform_impl(std::true_type, Self&& self, F&& f) noexcept
    -> expected<remove_cvref_t<decltype(invoke(std::forward<F>(f)))>,
                remove_cvref_t<decltype(std::forward<Self>(self).error())>>
{
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using T2 = remove_cvref_t<invoke_result_t<F>>;
  using Exp = expected<T2, E>;
  static_assert(
      disjunction<
          is_void<Exp>,
          conjunction<negation<is_same<Exp, in_place_t>>,
                      negation<is_same<Exp, unexpect_t>>,
                      negation<is_specialization_of<Exp, unexpected>>>>::value,
      "");
  if (self.has_value()) {
    return Exp(invoke(std::forward<F>(f)));
  } else {
    return Exp(unexpect, std::forward<Self>(self).error());
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_transform_impl(std::false_type, Self&& self, F&& f) noexcept
    -> expected<remove_cvref_t<decltype(invoke(
                    std::forward<F>(f), std::forward<Self>(self).value()))>,
                remove_cvref_t<decltype(std::forward<Self>(self).error())>>
{
  using T = decltype(std::declval<Self>().value());
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using T2 = remove_cvref_t<invoke_result_t<F, T>>;
  using Exp = expected<T2, E>;
  static_assert(
      disjunction<
          is_void<Exp>,
          conjunction<negation<is_same<Exp, in_place_t>>,
                      negation<is_same<Exp, unexpect_t>>,
                      negation<is_specialization_of<Exp, unexpected>>>>::value,
      "");
  if (self.has_value()) {
    return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
  } else {
    return Exp(unexpect, std::forward<Self>(self).error());
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_transform_error_impl(std::true_type, Self&& self, F&& f) noexcept
    -> expected<remove_cvref_t<decltype(std::declval<Self>().value())>,
                remove_cvref_t<decltype(invoke(
                    std::forward<F>(f), std::forward<Self>(self).error()))>>
{
  using T = remove_cvref_t<decltype(std::declval<Self>().value())>;
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using E2 = remove_cvref_t<invoke_result_t<F, E>>;
  using Exp = expected<T, E2>;
  static_assert(
      conjunction<negation<is_same<Exp, in_place_t>>,
                  negation<is_same<Exp, unexpect_t>>,
                  negation<is_specialization_of<Exp, unexpected>>>::value,
      "");
  if (self.has_value()) {
    return Exp(in_place);
  } else {
    return Exp(unexpect,
               invoke(std::forward<F>(f), std::forward<Self>(self).error()));
  }
}

template <typename Self, typename F>
DS_CXX14_CONSTEXPR auto
__expected_transform_error_impl(std::false_type, Self&& self, F&& f) noexcept
    -> expected<remove_cvref_t<decltype(std::declval<Self>().value())>,
                remove_cvref_t<decltype(invoke(
                    std::forward<F>(f), std::forward<Self>(self).error()))>>
{
  using T = remove_cvref_t<decltype(std::declval<Self>().value())>;
  using E = remove_cvref_t<decltype(std::declval<Self>().error())>;
  using E2 = remove_cvref_t<invoke_result_t<F, E>>;
  using Exp = expected<T, E2>;
  static_assert(
      conjunction<negation<is_same<Exp, in_place_t>>,
                  negation<is_same<Exp, unexpect_t>>,
                  negation<is_specialization_of<Exp, unexpected>>>::value,
      "");
  if (self.has_value()) {
    return Exp(in_place, std::forward<Self>(self).value());
  } else {
    return Exp(unexpect,
               invoke(std::forward<F>(f), std::forward<Self>(self).error()));
  }
}

template <typename T, typename E>
class expected : private __expected_move_assign_base<T, E>,
                 private __expected_default_constructible_base<T> {
  using __base_type = __expected_move_assign_base<T, E>;
  using __dc_base_type = __expected_default_constructible_base<T>;
  using __expected_move_assign_base<T, E>::__expected_move_assign_base;

  template <typename T2, typename E2>
  struct __check_expected_constructible
      : negation<conjunction<
            is_constructible<T, expected<T2, E2>&>,
            is_constructible<T, expected<T2, E2>>,
            is_constructible<T, const expected<T2, E2>&>,
            is_constructible<T, const expected<T2, E2>>,
            is_convertible<expected<T2, E2>&, T>,
            is_convertible<expected<T2, E2>, T>,
            is_convertible<const expected<T2, E2>&, T>,
            is_convertible<const expected<T2, E2>, T>,
            is_constructible<unexpected<E>, expected<T2, E2>&>,
            is_constructible<unexpected<E>, expected<T2, E2>>,
            is_constructible<unexpected<E>, const expected<T2, E2>&>,
            is_constructible<unexpected<E>, const expected<T2, E2>>>> { };

public:
  static_assert(
      !is_same<remove_cvref_t<T>, in_place_t>::value,
      "instantiation of expected with <T = in_place_t> is ill-formed");
  static_assert(
      !is_same<remove_cvref_t<T>, unexpect_t>::value,
      "instantiation of expected with <T = unexpect_t> is ill-formed");
  static_assert(!is_void<remove_cvref_t<E>>::value,
                "instantiation of expected with <E = void> is ill-formed");
  static_assert(
      !is_same<remove_cvref_t<E>, in_place_t>::value,
      "instantiation of expected with <E = in_place_t> is ill-formed");
  static_assert(
      !is_same<remove_cvref_t<E>, unexpect_t>::value,
      "instantiation of expected with <E = unexpect_t> is ill-formed");
  static_assert(!is_reference<T>::value,
                "instantiation of expected with <T = &> is ill-formed");
  static_assert(disjunction<is_void<T>, is_destructible<T>>::value,
                "instantiation of expected with <T = [non-void] and "
                "[non-destructible]> is ill-formed");
  static_assert(!is_array<T>::value,
                "instantiation of expected with <T = []> is ill-formed");

  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <typename U>
  using rebind = expected<U, E>;

  DS_CXX14_CONSTEXPR expected() noexcept = default;

  DS_CXX14_CONSTEXPR expected(const expected&) noexcept = default;

  DS_CXX14_CONSTEXPR expected(expected&&) noexcept = default;

  template <
      typename T2,
      typename E2,
      enable_if_t<conjunction<disjunction<conjunction<is_void<T>, is_void<T2>>,
                                          is_constructible<T, const T2&>>,
                              is_constructible<E, const E2&>,
                              __check_expected_constructible<T2, E2>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR
      DS_CXX20_EXPLICIT((!is_convertible<const T2&, T>::value
                         || !is_convertible<const E2&, E>::value))
          expected(const expected<T2, E2>& other) noexcept
      : __dc_base_type(in_place)
  {
    this->__construct_from(other);
  }

  template <
      typename T2,
      typename E2,
      enable_if_t<conjunction<disjunction<conjunction<is_void<T>, is_void<T2>>,
                                          is_constructible<T, T2>>,
                              is_constructible<E, E2>,
                              __check_expected_constructible<T2, E2>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<T2, T>::value
                                        || !is_convertible<E2, E>::value))
      expected(expected<T2, E2>&& other) noexcept
      : __dc_base_type(in_place)
  {
    this->__construct_from(std::move(other));
  }

  template <typename U = T,
            enable_if_t<
                conjunction<
                    negation<is_void<T>>,
                    negation<is_same<remove_cvref<U>, in_place_t>>,
                    negation<is_same<remove_cvref<U>, unexpect_t>>,
                    negation<is_same<remove_cvref<U>, expected>>,
                    negation<is_specialization_of<remove_cvref<U>, unexpected>>,
                    is_constructible<T, U>>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<U, T>::value))
      expected(U&& u) noexcept
      : __base_type(in_place, std::forward<U>(u))
      , __dc_base_type(in_place)
  {
  }

  template <typename E2,
            enable_if_t<is_constructible<E, const E2&>::value, int> = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<const E2&, E>::value))
      expected(const unexpected<E2>& une) noexcept
      : __base_type(unexpect, une.error())
      , __dc_base_type(in_place)
  {
  }

  template <typename E2, enable_if_t<is_constructible<E, E2>::value, int> = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<E2, E>::value))
      expected(unexpected<E2>&& une) noexcept
      : __base_type(unexpect, std::move(une).error())
      , __dc_base_type(in_place)
  {
  }

  template <typename... Args
#if defined(_MSC_VER) && !defined(__clang__) && !defined(DS_HAS_CXX17)
  // workaround: MSVC /std:c++14
#else
            ,
            enable_if_t<
                disjunction<is_void<T>, is_constructible<T, Args...>>::value,
                int>
            = 0
#endif
            >
  DS_CXX14_CONSTEXPR explicit expected(in_place_t, Args&&... args) noexcept
      : __base_type(in_place, std::forward<Args>(args)...)
      , __dc_base_type(in_place)
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
  DS_CXX14_CONSTEXPR explicit expected(in_place_t,
                                       std::initializer_list<U> init,
                                       Args&&... args) noexcept
      : __base_type(in_place, init, std::forward<Args>(args)...)
      , __dc_base_type(in_place)
  {
  }

  template <typename... Args
#if defined(_MSC_VER) && !defined(__clang__) && !defined(DS_HAS_CXX17)
  // workaround: MSVC /std:c++14
#else
            ,
            enable_if_t<is_constructible<E, Args...>::value, int> = 0
#endif
            >
  DS_CXX14_CONSTEXPR explicit expected(unexpect_t, Args&&... args) noexcept
      : __base_type(unexpect, std::forward<Args>(args)...)
      , __dc_base_type(in_place)
  {
  }

  template <typename U,
            typename... Args,
            enable_if_t<
                is_constructible<E, std::initializer_list<U>&, Args...>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR explicit expected(unexpect_t,
                                       std::initializer_list<U> init,
                                       Args&&... args) noexcept
      : __base_type(unexpect, init, std::forward<Args>(args)...)
      , __dc_base_type(in_place)
  {
  }

  DS_CXX14_CONSTEXPR expected& operator=(const expected&) noexcept = default;

  DS_CXX14_CONSTEXPR expected& operator=(expected&&) noexcept = default;

  template <typename U = T,
            enable_if_t<
                conjunction<
                    negation<is_void<T>>,
                    negation<is_same<remove_cvref<U>, expected>>,
                    negation<is_specialization_of<remove_cvref<U>, unexpected>>,
                    is_constructible<T, U>,
                    is_assignable<add_lvalue_reference_t<T>, U>>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR expected& operator=(U&& u) noexcept
  {
    if (this->has_value()) {
      this->val_ = std::forward<U>(u);
    } else {
      this->__destroy_err();
      this->__construct_val(std::forward<U>(u));
      this->has_ = true;
    }

    return *this;
  }

  template <typename E2,
            enable_if_t<conjunction<is_constructible<E, const E2&>,
                                    is_assignable<add_lvalue_reference_t<E>,
                                                  const E2&>>::value,
                        int>
            = 0>
  DS_CXX14_CONSTEXPR expected& operator=(const unexpected<E2>& une) noexcept
  {
    if (this->has_value()) {
      this->__destroy_val();
      this->has_ = false;
      this->__construct_err(une.error());
    } else {
      this->__get_err() = une.error();
    }

    return *this;
  }

  template <
      typename E2,
      enable_if_t<
          conjunction<is_constructible<E, E2>,
                      is_assignable<add_lvalue_reference_t<E>, E2>>::value,
          int>
      = 0>
  DS_CXX14_CONSTEXPR expected& operator=(unexpected<E2>&& une) noexcept
  {
    if (this->has_value()) {
      this->__destroy_val();
      this->has_ = false;
      this->__construct_err(std::move(une).error());
    } else {
      this->__get_err() = std::move(une).error();
    }

    return *this;
  }

  template <bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR T* operator->() noexcept
  {
    return std::addressof(this->val_);
  }

  template <bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR const T* operator->() const noexcept
  {
    return std::addressof(this->val_);
  }

  using __base_type::operator*;

  DS_CXX14_CONSTEXPR explicit operator bool() const noexcept
  {
    return this->has_;
  }

  using __base_type::has_value;

  using __base_type::value;

  using __base_type::__get_err;

  DS_CXX14_CONSTEXPR E& error() & noexcept
  {
    DS_ASSERT(!this->has_value());
    return this->__get_err();
  }

  DS_CXX14_CONSTEXPR const E& error() const& noexcept
  {
    DS_ASSERT(!this->has_value());
    return this->__get_err();
  }

  DS_CXX14_CONSTEXPR E&& error() && noexcept
  {
    DS_ASSERT(!this->has_value());
    return std::move(this->__get_err());
  }

  DS_CXX14_CONSTEXPR const E&& error() const&& noexcept
  {
    DS_ASSERT(!this->has_value());
    return std::move(this->__get_err());
  }

  template <typename U = T>
  DS_CXX14_CONSTEXPR T value_or(U&& default_value) const& noexcept
  {
    if (this->has_value()) {
      return value();
    } else {
      return static_cast<T>(std::forward<U>(default_value));
    }
  }

  template <typename U = T>
  DS_CXX14_CONSTEXPR T value_or(U&& default_value) && noexcept
  {
    if (this->has_value()) {
      return std::move(value());
    } else {
      return static_cast<T>(std::forward<U>(default_value));
    }
  }

  template <typename U = E>
  DS_CXX14_CONSTEXPR E error_or(U&& default_error) const& noexcept
  {
    static_assert(is_copy_constructible<E>::value, "");
    static_assert(is_convertible<U, E>::value, "");
    if (this->has_value()) {
      return static_cast<E>(std::forward<U>(default_error));
    } else {
      return error();
    }
  }

  template <typename U = E>
  DS_CXX14_CONSTEXPR E error_or(U&& default_error) && noexcept
  {
    static_assert(is_copy_constructible<E>::value, "");
    static_assert(is_convertible<U, E>::value, "");
    if (this->has_value()) {
      return static_cast<E>(std::forward<U>(default_error));
    } else {
      return std::move(error());
    }
  }

  template <
      typename... Args,
      enable_if_t<disjunction<is_void<T>, is_constructible<T, Args...>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR auto emplace(Args&&... args) noexcept
      -> decltype(this->__construct_val(std::forward<Args>(args)...))
  {
    if (this->has_value()) {
      this->__destroy_val();
    } else {
      this->__destroy_err();
    }

    this->has_ = true;
    return this->__construct_val(std::forward<Args>(args)...);
  }

  template <typename U,
            typename... Args,
            enable_if_t<
                is_constructible<T, std::initializer_list<U>&, Args...>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR auto emplace(std::initializer_list<U> init,
                                  Args&&... args) noexcept
      -> decltype(this->__construct_val(init, std::forward<Args>(args)...))
  {
    if (this->has_value()) {
      this->__destroy_val();
    } else {
      this->__destroy_err();
    }

    this->has_ = true;
    return this->__construct_val(init, std::forward<Args>(args)...);
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) & noexcept
      -> decltype(__expected_and_then_impl(is_void<T> {},
                                           *this,
                                           std::forward<F>(f)))
  {
    return __expected_and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) const& noexcept
      -> decltype(__expected_and_then_impl(is_void<T> {},
                                           *this,
                                           std::forward<F>(f)))
  {
    return __expected_and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) && noexcept
      -> decltype(__expected_and_then_impl(is_void<T> {},
                                           std::move(*this),
                                           std::forward<F>(f)))
  {
    return __expected_and_then_impl(is_void<T> {}, std::move(*this),
                                    std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) const&& noexcept
      -> decltype(__expected_and_then_impl(is_void<T> {},
                                           std::move(*this),
                                           std::forward<F>(f)))
  {
    return __expected_and_then_impl(is_void<T> {}, std::move(*this),
                                    std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) & noexcept
      -> decltype(__expected_or_else_impl(is_void<T> {},
                                          *this,
                                          std::forward<F>(f)))
  {
    return __expected_or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) const& noexcept
      -> decltype(__expected_or_else_impl(is_void<T> {},
                                          *this,
                                          std::forward<F>(f)))
  {
    return __expected_or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) && noexcept
      -> decltype(__expected_or_else_impl(is_void<T> {},
                                          std::move(*this),
                                          std::forward<F>(f)))
  {
    return __expected_or_else_impl(is_void<T> {}, std::move(*this),
                                   std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) const&& noexcept
      -> decltype(__expected_or_else_impl(is_void<T> {},
                                          std::move(*this),
                                          std::forward<F>(f)))
  {
    return __expected_or_else_impl(is_void<T> {}, std::move(*this),
                                   std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) & noexcept
      -> decltype(__expected_transform_impl(is_void<T> {},
                                            *this,
                                            std::forward<F>(f)))
  {
    return __expected_transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) const& noexcept
      -> decltype(__expected_transform_impl(is_void<T> {},
                                            *this,
                                            std::forward<F>(f)))
  {
    return __expected_transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) && noexcept
      -> decltype(__expected_transform_impl(is_void<T> {},
                                            std::move(*this),
                                            std::forward<F>(f)))
  {
    return __expected_transform_impl(is_void<T> {}, std::move(*this),
                                     std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) const&& noexcept
      -> decltype(__expected_transform_impl(is_void<T> {},
                                            std::move(*this),
                                            std::forward<F>(f)))
  {
    return __expected_transform_impl(is_void<T> {}, std::move(*this),
                                     std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto transform_error(F&& f) & noexcept
      -> decltype(__expected_transform_error_impl(is_void<T> {},
                                                  *this,
                                                  std::forward<F>(f)))
  {
    return __expected_transform_error_impl(is_void<T> {}, *this,
                                           std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto transform_error(F&& f) const& noexcept
      -> decltype(__expected_transform_error_impl(is_void<T> {},
                                                  *this,
                                                  std::forward<F>(f)))
  {
    return __expected_transform_error_impl(is_void<T> {}, *this,
                                           std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto transform_error(F&& f) && noexcept
      -> decltype(__expected_transform_error_impl(is_void<T> {},
                                                  std::move(*this),
                                                  std::forward<F>(f)))
  {
    return __expected_transform_error_impl(is_void<T> {}, std::move(*this),
                                           std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto transform_error(F&& f) const&& noexcept
      -> decltype(__expected_transform_error_impl(is_void<T> {},
                                                  std::move(*this),
                                                  std::forward<F>(f)))
  {
    return __expected_transform_error_impl(is_void<T> {}, std::move(*this),
                                           std::forward<F>(f));
  }

  DS_CXX14_CONSTEXPR void swap(expected& other) noexcept
  {
    if (this->has_value()) {
      if (other.has_value()) {
        using std::swap;
        swap(this->value(), other.value());
      } else {
        auto val = std::move(this->value());
        this->__destroy_val();
        this->__construct_err(std::move(other.__get_err()));
        other.__destroy_err();
        other.__construct_val(std::move(val));
      }
    } else {
      if (other.has_value()) {
        other.swap(*this);
      } else {
        using std::swap;
        swap(this->__get_err(), other.__get_err());
      }
    }
  }

  friend DS_CXX14_CONSTEXPR void swap(expected& lhs, expected& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  template <typename T2,
            typename E2,
            enable_if_t<conjunction<is_void<T>, is_void<T2>>::value, int> = 0>
  friend DS_CXX14_CONSTEXPR bool
  operator==(const expected& lhs, const expected<T2, E2>& rhs) noexcept
  {
    if (lhs.has_value() != rhs.has_value()) {
      return false;
    }
    if (lhs.has_value()) {
      return true;
    }

    return lhs.error() == rhs.error();
  }

  template <typename T2,
            typename E2,
            enable_if_t<
                conjunction<negation<is_void<T>>, negation<is_void<T2>>>::value,
                int>
            = 0>
  friend DS_CXX14_CONSTEXPR bool
  operator==(const expected& lhs, const expected<T2, E2>& rhs) noexcept
  {
    if (lhs.has_value() != rhs.has_value()) {
      return false;
    }
    if (lhs.has_value()) {
      return lhs.value() == rhs.value();
    }

    return lhs.error() == rhs.error();
  }

  template <typename T2, bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs,
                                            const T2& rhs) noexcept
  {
    return lhs.has_value() && static_cast<bool>(lhs.value() == rhs);
  }

  template <typename E2>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs,
                                            const unexpected<E2>& rhs) noexcept
  {
    return !lhs.has_value() && static_cast<bool>(lhs.error() == rhs.error());
  }
};
}
