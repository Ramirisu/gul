# GUL

[![build](https://github.com/Ramirisu/gul/actions/workflows/build_matrix.yml/badge.svg)](https://github.com/Ramirisu/gul/actions/workflows/build_matrix.yml)
[![codecov](https://codecov.io/gh/Ramirisu/gul/branch/main/graph/badge.svg?token=XEKI3XKCK0)](https://codecov.io/gh/Ramirisu/gul)
[![codeql](https://github.com/Ramirisu/gul/actions/workflows/codeql.yml/badge.svg?branch=main)](https://github.com/Ramirisu/gul/actions/workflows/codeql.yml)
![std](https://img.shields.io/badge/std-11%2F14%2F17%2F20-blue.svg)
![license](https://img.shields.io/badge/license-BSL--1.0-blue)

gul is an open-source library providing general utilities for c++11. Many of them are backported from later standard version.

## Features

* Header-only, no external dependencies.
* Support exceptions disabled with `-fno-exceptions`.

|   Utility Class    | Description                                                                                                                                                                                                    | From std?                                                           |
| :----------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------------------------------------------ |
|       `byte`       | A type represents the byte concept.                                                                                                                                                                            | [c++17](https://en.cppreference.com/w/cpp/types/byte)               |
|     `optional`     | A type either holds a value of type `T`, or is in *valueless* state.<br />Extensions:<ul><li>`optional<void>`</li><li>`optional<T&>`</li><li>`optional::to_expected_or`</li></ul>                              | [c++17, 20, 23](https://en.cppreference.com/w/cpp/utility/optional) |
|     `expected`     | A type either holds a value of type `T`, or an *unexpected* value of type `E`.<br />Extensions:<ul><li>`expected<T&, E>`</li><li>`expected::value_to_optional`</li><li>`expected::error_to_optional`</li></ul> | [c++23](https://en.cppreference.com/w/cpp/utility/expected)         |
| `integer_sequence` | A compile-time sequence of integers.                                                                                                                                                                           | [c++14](https://en.cppreference.com/w/cpp/utility/integer_sequence) |

|                                                  Utility Function                                                  | Description                                                            |                            From std?                             |
| :----------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------- | :--------------------------------------------------------------: |
|                                                     `exchange`                                                     | Replaces the argument with a new value and returns its previous value. | [c++14, 23](https://en.cppreference.com/w/cpp/utility/exchange)  |
|                                                     `as_const`                                                     | Obtains a reference to const to its argument.                          |   [c++17](https://en.cppreference.com/w/cpp/utility/as_const)    |
| `cmp_equal`<br />`cmp_not_equal`<br />`cmp_less`<br />`cmp_greater`<br />`cmp_less_equal`<br />`cmp_greater_equal` | Compares two integer values without value change caused by conversion. |    [c++20](https://en.cppreference.com/w/cpp/utility/intcmp)     |
|                                                  `to_underlying`                                                   | Converts an enumeration to its underlying type.                        | [c++23](https://en.cppreference.com/w/cpp/utility/to_underlying) |

|        Functional        |                              From std?                               |
| :----------------------: | :------------------------------------------------------------------: |
| `invoke`<br />`invoke_r` | [c++17](https://en.cppreference.com/w/cpp/utility/functional/invoke) |

|             Memory             |                           From std?                           |
| :----------------------------: | :-----------------------------------------------------------: |
|   `out_ptr`<br />`out_ptr_t`   |  [c++23](https://en.cppreference.com/w/cpp/memory/out_ptr_t)  |
| `inout_ptr`<br />`inout_ptr_t` | [c++23](https://en.cppreference.com/w/cpp/memory/inout_ptr_t) |

|                                   Container                                   | Description                                                                                                                                                                                                                                  |                                  From std?                                  |
| :---------------------------------------------------------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :-------------------------------------------------------------------------: |
| `string_view`<br />`wstring_view`<br />`u16string_view`<br />`u32string_view` | A non-owning type can refer to a constant contiguous sequence of `char`-like objects with the first element of the sequence at position zero.<br />Extensions:<ul><li>`basic_string_view::first`</li><li>`basic_string_view::last`</li></ul> | [c++17, 20, 23](https://en.cppreference.com/w/cpp/string/basic_string_view) |
|                                    `span`                                     | A type can refer to a contiguous sequence of objects with the first element of the sequence at position zero.                                                                                                                                |          [c++20](https://en.cppreference.com/w/cpp/container/span)          |
|                                  `fifo_map`                                   | An associative container that contains key-value pairs with unique keys. `Key`s are sorted by insertion order.                                                                                                                               |                                    none                                     |
|                                   `lru_map`                                   | An associative container that contains key-value pairs with at most `capacity` unique keys. The least recently used `Key` will be purged when the map is full during insertion.                                                              |                                    none                                     |

|                                             Type Traits                                              |                              From std?                              |
| :--------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------: |
|                                          `is_null_pointer`                                           |  [c++14](https://en.cppreference.com/w/cpp/types/is_null_pointer)   |
|                                       `is_nothrow_convertible`                                       |   [c++20](https://en.cppreference.com/w/cpp/types/is_convertible)   |
| `is_swappable_with`<br />`is_swappable`<br />`is_nothrow_swappable_with`<br />`is_nothrow_swappable` |    [c++17](https://en.cppreference.com/w/cpp/types/is_swappable)    |
|                                          `is_bounded_array`                                          |  [c++20](https://en.cppreference.com/w/cpp/types/is_bounded_array)  |
|                                         `is_unbounded_array`                                         | [c++20](https://en.cppreference.com/w/cpp/types/is_unbounded_array) |
|                                           `is_scoped_enum`                                           |   [c++23](https://en.cppreference.com/w/cpp/types/is_scoped_enum)   |
|                                 `is_invocable`<br />`is_invocable_r`                                 |    [c++17](https://en.cppreference.com/w/cpp/types/is_invocable)    |
|                                        `is_specialization_of`                                        |                                none                                 |
|                                  `conjunction`<br />`conjunction_t`                                  |    [c++17](https://en.cppreference.com/w/cpp/types/conjunction)     |
|                                  `disjunction`<br />`disjunction_t`                                  |    [c++17](https://en.cppreference.com/w/cpp/types/disjunction)     |
|                                     `negation`<br />`negation_t`                                     |      [c++17](https://en.cppreference.com/w/cpp/types/negation)      |
|                                `invoke_result`<br />`invoke_result_t`                                |     [c++17](https://en.cppreference.com/w/cpp/types/result_of)      |
|                                               `void_t`                                               |       [c++17](https://en.cppreference.com/w/cpp/types/void_t)       |
|                                 `remove_cvref`<br />`remove_cvref_t`                                 |    [c++20](https://en.cppreference.com/w/cpp/types/remove_cvref)    |
|                                `type_identity`<br />`type_identity_t`                                |   [c++20](https://en.cppreference.com/w/cpp/types/type_identity)    |
|                                          `function_traits`                                           |                                none                                 |

## Integration

Requirements

* CMake 3.15 (or later)
* GCC 4.8 (or later)
* Clang 3.5 (or later)
* MSVC 2015 v140 (or later)
* Apple Clang 12 (or later)

CMake

```cmake
  add_executable(name)
  target_link_libraries(name PRIVATE gul)
```

gul is header-only. Just `#include` the header file `all.hpp` to utilize all features.

```cpp
  #include <gul/all.hpp>
```

## Building tests

CMake

| Option             | Description                | Value  | Default |
| :----------------- | :------------------------- | :----: | :-----: |
| GUL_BUILD_TESTS    | Build tests                | ON/OFF |   OFF   |
| GUL_ENABLE_CODECOV | Enable code coverage build | ON/OFF |   OFF   |

```sh
cd gul/
cmake -B build -DGUL_BUILD_TESTS=ON
cmake --build build
cd build && ctest && cd ..
```

## License

This project is distributed under the [Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt).
