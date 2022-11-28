# GUL

[![build](https://github.com/Ramirisu/gul/actions/workflows/build_matrix.yml/badge.svg)](https://github.com/Ramirisu/gul/actions/workflows/build_matrix.yml)
![std](https://img.shields.io/badge/std-11%2F14%2F17%2F20-blue.svg)
![license](https://img.shields.io/badge/license-BSL--1.0-blue)

gul is an open-source library providing general utilities for c++11. Many of them are backported from later standard version.

## Features

* Header-only, no external dependencies.
* Support exceptions disabled with `-fno-exceptions`.

| Utility Class | Description                                                                    |                          From std?                          |
| :-----------: | :----------------------------------------------------------------------------- | :---------------------------------------------------------: |
|    `byte`     | A type represents the byte concept.                                            |    [c++17](https://en.cppreference.com/w/cpp/types/byte)    |
|  `optional`   | A type either holds a value of type `T`, or is in *valueless* state.           | [c++17](https://en.cppreference.com/w/cpp/utility/optional) |
|  `expected`   | A type either holds a value of type `T`, or an *unexpected* value of type `E`. | [c++23](https://en.cppreference.com/w/cpp/utility/expected) |


|                                               Utility Function                                                | Description                                                            |                            From std?                             |
| :-----------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------- | :--------------------------------------------------------------: |
|                                                  `exchange`                                                   | Replaces the argument with a new value and returns its previous value. |   [c++14](https://en.cppreference.com/w/cpp/utility/exchange)    |
|                                                  `as_const`                                                   | Obtains a reference to const to its argument.                          |   [c++17](https://en.cppreference.com/w/cpp/utility/as_const)    |
| `cmp_equal`</br>`cmp_not_equal`</br>`cmp_less`</br>`cmp_greater`</br>`cmp_less_equal`</br>`cmp_greater_equal` | Compares two integer values without value change caused by conversion. |    [c++20](https://en.cppreference.com/w/cpp/utility/intcmp)     |
|                                                `to_underlying`                                                | Converts an enumeration to its underlying type.                        | [c++23](https://en.cppreference.com/w/cpp/utility/to_underlying) |

|                                 Container                                  | Description                                                                                                                                                                     |                              From std?                              |
| :------------------------------------------------------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :-----------------------------------------------------------------: |
| `string_view`</br>`wstring_view`</br>`u16string_view`</br>`u32string_view` | A non-owning type can refer to a constant contiguous sequence of `char`-like objects with the first element of the sequence at position zero                                    | [c++17](https://en.cppreference.com/w/cpp/string/basic_string_view) |
|                                   `span`                                   | A type can refer to a contiguous sequence of objects with the first element of the sequence at position zero.                                                                   |      [c++20](https://en.cppreference.com/w/cpp/container/span)      |
|                                 `fifo_map`                                 | An associative container that contains key-value pairs with unique keys. `Key`s are sorted by insertion order.                                                                  |                                none                                 |
|                                 `lru_map`                                  | An associative container that contains key-value pairs with at most `capacity` unique keys. The least recently used `Key` will be purged when the map is full during insertion. |                                none                                 |

## Integration

Requirements

* CMake 3.15 (or later)
* GCC 4.8 (or later)
* Clang 3.5 (or later)
* MSVC 2017 v141 (or later)

CMake

```cmake
  add_executable(your_exe)
  target_link_libraries(your_exe PRIVATE gul)
```

gul is header-only. Just `#include` the header file `all.hpp` to utilize all features.

```cpp
  #include <gul/all.hpp>
```

## Building tests

CMake

| Option          | Description | Value  | Default |
| :-------------- | :---------- | :----: | :-----: |
| GUL_BUILD_TESTS | Build tests | ON/OFF |   OFF   |

```sh
cd gul/
cmake -B build -DGUL_BUILD_TESTS=ON
cmake --build build
cd build && ctest && cd ..
```

## License

This project is distributed under the [Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt).
