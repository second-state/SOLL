// SPDX-License-Identifier: GPL-3.0-or-later
// RUN: %soll %s
// REQUIRES: ErrorHandling
contract C {
    function f() {
        xabcde;
        $abcde;
        _abcde;
    }
}
