// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef SEMASTART
__SEMASTART = DIAG_START_SEMA,
#undef SEMASTART
#endif

DIAG(err_undeclared_var_use, CLASS_ERROR, (unsigned)diag::Severity::Fatal, "Use of undeclared identifier '%0'", 0, false, 0)
DIAG(err_unimplemented_identifier, CLASS_ERROR, (unsigned)diag::Severity::Error, "'%0' is not yet supported", 0, false, 1)
DIAG(err_unimplemented_constructor_parameter, CLASS_ERROR, (unsigned)diag::Severity::Error, "Constructor with parameter is not yet supported", 0, false, 1)
DIAG(err_typecheck_invalid_cast, CLASS_ERROR, (unsigned)diag::Severity::Error, "Invalid type casting from '%0' to '%1'.", 0, false, 1)
DIAG(err_typecheck_unimplemented_cast, CLASS_ERROR, (unsigned)diag::Severity::Error, "type casting from '%0' to '%1' is not yet supported", 0, false, 1)
DIAG(err_typecheck_no_implicit_convertion, CLASS_ERROR, (unsigned)diag::Severity::Error, "Cannot implicit convert between '%0' and '%1'", 0, false, 1)
DIAG(err_typecheck_subscript_value, CLASS_ERROR, (unsigned)diag::Severity::Error, "Subscripted value is not an mapping, array, string or bytes", 0, false, 1)
DIAG(err_typecheck_subscript_not_integer, CLASS_ERROR, (unsigned)diag::Severity::Error, "Subscript is not an integer", 0, false, 1)
DIAG(err_typecheck_unary_expr, CLASS_ERROR, (unsigned)diag::Severity::Error, "Invalid argument type %0 to unary expression", 0, false, 1)
DIAG(err_typecheck_invalid_operands, CLASS_ERROR, (unsigned)diag::Severity::Error, "Invalid operands to binary expression ('%0' and '%1')", 0, false, 1)
DIAG(err_typecheck_exp_signed, CLASS_ERROR, (unsigned)diag::Severity::Error, "Exponentiation power is not allowed to be a signed integer type.", 0, false, 1)
DIAG(err_ovl_no_viable_function_in_call, CLASS_ERROR, (unsigned)diag::Severity::Error, "No matching function for call to '%0'", 0, false, 1)
DIAG(err_no_member, CLASS_ERROR, (unsigned)diag::Severity::Error, "No member named ;%0; in ;%1'.", 0, false, 1)
DIAG(warn_impcast_integer_precision, CLASS_WARNING, (unsigned)diag::Severity::Warning, "Implicit conversion loses integer precision: '%0' to '%1'.", 0, false, 1)
