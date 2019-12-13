// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef PARSESTART
__PARSESTART = DIAG_START_PARSE,
#undef PARSESTART
#endif

DIAG(err_extraneous_token_before_semi, CLASS_ERROR, (unsigned)diag::Severity::Error, "extraneous '%0' before ';'", 0, false, 1)
DIAG(err_unimplemented_token, CLASS_ERROR, (unsigned)diag::Severity::Error, "'%0' is not yet supported.", 0, false, 1)
DIAG(err_multiple_variable_location, CLASS_ERROR, (unsigned)diag::Severity::Error, "Location already specified.", 0, false, 1)
DIAG(err_location_without_typename, CLASS_ERROR, (unsigned)diag::Severity::Error, "Location specifier needs explicit type name.", 0, false, 1)
DIAG(err_unknown_pragma, CLASS_ERROR, (unsigned)diag::Severity::Error, "Token incompatible with Solidity parser as part of pragma directive.", 0, false, 1)
DIAG(err_expected_contract_kind, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'contract', 'interface', or 'library'.", 0, false, 1)
DIAG(err_expected_contract_part, CLASS_ERROR, (unsigned)diag::Severity::Error, "Function, variable, struct or modifier declaration expected.", 0, false, 1)
DIAG(err_expected_event, CLASS_ERROR, (unsigned)diag::Severity::Error, "Expected event name or path.", 0, false, 1)
DIAG(err_expected_location, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'storage', 'memory', or 'calldata'.", 0, false, 1)
DIAG(err_expected_visibility, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'public', 'internal', 'private', or 'external'.", 0, false, 1)
DIAG(err_expected_state_mutability, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'payable', 'view', or 'pure'.", 0, false, 1)
DIAG(err_trailing_comma, CLASS_ERROR, (unsigned)diag::Severity::Error, "Unexpected trailing comma.", 0, false, 1)
DIAG(warn_constant_removed, CLASS_WARNING, (unsigned)diag::Severity::Warning, "The state mutability modifier 'constant' was removed in version 0.5.0. Use 'view' or 'pure' instead.", 0, false, 1)
