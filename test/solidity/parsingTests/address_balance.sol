// SPDX-License-Identifier: GPL-3.0-or-later
// RUN: %soll %s
contract C {
    function f() {
        uint256 bal = address(0xbf4ed7b27f1d666546e30d74d50d173d20bca754).balance;
        address(0xbf4ed7b27f1d666546e30d74d50d173d20bca754).balance < 1;
    }
}
