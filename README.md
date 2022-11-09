# ds

[![build](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml/badge.svg)](https://github.com/Ramirisu/ds/actions/workflows/build_matrix.yml)

ds is an open-source library providing general utilities for c++11.

## Features

|  Feature   | Description                                                                                                                                                                     |
| :--------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `optional` | A type either holds a value of type `T`, or is with *valueless* state.                                                                                                          |
| `expected` | A type either holds a value of type `T`, or an *unexpected* value of type `E`.                                                                                                  |
| `fifo_map` | An associative container that contains key-value pairs with unique keys. `Key`s are sorted by insertion order.                                                                  |
| `lru_map`  | An associative container that contains key-value pairs with at most `capacity` unique keys. The least recently used `Key` will be purged when the map is full during insertion. |

## Building

CMake

| Options          | Description      | Value  | Default |
| :--------------- | :--------------- | :----: | :-----: |
| DS_BUILD_TESTING | Build unit tests | ON/OFF |   ON    |

```sh
cd ds/
cmake -B build -DDS_BUILD_TESTING=ON
cmake --build build
cd build && ctest && cd ..
```

## License

This project is distributed under the [MIT License](https://github.com/Ramirisu/ds/blob/main/LICENSE).
