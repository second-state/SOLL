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
add_test(check-soll
  ${LIT_COMMAND}
  --timeout 10
  --param soll_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set_property(TEST check-soll APPEND PROPERTY DEPENDS soll)
