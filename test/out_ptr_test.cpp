//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/out_ptr.hpp>

using namespace gul;

TEST_SUITE_BEGIN("out_ptr");

namespace {
template <typename T>
class non_resettable_ptr {
public:
  non_resettable_ptr() = default;

  explicit non_resettable_ptr(T* ptr)
      : ptr_(ptr)
  {
  }

  T* get() noexcept
  {
    return ptr_.get();
  }

  T& operator*() noexcept
  {
    return *ptr_;
  }

  T* release()
  {
    return ptr_.release();
  }

private:
  std::unique_ptr<T> ptr_;
};
}

namespace std {
template <typename T>
struct pointer_traits<non_resettable_ptr<T>> {
  using element_type = T;
};
}

TEST_CASE("out_ptr")
{
  {
    const auto deleter = [](void* ptr) {
      delete reinterpret_cast<int*>(ptr);
      ptr = nullptr;
    };

    void* sptr = nullptr;
    {
      const auto f = [](void** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr));

      CHECK_EQ(*reinterpret_cast<int*>(sptr), 10);
      deleter(sptr);
    }
    {
      const auto f = [](void** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr));

      CHECK_EQ(*reinterpret_cast<int*>(sptr), 10);
      deleter(sptr);
    }
  }
  {
    const auto deleter = [](int* ptr) {
      delete ptr;
      ptr = nullptr;
    };

    int* sptr = nullptr;
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr));

      CHECK_EQ(*sptr, 10);
      deleter(sptr);
    }
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr));

      CHECK_EQ(*sptr, 10);
      deleter(sptr);
    }
  }
  {
    int deleted = 0;
    const auto deleter = [&deleted](int* ptr) {
      ++deleted;
      delete ptr;
    };

    std::unique_ptr<int, decltype(deleter)> sptr(nullptr, deleter);
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr));

      CHECK_EQ(deleted, 0);
      CHECK_EQ(*sptr, 10);
    }
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(20);
      };

      f(out_ptr(sptr));

      CHECK_EQ(deleted, 1);
      CHECK_EQ(*sptr, 20);
    }
  }
  {
    int deleted = 0;
    const auto deleter = [&deleted](int* ptr) {
      ++deleted;
      delete ptr;
    };

    std::shared_ptr<int> sptr;
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(out_ptr(sptr, deleter));

      CHECK_EQ(deleted, 0);
      CHECK_EQ(*sptr, 10);
    }
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(20);
      };

      f(out_ptr(sptr, deleter));

      CHECK_EQ(deleted, 1);
      CHECK_EQ(*sptr, 20);
    }
  }
}

TEST_CASE("inout_ptr")
{
  {
    const auto deleter = [](void* ptr) {
      delete reinterpret_cast<int*>(ptr);
      ptr = nullptr;
    };

    void* sptr = nullptr;
    {
      const auto f = [](void** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*reinterpret_cast<int*>(sptr), 10);
    }
    {
      const auto f = [&](void** pptr) {
        deleter(*pptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*reinterpret_cast<int*>(sptr), 10);
      deleter(sptr);
    }
  }
  {
    const auto deleter = [](int* ptr) {
      delete ptr;
      ptr = nullptr;
    };

    int* sptr = nullptr;
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*sptr, 10);
    }
    {
      const auto f = [&](int** pptr) {
        deleter(*pptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*sptr, 10);
      deleter(sptr);
    }
  }
  {
    int deleted = 0;
    const auto deleter = [&deleted](int* ptr) {
      ++deleted;
      delete ptr;
      ptr = nullptr;
    };

    std::unique_ptr<int, decltype(deleter)> sptr(nullptr, deleter);
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(deleted, 0);
      CHECK_EQ(*sptr, 10);
    }
    {
      const auto f = [](int** pptr) {
        delete *pptr;
        *pptr = new int(20);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(deleted, 0);
      CHECK_EQ(*sptr, 20);
    }
  }
  {
    non_resettable_ptr<int> sptr;
    {
      const auto f = [](int** pptr) {
        CHECK_EQ(*pptr, nullptr);
        *pptr = new int(10);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*sptr, 10);
    }
    {
      const auto f = [](int** pptr) {
        delete *pptr;
        *pptr = new int(20);
      };

      f(inout_ptr(sptr));

      CHECK_EQ(*sptr, 20);
    }
  }
}

TEST_SUITE_END();
