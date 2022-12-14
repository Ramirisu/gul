//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/constructor_base.hpp>

#include <gul/fwd.hpp>

#include <gul/invoke.hpp>
#include <gul/type_traits.hpp>
#include <gul/utility.hpp>

GUL_NAMESPACE_BEGIN

struct unexpect_t {
  struct tag_t {
    constexpr explicit tag_t() = default;
  };
  constexpr explicit unexpect_t(tag_t, tag_t) {};
};

GUL_CXX17_INLINE constexpr unexpect_t unexpect { unexpect_t::tag_t {},
                                                 unexpect_t::tag_t {} };

template <typename E>
class unexpected {
  E err_;

public:
  static_assert(std::is_same<remove_cv_t<E>, E>::value,
                "[unexpected] E must not be cv-qualified");
  static_assert(std::is_object<E>::value,
                "[unexpected] E must be an object type");
  static_assert(!std::is_array<E>::value,
                "[unexpected] E must not be an array type");
  static_assert(!detail::is_specialization_of<E, gul::unexpected>::value,
                "[unexpected] E must not be a specialization of unexpected");

  using error_type = E;

  template <typename E2,
            GUL_REQUIRES(conjunction<
                         negation<std::is_same<remove_cvref_t<E2>, in_place_t>>,
                         negation<std::is_same<remove_cvref_t<E2>, unexpected>>,
                         std::is_constructible<E, E2>>::value)>
  GUL_CXX14_CONSTEXPR explicit unexpected(E2&& err)
      : err_(std::forward<E2>(err))
  {
  }

  template <typename... Args,
            GUL_REQUIRES(std::is_constructible<E, Args...>::value)>
  GUL_CXX14_CONSTEXPR explicit unexpected(in_place_t, Args&&... args)
      : err_(std::forward<Args>(args)...)
  {
  }

  template <
      typename U,
      typename... Args,
      GUL_REQUIRES(
          std::is_constructible<E, std::initializer_list<U>&, Args...>::value)>
  GUL_CXX14_CONSTEXPR explicit unexpected(in_place_t,
                                          std::initializer_list<U> init,
                                          Args&&... args)
      : err_(init, std::forward<Args>(args)...)
  {
  }

  GUL_CXX14_CONSTEXPR unexpected(const unexpected&) = default;

  GUL_CXX14_CONSTEXPR unexpected(unexpected&&) = default;

  GUL_CXX14_CONSTEXPR E& error() & noexcept
  {
    return err_;
  }

  GUL_CXX14_CONSTEXPR const E& error() const& noexcept
  {
    return err_;
  }

  GUL_CXX14_CONSTEXPR E&& error() && noexcept
  {
    return std::move(err_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const E&& error() const&& noexcept
  {
    return std::move(err_);
  }
#endif

  GUL_CXX14_CONSTEXPR void
  swap(unexpected& other) noexcept(detail::is_nothrow_swappable<E>::value)
  {
    using std::swap;
    swap(err_, other.err_);
  }

  friend GUL_CXX14_CONSTEXPR void
  swap(unexpected& lhs, unexpected& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }
};

template <typename E1,
          typename E2,
          GUL_REQUIRES(detail::is_eq_comparable_with<E1, E2>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const unexpected<E1>& lhs,
                                    const unexpected<E2>& rhs)
{
  return lhs.error() == rhs.error();
}

template <typename E1,
          typename E2,
          GUL_REQUIRES(detail::is_eq_comparable_with<E1, E2>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const unexpected<E1>& lhs,
                                    const unexpected<E2>& rhs)
{
  return !(lhs.error() == rhs.error());
}

#if !GUL_NO_EXCEPTIONS

template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
protected:
  bad_expected_access() noexcept = default;

  ~bad_expected_access() noexcept override = default;

  bad_expected_access(const bad_expected_access&) noexcept = default;

  bad_expected_access& operator=(const bad_expected_access&) noexcept = default;
};

template <typename E>
class bad_expected_access : public bad_expected_access<void> {
public:
  explicit bad_expected_access(E err)
      : err_(std::move(err))
  {
  }

  E& error() & noexcept
  {
    return err_;
  }

  const E& error() const& noexcept
  {
    return err_;
  }

  E&& error() && noexcept
  {
    return std::move(err_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  const E&& error() const&& noexcept
  {
    return std::move(err_);
  }
#endif

  const char* what() const noexcept override
  {
    return "bad expected access";
  }

private:
  E err_;
};

#endif

namespace detail {

template <typename E>
struct expected_throw_base {
  void throw_bad_expected_access(E err) const
  {
#if GUL_NO_EXCEPTIONS
    GUL_UNUSED(err);
    std::abort();
#else
    throw bad_expected_access<E>(err);
#endif
  }
};

template <typename T,
          typename E,
          bool = std::is_reference<T>::value,
          bool = disjunction<std::is_void<T>,
                             std::is_trivially_destructible<T>>::value,
          bool = std::is_trivially_destructible<E>::value>
struct expected_destruct_base : expected_throw_base<E> {
  union {
    char nul_;
    conditional_t<std::is_void<T>::value, char, T> val_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }
};

template <typename T, typename E>
struct expected_destruct_base<T, E, false, true, false>
    : expected_throw_base<E> {
  union {
    char nul_;
    conditional_t<std::is_void<T>::value, char, T> val_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~expected_destruct_base()
  {
    if (!has_) {
      err_.~E();
    }
  }
};

template <typename T, typename E>
struct expected_destruct_base<T, E, false, false, true>
    : expected_throw_base<E> {
  union {
    char nul_;
    T val_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~expected_destruct_base()
  {
    if (has_) {
      val_.~T();
    }
  }
};

template <typename T, typename E>
struct expected_destruct_base<T, E, false, false, false>
    : expected_throw_base<E> {
  union {
    char nul_;
    T val_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : nul_()
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                      Args&&... args)
      : val_(std::forward<Args>(args)...)
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~expected_destruct_base()
  {
    if (has_) {
      val_.~T();
    } else {
      err_.~E();
    }
  }
};

template <typename T, typename E, bool Unused>
struct expected_destruct_base<T, E, true, Unused, true>
    : expected_throw_base<E> {
  union {
    remove_reference_t<T>* valptr_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : valptr_(nullptr)
      , has_(true)
  {
  }

  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(
      in_place_t, remove_reference_t<T>& arg)
      : valptr_(std::addressof(arg))
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }
};

template <typename T, typename E, bool Unused>
struct expected_destruct_base<T, E, true, Unused, false>
    : expected_throw_base<E> {
  union {
    remove_reference_t<T>* valptr_;
    E err_;
  };
  bool has_;

  GUL_CXX14_CONSTEXPR expected_destruct_base() noexcept
      : valptr_(nullptr)
      , has_(true)
  {
  }

  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(
      in_place_t, remove_reference_t<T>& arg)
      : valptr_(std::addressof(arg))
      , has_(true)
  {
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                      Args&&... args)
      : err_(std::forward<Args>(args)...)
      , has_(false)
  {
  }

  ~expected_destruct_base()
  {
    if (!has_) {
      err_.~E();
    }
  }
};

template <typename T, typename E, bool = std::is_reference<T>::value>
struct expected_storage_base : expected_destruct_base<T, E> {
  using expected_destruct_base<T, E>::expected_destruct_base;

  GUL_CXX14_CONSTEXPR add_pointer_t<T> operator->() noexcept
  {
    return std::addressof(this->val_);
  }

  GUL_CXX14_CONSTEXPR add_pointer_t<const T> operator->() const noexcept
  {
    return std::addressof(this->val_);
  }

  GUL_CXX14_CONSTEXPR T& operator*() & noexcept
  {
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR const T& operator*() const& noexcept
  {
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR T&& operator*() && noexcept
  {
    return std::move(this->val_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const T&& operator*() const&& noexcept
  {
    return std::move(this->val_);
  }
#endif

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return this->has_;
  }

  GUL_CXX14_CONSTEXPR T& value() &
  {
    if (!this->has_) {
      this->throw_bad_expected_access(get_err());
    }
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR const T& value() const&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(get_err());
    }
    return this->val_;
  }

  GUL_CXX14_CONSTEXPR T&& value() &&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(std::move(get_err()));
    }
    return std::move(this->val_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const T&& value() const&&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(std::move(get_err()));
    }
    return std::move(this->val_);
  }
#endif

  GUL_CXX14_CONSTEXPR E& get_err() & noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR const E& get_err() const& noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR E&& get_err() && noexcept
  {
    return std::move(this->err_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const E&& get_err() const&& noexcept
  {
    return std::move(this->err_);
  }
#endif

  template <typename... Args>
  GUL_CXX14_CONSTEXPR T& construct_val(Args&&... args)
  {
    auto& val
        = *(::new (std::addressof(this->val_)) T(std::forward<Args>(args)...));
    this->has_ = true;
    return val;
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR E& construct_err(Args&&... args)
  {
    auto& err
        = *(::new (std::addressof(this->err_)) E(std::forward<Args>(args)...));
    this->has_ = false;
    return err;
  }

  GUL_CXX14_CONSTEXPR void destroy_val()
  {
    this->val_.~T();
  }

  GUL_CXX14_CONSTEXPR void destroy_err()
  {
    this->err_.~E();
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct_val(std::forward<Other>(other).value());
    } else {
      construct_err(std::forward<Other>(other).get_err());
    }
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void assign_from(Other&& other)
  {
    if (this->has_) {
      if (other.has_value()) {
        this->val_ = std::forward<Other>(other).value();
      } else {
        destroy_val();
        construct_err(std::forward<Other>(other).get_err());
      }
    } else {
      if (other.has_value()) {
        destroy_err();
        construct_val(std::forward<Other>(other).value());
      } else {
        this->err_ = std::forward<Other>(other).get_err();
      }
    }
  }

  template <bool B = conjunction<
                detail::is_swappable<T>,
                detail::is_swappable<E>,
                std::is_move_constructible<T>,
                std::is_move_constructible<E>,
                disjunction<std::is_nothrow_move_constructible<T>,
                            std::is_nothrow_move_constructible<E>>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR void swap(expected_storage_base& other) noexcept(
      conjunction<std::is_nothrow_move_constructible<T>,
                  detail::is_nothrow_swappable<T>,
                  std::is_nothrow_move_constructible<E>,
                  detail::is_nothrow_swappable<E>>::value)
  {
    if (this->has_value()) {
      if (other.has_value()) {
        using std::swap;
        swap(this->value(), other.value());
      } else {
        swap_impl(other, std::is_nothrow_move_constructible<T> {},
                  std::is_nothrow_move_constructible<E> {});
      }
    } else {
      if (other.has_value()) {
        other.swap(*this);
      } else {
        using std::swap;
        swap(this->get_err(), other.get_err());
      }
    }
  }

private:
  GUL_CXX14_CONSTEXPR void swap_impl(expected_storage_base& other,
                                     std::true_type,
                                     std::true_type) noexcept
  {
    auto val = std::move(this->value());
    this->destroy_val();
    this->construct_err(std::move(other.get_err()));
    other.destroy_err();
    other.construct_val(std::move(val));
  }

  GUL_CXX20_CONSTEXPR void
  swap_impl(expected_storage_base& other, std::true_type, std::false_type)
  {
    auto val(std::move(this->value()));
    this->destroy_val();
    GUL_TRY
    {
      this->construct_err(std::move(other.get_err()));
      other.destroy_err();
      other.construct_val(std::move(val));
    }
    GUL_CATCH(...)
    {
      this->construct_val(std::move(val));
      GUL_RETHROW();
    }
  }

  GUL_CXX20_CONSTEXPR void
  swap_impl(expected_storage_base& other, std::false_type, std::true_type)
  {
    auto err(std::move(other.get_err()));
    other.destroy_err();
    GUL_TRY
    {
      other.construct_val(std::move(this->value()));
      this->destroy_val();
      this->construct_err(std::move(err));
    }
    GUL_CATCH(...)
    {
      other.construct_err(std::move(err));
      GUL_RETHROW();
    }
  }
};

template <typename T, typename E>
struct expected_storage_base<T, E, true> : expected_destruct_base<T, E> {
  using expected_destruct_base<T, E>::expected_destruct_base;

  GUL_CXX14_CONSTEXPR add_pointer_t<T> operator->() noexcept
  {
    return this->valptr_;
  }

  GUL_CXX14_CONSTEXPR add_pointer_t<const T> operator->() const noexcept
  {
    return this->valptr_;
  }

  GUL_CXX14_CONSTEXPR T& operator*() const& noexcept
  {
    return *this->valptr_;
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR T&& operator*() const&& noexcept
  {
    return std::forward<T>(*this->valptr_);
  }
#endif

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return this->has_;
  }

  GUL_CXX14_CONSTEXPR T& value() const&
  {
    if (!has_value()) {
      this->throw_bad_expected_access(get_err());
    }
    return *this->valptr_;
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR T&& value() const&&
  {
    if (!has_value()) {
      this->throw_bad_expected_access(std::move(get_err()));
    }
    return std::forward<T>(*this->valptr_);
  }
#endif

  GUL_CXX14_CONSTEXPR E& get_err() & noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR const E& get_err() const& noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR E&& get_err() && noexcept
  {
    return std::move(this->err_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const E&& get_err() const&& noexcept
  {
    return std::move(this->err_);
  }
#endif

  GUL_CXX14_CONSTEXPR T& construct_val(remove_reference_t<T>& arg)
  {
    this->valptr_ = std::addressof(arg);
    this->has_ = true;
    return *this->valptr_;
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR E& construct_err(Args&&... args)
  {
    auto& err
        = *(::new (std::addressof(this->err_)) E(std::forward<Args>(args)...));
    this->has_ = false;
    return err;
  }

  GUL_CXX14_CONSTEXPR void destroy_val()
  {
    this->valptr_ = nullptr;
  }

  GUL_CXX14_CONSTEXPR void destroy_err()
  {
    this->err_.~E();
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct_val(std::forward<Other>(other).value());
    } else {
      construct_err(std::forward<Other>(other).get_err());
    }
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void assign_from(Other&& other)
  {
    if (this->has_) {
      if (other.has_value()) {
        this->valptr_ = std::addressof(std::forward<Other>(other).value());
      } else {
        destroy_val();
        construct_err(std::forward<Other>(other).get_err());
      }
    } else {
      if (other.has_value()) {
        destroy_err();
        construct_val(std::forward<Other>(other).value());
      } else {
        this->err_ = std::forward<Other>(other).get_err();
      }
    }
  }

  GUL_CXX14_CONSTEXPR void swap(expected_storage_base&)
  {
    static_assert(!std::is_reference<T>::value,
                  "[expected::swap] T must not be a reference type");
  }
};

template <typename E>
struct expected_storage_base<void, E, false> : expected_destruct_base<void, E> {
  using expected_destruct_base<void, E, false>::expected_destruct_base;

  GUL_CXX14_CONSTEXPR void operator->() noexcept { }

  GUL_CXX14_CONSTEXPR void operator->() const noexcept { }

  GUL_CXX14_CONSTEXPR void operator*() const& noexcept { }

  GUL_CXX14_CONSTEXPR void operator*() && noexcept { }

  GUL_CXX14_CONSTEXPR bool has_value() const noexcept
  {
    return this->has_;
  }

  GUL_CXX14_CONSTEXPR void value() &
  {
    if (!this->has_) {
      this->throw_bad_expected_access(get_err());
    }
  }

  GUL_CXX14_CONSTEXPR void value() const&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(get_err());
    }
  }

  GUL_CXX14_CONSTEXPR void value() &&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(std::move(get_err()));
    }
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR void value() const&&
  {
    if (!this->has_) {
      this->throw_bad_expected_access(std::move(get_err()));
    }
  }
#endif

  GUL_CXX14_CONSTEXPR E& get_err() & noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR const E& get_err() const& noexcept
  {
    return this->err_;
  }

  GUL_CXX14_CONSTEXPR E&& get_err() && noexcept
  {
    return std::move(this->err_);
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const E&& get_err() const&& noexcept
  {
    return std::move(this->err_);
  }
#endif

  GUL_CXX14_CONSTEXPR void construct_val() noexcept
  {
    this->has_ = true;
  }

  template <typename... Args>
  GUL_CXX14_CONSTEXPR E& construct_err(Args&&... args)
  {
    auto& err
        = *(::new (std::addressof(this->err_)) E(std::forward<Args>(args)...));
    this->has_ = false;
    return err;
  }

  GUL_CXX14_CONSTEXPR void destroy_val() noexcept { }

  GUL_CXX14_CONSTEXPR void destroy_err()
  {
    this->err_.~E();
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void construct_from(Other&& other)
  {
    if (other.has_value()) {
      construct_val();
    } else {
      construct_err(std::forward<Other>(other).get_err());
    }
  }

  template <typename Other>
  GUL_CXX14_CONSTEXPR void assign_from(Other&& other)
  {
    if (this->has_) {
      if (other.has_value()) {
      } else {
        destroy_val();
        construct_err(std::forward<Other>(other).get_err());
      }
    } else {
      if (other.has_value()) {
        destroy_err();
        construct_val();
      } else {
        this->err_ = std::forward<Other>(other).get_err();
      }
    }
  }

  GUL_CXX14_CONSTEXPR void swap(expected_storage_base& other) noexcept(
      conjunction<std::is_nothrow_move_constructible<E>,
                  detail::is_nothrow_swappable<E>>::value)
  {
    if (this->has_value()) {
      if (!other.has_value()) {
        this->destroy_val();
        this->construct_err(std::move(other.get_err()));
        other.destroy_err();
        other.construct_val();
      }
    } else {
      if (other.has_value()) {
        other.swap(*this);
      } else {
        using std::swap;
        swap(this->get_err(), other.get_err());
      }
    }
  }
};

template <typename T,
          typename E,
          bool = disjunction<std::is_void<T>,
                             detail::is_trivially_copy_constructible<T>>::value,
          bool = detail::is_trivially_copy_constructible<E>::value>
struct expected_copy_construct_base : expected_storage_base<T, E> {
  using expected_storage_base<T, E>::expected_storage_base;

  GUL_CXX14_CONSTEXPR expected_copy_construct_base() = default;

  GUL_CXX14_CONSTEXPR
  expected_copy_construct_base(const expected_copy_construct_base& other)
      : expected_storage_base<T, E>::expected_storage_base()
  {
    this->construct_from(other);
  }

  GUL_CXX14_CONSTEXPR
  expected_copy_construct_base(expected_copy_construct_base&&) = default;

  GUL_CXX14_CONSTEXPR expected_copy_construct_base&
  operator=(const expected_copy_construct_base&)
      = default;

  GUL_CXX14_CONSTEXPR expected_copy_construct_base&
  operator=(expected_copy_construct_base&&)
      = default;
};

template <typename T, typename E>
struct expected_copy_construct_base<T, E, true, true>
    : expected_storage_base<T, E> {
  using expected_storage_base<T, E>::expected_storage_base;
};

template <typename T,
          typename E,
          bool = disjunction<std::is_void<T>,
                             detail::is_trivially_move_constructible<T>>::value,
          bool = detail::is_trivially_move_constructible<E>::value>
struct expected_move_construct_base : expected_copy_construct_base<T, E> {
  using expected_copy_construct_base<T, E>::expected_copy_construct_base;

  GUL_CXX14_CONSTEXPR expected_move_construct_base() = default;

  GUL_CXX14_CONSTEXPR
  expected_move_construct_base(const expected_move_construct_base&) = default;

  GUL_CXX14_CONSTEXPR
  expected_move_construct_base(expected_move_construct_base&& other)
  {
    this->construct_from(std::move(other));
  }

  GUL_CXX14_CONSTEXPR expected_move_construct_base&
  operator=(const expected_move_construct_base&)
      = default;

  GUL_CXX14_CONSTEXPR expected_move_construct_base&
  operator=(expected_move_construct_base&&)
      = default;
};

template <typename T, typename E>
struct expected_move_construct_base<T, E, true, true>
    : expected_copy_construct_base<T, E> {
  using expected_copy_construct_base<T, E>::expected_copy_construct_base;
};

template <typename T,
          typename E,
          bool = disjunction<
              std::is_void<T>,
              conjunction<std::is_trivially_destructible<T>,
                          detail::is_trivially_copy_constructible<T>,
                          detail::is_trivially_copy_assignable<T>>>::value,
          bool = conjunction<std::is_trivially_destructible<E>,
                             detail::is_trivially_copy_constructible<E>,
                             detail::is_trivially_copy_assignable<E>>::value>
struct expected_copy_assign_base : expected_move_construct_base<T, E> {
  using expected_move_construct_base<T, E>::expected_move_construct_base;

  GUL_CXX14_CONSTEXPR expected_copy_assign_base() = default;

  GUL_CXX14_CONSTEXPR
  expected_copy_assign_base(const expected_copy_assign_base&) = default;

  GUL_CXX14_CONSTEXPR
  expected_copy_assign_base(expected_copy_assign_base&&) = default;

  GUL_CXX14_CONSTEXPR expected_copy_assign_base&
  operator=(const expected_copy_assign_base& other)
  {
    this->assign_from(other);
    return *this;
  }

  GUL_CXX14_CONSTEXPR expected_copy_assign_base&
  operator=(expected_copy_assign_base&&)
      = default;
};

template <typename T, typename E>
struct expected_copy_assign_base<T, E, true, true>
    : expected_move_construct_base<T, E> {
  using expected_move_construct_base<T, E>::expected_move_construct_base;
};

template <typename T,
          typename E,
          bool = disjunction<
              std::is_void<T>,
              conjunction<std::is_trivially_destructible<T>,
                          detail::is_trivially_move_constructible<T>,
                          detail::is_trivially_move_assignable<T>>>::value,
          bool = conjunction<std::is_trivially_destructible<E>,
                             detail::is_trivially_move_constructible<E>,
                             detail::is_trivially_move_assignable<E>>::value>
struct expected_move_assign_base : expected_copy_assign_base<T, E> {
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;

  GUL_CXX14_CONSTEXPR expected_move_assign_base() = default;

  GUL_CXX14_CONSTEXPR
  expected_move_assign_base(const expected_move_assign_base&) = default;

  GUL_CXX14_CONSTEXPR
  expected_move_assign_base(expected_move_assign_base&&) = default;

  GUL_CXX14_CONSTEXPR expected_move_assign_base&
  operator=(const expected_move_assign_base&)
      = default;

  GUL_CXX14_CONSTEXPR expected_move_assign_base&
  operator=(expected_move_assign_base&& other)
  {
    this->assign_from(std::move(other));
    return *this;
  }
};

template <typename T, typename E>
struct expected_move_assign_base<T, E, true, true>
    : expected_copy_assign_base<T, E> {
  using expected_copy_assign_base<T, E>::expected_copy_assign_base;
};

template <typename T>
using expected_enable_default_construct_base = detail::default_construct_base<
    disjunction<std::is_void<T>, std::is_default_constructible<T>>::value>;

template <typename T, typename E>
using expected_enable_copy_construct_base = detail::copy_construct_base<
    conjunction<disjunction<std::is_void<T>, std::is_copy_constructible<T>>,
                std::is_copy_constructible<E>>::value>;

template <typename T, typename E>
using expected_enable_move_construct_base = detail::move_construct_base<
    conjunction<disjunction<std::is_void<T>, std::is_move_constructible<T>>,
                std::is_move_constructible<E>>::value>;

template <typename T, typename E>
using expected_enable_copy_assign_base = detail::copy_assign_base<
    conjunction<disjunction<std::is_void<T>,
                            conjunction<std::is_copy_constructible<T>,
                                        std::is_copy_assignable<T>>>,
                std::is_copy_constructible<E>,
                std::is_copy_assignable<E>>::value>;

template <typename T, typename E>
using expected_enable_move_assign_base = detail::move_assign_base<
    conjunction<disjunction<std::is_void<T>,
                            conjunction<std::is_move_constructible<T>,
                                        std::is_move_assignable<T>>>,
                std::is_move_constructible<E>,
                std::is_move_assignable<E>>::value>;
}

template <typename T, typename E>
class expected : private detail::expected_move_assign_base<T, E>,
                 private detail::expected_enable_default_construct_base<T>,
                 private detail::expected_enable_copy_construct_base<T, E>,
                 private detail::expected_enable_move_construct_base<T, E>,
                 private detail::expected_enable_copy_assign_base<T, E>,
                 private detail::expected_enable_move_assign_base<T, E> {
  using base_type = detail::expected_move_assign_base<T, E>;
  using dc_base_type = detail::expected_enable_default_construct_base<T>;

  template <typename T2, typename E2>
  struct is_expected_constructible
      : negation<conjunction<
            std::is_constructible<T, expected<T2, E2>&>,
            std::is_constructible<T, expected<T2, E2>>,
            std::is_constructible<T, const expected<T2, E2>&>,
            std::is_constructible<T, const expected<T2, E2>>,
            std::is_convertible<expected<T2, E2>&, T>,
            std::is_convertible<expected<T2, E2>, T>,
            std::is_convertible<const expected<T2, E2>&, T>,
            std::is_convertible<const expected<T2, E2>, T>,
            std::is_constructible<unexpected<E>, expected<T2, E2>&>,
            std::is_constructible<unexpected<E>, expected<T2, E2>>,
            std::is_constructible<unexpected<E>, const expected<T2, E2>&>,
            std::is_constructible<unexpected<E>, const expected<T2, E2>>>> { };

  template <typename G>
  using rebind_error = expected<T, G>;

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  and_then_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(gul::invoke(std::forward<F>(f)))>
  {
    using SelfE = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Exp = remove_cvref_t<invoke_result_t<F>>;
    static_assert(detail::is_specialization_of<Exp, gul::expected>::value, "");
    static_assert(std::is_same<typename Exp::error_type, SelfE>::value, "");
    if (self.has_value()) {
      return gul::invoke(std::forward<F>(f));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  and_then_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(gul::invoke(std::forward<F>(f),
                                             std::forward<Self>(self).value()))>
  {
    using SelfT = decltype(std::declval<Self>().value());
    using SelfE = decltype(std::declval<Self>().error());
    using Exp = remove_cvref_t<invoke_result_t<F, SelfT>>;
    static_assert(detail::is_specialization_of<Exp, gul::expected>::value, "");
    static_assert(
        std::is_same<typename Exp::error_type, remove_cvref_t<SelfE>>::value,
        "");
    if (self.has_value()) {
      return gul::invoke(std::forward<F>(f), std::forward<Self>(self).value());
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  or_else_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(gul::invoke(std::forward<F>(f),
                                             std::forward<Self>(self).error()))>
  {
    using SelfE = decltype(std::declval<Self>().error());
    using Exp = remove_cvref_t<invoke_result_t<F, SelfE>>;
    using U = typename Exp::value_type;
    static_assert(detail::is_specialization_of<Exp, gul::expected>::value, "");
    static_assert(std::is_same<typename Exp::value_type, U>::value, "");
    if (self.has_value()) {
      return Exp();
    } else {
      return gul::invoke(std::forward<F>(f), std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  or_else_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(gul::invoke(std::forward<F>(f),
                                             std::forward<Self>(self).error()))>
  {
    using SelfE = decltype(std::declval<Self>().error());
    using Exp = remove_cvref_t<invoke_result_t<F, SelfE>>;
    using U = typename Exp::value_type;
    static_assert(detail::is_specialization_of<Exp, gul::expected>::value, "");
    static_assert(std::is_same<typename Exp::value_type, U>::value, "");
    if (self.has_value()) {
      return Exp(in_place, std::forward<Self>(self).value());
    } else {
      return gul::invoke(std::forward<F>(f), std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_impl(std::true_type, Self&& self, F&& f) ->
      typename remove_cvref_t<Self>::template rebind<
          remove_cv_t<decltype(gul::invoke(std::forward<F>(f)))>>
  {
    using U = remove_cv_t<invoke_result_t<F>>;
    using Exp = typename remove_cvref_t<Self>::template rebind<U>;
    static_assert(!std::is_void<U>::value, "");
    if (self.has_value()) {
      return Exp(gul::invoke(std::forward<F>(f)));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_impl(std::false_type, Self&& self, F&& f) ->
      typename remove_cvref_t<Self>::template rebind<
          remove_cv_t<decltype(gul::invoke(std::forward<F>(f),
                                           std::forward<Self>(self).value()))>>
  {
    using SelfT = decltype(std::declval<Self>().value());
    using U = remove_cv_t<invoke_result_t<F, SelfT>>;
    using Exp = typename remove_cvref_t<Self>::template rebind<U>;
    static_assert(!std::is_void<U>::value, "");
    if (self.has_value()) {
      return Exp(
          gul::invoke(std::forward<F>(f), std::forward<Self>(self).value()));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_error_impl(std::true_type, Self&& self, F&& f) ->
      typename remove_cvref_t<Self>::template rebind_error<
          remove_cv_t<decltype(gul::invoke(std::forward<F>(f),
                                           std::forward<Self>(self).error()))>>
  {
    using SelfE = decltype(std::declval<Self>().error());
    using G = remove_cv_t<invoke_result_t<F, SelfE>>;
    using Exp = typename remove_cvref_t<Self>::template rebind_error<G>;
    if (self.has_value()) {
      return Exp(in_place);
    } else {
      return Exp(
          unexpect,
          gul::invoke(std::forward<F>(f), std::forward<Self>(self).error()));
    }
  }

  template <typename Self, typename F>
  static GUL_CXX14_CONSTEXPR auto
  transform_error_impl(std::false_type, Self&& self, F&& f) ->
      typename remove_cvref_t<Self>::template rebind_error<
          remove_cv_t<decltype(gul::invoke(std::forward<F>(f),
                                           std::forward<Self>(self).error()))>>
  {
    using SelfE = decltype(std::declval<Self>().error());
    using G = remove_cv_t<invoke_result_t<F, SelfE>>;
    using Exp = typename remove_cvref_t<Self>::template rebind_error<G>;
    if (self.has_value()) {
      return Exp(in_place, std::forward<Self>(self).value());
    } else {
      return Exp(
          unexpect,
          gul::invoke(std::forward<F>(f), std::forward<Self>(self).error()));
    }
  }

public:
  static_assert(!std::is_rvalue_reference<T>::value,
                "[expected] T must not be an rvalue reference type");
  static_assert(!std::is_function<T>::value,
                "[expected] T must not be an function type");
  static_assert(!std::is_same<remove_cvref_t<T>, in_place_t>::value,
                "[expected] T must not be in_place_t");
  static_assert(!std::is_same<remove_cvref_t<T>, unexpect_t>::value,
                "[expected] T must not be unexpect_t");
  static_assert(!detail::is_specialization_of<T, unexpected>::value,
                "[expected] T must not be a specialization of unexpected");

  static_assert(std::is_same<remove_cv_t<E>, E>::value,
                "[expected] E must not be cv-qualified");
  static_assert(std::is_object<E>::value,
                "[expected] E must be an object type");
  static_assert(!std::is_array<E>::value,
                "[expected] E must not be an array type");
  static_assert(!detail::is_specialization_of<E, gul::unexpected>::value,
                "[expected] E must not be a specialization of unexpected");

  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <typename U>
  using rebind = expected<U, E>;

  GUL_CXX14_CONSTEXPR expected() = default;

  GUL_CXX14_CONSTEXPR expected(const expected&) = default;

  GUL_CXX14_CONSTEXPR expected(expected&&) = default;

  template <
      typename T2,
      typename E2,
      GUL_REQUIRES(conjunction<
                   disjunction<conjunction<std::is_void<T>, std::is_void<T2>>,
                               std::is_constructible<T, const T2&>>,
                   std::is_constructible<E, const E2&>,
                   is_expected_constructible<T2, E2>>::value)>
  GUL_CXX14_CONSTEXPR
      GUL_CXX20_EXPLICIT((!std::is_convertible<const T2&, T>::value
                          || !std::is_convertible<const E2&, E>::value))
          expected(const expected<T2, E2>& other)
      : dc_base_type(in_place)
  {
    (*this).construct_from(other);
  }

  template <
      typename T2,
      typename E2,
      GUL_REQUIRES(conjunction<
                   disjunction<conjunction<std::is_void<T>, std::is_void<T2>>,
                               std::is_constructible<T, T2>>,
                   std::is_constructible<E, E2>,
                   is_expected_constructible<T2, E2>>::value)>
  GUL_CXX14_CONSTEXPR
      GUL_CXX20_EXPLICIT((!std::is_convertible<T2, T>::value
                          || !std::is_convertible<E2, E>::value))
          expected(expected<T2, E2>&& other)
      : dc_base_type(in_place)
  {
    (*this).construct_from(std::move(other));
  }

  template <
      typename U = T,
      GUL_REQUIRES(
          conjunction<negation<std::is_void<T>>,
                      negation<std::is_reference<T>>,
                      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
                      negation<std::is_same<remove_cvref_t<U>, unexpect_t>>,
                      negation<std::is_same<remove_cvref_t<U>, expected>>,
                      negation<detail::is_specialization_of<remove_cvref_t<U>,
                                                            unexpected>>,
                      std::is_constructible<T, U>>::value)>
  GUL_CXX14_CONSTEXPR GUL_CXX20_EXPLICIT((!std::is_convertible<U, T>::value))
      expected(U&& u)
      : base_type(in_place, std::forward<U>(u))
      , dc_base_type(in_place)
  {
  }

  // perfect forwarding constructor for T being a reference type
  // when T is a reference type, the constructor must be explicit to make sure
  // user is willing to construct it from an object. or a dangling reference
  // might be created.
  template <
      typename U = T,
      GUL_REQUIRES(
          conjunction<std::is_reference<T>,
                      negation<std::is_same<remove_cvref_t<U>, in_place_t>>,
                      negation<std::is_same<remove_cvref_t<U>, unexpect_t>>,
                      negation<std::is_same<remove_cvref_t<U>, expected>>,
                      negation<detail::is_specialization_of<remove_cvref_t<U>,
                                                            unexpected>>,
                      std::is_constructible<T, U>>::value)>
  GUL_CXX14_CONSTEXPR explicit expected(U&& u)
      : base_type(in_place, std::forward<U>(u))
      , dc_base_type(in_place)
  {
  }

  template <typename E2,
            GUL_REQUIRES(std::is_constructible<E, const E2&>::value)>
  GUL_CXX14_CONSTEXPR
      GUL_CXX20_EXPLICIT((!std::is_convertible<const E2&, E>::value))
          expected(const unexpected<E2>& une)
      : base_type(unexpect, une.error())
      , dc_base_type(in_place)
  {
  }

  template <typename E2, GUL_REQUIRES(std::is_constructible<E, E2>::value)>
  GUL_CXX14_CONSTEXPR GUL_CXX20_EXPLICIT((!std::is_convertible<E2, E>::value))
      expected(unexpected<E2>&& une)
      : base_type(unexpect, std::move(une).error())
      , dc_base_type(in_place)
  {
  }
  template <bool B = disjunction<std::is_void<T>,
                                 std::is_default_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR explicit expected(in_place_t) noexcept(
      std::is_void<T>::value)
      : base_type(in_place)
      , dc_base_type(in_place)
  {
  }

  template <typename... Args
#if defined(_MSC_VER) && !defined(__clang__) && !defined(GUL_HAS_CXX17)
// workaround: MSVC /std:c++14
#else
            ,
            GUL_REQUIRES(std::is_constructible<T, Args...>::value)
#endif
            >
  GUL_CXX14_CONSTEXPR explicit expected(in_place_t, Args&&... args) noexcept(
      std::is_void<T>::value)
      : base_type(in_place, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  template <
      typename U,
      typename... Args,
      GUL_REQUIRES(conjunction<negation<std::is_void<T>>,
                               std::is_constructible<T,
                                                     std::initializer_list<U>&,
                                                     Args...>>::value)>
  GUL_CXX14_CONSTEXPR explicit expected(in_place_t,
                                        std::initializer_list<U> init,
                                        Args&&... args)
      : base_type(in_place, init, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  template <bool B = std::is_default_constructible<E>::value, GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR explicit expected(unexpect_t)
      : base_type(unexpect)
      , dc_base_type(in_place)
  {
  }

  template <typename... Args
#if defined(_MSC_VER) && !defined(__clang__) && !defined(GUL_HAS_CXX17)
// workaround: MSVC /std:c++14
#else
            ,
            GUL_REQUIRES(std::is_constructible<E, Args...>::value)
#endif
            >
  GUL_CXX14_CONSTEXPR explicit expected(unexpect_t, Args&&... args)
      : base_type(unexpect, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  template <
      typename U,
      typename... Args,
      GUL_REQUIRES(
          std::is_constructible<E, std::initializer_list<U>&, Args...>::value)>
  GUL_CXX14_CONSTEXPR explicit expected(unexpect_t,
                                        std::initializer_list<U> init,
                                        Args&&... args)
      : base_type(unexpect, init, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  GUL_CXX14_CONSTEXPR expected& operator=(const expected&) = default;

  GUL_CXX14_CONSTEXPR expected& operator=(expected&&) = default;

  template <
      typename U = T,
      GUL_REQUIRES(
          conjunction<negation<std::is_void<T>>,
                      negation<std::is_reference<T>>,
                      negation<std::is_same<remove_cvref_t<U>, expected>>,
                      negation<detail::is_specialization_of<remove_cvref_t<U>,
                                                            unexpected>>,
                      std::is_constructible<T, U>,
                      std::is_assignable<add_lvalue_reference_t<T>, U>>::value)>
  GUL_CXX14_CONSTEXPR expected& operator=(U&& u)
  {
    if (has_value()) {
      value() = std::forward<U>(u);
    } else {
      (*this).destroy_err();
      (*this).construct_val(std::forward<U>(u));
      (*this).has_ = true;
    }

    return *this;
  }

  template <
      typename E2,
      GUL_REQUIRES(conjunction<std::is_constructible<E, const E2&>,
                               std::is_assignable<add_lvalue_reference_t<E>,
                                                  const E2&>>::value)>
  GUL_CXX14_CONSTEXPR expected& operator=(const unexpected<E2>& une)
  {
    if (has_value()) {
      (*this).destroy_val();
      (*this).has_ = false;
      (*this).construct_err(une.error());
    } else {
      (*this).get_err() = une.error();
    }

    return *this;
  }

  template <
      typename E2,
      GUL_REQUIRES(conjunction<
                   std::is_constructible<E, E2>,
                   std::is_assignable<add_lvalue_reference_t<E>, E2>>::value)>
  GUL_CXX14_CONSTEXPR expected& operator=(unexpected<E2>&& une)
  {
    if (has_value()) {
      (*this).destroy_val();
      (*this).has_ = false;
      (*this).construct_err(std::move(une).error());
    } else {
      (*this).get_err() = std::move(une).error();
    }

    return *this;
  }

  using base_type::operator->;

  using base_type::operator*;

  GUL_CXX14_CONSTEXPR explicit operator bool() const noexcept
  {
    return has_value();
  }

  using base_type::has_value;

  using base_type::value;

  using base_type::get_err;

  GUL_CXX14_CONSTEXPR E& error() & noexcept
  {
    GUL_ASSERT(!has_value());
    return get_err();
  }

  GUL_CXX14_CONSTEXPR const E& error() const& noexcept
  {
    GUL_ASSERT(!has_value());
    return get_err();
  }

  GUL_CXX14_CONSTEXPR E&& error() && noexcept
  {
    GUL_ASSERT(!has_value());
    return std::move(*this).get_err();
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR const E&& error() const&& noexcept
  {
    GUL_ASSERT(!has_value());
    return std::move(*this).get_err();
  }
#endif

  template <typename U = remove_reference_t<T>>
  GUL_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) const&
  {
    using raw_type = remove_reference_t<T>;
    static_assert(std::is_copy_constructible<raw_type>::value,
                  "[expected::value_or] T must be copy constructible");
    static_assert(std::is_convertible<U, raw_type>::value,
                  "[expected::value_or] U must be convertible to T");
    if (has_value()) {
      return value();
    } else {
      return static_cast<raw_type>(std::forward<U>(default_value));
    }
  }

  template <typename U = remove_reference_t<T>>
  GUL_CXX14_CONSTEXPR remove_reference_t<T> value_or(U&& default_value) &&
  {
    using raw_type = remove_reference_t<T>;
    static_assert(std::is_move_constructible<raw_type>::value,
                  "[expected::value_or] T must be move constructible");
    static_assert(std::is_convertible<U, raw_type>::value,
                  "[expected::value_or] U must be convertible to T");
    if (has_value()) {
      return std::move(value());
    } else {
      return static_cast<raw_type>(std::forward<U>(default_value));
    }
  }

  template <typename U = E>
  GUL_CXX14_CONSTEXPR E error_or(U&& default_error) const&
  {
    static_assert(std::is_copy_constructible<E>::value,
                  "[expected::error_or] E must be copy constructible");
    static_assert(std::is_convertible<U, E>::value,
                  "[expected::error_or] U must be convertible to E");
    if (has_value()) {
      return static_cast<E>(std::forward<U>(default_error));
    } else {
      return error();
    }
  }

  template <typename U = E>
  GUL_CXX14_CONSTEXPR E error_or(U&& default_error) &&
  {
    static_assert(std::is_move_constructible<E>::value,
                  "[expected::error_or] E must be move constructible");
    static_assert(std::is_convertible<U, E>::value,
                  "[expected::error_or] U must be convertible to E");
    if (has_value()) {
      return static_cast<E>(std::forward<U>(default_error));
    } else {
      return std::move(error());
    }
  }

  template <typename... Args,
            GUL_REQUIRES(disjunction<std::is_void<T>,
                                     std::is_constructible<T, Args...>>::value)>
  GUL_CXX14_CONSTEXPR auto
  emplace(Args&&... args) noexcept(std::is_void<T>::value)
      -> decltype((*this).construct_val(std::forward<Args>(args)...))
  {
    if (has_value()) {
      (*this).destroy_val();
    } else {
      (*this).destroy_err();
    }

    (*this).has_ = true;
    return (*this).construct_val(std::forward<Args>(args)...);
  }

  template <
      typename U,
      typename... Args,
      GUL_REQUIRES(
          std::is_constructible<T, std::initializer_list<U>&, Args...>::value)>
  GUL_CXX14_CONSTEXPR auto emplace(std::initializer_list<U> init,
                                   Args&&... args)
      -> decltype((*this).construct_val(init, std::forward<Args>(args)...))
  {
    if (has_value()) {
      (*this).destroy_val();
    } else {
      (*this).destroy_err();
    }

    (*this).has_ = true;
    return (*this).construct_val(init, std::forward<Args>(args)...);
  }

  /// expected<T, E>::and_then(F) -> expected<T, E>
  /// F() -> expected<T, E>
  /// where T is void
  ///
  /// expected<T, E>::and_then(F) -> expected<U, E>
  /// F(T) -> expected<U, E>
  /// where T is not void
  template <typename F, GUL_REQUIRES(std::is_copy_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) & -> decltype(and_then_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, GUL_REQUIRES(std::is_copy_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) const& -> decltype(and_then_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, GUL_REQUIRES(std::is_move_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) && -> decltype(and_then_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, std::move(*this),
                         std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F, GUL_REQUIRES(std::is_move_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto and_then(F&& f) const&& -> decltype(and_then_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(std::is_void<T> {}, std::move(*this),
                         std::forward<F>(f));
  }
#endif

  /// expected<T, E>::or_else(F) -> expected<T, G>
  /// F(E) -> expected<T, G>
  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_copy_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) & -> decltype(or_else_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_copy_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) const& -> decltype(or_else_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_move_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) && -> decltype(or_else_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, std::move(*this),
                        std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_move_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto or_else(F&& f) const&& -> decltype(or_else_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return or_else_impl(std::is_void<T> {}, std::move(*this),
                        std::forward<F>(f));
  }
#endif

  /// expected<T, E>::transform(F) -> expected<U, E>
  /// F() -> U
  /// where T is void
  ///
  /// expected<T, E>::transform(F) -> expected<U, E>
  /// F(T) -> U
  /// where T is not void
  template <typename F, GUL_REQUIRES(std::is_copy_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) & -> decltype(transform_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, GUL_REQUIRES(std::is_copy_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) const& -> decltype(transform_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, GUL_REQUIRES(std::is_move_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) && -> decltype(transform_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, std::move(*this),
                          std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F, GUL_REQUIRES(std::is_move_constructible<E>::value)>
  GUL_CXX14_CONSTEXPR auto transform(F&& f) const&& -> decltype(transform_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(std::is_void<T> {}, std::move(*this),
                          std::forward<F>(f));
  }
#endif

  /// expected<T, E>::transform_error(F) -> expected<T, G>
  /// F(E) -> G
  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_copy_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto
  transform_error(F&& f) & -> decltype(transform_error_impl(std::is_void<T> {},
                                                            *this,
                                                            std::forward<F>(f)))
  {
    return transform_error_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_copy_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto
  transform_error(F&& f) const& -> decltype(transform_error_impl(
      std::is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_error_impl(std::is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_move_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto
  transform_error(F&& f) && -> decltype(transform_error_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_error_impl(std::is_void<T> {}, std::move(*this),
                                std::forward<F>(f));
  }

#if !defined(GUL_CXX_COMPILER_GCC48)
  template <typename F,
            bool B = disjunction<std::is_void<T>,
                                 std::is_move_constructible<T>>::value,
            GUL_REQUIRES(B)>
  GUL_CXX14_CONSTEXPR auto
  transform_error(F&& f) const&& -> decltype(transform_error_impl(
      std::is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_error_impl(std::is_void<T> {}, std::move(*this),
                                std::forward<F>(f));
  }
#endif

  /// expected<T, E>::value_to_optional() -> optional<T>
  /// !!!experimental non-std extension!!!
  GUL_CXX14_CONSTEXPR auto value_to_optional() & -> optional<T>;

  GUL_CXX14_CONSTEXPR auto value_to_optional() const& -> optional<T>;

  GUL_CXX14_CONSTEXPR auto value_to_optional() && -> optional<T>;

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR auto value_to_optional() const&& -> optional<T>;
#endif

  /// expected<T, E>::error_to_optional() -> optional<E>
  /// !!!experimental non-std extension!!!
  GUL_CXX14_CONSTEXPR auto error_to_optional() & -> optional<E>;

  GUL_CXX14_CONSTEXPR auto error_to_optional() const& -> optional<E>;

  GUL_CXX14_CONSTEXPR auto error_to_optional() && -> optional<E>;

#if !defined(GUL_CXX_COMPILER_GCC48)
  GUL_CXX14_CONSTEXPR auto error_to_optional() const&& -> optional<E>;
#endif

  using base_type::swap;

  friend GUL_CXX14_CONSTEXPR void
  swap(expected& lhs, expected& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }
};

template <
    typename T1,
    typename E1,
    typename T2,
    typename E2,
    GUL_REQUIRES(conjunction<std::is_void<T1>,
                             std::is_void<T2>,
                             detail::is_eq_comparable_with<E1, E2>>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const expected<T1, E1>& lhs,
                                    const expected<T2, E2>& rhs)
{
  if (lhs.has_value() != rhs.has_value()) {
    return false;
  }
  if (lhs.has_value()) {
    return true;
  }

  return lhs.error() == rhs.error();
}

template <typename E1,
          typename E2,
          GUL_REQUIRES(detail::is_eq_comparable_with<E1, E2>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const expected<void, E1>& lhs,
                                    const expected<void, E2>& rhs)
{
  return !(lhs == rhs);
}

template <
    typename T1,
    typename E1,
    typename T2,
    typename E2,
    GUL_REQUIRES(conjunction<detail::is_eq_comparable_with<T1, T2>,
                             detail::is_eq_comparable_with<E1, E2>>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const expected<T1, E1>& lhs,
                                    const expected<T2, E2>& rhs)
{
  if (lhs.has_value() != rhs.has_value()) {
    return false;
  }
  if (lhs.has_value()) {
    return lhs.value() == rhs.value();
  }

  return lhs.error() == rhs.error();
}

template <
    typename T1,
    typename E1,
    typename T2,
    typename E2,
    GUL_REQUIRES(conjunction<detail::is_eq_comparable_with<T1, T2>,
                             detail::is_eq_comparable_with<E1, E2>>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const expected<T1, E1>& lhs,
                                    const expected<T2, E2>& rhs)
{
  return !(lhs == rhs);
}

template <typename T1,
          typename E1,
          typename T2,
          GUL_REQUIRES(detail::is_eq_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const expected<T1, E1>& lhs, const T2& rhs)
{
  return lhs.has_value() && static_cast<bool>(lhs.value() == rhs);
}

template <typename T1,
          typename E1,
          typename T2,
          GUL_REQUIRES(detail::is_eq_comparable_with<T1, T2>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const expected<T1, E1>& lhs, const T2& rhs)
{
  return !(lhs == rhs);
}

template <typename T1,
          typename E1,
          typename E2,
          GUL_REQUIRES(detail::is_eq_comparable_with<E1, E2>::value)>
GUL_CXX14_CONSTEXPR bool operator==(const expected<T1, E1>& lhs,
                                    const unexpected<E2>& rhs)
{
  return !lhs.has_value() && static_cast<bool>(lhs.error() == rhs.error());
}

template <typename T1,
          typename E1,
          typename E2,
          GUL_REQUIRES(detail::is_eq_comparable_with<E1, E2>::value)>
GUL_CXX14_CONSTEXPR bool operator!=(const expected<T1, E1>& lhs,
                                    const unexpected<E2>& rhs)
{
  return !(lhs == rhs);
}

GUL_NAMESPACE_END

#include <gul/optional.hpp>

#include <gul/impl/expected.ipp>
