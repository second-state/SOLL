// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
contract HelloWorld {
   function sayHello(string _name) public pure returns (string) {
       return _name;
   }
}
