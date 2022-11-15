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
  DS_CXX14_CONSTEXPR explicit unexpected(E2&& err)
      : err_(std::forward<E2>(err))
  {
  }

  template <typename... Args,
            enable_if_t<is_constructible<E, Args...>::value, int> = 0>
  DS_CXX14_CONSTEXPR explicit unexpected(in_place_t, Args&&... args)
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
                                         Args&&... args)
      : err_(init, std::forward<Args>(args)...)
  {
  }

  DS_CXX14_CONSTEXPR unexpected(const unexpected&) = default;

  DS_CXX14_CONSTEXPR unexpected(unexpected&&) = default;

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

  DS_CXX14_CONSTEXPR void
  swap(unexpected& other) noexcept(is_nothrow_swappable<E>::value)
  {
    using std::swap;
    swap(err_, other.err_);
  }

  friend DS_CXX14_CONSTEXPR void
  swap(unexpected& lhs, unexpected& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }

  template <typename E2>
  friend DS_CXX14_CONSTEXPR bool operator==(const unexpected& lhs,
                                            const unexpected<E2>& rhs)
  {
    return lhs.err_ == rhs.err_;
  }
};

#if !DS_NO_EXCEPTIONS

template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
public:
  const char* what() const noexcept override
  {
    return "bad expected access";
  }

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

  const E& error() const& noexcept
  {
    return err_;
  }

  E& error() & noexcept
  {
    return err_;
  }

  const E&& error() const&& noexcept
  {
    return std::move(err_);
  }

  E&& error() && noexcept
  {
    return std::move(err_);
  }

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
#if DS_NO_EXCEPTIONS
      std::abort();
#else
      throw bad_expected_access<E>(err);
#endif
    }
  };

  template <typename T,
            typename E,
            bool = disjunction<is_void<T>, is_trivially_destructible<T>>::value,
            bool = is_trivially_destructible<E>::value>
  struct expected_destruct_base : expected_throw_base<E> {
    union {
      char nul_;
      conditional_t<is_void<T>::value, char, T> val_;
      E err_;
    };
    bool has_;

    DS_CXX14_CONSTEXPR expected_destruct_base() noexcept
        : nul_()
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                       Args&&... args)
        : val_(std::forward<Args>(args)...)
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
                                                       Args&&... args)
        : err_(std::forward<Args>(args)...)
        , has_(false)
    {
    }
  };

  template <typename T, typename E>
  struct expected_destruct_base<T, E, true, false> : expected_throw_base<E> {
    union {
      char nul_;
      conditional_t<is_void<T>::value, char, T> val_;
      E err_;
    };
    bool has_;

    DS_CXX14_CONSTEXPR expected_destruct_base() noexcept
        : nul_()
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                       Args&&... args)
        : val_(std::forward<Args>(args)...)
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
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
  struct expected_destruct_base<T, E, false, true> : expected_throw_base<E> {
    union {
      char nul_;
      T val_;
      E err_;
    };
    bool has_;

    DS_CXX14_CONSTEXPR expected_destruct_base() noexcept
        : nul_()
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                       Args&&... args)
        : val_(std::forward<Args>(args)...)
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
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
  struct expected_destruct_base<T, E, false, false> : expected_throw_base<E> {
    union {
      char nul_;
      T val_;
      E err_;
    };
    bool has_;

    DS_CXX14_CONSTEXPR expected_destruct_base() noexcept
        : nul_()
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(in_place_t,
                                                       Args&&... args)
        : val_(std::forward<Args>(args)...)
        , has_(true)
    {
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR explicit expected_destruct_base(unexpect_t,
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

  template <typename T, typename E>
  struct expected_storage_base : expected_destruct_base<T, E> {
    using expected_destruct_base<T, E>::expected_destruct_base;

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

    DS_CXX14_CONSTEXPR T& value() &
    {
      if (!this->has_) {
        this->throw_bad_expected_access(get_err());
      }
      return this->val_;
    }

    DS_CXX14_CONSTEXPR const T& value() const&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(get_err());
      }
      return this->val_;
    }

    DS_CXX14_CONSTEXPR T&& value() &&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(std::move(get_err()));
      }
      return std::move(this->val_);
    }

    DS_CXX14_CONSTEXPR const T&& value() const&&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(std::move(get_err()));
      }
      return std::move(this->val_);
    }

    DS_CXX14_CONSTEXPR E& get_err() & noexcept
    {
      return this->err_;
    }

    DS_CXX14_CONSTEXPR const E& get_err() const& noexcept
    {
      return this->err_;
    }

    DS_CXX14_CONSTEXPR E&& get_err() && noexcept
    {
      return std::move(this->err_);
    }

    DS_CXX14_CONSTEXPR const E&& get_err() const&& noexcept
    {
      return std::move(this->err_);
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR T& construct_val(Args&&... args)
    {
      auto& val = *(::new (std::addressof(this->val_))
                        T(std::forward<Args>(args)...));
      this->has_ = true;
      return val;
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR E& construct_err(Args&&... args)
    {
      auto& err = *(::new (std::addressof(this->err_))
                        E(std::forward<Args>(args)...));
      this->has_ = false;
      return err;
    }

    DS_CXX14_CONSTEXPR void destroy_val()
    {
      this->val_.~T();
    }

    DS_CXX14_CONSTEXPR void destroy_err()
    {
      this->err_.~E();
    }

    template <typename Other>
    DS_CXX14_CONSTEXPR void construct_from(Other&& other)
    {
      if (other.has_value()) {
        construct_val(std::forward<Other>(other).value());
      } else {
        construct_err(std::forward<Other>(other).get_err());
      }
    }

    template <typename Other>
    DS_CXX14_CONSTEXPR void assign_from(Other&& other)
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
                  is_swappable<T>,
                  is_swappable<E>,
                  is_move_constructible<T>,
                  is_move_constructible<E>,
                  disjunction<is_nothrow_move_constructible<T>,
                              is_nothrow_move_constructible<E>>>::value,
              enable_if_t<B, int> = 0>
    DS_CXX14_CONSTEXPR void swap(expected_storage_base& other) noexcept(
        conjunction<is_nothrow_move_constructible<T>,
                    is_nothrow_swappable<T>,
                    is_nothrow_move_constructible<E>,
                    is_nothrow_swappable<E>>::value)
    {
      if (this->has_value()) {
        if (other.has_value()) {
          using std::swap;
          swap(this->value(), other.value());
        } else {
          swap_impl(other, is_nothrow_move_constructible<T> {},
                    is_nothrow_move_constructible<E> {});
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
    DS_CXX14_CONSTEXPR void swap_impl(expected_storage_base& other,
                                      std::true_type,
                                      std::true_type) noexcept
    {
      auto val = std::move(this->value());
      this->destroy_val();
      this->construct_err(std::move(other.get_err()));
      other.destroy_err();
      other.construct_val(std::move(val));
    }

    DS_CXX20_CONSTEXPR void
    swap_impl(expected_storage_base& other, std::true_type, std::false_type)
    {
      auto val(std::move(this->value()));
      this->destroy_val();
      try {
        this->construct_err(std::move(other.get_err()));
        other.destroy_err();
        other.construct_val(std::move(val));
      } catch (...) {
        this->construct_val(std::move(val));
        throw;
      }
    }

    DS_CXX20_CONSTEXPR void
    swap_impl(expected_storage_base& other, std::false_type, std::true_type)
    {
      auto err(std::move(other.get_err()));
      other.destroy_err();
      try {
        other.construct_val(std::move(this->value()));
        this->destroy_val();
        this->construct_err(std::move(err));
      } catch (...) {
        other.construct_err(std::move(err));
        throw;
      }
    }
  };

  template <typename E>
  struct expected_storage_base<void, E> : expected_destruct_base<void, E> {
    using expected_destruct_base<void, E>::expected_destruct_base;

    DS_CXX14_CONSTEXPR void operator*() const& noexcept { }

    DS_CXX14_CONSTEXPR void operator*() && noexcept { }

    DS_CXX14_CONSTEXPR bool has_value() const noexcept
    {
      return this->has_;
    }

    DS_CXX14_CONSTEXPR void value() &
    {
      if (!this->has_) {
        this->throw_bad_expected_access(get_err());
      }
    }

    DS_CXX14_CONSTEXPR void value() const&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(get_err());
      }
    }

    DS_CXX14_CONSTEXPR void value() &&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(std::move(get_err()));
      }
    }

    DS_CXX14_CONSTEXPR void value() const&&
    {
      if (!this->has_) {
        this->throw_bad_expected_access(std::move(get_err()));
      }
    }

    DS_CXX14_CONSTEXPR E& get_err() & noexcept
    {
      return this->err_;
    }

    DS_CXX14_CONSTEXPR const E& get_err() const& noexcept
    {
      return this->err_;
    }

    DS_CXX14_CONSTEXPR E&& get_err() && noexcept
    {
      return std::move(this->err_);
    }

    DS_CXX14_CONSTEXPR const E&& get_err() const&& noexcept
    {
      return std::move(this->err_);
    }

    DS_CXX14_CONSTEXPR void construct_val() noexcept
    {
      this->has_ = true;
    }

    template <typename... Args>
    DS_CXX14_CONSTEXPR E& construct_err(Args&&... args)
    {
      auto& err = *(::new (std::addressof(this->err_))
                        E(std::forward<Args>(args)...));
      this->has_ = false;
      return err;
    }

    DS_CXX14_CONSTEXPR void destroy_val() noexcept { }

    DS_CXX14_CONSTEXPR void destroy_err()
    {
      this->err_.~E();
    }

    template <typename Other>
    DS_CXX14_CONSTEXPR void construct_from(Other&& other)
    {
      if (other.has_value()) {
        construct_val();
      } else {
        construct_err(std::forward<Other>(other).get_err());
      }
    }

    template <typename Other>
    DS_CXX14_CONSTEXPR void assign_from(Other&& other)
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

    DS_CXX14_CONSTEXPR void swap(expected_storage_base& other) noexcept(
        conjunction<is_nothrow_move_constructible<E>,
                    is_nothrow_swappable<E>>::value)
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

  template <
      typename T,
      typename E,
      bool = disjunction<is_void<T>, is_trivially_copy_constructible<T>>::value,
      bool = is_trivially_copy_constructible<E>::value>
  struct expected_copy_construct_base : expected_storage_base<T, E> {
    using expected_storage_base<T, E>::expected_storage_base;

    DS_CXX14_CONSTEXPR expected_copy_construct_base() = default;

    DS_CXX14_CONSTEXPR
    expected_copy_construct_base(const expected_copy_construct_base& other)
    {
      this->construct_from(other);
    }

    DS_CXX14_CONSTEXPR
    expected_copy_construct_base(expected_copy_construct_base&&) = default;

    DS_CXX14_CONSTEXPR expected_copy_construct_base&
    operator=(const expected_copy_construct_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_copy_construct_base&
    operator=(expected_copy_construct_base&&)
        = default;
  };

  template <typename T, typename E>
  struct expected_copy_construct_base<T, E, true, true>
      : expected_storage_base<T, E> {
    using expected_storage_base<T, E>::expected_storage_base;

    DS_CXX14_CONSTEXPR expected_copy_construct_base() = default;

    DS_CXX14_CONSTEXPR
    expected_copy_construct_base(const expected_copy_construct_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_copy_construct_base(expected_copy_construct_base&&) = default;

    DS_CXX14_CONSTEXPR expected_copy_construct_base&
    operator=(const expected_copy_construct_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_copy_construct_base&
    operator=(expected_copy_construct_base&&)
        = default;
  };

  template <
      typename T,
      typename E,
      bool = disjunction<is_void<T>, is_trivially_move_constructible<T>>::value,
      bool = is_trivially_move_constructible<E>::value>
  struct expected_move_construct_base : expected_copy_construct_base<T, E> {
    using expected_copy_construct_base<T, E>::expected_copy_construct_base;

    DS_CXX14_CONSTEXPR expected_move_construct_base() = default;

    DS_CXX14_CONSTEXPR
    expected_move_construct_base(const expected_move_construct_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_move_construct_base(expected_move_construct_base&& other)
    {
      this->construct_from(std::move(other));
    }

    DS_CXX14_CONSTEXPR expected_move_construct_base&
    operator=(const expected_move_construct_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_move_construct_base&
    operator=(expected_move_construct_base&&)
        = default;
  };

  template <typename T, typename E>
  struct expected_move_construct_base<T, E, true, true>
      : expected_copy_construct_base<T, E> {
    using expected_copy_construct_base<T, E>::expected_copy_construct_base;

    DS_CXX14_CONSTEXPR expected_move_construct_base() = default;

    DS_CXX14_CONSTEXPR
    expected_move_construct_base(const expected_move_construct_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_move_construct_base(expected_move_construct_base&&) = default;

    DS_CXX14_CONSTEXPR expected_move_construct_base&
    operator=(const expected_move_construct_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_move_construct_base&
    operator=(expected_move_construct_base&&)
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
  struct expected_copy_assign_base : expected_move_construct_base<T, E> {
    using expected_move_construct_base<T, E>::expected_move_construct_base;

    DS_CXX14_CONSTEXPR expected_copy_assign_base() = default;

    DS_CXX14_CONSTEXPR
    expected_copy_assign_base(const expected_copy_assign_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_copy_assign_base(expected_copy_assign_base&&) = default;

    DS_CXX14_CONSTEXPR expected_copy_assign_base&
    operator=(const expected_copy_assign_base& other)
    {
      this->assign_from(other);
    }

    DS_CXX14_CONSTEXPR expected_copy_assign_base&
    operator=(expected_copy_assign_base&&)
        = default;
  };

  template <typename T, typename E>
  struct expected_copy_assign_base<T, E, true, true>
      : expected_move_construct_base<T, E> {
    using expected_move_construct_base<T, E>::expected_move_construct_base;

    DS_CXX14_CONSTEXPR expected_copy_assign_base() = default;

    DS_CXX14_CONSTEXPR
    expected_copy_assign_base(const expected_copy_assign_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_copy_assign_base(expected_copy_assign_base&&) = default;

    DS_CXX14_CONSTEXPR expected_copy_assign_base&
    operator=(const expected_copy_assign_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_copy_assign_base&
    operator=(expected_copy_assign_base&&)
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
  struct expected_move_assign_base : expected_copy_assign_base<T, E> {
    using expected_copy_assign_base<T, E>::expected_copy_assign_base;

    DS_CXX14_CONSTEXPR expected_move_assign_base() = default;

    DS_CXX14_CONSTEXPR
    expected_move_assign_base(const expected_move_assign_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_move_assign_base(expected_move_assign_base&&) = default;

    DS_CXX14_CONSTEXPR expected_move_assign_base&
    operator=(const expected_move_assign_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_move_assign_base&
    operator=(expected_move_assign_base&& other)
    {
      this->assign_from(std::move(other));
    }
  };

  template <typename T, typename E>
  struct expected_move_assign_base<T, E, true, true>
      : expected_copy_assign_base<T, E> {
    using expected_copy_assign_base<T, E>::expected_copy_assign_base;

    DS_CXX14_CONSTEXPR expected_move_assign_base() = default;

    DS_CXX14_CONSTEXPR
    expected_move_assign_base(const expected_move_assign_base&) = default;

    DS_CXX14_CONSTEXPR
    expected_move_assign_base(expected_move_assign_base&&) = default;

    DS_CXX14_CONSTEXPR expected_move_assign_base&
    operator=(const expected_move_assign_base&)
        = default;

    DS_CXX14_CONSTEXPR expected_move_assign_base&
    operator=(expected_move_assign_base&&)
        = default;
  };

  template <typename T,
            bool = disjunction<is_void<T>, is_default_constructible<T>>::value>
  struct expected_default_constructible_base {
    DS_CXX14_CONSTEXPR
    expected_default_constructible_base(in_place_t) noexcept { }

    DS_CXX14_CONSTEXPR expected_default_constructible_base() noexcept = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base(
        const expected_default_constructible_base&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base(
        expected_default_constructible_base&&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base&
    operator=(const expected_default_constructible_base&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base&
    operator=(expected_default_constructible_base&&) noexcept
        = default;
  };

  template <typename T>
  struct expected_default_constructible_base<T, false> {
    DS_CXX14_CONSTEXPR
    expected_default_constructible_base(in_place_t) noexcept { }

    DS_CXX14_CONSTEXPR expected_default_constructible_base() noexcept = delete;

    DS_CXX14_CONSTEXPR expected_default_constructible_base(
        const expected_default_constructible_base&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base(
        expected_default_constructible_base&&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base&
    operator=(const expected_default_constructible_base&) noexcept
        = default;

    DS_CXX14_CONSTEXPR expected_default_constructible_base&
    operator=(expected_default_constructible_base&&) noexcept
        = default;
  };
}

template <typename T, typename E>
class expected : private detail::expected_move_assign_base<T, E>,
                 private detail::expected_default_constructible_base<T> {
  using base_type = detail::expected_move_assign_base<T, E>;
  using dc_base_type = detail::expected_default_constructible_base<T>;
  using detail::expected_move_assign_base<T, E>::expected_move_assign_base;

  template <typename T2, typename E2>
  struct is_expected_constructible
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

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  and_then_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
  {
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Exp = remove_cvref_t<invoke_result_t<F>>;
    static_assert(is_specialization_of<Exp, ds::expected>::value, "");
    static_assert(is_same<typename Exp::error_type, Err>::value, "");
    if (self.has_value()) {
      return Exp(invoke(std::forward<F>(f)));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  and_then_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f),
                                        std::forward<Self>(self).value()))>
  {
    using Val = decltype(std::declval<Self>().value());
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Exp = remove_cvref_t<invoke_result_t<F, Val>>;
    static_assert(is_specialization_of<Exp, ds::expected>::value, "");
    static_assert(is_same<typename Exp::error_type, Err>::value, "");
    if (self.has_value()) {
      return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  or_else_impl(std::true_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f)))>
  {
    using Exp = remove_cvref_t<invoke_result_t<F>>;
    using Val = typename Exp::value_type;
    static_assert(is_specialization_of<Exp, ds::expected>::value, "");
    static_assert(is_same<typename Exp::value_type, Val>::value, "");
    if (self.has_value()) {
      return Exp();
    } else {
      return Exp(invoke(std::forward<F>(f)));
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  or_else_impl(std::false_type, Self&& self, F&& f)
      -> remove_cvref_t<decltype(invoke(std::forward<F>(f),
                                        std::forward<Self>(self).error()))>
  {
    using Err = decltype(std::declval<Self>().error());
    using Exp = remove_cvref_t<invoke_result_t<F, Err>>;
    using Val = typename Exp::value_type;
    static_assert(is_specialization_of<Exp, ds::expected>::value, "");
    static_assert(is_same<typename Exp::value_type, Val>::value, "");
    if (self.has_value()) {
      return Exp(in_place, std::forward<Self>(self).value());
    } else {
      return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).error()));
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  transform_impl(std::true_type, Self&& self, F&& f)
      -> expected<remove_cvref_t<decltype(invoke(std::forward<F>(f)))>,
                  remove_cvref_t<decltype(std::forward<Self>(self).error())>>
  {
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using T2 = remove_cvref_t<invoke_result_t<F>>;
    using Exp = expected<T2, Err>;
    static_assert(
        disjunction<
            is_void<Exp>,
            conjunction<negation<is_same<Exp, in_place_t>>,
                        negation<is_same<Exp, unexpect_t>>,
                        negation<is_specialization_of<Exp, unexpected>>>>::
            value,
        "");
    if (self.has_value()) {
      return Exp(invoke(std::forward<F>(f)));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  transform_impl(std::false_type, Self&& self, F&& f)
      -> expected<remove_cvref_t<decltype(invoke(
                      std::forward<F>(f), std::forward<Self>(self).value()))>,
                  remove_cvref_t<decltype(std::forward<Self>(self).error())>>
  {
    using Val = decltype(std::declval<Self>().value());
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Val2 = remove_cvref_t<invoke_result_t<F, Val>>;
    using Exp = expected<Val2, Err>;
    static_assert(
        disjunction<
            is_void<Exp>,
            conjunction<negation<is_same<Exp, in_place_t>>,
                        negation<is_same<Exp, unexpect_t>>,
                        negation<is_specialization_of<Exp, unexpected>>>>::
            value,
        "");
    if (self.has_value()) {
      return Exp(invoke(std::forward<F>(f), std::forward<Self>(self).value()));
    } else {
      return Exp(unexpect, std::forward<Self>(self).error());
    }
  }

  template <typename Self, typename F>
  static DS_CXX14_CONSTEXPR auto
  transform_error_impl(std::true_type, Self&& self, F&& f)
      -> expected<remove_cvref_t<decltype(std::declval<Self>().value())>,
                  remove_cvref_t<decltype(invoke(
                      std::forward<F>(f), std::forward<Self>(self).error()))>>
  {
    using Val = remove_cvref_t<decltype(std::declval<Self>().value())>;
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Err2 = remove_cvref_t<invoke_result_t<F, Err>>;
    using Exp = expected<Val, Err2>;
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
  static DS_CXX14_CONSTEXPR auto
  transform_error_impl(std::false_type, Self&& self, F&& f)
      -> expected<remove_cvref_t<decltype(std::declval<Self>().value())>,
                  remove_cvref_t<decltype(invoke(
                      std::forward<F>(f), std::forward<Self>(self).error()))>>
  {
    using Val = remove_cvref_t<decltype(std::declval<Self>().value())>;
    using Err = remove_cvref_t<decltype(std::declval<Self>().error())>;
    using Err2 = remove_cvref_t<invoke_result_t<F, Err>>;
    using Exp = expected<Val, Err2>;
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

  DS_CXX14_CONSTEXPR expected() = default;

  DS_CXX14_CONSTEXPR expected(const expected&) = default;

  DS_CXX14_CONSTEXPR expected(expected&&) noexcept(
      conjunction<disjunction<is_void<T>, is_nothrow_move_constructible<T>>,
                  is_nothrow_move_constructible<E>>::value)
      = default;

  template <
      typename T2,
      typename E2,
      enable_if_t<conjunction<disjunction<conjunction<is_void<T>, is_void<T2>>,
                                          is_constructible<T, const T2&>>,
                              is_constructible<E, const E2&>,
                              is_expected_constructible<T2, E2>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR
      DS_CXX20_EXPLICIT((!is_convertible<const T2&, T>::value
                         || !is_convertible<const E2&, E>::value))
          expected(const expected<T2, E2>& other)
      : dc_base_type(in_place)
  {
    this->construct_from(other);
  }

  template <
      typename T2,
      typename E2,
      enable_if_t<conjunction<disjunction<conjunction<is_void<T>, is_void<T2>>,
                                          is_constructible<T, T2>>,
                              is_constructible<E, E2>,
                              is_expected_constructible<T2, E2>>::value,
                  int>
      = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<T2, T>::value
                                        || !is_convertible<E2, E>::value))
      expected(expected<T2, E2>&& other)
      : dc_base_type(in_place)
  {
    this->construct_from(std::move(other));
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
      expected(U&& u)
      : base_type(in_place, std::forward<U>(u))
      , dc_base_type(in_place)
  {
  }

  template <typename E2,
            enable_if_t<is_constructible<E, const E2&>::value, int> = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<const E2&, E>::value))
      expected(const unexpected<E2>& une)
      : base_type(unexpect, une.error())
      , dc_base_type(in_place)
  {
  }

  template <typename E2, enable_if_t<is_constructible<E, E2>::value, int> = 0>
  DS_CXX14_CONSTEXPR DS_CXX20_EXPLICIT((!is_convertible<E2, E>::value))
      expected(unexpected<E2>&& une)
      : base_type(unexpect, std::move(une).error())
      , dc_base_type(in_place)
  {
  }
  template <bool B
            = disjunction<is_void<T>, is_default_constructible<T>>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR explicit expected(in_place_t) noexcept(is_void<T>::value)
      : base_type(in_place)
      , dc_base_type(in_place)
  {
  }

  template <typename... Args
#if defined(_MSC_VER) && !defined(__clang__) && !defined(DS_HAS_CXX17)
  // workaround: MSVC /std:c++14
#else
            ,
            enable_if_t<is_constructible<T, Args...>::value, int> = 0
#endif
            >
  DS_CXX14_CONSTEXPR explicit expected(in_place_t, Args&&... args) noexcept(
      is_void<T>::value)
      : base_type(in_place, std::forward<Args>(args)...)
      , dc_base_type(in_place)
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
                                       Args&&... args)
      : base_type(in_place, init, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  template <bool B = is_default_constructible<E>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR explicit expected(unexpect_t)
      : base_type(unexpect)
      , dc_base_type(in_place)
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
  DS_CXX14_CONSTEXPR explicit expected(unexpect_t, Args&&... args)
      : base_type(unexpect, std::forward<Args>(args)...)
      , dc_base_type(in_place)
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
                                       Args&&... args)
      : base_type(unexpect, init, std::forward<Args>(args)...)
      , dc_base_type(in_place)
  {
  }

  DS_CXX14_CONSTEXPR expected& operator=(const expected&) = default;

  DS_CXX14_CONSTEXPR expected& operator=(expected&&) noexcept(
      conjunction<disjunction<is_void<T>,
                              conjunction<is_nothrow_move_constructible<T>,
                                          is_nothrow_move_assignable<T>>>,
                  is_nothrow_move_constructible<E>,
                  is_nothrow_move_assignable<E>>::value)
      = default;

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
  DS_CXX14_CONSTEXPR expected& operator=(U&& u)
  {
    if (this->has_value()) {
      this->val_ = std::forward<U>(u);
    } else {
      this->destroy_err();
      this->construct_val(std::forward<U>(u));
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
  DS_CXX14_CONSTEXPR expected& operator=(const unexpected<E2>& une)
  {
    if (this->has_value()) {
      this->destroy_val();
      this->has_ = false;
      this->construct_err(une.error());
    } else {
      this->get_err() = une.error();
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
  DS_CXX14_CONSTEXPR expected& operator=(unexpected<E2>&& une)
  {
    if (this->has_value()) {
      this->destroy_val();
      this->has_ = false;
      this->construct_err(std::move(une).error());
    } else {
      this->get_err() = std::move(une).error();
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

  using base_type::operator*;

  DS_CXX14_CONSTEXPR explicit operator bool() const noexcept
  {
    return this->has_;
  }

  using base_type::has_value;

  using base_type::value;

  using base_type::get_err;

  DS_CXX14_CONSTEXPR E& error() & noexcept
  {
    DS_ASSERT(!this->has_value());
    return this->get_err();
  }

  DS_CXX14_CONSTEXPR const E& error() const& noexcept
  {
    DS_ASSERT(!this->has_value());
    return this->get_err();
  }

  DS_CXX14_CONSTEXPR E&& error() && noexcept
  {
    DS_ASSERT(!this->has_value());
    return std::move(this->get_err());
  }

  DS_CXX14_CONSTEXPR const E&& error() const&& noexcept
  {
    DS_ASSERT(!this->has_value());
    return std::move(this->get_err());
  }

  template <typename U = T>
  DS_CXX14_CONSTEXPR T value_or(U&& default_value) const&
  {
    if (this->has_value()) {
      return value();
    } else {
      return static_cast<T>(std::forward<U>(default_value));
    }
  }

  template <typename U = T>
  DS_CXX14_CONSTEXPR T value_or(U&& default_value) &&
  {
    if (this->has_value()) {
      return std::move(value());
    } else {
      return static_cast<T>(std::forward<U>(default_value));
    }
  }

  template <typename U = E>
  DS_CXX14_CONSTEXPR E error_or(U&& default_error) const&
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
  DS_CXX14_CONSTEXPR E error_or(U&& default_error) &&
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
  DS_CXX14_CONSTEXPR auto emplace(Args&&... args) noexcept(is_void<T>::value)
      -> decltype(this->construct_val(std::forward<Args>(args)...))
  {
    if (this->has_value()) {
      this->destroy_val();
    } else {
      this->destroy_err();
    }

    this->has_ = true;
    return this->construct_val(std::forward<Args>(args)...);
  }

  template <typename U,
            typename... Args,
            enable_if_t<
                is_constructible<T, std::initializer_list<U>&, Args...>::value,
                int>
            = 0>
  DS_CXX14_CONSTEXPR auto emplace(std::initializer_list<U> init, Args&&... args)
      -> decltype(this->construct_val(init, std::forward<Args>(args)...))
  {
    if (this->has_value()) {
      this->destroy_val();
    } else {
      this->destroy_err();
    }

    this->has_ = true;
    return this->construct_val(init, std::forward<Args>(args)...);
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) & -> decltype(and_then_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) const& -> decltype(and_then_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return and_then_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) && -> decltype(and_then_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto and_then(F&& f) const&& -> decltype(and_then_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return and_then_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) & -> decltype(or_else_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f)
      const& -> decltype(or_else_impl(is_void<T> {}, *this, std::forward<F>(f)))
  {
    return or_else_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) && -> decltype(or_else_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return or_else_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto or_else(F&& f) const&& -> decltype(or_else_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return or_else_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) & -> decltype(transform_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_copy_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) const& -> decltype(transform_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) && -> decltype(transform_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F, enable_if_t<is_move_constructible<E>::value, int> = 0>
  DS_CXX14_CONSTEXPR auto transform(F&& f) const&& -> decltype(transform_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_impl(is_void<T> {}, std::move(*this), std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto
  transform_error(F&& f) & -> decltype(transform_error_impl(is_void<T> {},
                                                            *this,
                                                            std::forward<F>(f)))
  {
    return transform_error_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_copy_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto
  transform_error(F&& f) const& -> decltype(transform_error_impl(
      is_void<T> {}, *this, std::forward<F>(f)))
  {
    return transform_error_impl(is_void<T> {}, *this, std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto
  transform_error(F&& f) && -> decltype(transform_error_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_error_impl(is_void<T> {}, std::move(*this),
                                std::forward<F>(f));
  }

  template <typename F,
            bool B = is_move_constructible<T>::value,
            enable_if_t<B, int> = 0>
  DS_CXX14_CONSTEXPR auto
  transform_error(F&& f) const&& -> decltype(transform_error_impl(
      is_void<T> {}, std::move(*this), std::forward<F>(f)))
  {
    return transform_error_impl(is_void<T> {}, std::move(*this),
                                std::forward<F>(f));
  }

  using base_type::swap;

  friend DS_CXX14_CONSTEXPR void
  swap(expected& lhs, expected& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }

  template <typename T2,
            typename E2,
            enable_if_t<conjunction<is_void<T>, is_void<T2>>::value, int> = 0>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs,
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

  template <typename T2,
            typename E2,
            enable_if_t<
                conjunction<negation<is_void<T>>, negation<is_void<T2>>>::value,
                int>
            = 0>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs,
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

  template <typename T2, bool B = !is_void<T>::value, enable_if_t<B, int> = 0>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs, const T2& rhs)
  {
    return lhs.has_value() && static_cast<bool>(lhs.value() == rhs);
  }

  template <typename E2>
  friend DS_CXX14_CONSTEXPR bool operator==(const expected& lhs,
                                            const unexpected<E2>& rhs)
  {
    return !lhs.has_value() && static_cast<bool>(lhs.error() == rhs.error());
  }
};
}
