cmake_minimum_required(VERSION 3.15)

project(
  gul
  VERSION 0.0.0
  LANGUAGES CXX)

option(GUL_BUILD_TESTS "Build tests" OFF)
option(GUL_ENABLE_CODECOV "Enable code coverage" OFF)

include(cmake/CPM.cmake)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library(gul INTERFACE)
target_include_directories(gul INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(GUL_BUILD_TESTS)
  cpmaddpackage("gh:doctest/doctest@2.4.9")

  enable_testing()
  add_subdirectory(test)
endif()
