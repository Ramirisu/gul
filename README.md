# ds

[![build](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml/badge.svg)](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml)
![std](https://img.shields.io/badge/std-11%2F14%2F17%2F20-blue.svg)
![license](https://img.shields.io/badge/license-BSL--1.0-blue)

ds is an open-source library providing general utilities for c++11. Many of them are backported from later standard version.

## Features

|                                  Feature                                   | Description                                                                                                                                                                     |                              From std?                              |
| :------------------------------------------------------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :-----------------------------------------------------------------: |
|                                   `byte`                                   | A type represents the byte concept.                                                                                                                                             |        [c++17](https://en.cppreference.com/w/cpp/types/byte)        |
|                                 `optional`                                 | A type either holds a value of type `T`, or is in *valueless* state.                                                                                                            |     [c++17](https://en.cppreference.com/w/cpp/utility/optional)     |
|                                 `expected`                                 | A type either holds a value of type `T`, or an *unexpected* value of type `E`.                                                                                                  |     [c++23](https://en.cppreference.com/w/cpp/utility/expected)     |
| `string_view`</br>`wstring_view`</br>`u16string_view`</br>`u32string_view` | A non-owning type can refer to a constant contiguous sequence of `char`-like objects with the first element of the sequence at position zero                                    | [c++17](https://en.cppreference.com/w/cpp/string/basic_string_view) |
|                                   `span`                                   | A type can refer to a contiguous sequence of objects with the first element of the sequence at position zero.                                                                   |      [c++20](https://en.cppreference.com/w/cpp/container/span)      |
|                                 `fifo_map`                                 | An associative container that contains key-value pairs with unique keys. `Key`s are sorted by insertion order.                                                                  |                                none                                 |
|                                 `lru_map`                                  | An associative container that contains key-value pairs with at most `capacity` unique keys. The least recently used `Key` will be purged when the map is full during insertion. |                                none                                 |

## Building

Compiler Support

* GCC 5.5
* Clang 9
* MSVC 2017 15.9 v141

CMake

| Option         | Description | Value  | Default |
| :------------- | :---------- | :----: | :-----: |
| DS_BUILD_TESTS | Build tests | ON/OFF |   ON    |

```sh
cd ds/
cmake -B build -DDS_BUILD_TESTS=ON
cmake --build build
cd build && ctest && cd ..
```

## License

This project is distributed under the [Boost Software License 1.0](https://github.com/Ramirisu/ds/blob/main/LICENSE).
