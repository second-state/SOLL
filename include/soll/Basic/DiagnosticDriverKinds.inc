// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef DRIVERSTART
__DRIVERSTART = DIAG_START_DRIVER,
#undef DRIVERSTART
#endif

DIAG(err_drv_no_such_file, CLASS_ERROR, (unsigned)diag::Severity::Error, "no such file or directory: '%0'", 0, false, 0)
