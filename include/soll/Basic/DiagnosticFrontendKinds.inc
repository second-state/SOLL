// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef FRONTENDSTART
__FRONTENDRSTART = DIAG_START_FRONTEND,
#undef FRONTENDSTART
#endif

DIAG(err_fe_error_opening, CLASS_ERROR, (unsigned)diag::Severity::Error, "error opening '%0': %1", 0, false, 0)
DIAG(err_fe_error_reading, CLASS_ERROR, (unsigned)diag::Severity::Error, "error reading '%0'", 0, false, 0)
DIAG(err_fe_error_reading_stdin, CLASS_ERROR, (unsigned)diag::Severity::Error, "error reading stdin: %0", 0, false, 0)
DIAG(err_fe_unable_to_interface_with_target, CLASS_ERROR, (unsigned)diag::Severity::Error, "unable to interface with target machine", 0, false, 0)
DIAG(err_fe_error_data_layout_mismatch, CLASS_ERROR, (unsigned)diag::Severity::Error, "backend data layout '%0' does not match expected target description '%1'", 0, false, 0)
