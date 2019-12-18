# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
cmake_minimum_required(VERSION 3.9)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/utils/unittests/cmake")
set(SOLL_VERSION_MAJOR 0)
set(SOLL_VERSION_MINOR 0)
set(SOLL_VERSION_PATCH 3)
set(SOLL_VERSION_STRING
  "${SOLL_VERSION_MAJOR}.${SOLL_VERSION_MINOR}.${SOLL_VERSION_PATCH}"
  )

project(SOLL
  VERSION ${SOLL_VERSION_MAJOR}.${SOLL_VERSION_MINOR}.${SOLL_VERSION_PATCH}
  LANGUAGES CXX
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/soll/Config/Config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/include/soll/Config/Config.h
  )

find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
include(AddLLVM)

include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
link_directories("${LLVM_LIBRARY_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SOLL_INCLUDE_TESTS
  "Generate build targets for the Soll unit tests."
  ${LLVM_INCLUDE_TESTS})

option(SOLL_ENABLE_EVM
  "Enable EVM backend for the SOLL. This feature depends on EVM_LLVM project."
  ${SOLL_ENABLE_EVM})

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(utils)

if(EXISTS ${LLVM_BINARY_DIR}/build/utils/lit/lit.py)
  # Note: path not really used, except for checking if lit was found
  set(LLVM_LIT ${LLVM_BINARY_DIR}/build/utils/lit/lit.py)
else()
  # Seek installed Lit.
  find_program(LLVM_LIT
    NAMES llvm-lit lit.py lit
    PATHS "${LLVM_BINARY_DIR}/build/utils/lit"
    DOC "Path to lit.py")
endif()

if(LLVM_LIT)
  set(LLVM_EXTERNAL_LIT "${LLVM_LIT}")
  set(LIT_ARGS_DEFAULT "-sv")
  if (MSVC OR XCODE)
    set(LIT_ARGS_DEFAULT "${LIT_ARGS_DEFAULT} --no-progress-bar")
  endif()
  set(LLVM_LIT_ARGS "${LIT_ARGS_DEFAULT}" CACHE STRING "Default options for lit")
else()
  set(SOLL_INCLUDE_TESTS OFF)
endif()

if(SOLL_INCLUDE_TESTS)
  include(CTest)
  include(Catch)
  add_subdirectory(unittests)
  add_subdirectory(test)
endif()
