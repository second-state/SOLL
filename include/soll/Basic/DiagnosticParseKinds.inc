// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef PARSESTART
__PARSESTART = DIAG_START_PARSE,
#undef PARSESTART
#endif

DIAG(err_extraneous_token_before_semi, CLASS_ERROR, (unsigned)diag::Severity::Error, "Extraneous '%0' before ';'.", 0, false, 1)
DIAG(err_unimplemented_token, CLASS_ERROR, (unsigned)diag::Severity::Error, "'%0' is not yet supported.", 0, false, 1)
DIAG(err_unimplemented_tuple_return, CLASS_ERROR, (unsigned)diag::Severity::Error, "Tuple return is not yet supported.", 0, false, 1)
DIAG(err_unimplemented_inline_array, CLASS_ERROR, (unsigned)diag::Severity::Error, "Inline array is not yet supported.", 0, false, 1)
DIAG(err_hex_escape_incomplete, CLASS_ERROR, (unsigned)diag::Severity::Error, "Incomplete hex string.", 0, false, 1)
DIAG(err_ucn_escape_incomplete, CLASS_ERROR, (unsigned)diag::Severity::Error, "Incomplete universal character name.", 0, false, 1)
DIAG(err_unknown_escape, CLASS_ERROR, (unsigned)diag::Severity::Error, "Unknown escape sequence '\\%0'.", 0, false, 1)
DIAG(err_invalid_numeric_literal, CLASS_ERROR, (unsigned)diag::Severity::Error, "Invalid numeric literal '%0'.", 0, false, 1)
DIAG(err_negative_array_size, CLASS_ERROR, (unsigned)diag::Severity::Error, "Declare an array with a negative size.", 0, false, 1)
DIAG(err_multiple_variable_location, CLASS_ERROR, (unsigned)diag::Severity::Error, "Location already specified.", 0, false, 1)
DIAG(err_duplicate_definition, CLASS_ERROR, (unsigned)diag::Severity::Error, "'%0' duplicate definition.", 0, false, 1)
DIAG(err_location_without_typename, CLASS_ERROR, (unsigned)diag::Severity::Error, "Location specifier needs explicit type name.", 0, false, 1)
DIAG(err_switch_without_case, CLASS_ERROR, (unsigned)diag::Severity::Error, "Switch statement without any cases.", 0, false, 1)
DIAG(err_unknown_pragma, CLASS_ERROR, (unsigned)diag::Severity::Error, "Token incompatible with Solidity parser as part of pragma directive.", 0, false, 1)
DIAG(err_expected_contract_kind, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'contract', 'interface', or 'library'.", 0, false, 1)
DIAG(err_expected_contract_part, CLASS_ERROR, (unsigned)diag::Severity::Error, "Function, variable, struct or modifier declaration expected.", 0, false, 1)
DIAG(err_expected_event, CLASS_ERROR, (unsigned)diag::Severity::Error, "Expected event name or path.", 0, false, 1)
DIAG(err_expected_location, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'storage', 'memory', or 'calldata'.", 0, false, 1)
DIAG(err_expected_visibility, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'public', 'internal', 'private', or 'external'.", 0, false, 1)
DIAG(err_expected_state_mutability, CLASS_ERROR, (unsigned)diag::Severity::Error, "Except 'payable', 'view', or 'pure'.", 0, false, 1)
DIAG(err_trailing_comma, CLASS_ERROR, (unsigned)diag::Severity::Error, "Unexpected trailing comma.", 0, false, 1)
DIAG(warn_constant_removed, CLASS_WARNING, (unsigned)diag::Severity::Warning, "The state mutability modifier 'constant' was removed in version 0.5.0. Use 'view' or 'pure' instead.", 0, false, 1)
DIAG(err_multiple_constuctors, CLASS_ERROR, (unsigned)diag::Severity::Error, "multiple constructor defined", 0, false, 1)
DIAG(err_multiple_fallbacks, CLASS_ERROR, (unsigned)diag::Severity::Error, "multiple fallback defined", 0, false, 1)
DIAG(warn_virtual_already_specified, CLASS_WARNING, (unsigned)diag::Severity::Warning, "Virtual already specified.", 0, false, 1)
DIAG(warn_override_already_specified, CLASS_WARNING, (unsigned)diag::Severity::Warning, "Override already specified.", 0, false, 1)
DIAG(err_multiinherited_unsupport, CLASS_ERROR, (unsigned)diag::Severity::Error, "Multinherited overrided is not supported!", 0, false, 1)
