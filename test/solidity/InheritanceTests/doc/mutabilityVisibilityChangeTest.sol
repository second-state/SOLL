// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s

contract Base
{
    function foo() virtual external view {}
}

contract Middle is Base {}

contract Inherited is Middle
{
    function foo() override public pure {}
}
