// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef CODEGENSTART
__CODEGENSTART = DIAG_START_CODEGEN,
#undef CODEGENSTART
#endif

DIAG(err_address_call_without_payload, CLASS_ERROR, (unsigned)diag::Severity::Error, "address.call() should have a payload.", 0, false, 1)
