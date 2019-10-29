#ifdef FRONTENDSTART
__FRONTENDRSTART = DIAG_START_FRONTEND,
#undef FRONTENDSTART
#endif

DIAG(err_fe_error_opening, CLASS_ERROR, (unsigned)diag::Severity::Error, "error opening '%0': %1", 0, false, 0)
DIAG(err_fe_error_reading, CLASS_ERROR, (unsigned)diag::Severity::Error, "error reading '%0'", 0, false, 0)
DIAG(err_fe_error_reading_stdin, CLASS_ERROR, (unsigned)diag::Severity::Error, "error reading stdin: %0", 0, false, 0)
