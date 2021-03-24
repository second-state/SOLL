// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef CODEGENSTART
__CODEGENSTART = DIAG_START_CODEGEN,
#undef CODEGENSTART
#endif

DIAG(err_address_call_without_payload, CLASS_ERROR, (unsigned)diag::Severity::Error, "address.call() should have a payload.", 0, false, 1)
DIAG(err_can_not_emit_interface, CLASS_ERROR, (unsigned)diag::Severity::Error, "Interface can not be emited", 0, false, 1)
DIAG(err_can_not_emit_contract_with_implemented_part, CLASS_ERROR, (unsigned)diag::Severity::Error, "The contract with implemented part can not be emited", 0, false, 1)
