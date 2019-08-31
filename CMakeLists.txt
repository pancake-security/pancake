cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Pancake C CXX)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SUPPRESS_WARN_FLAGS
            "${SUPPRESS_WARN_FLAGS} -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-result -Wno-conversion-null" )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(SUPPRESS_WARN_FLAGS
            "${SUPPRESS_WARN_FLAGS} -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-missing-field-initializers -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-private-field -Wno-unused-const-variable")
endif()

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE false
        CACHE STRING "Show all outputs including compiler lines." FORCE)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

# Dependencies
include(Dependencies)

# Testing
if (BUILD_TESTS)
    include(CTest)
endif()

if (BUILD_STRESS_TESTS)
    add_definitions(-DSTRESS_TEST)
endif()

# Client library
add_subdirectory(libclient)

# Storage library
add_subdirectory(libstorage)

if (BUILD_BENCHMARK)
  # RPC Framework
  add_subdirectory(benchmark)
endif()
