// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
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
;
