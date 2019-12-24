# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
if(EXISTS ${LLVM_EXTERNAL_LIT})
else()
  if(EXISTS ${LLVM_BINARY_DIR}/build/utils/lit/lit.py)
    set(LLVM_LIT ${LLVM_BINARY_DIR}/build/utils/lit/lit.py)
  else()
    # Seek installed Lit.
    find_program(LLVM_LIT
      NAMES llvm-lit lit.py lit
      PATHS "${LLVM_BINARY_DIR}/build/utils/lit"
      DOC "Path to lit.py")
  endif()

  if(EXISTS ${LLVM_LIT})
    set(LLVM_EXTERNAL_LIT "${LLVM_LIT}" CACHE PATH "" FORCE)
    set(LIT_ARGS_DEFAULT "-sv")
    if (MSVC OR XCODE)
      set(LIT_ARGS_DEFAULT "${LIT_ARGS_DEFAULT} --no-progress-bar")
    endif()
    set(LLVM_LIT_ARGS "${LIT_ARGS_DEFAULT}" CACHE STRING "Default options for lit")
  endif()
endif()

configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
  MAIN_CONFIG
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
  )

get_llvm_lit_path(
  lit_base_dir
  lit_file_name
  ALLOW_EXTERNAL
  )

set(LIT_COMMAND "${PYTHON_EXECUTABLE};${lit_base_dir}/${lit_file_name}")

add_test(check-soll-benchmark
  ${LIT_COMMAND}
  --timeout 10
  --param soll_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/benchmark
  )
set_property(TEST check-soll-benchmark APPEND PROPERTY DEPENDS soll)

add_test(check-soll-solidity
  ${LIT_COMMAND}
  --timeout 10
  --param soll_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/solidity
  )
set_property(TEST check-soll-solidity APPEND PROPERTY DEPENDS soll)

add_test(check-soll-yul
  ${LIT_COMMAND}
  --timeout 10
  --param soll_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/yul
  )
set_property(TEST check-soll-yul APPEND PROPERTY DEPENDS soll)

add_test(check-soll-libyul
  ${LIT_COMMAND}
  --timeout 10
  --param soll_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/libyul
  )
set_property(TEST check-soll-libyul APPEND PROPERTY DEPENDS soll)
