// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --libraries "LA:0x111122223333444455556666777788889999aAaa LB:0x777788889999AaAAbBbbCcccddDdeeeEfFFfCcCc" --runtime --action=EmitLLVM %s
pragma solidity ^0.5.0;

library LA {
    function ADD(uint self, uint value)
        public
        pure
        returns (uint)
    {
        return self+value;
    }
    function Multi(uint self, uint value)
        public
        pure
        returns (uint, uint)
    {
        return (self+value,self-value);
    }
}

library LB {
    function SUB(uint self, uint value)
        public
        pure
        returns (uint)
    {
        return self-value;
    }
}

contract C {
    using LA, LB for uint;
    function get() public pure 
    returns (uint)
    {
        uint A = 123;
        A = A.ADD(2217);
        A = A.SUB(124);
        return A;
    }
}

contract D {
    using LA for uint;
    using LB for *;
    function get() public pure 
    returns (uint)
    {
        uint A = 123;
        A = A.ADD(2217);
        A = A.SUB(124);
        uint32 B;
        (A, B) = A.Multi(2217);
        return A;
    }
}