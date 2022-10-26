cmake_minimum_required(VERSION 3.15)

project(
  ds
  VERSION 0.0.0
  LANGUAGES CXX)

option(DS_BUILD_TESTS "Build tests" ON)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library(ds INTERFACE)
target_include_directories(ds INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(DS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
