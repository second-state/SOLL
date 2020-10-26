// SPDX-License-Identifier: GPL-3.0-or-later
// RUN: %soll %s
contract C {
    function f() {
        0xffffff;
        1.234e5;
        0x7122_7122_aaaa_eeee;
        465_4564;
        0.1_2e1_0;
    }
}
