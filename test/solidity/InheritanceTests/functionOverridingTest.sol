// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s

contract Base {
    function zz() virtual {}
}

contract A is Base {
    function zz() override {}
}
