// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
contract O {
    int x;
}

contract A is O {

}

contract B is O {

}

contract C is O {

}

contract D is O {

}

contract E is O {

}

contract K1 is A, B, C {

}

contract K2 is D, B, E {

}

contract K3 is D, A {

}

contract Z is K1, K2, K3 {

}

contract X {}
contract AA is X {}
// CHECK: solidityC3ExampleFailTest.sol:48:1
// CHECK: Fail to apply C3 linearization on 'CC'.
// CHECK: Fail to resolve inherit relationship
contract CC is AA, X {}
;
