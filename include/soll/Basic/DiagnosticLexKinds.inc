// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifdef LEXSTART
__LEXSTART = DIAG_START_LEX,
#undef LEXSTART
#endif

DIAG(backslash_newline_space, CLASS_WARNING, (unsigned)diag::Severity::Warning, "backslash and newline separated by space", 54, false, 1)
DIAG(err_invalid_utf8, CLASS_ERROR, (unsigned)diag::Severity::Error, "source file is not valid UTF-8", 0, false, 1)
DIAG(err_non_ascii, CLASS_ERROR, (unsigned)diag::Severity::Error, "non-ASCII characters are not allowed outside of literals and identifiers", 0, false, 1)
DIAG(err_unterminated_block_comment, CLASS_ERROR, (unsigned)diag::Severity::Error, "unterminated /* comment", 0, false, 1)
DIAG(escaped_newline_block_comment_end, CLASS_WARNING, (unsigned)diag::Severity::Warning, "escaped newline between */ characters at block comment end", 126, false, 1)
DIAG(ext_multi_line_line_comment, CLASS_EXTENSION, (unsigned)diag::Severity::Ignored, "multi-line // comment", 126, false, 1)
DIAG(ext_unterminated_char_or_string, CLASS_EXTENSION, (unsigned)diag::Severity::Warning, "missing terminating %select{'|'\"'}0 character", 343, false, 1)
DIAG(null_in_char_or_string, CLASS_WARNING, (unsigned)diag::Severity::Warning, "null character(s) preserved in %select{char|string}0 literal", 451, false, 1)
DIAG(null_in_file, CLASS_WARNING, (unsigned)diag::Severity::Warning, "null character ignored", 451, false, 1)
DIAG(warn_nested_block_comment, CLASS_WARNING, (unsigned)diag::Severity::Warning, "'/*' within block comment", 126, false, 1)
