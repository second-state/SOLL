// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef SEMASTART
__SEMASTART = DIAG_START_SEMA,
#undef SEMASTART
#endif

DIAG(err_undefined_variable, CLASS_ERROR, (unsigned)diag::Severity::Fatal, "No matching declaration found after variable lookup.", 0, false, 0)
DIAG(err_unimplemented_identifier, CLASS_ERROR, (unsigned)diag::Severity::Error, "%0 is not yet supported.", 0, false, 1)
