# ds

[![build](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml/badge.svg)](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml)
![std](https://img.shields.io/badge/std-11%2F14%2F17%2F20-blue.svg)

ds is an open-source library providing general utilities for c++11. Many of them are backported from later standard version.

## Features

|    Feature    | Description                                                                                                                                                                     | From std? |
| :-----------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :-------: |
|    `byte`     | A type represents the byte concept.                                                                                                                                             |   c++17   |
|  `optional`   | A type either holds a value of type `T`, or is in *valueless* state.                                                                                                            |   c++17   |
|  `expected`   | A type either holds a value of type `T`, or an *unexpected* value of type `E`.                                                                                                  |   c++23   |
| `string_view` | A non-owning type can refer to a constant contiguous sequence of `char`-like objects with the first element of the sequence at position zero                                    |   c++17   |
|    `span`     | A type can refer to a contiguous sequence of objects with the first element of the sequence at position zero.                                                                   |   c++20   |
|  `fifo_map`   | An associative container that contains key-value pairs with unique keys. `Key`s are sorted by insertion order.                                                                  |   none    |
|   `lru_map`   | An associative container that contains key-value pairs with at most `capacity` unique keys. The least recently used `Key` will be purged when the map is full during insertion. |   none    |

## Building

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

This project is distributed under the [MIT License](https://github.com/Ramirisu/ds/blob/main/LICENSE).
